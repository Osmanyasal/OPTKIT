import os
import re
from collections import defaultdict

def get_system_topology():
    """Return dict: {(socket, core): [cpu_ids]}"""
    topology = defaultdict(list)
    for cpu_dir in os.listdir('/sys/devices/system/cpu'):
        if not re.match(r'cpu\d+', cpu_dir):
            continue
        cpu_num = int(cpu_dir[3:])
        try:
            with open(f'/sys/devices/system/cpu/{cpu_dir}/topology/physical_package_id') as f:
                socket = int(f.read().strip())
            with open(f'/sys/devices/system/cpu/{cpu_dir}/topology/core_id') as f:
                core = int(f.read().strip())
            topology[(socket, core)].append(cpu_num)
        except Exception:
            continue
    return topology

def get_cache_hierarchy():
    """
    Returns:
      dict of
        socket_id -> level -> cache_type -> list of cache instances dict:
          {
            'size_kb': int,
            'shared_cpus': str,
            'source': str,
          }
    Deduplicates caches per socket, level, type, shared_cpus.
    """
    cache_hierarchy = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))
    seen = set()

    for cpu_dir in os.listdir('/sys/devices/system/cpu'):
        if not re.match(r'cpu\d+', cpu_dir):
            continue
        cpu_num = int(cpu_dir[3:])
        try:
            with open(f'/sys/devices/system/cpu/{cpu_dir}/topology/physical_package_id') as f:
                socket = int(f.read().strip())
        except Exception:
            continue

        cache_path = f'/sys/devices/system/cpu/{cpu_dir}/cache'
        if not os.path.isdir(cache_path):
            continue

        for index_dir in os.listdir(cache_path):
            if not re.match(r'index\d+', index_dir):
                continue
            base_path = f'{cache_path}/{index_dir}'
            try:
                with open(f'{base_path}/level') as f:
                    level = int(f.read().strip())
                with open(f'{base_path}/type') as f:
                    cache_type = f.read().strip()
                with open(f'{base_path}/size') as f:
                    size = f.read().strip()
                with open(f'{base_path}/shared_cpu_list') as f:
                    shared_cpus = f.read().strip()
                    # if '-' in shared_cpus:
                        # shared_cpus = ','.join(str(i) for i in range(int(shared_cpus.split('-')[0]), int(shared_cpus.split('-')[1]) + 1))
                
                # Size conversion to KiB int
                if 'K' in size:
                    size_kb = int(size.replace('K', ''))
                elif 'M' in size:
                    size_kb = int(float(size.replace('M', '')) * 1024)
                else:
                    size_kb = int(size) // 1024 if int(size) > 1024 else 1

                # Deduplicate per socket, level, type, shared_cpus
                key = (socket, level, cache_type, shared_cpus)
                if key in seen:
                    continue
                seen.add(key)

                cache_hierarchy[socket][level][cache_type].append({
                    'size_kb': size_kb,
                    'shared_cpus': shared_cpus,
                    'source': f'{cpu_dir}/{index_dir}'
                })
            except Exception:
                continue
    return cache_hierarchy

def read_cache_attribute(base_path, attribute):
    try:
        with open(os.path.join(base_path, attribute)) as f:
            return f.read().strip()
    except Exception:
        return None

def make_box_lines(content, width=9):
    top = "+" + "-"*(width-2) + "+"
    mid = "|" + content.center(width-2) + "|"
    bot = top
    return [top, mid, bot]

def join_vertical(boxes):
    joined = []
    for box in boxes:
        joined.extend(box)
    return joined

def pad_lines(lines, height):
    current = len(lines)
    if current < height:
        pad = [" " * len(lines[0])] * (height - current)
        return lines + pad
    return lines

def make_cache_box(size, box_w, height):
    top = "+" + "-"*(box_w-2) + "+"
    mid = "|" + size.center(box_w-2) + "|"
    empty_line = "|" + " "*(box_w-2) + "|"
    
    box = [top]
    mid_index = height // 2
    for i in range(1, height-1):
        box.append(mid if i == mid_index else empty_line)
    box.append(top)
    
    return box

def draw_dynamic_socket_layout(socket_id, core_groups, cache_hierarchy):
    num_cores = len(core_groups)
    cache_levels = sorted(cache_hierarchy.keys())

    core_boxes = []
    core_label_width = max(len(' '.join(map(str, core))) for core in core_groups) + 2
    core_box_w = max(core_label_width, 9)
    for core in core_groups:
        label = ' '.join(map(str, sorted(core)))
        core_boxes.append(make_box_lines(label, core_box_w))
    core_column = join_vertical(core_boxes)

    # Prepare cache columns and headers
    cache_columns = []
    column_widths = [core_box_w]
    headers = [" Core(s) ".center(core_box_w)]

    for level in cache_levels:
        level_types = cache_hierarchy[level]

        if 'Unified' in level_types:
            size_str = f"{level_types['Unified'][0]['size_kb']}K"
        elif 'Data' in level_types and 'Instruction' in level_types:
            d_size = level_types['Data'][0]['size_kb']
            i_size = level_types['Instruction'][0]['size_kb']
            size_str = f"d:{d_size}K+i:{i_size}K"
        else:
            first_type = next(iter(level_types))
            size_str = f"{level_types[first_type][0]['size_kb']}K"

        # Compute box width dynamically
        box_w = max(len(size_str) + 2, 9)
        column_widths.append(box_w)
        headers.append(f" L{level} Cache ".center(box_w))

        if level == max(cache_levels):
            cache_box = make_cache_box(size_str, box_w, num_cores * 3)
            cache_columns.append(cache_box)
        else:
            cache_boxes = [make_box_lines(size_str, box_w) for _ in core_groups]
            cache_columns.append(join_vertical(cache_boxes))

    all_columns = [core_column] + cache_columns
    max_height = max(len(col) for col in all_columns)
    padded_columns = [pad_lines(col, max_height) for col in all_columns]

    print(f"\nSocket {socket_id}:")
    print(" ".join(h for h in headers))
    print()
    for i in range(max_height):
        print(" ".join(padded_columns[j][i].ljust(column_widths[j]) for j in range(len(padded_columns))))

def print_cache_topology_for_socket_compact(socket_id, socket_caches):
    print("\n" + "*" * 80)
    print(f"Cache Topology Summary for Socket {socket_id}")
    print("*" * 80)
    
    # We'll group instances by cache properties (level, type, associativity, sets, line_size, inclusive)
    groups = defaultdict(lambda: {'shared_cpus_list': [], 'shared_threads': 0, 'count': 0})

    for level in sorted(socket_caches.keys()):
        for cache_type, instances in socket_caches[level].items():
            for inst in instances:
                base_path = f"/sys/devices/system/cpu/{inst['source'].split('/')[0]}/cache/{inst['source'].split('/')[1]}"
                associativity = read_cache_attribute(base_path, "ways_of_associativity") or "N/A"
                number_of_sets = read_cache_attribute(base_path, "number_of_sets") or "N/A"
                line_size = read_cache_attribute(base_path, "coherency_line_size") or "N/A"
                inclusive_flag = read_cache_attribute(base_path, "inclusive")
                cache_type_desc = "Inclusive" if inclusive_flag == "1" else "Non Inclusive"
                shared_cpus = inst['shared_cpus']
                
                # Count CPUs sharing cache
                cpus_set = set()
                parts = shared_cpus.split(',')
                for part in parts:
                    if '-' in part:
                        start, end = map(int, part.split('-'))
                        cpus_set.update(range(start, end + 1))
                    else:
                        cpus_set.add(int(part))
                shared_threads = len(cpus_set)

                size_kb = inst['size_kb']
                size_str = f"{size_kb // 1024} MB" if size_kb >= 1024 else f"{size_kb} kB"

                key = (level, cache_type, associativity, number_of_sets, line_size, cache_type_desc, size_str, shared_threads)
                groups[key]['shared_cpus_list'].append(",".join(str(i) for i in cpus_set))
                groups[key]['shared_threads'] = shared_threads
                groups[key]['count'] += 1

    # Now print grouped info
    for key, data in sorted(groups.items()):
        level, cache_type, associativity, number_of_sets, line_size, cache_type_desc, size_str, shared_threads = key
        shared_cpus_group_str = " ".join("(" + s + ")" for s in data['shared_cpus_list'])

        print(f"Level:\t\t\t{level}")
        print(f"Size:\t\t\t{size_str}")
        print(f"Type:\t\t\t{cache_type} cache")
        print(f"Associativity:\t\t{associativity}")
        print(f"Number of sets:\t\t{number_of_sets}")
        print(f"Cache line size:\t{line_size}")
        print(f"Cache type:\t\t{cache_type_desc}")
        print(f"Shared by threads:\t{shared_threads}")
        print(f"Cache groups:\t\t{shared_cpus_group_str}")
        print("-" * 80)

def print_cache_summary(cache_hierarchy):
    print("\nCache Information (sum of all sockets):")
    print("=" * 60)
    print("{:<8} {:<15} {:<15} {:<12} {:<15}".format(
        "Level", "Type", "Unit Size", "Instances", "Total Size"
    ))
    print("-" * 60)

    summary = defaultdict(lambda: defaultdict(lambda: [0, 0]))  # level -> type -> [unit_kb, count]

    for socket in cache_hierarchy:
        for level in cache_hierarchy[socket]:
            for cache_type, instances in cache_hierarchy[socket][level].items():
                unit_kb = instances[0]['size_kb']
                count = len(instances)
                summary[level][cache_type][0] = unit_kb
                summary[level][cache_type][1] += count

    for level in sorted(summary.keys()):
        for cache_type, (unit_kb, count) in summary[level].items():
            total_kb = unit_kb * count
            print("{:<8} {:<15} {:<15} {:<12} {:<15}".format(
                f"L{level}{cache_type[0].lower()}",
                cache_type,
                f"{unit_kb} KiB",
                f"({count})",
                f"{total_kb} KiB"
            ))

    # Print detailed info per socket
    for socket in sorted(cache_hierarchy.keys()):
        print_cache_topology_for_socket_compact(socket, cache_hierarchy[socket])

def main():
    print("********************************************************************************")
    print("CPU Topology and Cache Information")
    print("********************************************************************************")

    topology = get_system_topology()
    cache_hierarchy = get_cache_hierarchy()

    # Group cores by socket and sort cpus per core
    sockets = defaultdict(dict)
    for (socket, core), cpus in topology.items():
        sockets[socket][core] = sorted(cpus)

    # For each socket, prepare core groups for layout (each core's cpus)
    for socket, cores in sorted(sockets.items()):
        core_groups = [cpus for core_id, cpus in sorted(cores.items())]
        draw_dynamic_socket_layout(socket, core_groups, cache_hierarchy[socket])

    print_cache_summary(cache_hierarchy)

if __name__ == "__main__":
    main()
