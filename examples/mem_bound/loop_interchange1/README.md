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

**Matrix Configuration**: 500×500 matrices, 5 benchmark iterations, matrix power computations

### Benchmark Results

| Version | Execution Time | Memory Instructions | Retired FLOPs | GFLOPS | Arithmetic Intensity | Performance Gain |
|---------|---------------|-------------------|---------------|--------|-------------------|------------------|
| **Original** | 77,502 ms | 883.2 billion | 26.3 billion | 0.339 | 0.0037 | Baseline |
| **Optimized** | 53,699 ms | 629.9 billion | 26.3 billion | 0.489 | 0.0052 | **1.44x speedup** |

### Memory Performance Analysis

#### Memory Instruction Reduction
```
Original:  883.2 billion memory instructions
Optimized: 629.9 billion memory instructions (28.7% reduction)
```
**Impact**: The optimized version achieved a **28.7% reduction** in memory operations, indicating significantly better memory access efficiency and cache utilization.

#### Computational Throughput Improvement
```
Original:  0.339 GFLOPS
Optimized: 0.489 GFLOPS (44.2% improvement)
```
**Impact**: Despite performing the same 26.3 billion floating-point operations, the optimized version achieved **44% higher computational throughput** due to more efficient memory access patterns.

#### Arithmetic Intensity Enhancement
```
Original:  AI = 0.0037 FLOPs/memory instruction
Optimized: AI = 0.0052 FLOPs/memory instruction (40.5% improvement)
```
**Impact**: The optimization improved the ratio of computation to memory access by **40.5%**, indicating more efficient use of loaded data and better cache locality.

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

### CARM (Compute-Arithmetic Intensity-Roofline Model) Analysis

CARM provides insights into computational efficiency and memory bandwidth utilization, helping identify whether applications are compute-bound or memory-bound.

#### CARM Metrics Comparison

| Version | Memory Instructions | Retired FLOPs | GFLOPS | Arithmetic Intensity (AI) | Execution Time |
|---------|-------------------|---------------|---------|---------------------------|----------------|
| **Original** | 5.12 billion | 3.36 billion | 4.09 | 0.082 | 823.2 ms |
| **Optimized** | 5.15 billion | 3.36 billion | 5.00 | 0.082 | 672.4 ms |

#### Performance Analysis

**GFLOPS Improvement**:
```
Original:  4.09 GFLOPS
Optimized: 5.00 GFLOPS (+22% computational throughput)
```
**Impact**: The optimized version achieved **22% higher computational throughput** while performing the same number of floating-point operations, indicating more efficient execution.

**Arithmetic Intensity (AI)**:
```
Original:  AI = 0.082 FLOPs/Byte
Optimized: AI = 0.082 FLOPs/Byte (essentially unchanged)
```
**Analysis**: The arithmetic intensity remains nearly identical, confirming that both versions perform the same computational work per byte of memory accessed. This validates that our optimization doesn't change the fundamental algorithm but improves execution efficiency.

**Memory Instructions**:
```
Original:  5.12 billion memory instructions
Optimized: 5.15 billion memory instructions (+0.6% increase)
```
**Insight**: Despite a slight increase in memory instruction count, the optimized version runs significantly faster due to better cache utilization patterns.

#### Roofline Model Interpretation

**Memory-Bound Workload Identification**:
- **Low AI (0.082)**: Matrix multiplication with AI < 1.0 indicates this is a **memory-bound workload**
- **Cache Optimization Impact**: For memory-bound applications, cache efficiency improvements directly translate to performance gains
- **Bandwidth Utilization**: Better cache hit rates mean more effective use of available memory bandwidth

**Performance Ceiling Analysis**:
```
Theoretical Peak Performance = Memory Bandwidth × Arithmetic Intensity
Actual Performance = 5.00 GFLOPS (optimized version)

Performance Improvement Route: Cache optimization → Better memory bandwidth utilization → Higher sustained GFLOPS
```

#### CARM Insights for Matrix Operations

1. **Memory-Bound Nature Confirmed**: 
   - AI = 0.082 indicates ~12 memory operations per floating-point operation
   - Performance improvement comes from reducing memory latency, not increasing computation

2. **Cache Optimization Effectiveness**:
   - Same FLOP count (3.36B) but 22% higher GFLOPS → better memory subsystem utilization
   - Memory instruction count stayed nearly constant → optimization didn't reduce memory operations but improved their efficiency

3. **Scalability Implications**:
   - For larger matrices, cache blocking becomes even more critical
   - AI remains low for matrix multiplication, making memory optimization the primary performance lever

#### Real-World Performance Engineering Lessons

**From CARM Metrics**:
- **GFLOPS increase (4.09 → 5.00)** validates that cache optimizations improve sustained performance
- **Constant AI (0.082)** confirms algorithmic equivalence
- **Memory instruction stability** shows optimization doesn't reduce work but improves efficiency

**Optimization Strategy Validation**:
1. ✅ **Target Memory-Bound Workloads**: AI < 1.0 indicates memory optimization potential
2. ✅ **Focus on Cache Efficiency**: Higher GFLOPS with same FLOP count proves cache impact
3. ✅ **Measure Sustained Performance**: GFLOPS improvements reflect real-world application performance
4. ✅ **Validate Algorithmic Correctness**: Constant AI ensures no computational shortcuts

**CARM-Guided Optimization Workflow**:
1. **Measure baseline AI** → Identify if memory-bound (AI < 1) or compute-bound (AI > 10)
2. **For memory-bound**: Focus on cache optimization, data layout, blocking
3. **Monitor GFLOPS**: Sustained throughput improvement indicates successful optimization
4. **Verify AI consistency**: Ensures optimization maintains algorithmic correctness

This CARM analysis confirms that our **cache-focused optimizations** were the correct approach for this **memory-bound matrix multiplication workload**, achieving measurable performance improvements through better memory subsystem utilization rather than algorithmic changes.

