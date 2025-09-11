#include <iostream>
#include <vector>
#include <mpi.h>
#include <unistd.h> // Required for gethostname() and getpid()
#include <cstdlib>  // Required for getenv()
#include <string>   // Required for std::string

// Roll numbers: 22i-1601, 22i-1554, 22i-1689
// Question 2

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char container_id[256];
    gethostname(container_id, sizeof(container_id));
    const char* friendly_name = getenv("FRIENDLY_NAME");
    if (friendly_name == nullptr) { friendly_name = "unknown"; }
    pid_t pid = getpid();
    int num_workers = world_size - 1;

    if (world_rank == 0) {
        // --- MASTER PROCESS ---

        // Q2. Master has an array {2,3,4,5}.
        int data[] = {2, 3, 4, 5};
        int data_count = sizeof(data) / sizeof(int);

        if (data_count != num_workers) {
            std::cerr << "Error: This task requires exactly 4 workers (" << num_workers 
                      << " found). Please run with -np 5." << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        std::cout << "Master on " << friendly_name << " (ID: " << container_id << ", PID: " << pid 
                  << ", rank: 0): distributing array {2,3,4,5} to workers." << std::endl;

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
                  << ", rank: 0): gathered results {";
        for (int i = 0; i < results.size(); ++i) {
            std::cout << results[i] << (i == results.size() - 1 ? "" : ",");
        }
        std::cout << "}" << std::endl;

    } else {
        // --- WORKER PROCESSES ---
        int received_number;
        MPI_Recv(&received_number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int computed_result = 0;
        std::string operation_string;

        // Q2. Different workers perform different computations based on their rank.
        if (world_rank == 1) {
            computed_result = received_number * received_number;
            operation_string = "squared ->";
        } else if (world_rank == 2) {
            computed_result = received_number * received_number * received_number;
            operation_string = "cubed ->";
        } else if (world_rank == 3) {
            computed_result = received_number * 2;
            operation_string = "doubled ->";
        } else if (world_rank == 4) {
            computed_result = received_number - 1;
            operation_string = "subtracted 1 ->";
        }

        std::cout << "Worker on " << friendly_name << " (ID: " << container_id << ", PID: " << pid 
                  << ", rank: " << world_rank << "): received " << received_number 
                  << ", " << operation_string << " " << computed_result << std::endl;
        
        MPI_Send(&computed_result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}