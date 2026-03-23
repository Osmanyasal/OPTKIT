#!/usr/bin/env python3
"""optkit-cli GRU trainer + ONNX exporter.

Dataset layout:
- <folder>/target_range.txt (core_min/core_max; optional uncore_min/uncore_max)
- <folder>/*_screenshot/ subfolders, each with:
    - target.txt (core:<GHz>, optional uncore:<GHz>)
    - stat__cpu_pmu.json
    - stat__disk_io.json

Each JSON contains a "readings" array. The first entry is the overall aggregate
for the whole execution and is skipped; the remaining entries are treated as a
time series.

Model:
- GRU over the time series, followed by a linear head.
- Input normalization (per-feature mean/std) is applied.
- Output scaling uses target_range min/max; the exported ONNX outputs GHz.

Artifacts:
- <folder>/optkit_train/model.onnx
- <folder>/optkit_train/meta.json
"""

from __future__ import annotations

import argparse
import json
import math
import os
import sys
from dataclasses import dataclass
from glob import glob
from typing import Dict, List, Optional, Tuple

import numpy as np


def _require_torch() -> "tuple[object, object]":
    try:
        import torch  # type: ignore
        import torch.nn as nn  # type: ignore
    except Exception as e:  # pragma: no cover
        print("ERROR: Missing PyTorch dependency for GRU training.", file=sys.stderr)
        print("Install with:", file=sys.stderr)
        print("  cd /home/rt7/Desktop/OPTKIT/tools/optkit-cli && python3 -m venv .venv-train", file=sys.stderr)
        print("  /home/rt7/Desktop/OPTKIT/tools/optkit-cli/.venv-train/bin/pip install -r /home/rt7/Desktop/OPTKIT/tools/optkit-cli/requirements-train.txt", file=sys.stderr)
        print(f"Original import error: {e}", file=sys.stderr)
        raise
    return torch, nn


def _require_onnxruntime_quant() -> "tuple[object, object]":
    try:
        from onnxruntime.quantization import QuantType, quantize_dynamic  # type: ignore
    except Exception as e:  # pragma: no cover
        print("ERROR: Missing onnxruntime dependency for INT8 ONNX export.", file=sys.stderr)
        print("Install with:", file=sys.stderr)
        print(
            "  cd /home/rt7/Desktop/OPTKIT/tools/optkit-cli && /home/rt7/Desktop/OPTKIT/tools/optkit-cli/.venv-train/bin/pip install -r /home/rt7/Desktop/OPTKIT/tools/optkit-cli/requirements-train.txt",
            file=sys.stderr,
        )
        print(f"Original import error: {e}", file=sys.stderr)
        raise
    return quantize_dynamic, QuantType


def _print_onnx_initializer_memory(onnx_path: str, label: str) -> None:
    try:
        import onnx  # type: ignore
        from collections import defaultdict

        m = onnx.load(onnx_path)
        by_dtype = defaultdict(int)
        total = 0
        for t in m.graph.initializer:
            arr = onnx.numpy_helper.to_array(t)
            nb = int(arr.nbytes)
            by_dtype[str(arr.dtype)] += nb
            total += nb

        print(f"{label} initializer_bytes_total: {total} bytes ({total/1024.0:.1f} KiB)")
        for dt, nb in sorted(by_dtype.items(), key=lambda kv: -kv[1]):
            print(f"{label} initializer_bytes[{dt}]: {nb} bytes ({nb/1024.0:.1f} KiB)")
    except Exception as e:  # pragma: no cover
        print(f"WARNING: Could not compute ONNX initializer memory for {onnx_path}: {e}", file=sys.stderr)


@dataclass
class TargetRange:
    core_min: float
    core_max: float
    uncore_min: Optional[float] = None
    uncore_max: Optional[float] = None


def _parse_kv_file(path: str) -> Dict[str, str]:
    out: Dict[str, str] = {}
    with open(path, "r", encoding="utf-8") as f:
        for raw in f:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            if ":" not in line:
                continue
            k, v = line.split(":", 1)
            out[k.strip()] = v.strip()
    return out


def parse_target_range(path: str) -> TargetRange:
    kv = _parse_kv_file(path)
    if "core_min" not in kv or "core_max" not in kv:
        raise ValueError(f"target_range missing core_min/core_max: {path}")
    core_min = float(kv["core_min"])
    core_max = float(kv["core_max"])

    uncore_min = float(kv["uncore_min"]) if "uncore_min" in kv else None
    uncore_max = float(kv["uncore_max"]) if "uncore_max" in kv else None
    return TargetRange(core_min=core_min, core_max=core_max, uncore_min=uncore_min, uncore_max=uncore_max)


def parse_target_txt(path: str) -> Dict[str, float]:
    kv = _parse_kv_file(path)
    out: Dict[str, float] = {}
    if "core" in kv:
        out["core"] = float(kv["core"])
    if "uncore" in kv:
        out["uncore"] = float(kv["uncore"])
    return out


def _to_float(v) -> Optional[float]:
    if v is None:
        return None
    if isinstance(v, (int, float)):
        return float(v)
    if isinstance(v, str):
        s = v.strip()
        if not s:
            return None
        try:
            return float(s)
        except ValueError:
            return None
    return None


def load_stat_json(path: str, prefix: str) -> List[Dict[str, float]]:
    with open(path, "r", encoding="utf-8") as f:
        root = json.load(f)

    # Root is typically a list with one element containing {"readings": [...]}
    if isinstance(root, list):
        if not root:
            return []
        first = root[0]
        if isinstance(first, dict) and "readings" in first:
            readings = first.get("readings")
        else:
            # Fallback: flatten any elements that look like reading containers
            readings = []
            for elem in root:
                if isinstance(elem, dict) and isinstance(elem.get("readings"), list):
                    readings.extend(elem.get("readings"))
    elif isinstance(root, dict) and isinstance(root.get("readings"), list):
        readings = root.get("readings")
    else:
        return []

    if not isinstance(readings, list):
        return []

    samples: List[Dict[str, float]] = []
    for r in readings:
        if not isinstance(r, dict):
            continue
        d: Dict[str, float] = {}

        dur = _to_float(r.get("duration"))
        if dur is not None:
            d[f"{prefix}duration_ms"] = float(dur)

        meas = r.get("measurements", [])
        if isinstance(meas, list):
            for m in meas:
                if not isinstance(m, dict):
                    continue
                name = m.get("name")
                if not isinstance(name, str) or not name:
                    continue
                val = _to_float(m.get("value"))
                if val is None or (isinstance(val, float) and (math.isnan(val) or math.isinf(val))):
                    continue
                d[f"{prefix}{name}"] = float(val)

        samples.append(d)

    return samples


def discover_runs(dataset_folder: str) -> List[str]:
    runs = sorted([p for p in glob(os.path.join(dataset_folder, "*_screenshot")) if os.path.isdir(p)])
    return runs


def align_and_merge_timeseries(
    cpu_samples: List[Dict[str, float]],
    disk_samples: List[Dict[str, float]],
) -> List[Dict[str, float]]:
    # Skip the first reading: overall sum
    cpu = cpu_samples[1:] if len(cpu_samples) > 1 else []
    disk = disk_samples[1:] if len(disk_samples) > 1 else []

    n = min(len(cpu), len(disk))
    merged: List[Dict[str, float]] = []
    for i in range(n):
        d: Dict[str, float] = {}
        d.update(cpu[i])
        d.update(disk[i])
        merged.append(d)
    return merged


def build_feature_space(seqs: List[List[Dict[str, float]]]) -> List[str]:
    keys = set()
    for seq in seqs:
        for t in seq:
            keys.update(t.keys())
    return sorted(keys)


def seqs_to_arrays(
    seqs: List[List[Dict[str, float]]],
    feature_names: List[str],
) -> Tuple[List[np.ndarray], List[int]]:
    name_to_idx = {n: i for i, n in enumerate(feature_names)}
    arrays: List[np.ndarray] = []
    lengths: List[int] = []

    for seq in seqs:
        t = len(seq)
        lengths.append(t)
        a = np.zeros((t, len(feature_names)), dtype=np.float32)
        for i, d in enumerate(seq):
            for k, v in d.items():
                j = name_to_idx.get(k)
                if j is None:
                    continue
                a[i, j] = float(v)
        arrays.append(a)

    return arrays, lengths


def compute_x_norm(arrays: List[np.ndarray]) -> Tuple[np.ndarray, np.ndarray]:
    if not arrays:
        raise ValueError("No arrays for normalization")
    all_x = np.concatenate(arrays, axis=0)
    mean = all_x.mean(axis=0)
    std = all_x.std(axis=0)
    std = np.where(std < 1e-8, 1.0, std)
    return mean.astype(np.float32), std.astype(np.float32)


def normalize_x(arr: np.ndarray, mean: np.ndarray, std: np.ndarray) -> np.ndarray:
    return (arr - mean) / std


def scale_y(y: np.ndarray, y_min: np.ndarray, y_max: np.ndarray) -> np.ndarray:
    denom = (y_max - y_min)
    denom = np.where(np.abs(denom) < 1e-12, 1.0, denom)
    return (y - y_min) / denom


def unscale_y(y_scaled: np.ndarray, y_min: np.ndarray, y_max: np.ndarray) -> np.ndarray:
    return y_scaled * (y_max - y_min) + y_min


def build_training_examples(
    arrays: List[np.ndarray],
    targets: np.ndarray,
    window: int,
) -> Tuple[np.ndarray, np.ndarray]:
    """Return (X, Y) with X shaped (N, window, F) and Y shaped (N, out_dim)."""
    X_list: List[np.ndarray] = []
    Y_list: List[np.ndarray] = []

    for run_idx, arr in enumerate(arrays):
        T, F = arr.shape
        if T < window:
            continue
        for start in range(0, T - window + 1):
            chunk = arr[start : start + window]  # (window, F)
            X_list.append(chunk)
            Y_list.append(targets[run_idx])

    if not X_list:
        raise ValueError("No training examples built (check window length vs sample count)")

    X = np.stack(X_list, axis=0).astype(np.float32)
    Y = np.stack(Y_list, axis=0).astype(np.float32)
    return X, Y


def _np_to_torch(torch_mod, a: np.ndarray, device: str):
    return torch_mod.from_numpy(a).to(device=device)


def main(argv: List[str]) -> int:
    ap = argparse.ArgumentParser(prog="optkit-cli train", add_help=True)
    ap.add_argument("folder", help="Dataset folder containing target_range.txt and *_screenshot runs")
    ap.add_argument("--epochs", type=int, default=30)
    ap.add_argument("--batch-size", type=int, default=256)
    ap.add_argument("--lr", type=float, default=1e-3)
    ap.add_argument("--hidden-size", type=int, default=32)
    ap.add_argument("--num-layers", type=int, default=1)
    ap.add_argument("--window", type=int, default=0, help="Sequence window length (0 = auto from min series length)")
    ap.add_argument("--opset", type=int, default=17, help="ONNX opset version")
    ap.add_argument("--device", type=str, default="cpu", help="cpu or cuda")
    args = ap.parse_args(argv)

    try:
        torch, nn = _require_torch()
    except Exception:
        return 2

    dataset_folder = os.path.abspath(args.folder)
    target_range_path = os.path.join(dataset_folder, "target_range.txt")
    if not os.path.isfile(target_range_path):
        print(f"ERROR: missing target_range.txt at {target_range_path}", file=sys.stderr)
        return 2

    tr = parse_target_range(target_range_path)
    runs = discover_runs(dataset_folder)
    if not runs:
        print(f"ERROR: no *_screenshot folders found under {dataset_folder}", file=sys.stderr)
        return 2

    seqs: List[List[Dict[str, float]]] = []
    run_targets: List[Dict[str, float]] = []

    for run_dir in runs:
        target_path = os.path.join(run_dir, "target.txt")
        cpu_path = os.path.join(run_dir, "stat__cpu_pmu.json")
        disk_path = os.path.join(run_dir, "stat__disk_io.json")
        if not os.path.isfile(target_path):
            continue
        if not os.path.isfile(cpu_path) or not os.path.isfile(disk_path):
            continue

        tgt = parse_target_txt(target_path)
        if "core" not in tgt:
            continue

        cpu_samples = load_stat_json(cpu_path, prefix="cpu_pmu.")
        disk_samples = load_stat_json(disk_path, prefix="disk_io.")
        merged = align_and_merge_timeseries(cpu_samples, disk_samples)
        if len(merged) < 2:
            continue

        seqs.append(merged)
        run_targets.append(tgt)

    if not seqs:
        print("ERROR: no usable runs found (need target.txt + both JSONs with >=2 samples)", file=sys.stderr)
        return 2

    feature_names = build_feature_space(seqs)
    arrays, lengths = seqs_to_arrays(seqs, feature_names)

    x_mean, x_std = compute_x_norm(arrays)
    arrays_norm = [normalize_x(a, x_mean, x_std) for a in arrays]

    # Determine target dims.
    have_uncore = all("uncore" in t for t in run_targets)
    if have_uncore and tr.uncore_min is None:
        print("WARNING: uncore targets present but target_range.txt lacks uncore_min/uncore_max; uncore will be ignored", file=sys.stderr)
        have_uncore = False

    if have_uncore:
        y = np.array([[t["core"], t["uncore"]] for t in run_targets], dtype=np.float32)
        y_min = np.array([tr.core_min, tr.uncore_min], dtype=np.float32)
        y_max = np.array([tr.core_max, tr.uncore_max], dtype=np.float32)
        target_names = ["core", "uncore"]
    else:
        y = np.array([[t["core"]] for t in run_targets], dtype=np.float32)
        y_min = np.array([tr.core_min], dtype=np.float32)
        y_max = np.array([tr.core_max], dtype=np.float32)
        target_names = ["core"]

    y_scaled = scale_y(y, y_min, y_max)

    # Sliding-window examples from time series
    min_len = int(min(lengths))
    if args.window and args.window > 0:
        window = int(args.window)
    else:
        window = min(10, max(2, min_len))

    X, Y = build_training_examples(arrays_norm, y_scaled, window=window)

    # Train/val split (by examples)
    rng = np.random.default_rng(42)
    idx = np.arange(X.shape[0])
    rng.shuffle(idx)
    split = int(0.8 * len(idx))
    train_idx = idx[:split]
    val_idx = idx[split:]

    Xtr, Ytr = X[train_idx], Y[train_idx]
    Xva, Yva = X[val_idx], Y[val_idx]

    device = args.device
    if device == "cuda" and not torch.cuda.is_available():
        print("WARNING: --device cuda requested but CUDA is not available; using cpu", file=sys.stderr)
        device = "cpu"

    class GRUFreqModel(nn.Module):
        def __init__(self, input_size: int, hidden_size: int, num_layers: int, output_dim: int,
                     x_mean_np: np.ndarray, x_std_np: np.ndarray, y_min_np: np.ndarray, y_max_np: np.ndarray):
            super().__init__()
            self.gru = nn.GRU(input_size=input_size, hidden_size=hidden_size, num_layers=num_layers, batch_first=True)
            self.fc = nn.Linear(hidden_size, output_dim)

            # Buffers to bake normalization/scaling into the exported ONNX graph.
            self.register_buffer("x_mean", torch.from_numpy(x_mean_np.astype(np.float32)).view(1, 1, -1))
            self.register_buffer("x_std", torch.from_numpy(x_std_np.astype(np.float32)).view(1, 1, -1))
            self.register_buffer("y_min", torch.from_numpy(y_min_np.astype(np.float32)).view(1, -1))
            self.register_buffer("y_max", torch.from_numpy(y_max_np.astype(np.float32)).view(1, -1))

        def forward(self, x):
            # x: (batch, seq, feat) raw features
            x = (x - self.x_mean) / self.x_std
            out, _h = self.gru(x)  # out: (batch, seq, hidden)
            last = out[:, -1, :]
            y_scaled = self.fc(last)
            return y_scaled

    class GRUFreqModelExport(nn.Module):
        """Wrapper used only for ONNX export to return GHz outputs."""

        def __init__(self, base: GRUFreqModel):
            super().__init__()
            self.base = base

        def forward(self, x):
            y_scaled = self.base(x)
            y = y_scaled * (self.base.y_max - self.base.y_min) + self.base.y_min
            return y

    model = GRUFreqModel(
        input_size=X.shape[2],
        hidden_size=int(args.hidden_size),
        num_layers=int(args.num_layers),
        output_dim=Y.shape[1],
        x_mean_np=x_mean,
        x_std_np=x_std,
        y_min_np=y_min,
        y_max_np=y_max,
    ).to(device)

    optim = torch.optim.Adam(model.parameters(), lr=float(args.lr))
    loss_fn = nn.MSELoss()

    def iter_batches_np(Xb: np.ndarray, Yb: np.ndarray, bs: int):
        n = Xb.shape[0]
        order = np.arange(n)
        rng.shuffle(order)
        for i in range(0, n, bs):
            j = order[i : i + bs]
            yield Xb[j], Yb[j]

    for ep in range(1, int(args.epochs) + 1):
        model.train()
        losses: List[float] = []
        for xb_np, yb_np in iter_batches_np(Xtr, Ytr, int(args.batch_size)):
            xb = _np_to_torch(torch, xb_np, device)
            yb = _np_to_torch(torch, yb_np, device)
            optim.zero_grad(set_to_none=True)
            yhat = model(xb)
            loss = loss_fn(yhat, yb)
            loss.backward()
            optim.step()
            losses.append(float(loss.detach().cpu().item()))

        model.eval()
        with torch.no_grad():
            if Xva.shape[0]:
                yhat_scaled_va = model(_np_to_torch(torch, Xva, device)).detach().cpu().numpy()
                val_loss = float(np.mean((yhat_scaled_va - Yva) ** 2))
                # val MAE in GHz
                yhat_ghz = unscale_y(yhat_scaled_va, y_min, y_max)
                ytrue_ghz = unscale_y(Yva, y_min, y_max)
                val_mae = float(np.mean(np.abs(yhat_ghz - ytrue_ghz)))
            else:
                val_loss = float("nan")
                val_mae = float("nan")

        print(f"epoch {ep:02d}/{int(args.epochs)} train_mse={float(np.mean(losses)):.6f} val_mse={val_loss:.6f} val_mae_ghz={val_mae:.4f}")

    out_dir = os.path.join(dataset_folder, "optkit_train")
    os.makedirs(out_dir, exist_ok=True)

    onnx_fp32_path = os.path.join(out_dir, "model_fp32.onnx")
    onnx_path = os.path.join(out_dir, "model.onnx")
    meta_path = os.path.join(out_dir, "meta.json")

    # Export ONNX (GHz outputs)
    # Note: export wrapper outputs GHz while base model trains on scaled targets.
    model_cpu = model.to("cpu").eval()
    export_model = GRUFreqModelExport(model_cpu).eval()
    dummy = torch.zeros((1, window, X.shape[2]), dtype=torch.float32)
    try:
        torch.onnx.export(
            export_model,
            dummy,
            onnx_fp32_path,
            input_names=["x"],
            output_names=["y"],
            opset_version=int(args.opset),
            dynamic_axes={"x": {1: "seq_len"}},
            dynamo=False,
        )
    except Exception as e:
        print("ERROR: ONNX export failed. Ensure `onnx` is installed.", file=sys.stderr)
        print("Install with:", file=sys.stderr)
        print("  python3 -m pip install -r /home/rt7/Desktop/OPTKIT/tools/optkit-cli/requirements-train.txt", file=sys.stderr)
        print(f"Export error: {e}", file=sys.stderr)
        return 3

    # Quantize to INT8 (model.onnx)
    try:
        quantize_dynamic, QuantType = _require_onnxruntime_quant()
        quantize_dynamic(
            model_input=onnx_fp32_path,
            model_output=onnx_path,
            weight_type=QuantType.QInt8,
        )
    except Exception as e:
        print("ERROR: INT8 quantization failed.", file=sys.stderr)
        print(f"Quantization error: {e}", file=sys.stderr)
        return 3

    fp32_bytes = int(os.path.getsize(onnx_fp32_path))
    int8_bytes = int(os.path.getsize(onnx_path))
    print(f"model_fp32.onnx size: {fp32_bytes} bytes ({fp32_bytes/1024.0:.1f} KiB)")
    print(f"model.onnx (INT8) size: {int8_bytes} bytes ({int8_bytes/1024.0:.1f} KiB)")

    # L1-cache-relevant: persistent tensor bytes (initializers) by dtype.
    _print_onnx_initializer_memory(onnx_fp32_path, label="fp32")
    _print_onnx_initializer_memory(onnx_path, label="int8")

    meta = {
        "dataset_folder": dataset_folder,
        "num_runs": len(arrays),
        "num_features": len(feature_names),
        "min_seq_len": int(min_len),
        "window": int(window),
        "targets": target_names,
        "feature_names": feature_names,
        "target_range": {
            "core_min": tr.core_min,
            "core_max": tr.core_max,
            "uncore_min": tr.uncore_min,
            "uncore_max": tr.uncore_max,
        },
        "normalization": {
            "x_mean": x_mean.tolist(),
            "x_std": x_std.tolist(),
            "y_min": y_min.tolist(),
            "y_max": y_max.tolist(),
        },
        "artifacts": {
            "model_onnx": onnx_path,
            "model_onnx_fp32": onnx_fp32_path,
            "meta_json": meta_path,
        },
        "trainer": {
            "model": "gru",
            "epochs": int(args.epochs),
            "batch_size": int(args.batch_size),
            "lr": float(args.lr),
            "hidden_size": int(args.hidden_size),
            "num_layers": int(args.num_layers),
            "opset": int(args.opset),
        },
    }
    with open(meta_path, "w", encoding="utf-8") as f:
        json.dump(meta, f, indent=2)

    print(f"saved: {onnx_path}")
    print(f"saved_fp32: {onnx_fp32_path}")
    print(f"meta:  {meta_path}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
