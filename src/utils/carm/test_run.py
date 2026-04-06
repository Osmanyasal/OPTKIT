import os
import tempfile
import unittest

import run


class CacheDetectionTests(unittest.TestCase):
    def write_cache_entry(self, root, index_name, level, cache_type, size):
        index_path = os.path.join(root, index_name)
        os.makedirs(index_path, exist_ok=True)

        with open(os.path.join(index_path, "level"), "w", encoding="utf-8") as level_file:
            level_file.write(str(level))
        with open(os.path.join(index_path, "type"), "w", encoding="utf-8") as type_file:
            type_file.write(cache_type)
        with open(os.path.join(index_path, "size"), "w", encoding="utf-8") as size_file:
            size_file.write(size)

    def test_parse_cache_size_kib(self):
        self.assertEqual(run.parse_cache_size_kib("64K"), 64)
        self.assertEqual(run.parse_cache_size_kib("1M"), 1024)
        self.assertEqual(run.parse_cache_size_kib("2GiB"), 2 * 1024 * 1024)
        self.assertEqual(run.parse_cache_size_kib("unknown"), 0)

    def test_detect_linux_cache_sizes_uses_data_l1_and_largest_shared_levels(self):
        with tempfile.TemporaryDirectory() as cache_root:
            self.write_cache_entry(cache_root, "index0", 1, "Instruction", "64K")
            self.write_cache_entry(cache_root, "index1", 1, "Data", "64K")
            self.write_cache_entry(cache_root, "index2", 2, "Unified", "1024K")
            self.write_cache_entry(cache_root, "index3", 3, "Unified", "114688K")

            l1_size, l2_size, l3_size = run.detect_linux_cache_sizes(cache_root)

            self.assertEqual((l1_size, l2_size, l3_size), (64, 1024, 114688))


if __name__ == "__main__":
    unittest.main()