#!/bin/bash

set -euo pipefail

echo "[INFO] Host: $(hostname)"
echo "[INFO] CWD:   $(pwd)"

OPTKIT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
 
## compile OPTKIT and optkit-cli
# cd "$HOME/OPTKIT" && premake5 gmake
# make -j$(nproc) config=release optkit_static
pushd "$OPTKIT_ROOT/tools/optkit-cli" >/dev/null
make -j"$(nproc)"
OPTKIT_CLI="$PWD/optkit-cli"
popd >/dev/null

if [[ ! -x "$OPTKIT_CLI" ]]; then
	echo "[ERROR] optkit-cli not found or not executable: $OPTKIT_CLI" >&2
	exit 1
fi
# echo "[INFO] OPTKIT and optkit-cli compiled successfully"

cd /home/rt7/Desktop/OPTKIT/examples/nas_bench/NPB3.4.3/NPB3.4-OMP/bin

# mkdir bt_D_freq_scaling
# cd bt_D_freq_scaling
# "$OPTKIT_CLI" stat --bench freq-scaling --freq-limit 25 -m topdown_l1 -- ../bt.D.x
# "$OPTKIT_CLI" report */*
# cd ..

# mkdir cg_D_freq_scaling
# cd cg_D_freq_scaling
# "$OPTKIT_CLI" stat --bench freq-scaling --freq-limit 25 -m topdown_l1 -- ../cg.D.x
# "$OPTKIT_CLI" report */*
# cd ..

mkdir ep_D_freq_scaling
cd ep_D_freq_scaling
"$OPTKIT_CLI" stat --bench freq-scaling --freq-limit 25 -m topdown_l1 -- ../ep.D.x
"$OPTKIT_CLI" report */*
cd ..

mkdir ft_D_freq_scaling
cd ft_D_freq_scaling
"$OPTKIT_CLI" stat --bench freq-scaling --freq-limit 25 -m topdown_l1 -- ../ft.D.x
"$OPTKIT_CLI" report */*
cd ..

mkdir lu_D_freq_scaling
cd lu_D_freq_scaling
"$OPTKIT_CLI" stat --bench freq-scaling --freq-limit 25 -m topdown_l1 -- ../lu.D.x
"$OPTKIT_CLI" report */*
cd ..

mkdir mg_D_freq_scaling
cd mg_D_freq_scaling
"$OPTKIT_CLI" stat --bench freq-scaling --freq-limit 25 -m topdown_l1 -- ../mg.D.x
"$OPTKIT_CLI" report */*
cd ..

mkdir sp_D_freq_scaling
cd sp_D_freq_scaling
"$OPTKIT_CLI" stat --bench freq-scaling --freq-limit 25 -m topdown_l1 -- ../sp.D.x
"$OPTKIT_CLI" report */*
cd ..

mkdir ua_D_freq_scaling
cd ua_D_freq_scaling
"$OPTKIT_CLI" stat --bench freq-scaling --freq-limit 25 -m topdown_l1 -- ../ua.D.x
"$OPTKIT_CLI" report */*
cd ..