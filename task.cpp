#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        // Master process
        std::string message = "HELLOWORLDFROMMPI"; // A longer string to see the effect
        std::cout << "Original Message: " << message << std::endl;
        std::cout << "Total processes (world size): " << world_size << std::endl;

        int msg_len = message.length();
        int part_size = msg_len / world_size;
        
        // Send each worker its part of the string
        for (int i = 1; i < world_size; ++i) {
            int start_index = i * part_size;
            // The last worker gets any remaining characters
            int current_part_size = (i == world_size - 1) ? (msg_len - start_index) : part_size;
            std::string part = message.substr(start_index, current_part_size);
            MPI_Send(part.c_str(), part.size() + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        // Master processes its own part
        std::string master_part = message.substr(0, part_size);
        std::reverse(master_part.begin(), master_part.end());

        // A vector to store results in order
        std::vector<std::string> results(world_size);
        results[0] = master_part;

        // Receive reversed parts from workers
        for (int i = 1; i < world_size; ++i) {
            char buffer[256];
            MPI_Recv(buffer, 256, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            results[i] = std::string(buffer);
        }

        // Combine and print the final result
        std::string final_result = "";
        for (int i = 0; i < world_size; ++i) {
            final_result += results[i];
        }
        std::cout << "Final Result: " << final_result << std::endl;

    } else {
        // Worker processes
        char buffer[256];
        MPI_Recv(buffer, 256, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::string part(buffer);
        std::reverse(part.begin(), part.end());
        MPI_Send(part.c_str(), part.size() + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}