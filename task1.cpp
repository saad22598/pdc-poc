#include <iostream>
#include <vector>
#include <mpi.h>
#include <unistd.h> // Required for gethostname() and getpid()

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the hostname of the container/machine the process is running on.
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    // Get the Process ID of this specific MPI process.
    pid_t pid = getpid();

    int num_workers = world_size - 1;

    if (world_rank == 0) {
        // --- MASTER PROCESS ---
        int data[] = {1, 2, 3, 4};
        int data_count = sizeof(data) / sizeof(int);

        if (data_count != num_workers) {
            if (world_rank == 0) { // Only master should print the error
                std::cerr << "Error: The number of workers (" << num_workers 
                          << ") must match the number of data elements (" << data_count << ").\n"
                          << "Please run with -np " << data_count + 1 << " processes." << std::endl;
            }
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Updated print statement to include PID and Hostname
        std::cout << "Master (rank 0, PID " << pid << " on " << hostname << "): sending numbers 1,2,3,4 to workers..." << std::endl;

        for (int i = 0; i < num_workers; ++i) {
            int worker_rank = i + 1;
            MPI_Send(&data[i], 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD);
        }

        std::vector<int> results(num_workers);
        for (int i = 0; i < num_workers; ++i) {
            int worker_rank = i + 1;
            int received_result;
            MPI_Recv(&received_result, 1, MPI_INT, worker_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            results[i] = received_result;
        }

        // Updated print statement
        std::cout << "Master (rank 0, PID " << pid << " on " << hostname << "): collected results {";
        for (int i = 0; i < results.size(); ++i) {
            std::cout << results[i] << (i == results.size() - 1 ? "" : ",");
        }
        std::cout << "}" << std::endl;

    } else {
        // --- WORKER PROCESSES ---
        int received_number;
        MPI_Recv(&received_number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int computed_result = received_number * 10;
        
        // Updated print statement to include PID and Hostname
        std::cout << "Worker (rank " << world_rank << ", PID " << pid << " on " << hostname 
                  << "): received " << received_number 
                  << ", computed " << computed_result << std::endl;
        
        MPI_Send(&computed_result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}