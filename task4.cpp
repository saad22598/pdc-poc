#include <mpi.h>
#include <cstdio>

int main(int argc, char** argv) {
  int rank;
  int worldSize;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  const int elementsPerProcess = 5;
  int sendBuffer[elementsPerProcess];
  for (int i = 0; i < elementsPerProcess; ++i) {
    sendBuffer[i] = rank;  // Each process fills its buffer with its rank
  }

  int* receiveBuffer = nullptr;
  if (rank == 0) {
    receiveBuffer = new int[elementsPerProcess * worldSize];
  }

  MPI_Gather(
    sendBuffer,                 // send buffer
    elementsPerProcess,         // number of elements sent
    MPI_INT,                    // type of each element sent
    receiveBuffer,              // receive buffer (root only)
    elementsPerProcess,         // number of elements received from each proc
    MPI_INT,                    // type of each element received
    0,                          // root rank
    MPI_COMM_WORLD              // communicator
  );

  if (rank == 0) {
    for (int p = 0; p < worldSize; ++p) {
      for (int i = 0; i < elementsPerProcess; ++i) {
        std::printf("Process %d sent: %d\n", p, receiveBuffer[p * elementsPerProcess + i]);
      }
    }
    delete[] receiveBuffer;
  }

  std::printf("Bye from %d\n", rank);
  MPI_Finalize();
  return 0;
}