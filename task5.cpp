#include <mpi.h>
#include <cstdio>
#include <vector>

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);

  int worldSize = 0;
  int worldRank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

  // Enforce exactly 5 processes (hard-coded layout below assumes 5 ranks)
  if (worldSize != 5) {
    if (worldRank == 0) {
      std::printf("This program must be run with exactly 5 MPI processes.\n");
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // Hard-coded total elements and source data on root
  const int totalElements = 17;
  std::vector<int> sendBuffer;
  if (worldRank == 0) {
    sendBuffer.resize(totalElements);
    for (int i = 0; i < totalElements; ++i) {
      sendBuffer[i] = i;  // 0..16
    }
  }

  // Hard-coded sendCounts and displacements for 5 ranks and 17 elements
  // Rank 0 -> 4 elems [0..3]
  // Rank 1 -> 4 elems [4..7]
  // Rank 2 -> 3 elems [8..10]
  // Rank 3 -> 3 elems [11..13]
  // Rank 4 -> 3 elems [14..16]
  const int sendCounts[5]     = {4, 4, 3, 3, 3};
  const int displacements[5]  = {0, 4, 8, 11, 14};

  const int myReceiveCount = sendCounts[worldRank];
  std::vector<int> receiveBuffer(myReceiveCount, -1);

  MPI_Scatterv(
    worldRank == 0 ? sendBuffer.data() : nullptr, // sendbuf (root only)
    sendCounts,                                    // sendcounts (root only)
    displacements,                                 // displs (root only)
    MPI_INT,                                       // send type
    receiveBuffer.data(),                          // recvbuf (all ranks)
    myReceiveCount,                                // recvcount for this rank
    MPI_INT,                                       // recv type
    0,                                             // root
    MPI_COMM_WORLD                                 // communicator
  );

  for (int i = 0; i < myReceiveCount; ++i) {
    std::printf("Rank %d received element %d: %d\n", worldRank, i, receiveBuffer[i]);
  }

  std::printf("Bye from %d\n", worldRank);
  MPI_Finalize();
  return 0;
}