# Problem Statement: MPI Edit Distance Implementation

## Objective
Implement the Edit Distance (Needleman-Wunsch global alignment) algorithm using MPI with three different communication strategies and compare their performance.

## Requirements

### 1. Algorithm Implementation
- Implement the Needleman-Wunsch algorithm for global sequence alignment
- Calculate edit distance between two DNA/protein sequences
- Use dynamic programming approach

### 2. MPI Communication Strategies

#### Strategy 1: Point-to-Point Communication
- Use `MPI_Send` and `MPI_Recv` for data distribution
- Distribute computation across multiple processes
- Collect results using point-to-point communication

#### Strategy 2: Scatter + Gather Communication
- Use `MPI_Scatter` to distribute data
- Use `MPI_Gather` to collect results
- Implement collective communication patterns

#### Strategy 3: Gather-based Multiple Pair Computation
- Process multiple sequence pairs in parallel
- Use `MPI_Gather` to collect results from all processes
- Implement load balancing across processes

### 3. Input/Output Format

**Input:**
```
Sequence 1: ACGTAG
Sequence 2: ACTG
```

**Output:**
```
Edit Distance (P2P) = 2, Time = 0.000s
Edit Distance (Scatter) = 2, Time = 0.000s
Edit Distance (Gather) = [2, 3, 3], Time = 0.000s
```

### 4. Performance Requirements
- Measure execution time for each strategy
- Support different numbers of MPI processes (2, 4, 8)
- Compare performance across communication strategies

### 5. Docker Environment
- Containerize the application
- Support both single-container and multi-container execution
- Provide easy setup and execution instructions

## Expected Results
- Correct edit distance calculations for all strategies
- Performance comparison showing time differences
- Scalable implementation that works with different process counts
- Clean, readable output matching the specified format
