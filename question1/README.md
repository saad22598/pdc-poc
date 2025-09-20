# MPI Edit Distance Implementation

This project implements the **Edit Distance (Needleman-Wunsch)** algorithm using MPI with three different communication strategies:

- **Point-to-Point Communication** (MPI_Send/MPI_Recv)  
- **Scatter + Gather Communication** (MPI_Scatter/Gather)  
- **Gather-based Multiple Pair Computation** (MPI_Gather)

## Quick Start

### Option 1: Single Container (Recommended - Works Perfectly)

```bash
# Build the project
docker compose build

# Method 1: Use default sequences (ACGTAG vs ACTG)
docker run --rm -it pdc-poc-master bash -c "cd /app && mpirun -np 2 --allow-run-as-root ./edit_distance"

# Method 2: Provide custom sequences as command line arguments
docker run --rm -it pdc-poc-master bash -c "cd /app && mpirun -np 2 --allow-run-as-root ./edit_distance HELLO WORLD"

# Method 3: Interactive input (single process only)
docker run --rm -it pdc-poc-master bash -c "cd /app && mpirun -np 1 --allow-run-as-root ./edit_distance -i"

# Run with different process counts
docker run --rm -it pdc-poc-master bash -c "cd /app && mpirun -np 4 --allow-run-as-root ./edit_distance"
docker run --rm -it pdc-poc-master bash -c "cd /app && mpirun -np 8 --allow-run-as-root ./edit_distance"
```

### Option 2: Multi-Container Setup

```bash
# Build the project
docker compose build

# Run with 2 processes (1 master + 1 worker)
docker compose up --scale worker1=1 worker2=0 worker3=0 worker4=0

# Run with 4 processes (1 master + 3 workers)
docker compose up --scale worker1=1 worker2=1 worker3=1 worker4=0

# Run with 5 processes (1 master + 4 workers)
docker compose up --scale worker1=1 worker2=1 worker3=1 worker4=1

# View logs to see results
docker compose logs

# Shut down containers
docker compose down
```

## Expected Output

```
Sequence 1: ACGTAG
Sequence 2: ACTG

Edit Distance (P2P) = 2, Time = 0.000s
Edit Distance (Scatter) = 2, Time = 0.000s
Edit Distance (Gather) = [2, 3, 3], Time = 0.000s
```

## Input Methods

1. **Default Sequences**: No arguments - uses "ACGTAG" vs "ACTG"
2. **Command Line**: `./edit_distance SEQUENCE1 SEQUENCE2`
3. **Interactive**: `./edit_distance -i` (single process only)

## Troubleshooting

- **SSH errors**: Use Option 1 (single container) for reliable execution
- **Permission errors**: Ensure Docker has proper permissions
- **Build errors**: Check that all files are in the correct directory

## Files

- `edit_distance_mpi.cpp` - Main MPI implementation
- `Dockerfile` - Container configuration
- `docker-compose.yml` - Multi-container orchestration
- `hostfile` - MPI host configuration
- `README.md` - This file
