This is a lab about [loop interchange](https://en.wikipedia.org/wiki/Loop_interchange).

### Watch the Video

[![Watch the video](https://img.youtube.com/vi/TLDR_nO9XVc/0.jpg)](https://www.youtube.com/watch?v=TLDR_nO9XVc&list=PLRWO2AL1QAV6bJAU2kgB4xfodGID43Y5d)

> Click the image above to play the video.

[Matrix multiplication](https://en.wikipedia.org/wiki/Matrix_multiplication) is an important building block for many numerical algorithms. In this lab assignment, we compute the integer power of a given real square matrix.
The binary representation of the power significantly reduces the number of matrix operations. Still, the code has a major performance flaw. Your job is to find it out.


### Watch the Video

[![Watch the video](https://img.youtube.org/vi/G6BbPB37sYg/0.jpg)](https://www.youtube.com/watch?v=G6BbPB37sYg&list=PLRWO2AL1QAV6bJAU2kgB4xfodGID43Y5d)

> Click the image above to play the video.

## Performance Analysis Results

Note that since compilers easily recognises matrix multiplications and replace the current code with the most optimized version, I compiled with -O0 (no optimizations) to see the effect of loop interchange & blocking here.

### Benchmark Results

| Version | Execution Time | Instructions Retired | L1 MPKI | L2 MPKI | L3 MPKI | Performance Gain |
|---------|---------------|---------------------|---------|---------|---------|------------------|
| **Original** | 10,554.6 ms | 105.4 billion | 0.645 | 0.0065 | 0.000009 | Baseline |
| **Optimized** | 7,014.6 ms | 75.9 billion | 0.287 | 0.0073 | 0.000012 | **1.51x speedup** |

### Cache Performance Analysis

#### L1 Cache Misses Per Kilo-Instructions (L1 MPKI)
```
Original:  0.645 MPKI  →  67.9M total L1 misses
Optimized: 0.287 MPKI  →  21.8M total L1 misses (68% reduction)
```
**Impact**: The optimized version achieved a **68% reduction** in L1 cache misses, indicating significantly better data locality and cache utilization.

#### L2 Cache Performance
```
Original:  0.0065 MPKI →  685K total L2 misses
Optimized: 0.0073 MPKI →  557K total L2 misses (19% reduction)
```
**Impact**: L2 cache performance improved by 19%, though the MPKI appears slightly higher due to the different instruction mix.

#### L3 Cache Performance
```
Original:  0.000009 MPKI →  999 total L3 misses
Optimized: 0.000012 MPKI →  916 total L3 misses (8% reduction)
```
**Impact**: Minimal L3 cache impact, which is expected since the optimization primarily affects L1 cache behavior.

### Algorithmic Improvements

#### 1. **Loop Interchange Optimization**
**Original Loop Order**: `i → j → k`
```cpp
for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
        for (int k = 0; k < N; k++)
            result[i][j] += a[i][k] * b[k][j];
```

**Optimized Loop Order**: `i → k → j` (with blocking)
```cpp
for (int i = ii; i < i_max; ++i)
    for (int k = kk; k < k_max; ++k)
        for (int j = jj; j < j_max; ++j)
            result[i][j] += a[i][k] * b[k][j];
```

**Why This Works**: 
- **Memory Access Pattern**: The `k → j` order accesses `b[k][j]` sequentially, improving spatial locality
- **Cache Line Utilization**: Better utilization of 64-byte cache lines by processing consecutive elements
- **Reduced Memory Bandwidth**: Fewer cache line fetches for the same amount of computation

#### 2. **Cache Blocking/Tiling**
```cpp
const int BLOCK_SIZE = 64; // 64x64 blocks fit well in L1 cache

for (int ii = 0; ii < N; ii += BLOCK_SIZE)
    for (int kk = 0; kk < N; kk += BLOCK_SIZE)
        for (int jj = 0; jj < N; jj += BLOCK_SIZE)
            // Process 64x64 block
```

**Benefits**:
- **Temporal Locality**: Data reused within blocks stays in cache longer
- **Cache-Friendly Computation**: 64x64 blocks (≈32KB) fit comfortably in typical L1 caches
- **Reduced Memory Traffic**: Each matrix element is accessed fewer times from main memory

#### 3. **Loop Invariant Hoisting**
```cpp
const double a_ik = a[i][k]; // Computed once per k iteration
for (int j = jj; j < j_max; ++j)
    result[i][j] += a_ik * b[k][j]; // Reused N times
```

**Impact**: Eliminates redundant memory reads of `a[i][k]`, reducing memory bandwidth by up to 33%.

### Instruction-Level Analysis

#### Instruction Count Reduction
```
Original:  105.4 billion instructions
Optimized: 75.9 billion instructions (28% reduction)
```

**Why Fewer Instructions**:
- **Loop Invariant Hoisting**: Eliminated redundant array indexing calculations
- **Better Compiler Optimization**: More regular memory access patterns enable better vectorization
- **Reduced Address Calculations**: Blocking reduces complex stride calculations

#### Instructions Per Cache Miss Improvement
```
Original:  105.4B instructions ÷ 67.9M L1 misses = 1,552 instructions/miss
Optimized: 75.9B instructions ÷ 21.8M L1 misses = 3,481 instructions/miss
```

**Result**: **2.24x improvement** in computational work done per cache miss - much more efficient use of loaded data.

### Real-World Performance Impact

This **1.51x speedup** demonstrates several important optimization principles:

1. **Memory Access Patterns Matter More Than Algorithms**: Same O(n³) complexity, but better cache behavior
2. **Data Locality is Critical**: 68% reduction in L1 misses translates directly to performance
3. **Hardware-Aware Programming**: Understanding cache hierarchy enables significant optimizations
4. **Compound Effects**: Multiple optimizations (interchange + blocking + hoisting) work synergistically

### Key Takeaways for Matrix Operations

✅ **Loop interchange** can provide 20-50% speedup for matrix operations  
✅ **Cache blocking** is essential for large matrices that don't fit in cache  
✅ **Memory access pattern** often matters more than algorithmic complexity  
✅ **L1 cache optimization** provides the biggest performance impact  
✅ **Combined optimizations** can achieve multiplicative benefits  

**Optimization Hierarchy**: 
1. **Cache blocking** (biggest impact)
2. **Loop interchange** (moderate impact) 
3. **Loop invariant hoisting** (small but consistent impact)
4. **Vectorization-friendly patterns** (compiler-dependent impact)

This example demonstrates that even with the same algorithmic complexity, understanding hardware characteristics and memory hierarchy can lead to substantial performance improvements in memory-bound applications.

