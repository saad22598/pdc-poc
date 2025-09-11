#include <iostream>
#include <vector>
#include <mpi.h>
#include <unistd.h> // Required for gethostname() and getpid()
#include <cstdlib>  // Required for getenv()

// Roll numbers: 22i-1601, 22i-1554, 22i-1689
// Question 1

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv)

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // This will now get the unique Container ID, e.g., "039f5fe4970d"
    char container_id[256];
    gethostname(container_id, sizeof(container_id));

    // This gets the friendly name from the environment variable we set
    const char* friendly_name = getenv("FRIENDLY_NAME");
    if (friendly_name == nullptr) {
        friendly_name = "unknown"; // A fallback in case the variable isn't set
    }

    pid_t pid = getpid();
    int num_workers = world_size - 1;

    if (world_rank == 0) {
        // --- MASTER PROCESS ---
        int data[] = {1, 2, 3, 4};
        // ... (error checking code is the same) ...

        // Updated print statement with all information
        std::cout << "Master on " << friendly_name << " (ID: " << container_id << ", PID: " << pid 
                  << ", rank: 0): sending numbers 1,2,3,4 to workers..." << std::endl;

        for (int i = 0; i < num_workers; ++i) {
            MPI_Send(&data[i], 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }

        std::vector<int> results(num_workers);
        for (int i = 0; i < num_workers; ++i) {
            int received_result;
            MPI_Recv(&received_result, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            results[i] = received_result;
        }

        std::cout << "Master on " << friendly_name << " (ID: " << container_id << ", PID: " << pid 
                  << ", rank: 0): collected results {";
        for (int i = 0; i < results.size(); ++i) {
            std::cout << results[i] << (i == results.size() - 1 ? "" : ",");
        }
        std::cout << "}" << std::endl;

    } else {
        // --- WORKER PROCESSES ---
        int received_number;
        MPI_Recv(&received_number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int computed_result = received_number * 10;
        
        // Updated print statement with all information
        std::cout << "Worker on " << friendly_name << " (ID: " << container_id << ", PID: " << pid 
                  << ", rank: " << world_rank << "): received " << received_number 
                  << ", computed " << computed_result << std::endl;
        
        MPI_Send(&computed_result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}