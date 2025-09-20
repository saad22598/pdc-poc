#include <mpi.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <iomanip>

// Student: Syed Saad Mohsin
// ID: 221-1601
// PDC Assignment 1

using namespace std;

int find_minimum(int x, int y, int z) {
    return min(x, min(y, z));
}

int calculate_sequence_distance(const string& str1, const string& str2) {
    int len1 = str1.length();
    int len2 = str2.length();
    
    vector<vector<int>> distance_matrix(len1 + 1, vector<int>(len2 + 1));
    
    // Set up initial conditions
    for (int i = 0; i <= len1; i++) {
        distance_matrix[i][0] = i;
    }
    for (int j = 0; j <= len2; j++) {
        distance_matrix[0][j] = j;
    }
    
    // Build the distance matrix
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (str1[i-1] == str2[j-1]) {
                distance_matrix[i][j] = distance_matrix[i-1][j-1];
            } else {
                distance_matrix[i][j] = find_minimum(
                    distance_matrix[i-1][j] + 1,      // remove character
                    distance_matrix[i][j-1] + 1,      // add character
                    distance_matrix[i-1][j-1] + 1     // replace character
                );
            }
        }
    }
    
    return distance_matrix[len1][len2];
}

int point_to_point_distance(const string& str1, const string& str2, int process_id, int total_processes) {
    double begin_time, finish_time;
    
    if (process_id == 0) {
        begin_time = MPI_Wtime();
    }
    
    int local_result;
    
    if (total_processes == 1) {
        local_result = calculate_sequence_distance(str1, str2);
    } else {
        // Each process calculates independently, then collects results
        local_result = calculate_sequence_distance(str1, str2);
        
        // Collect results using point-to-point messaging
        if (process_id == 0) {
            vector<int> collected_results(total_processes);
            collected_results[0] = local_result;
            
            // Get results from other processes
            for (int proc = 1; proc < total_processes; proc++) {
                MPI_Recv(&collected_results[proc], 1, MPI_INT, proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            
            local_result = collected_results[0]; // Use primary result
        } else {
            // Send result to process 0
            MPI_Send(&local_result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    
    if (process_id == 0) {
        finish_time = MPI_Wtime();
        cout << "Sequence Distance (P2P) = " << local_result << ", Duration = " << fixed << setprecision(6) << (finish_time - begin_time) << "s" << endl;
    }
    
    return local_result;
}

int scatter_collect_distance(const string& str1, const string& str2, int process_id, int total_processes) {
    double begin_time, finish_time;
    
    if (process_id == 0) {
        begin_time = MPI_Wtime();
    }
    
    int local_result;
    
    if (total_processes == 1) {
        local_result = calculate_sequence_distance(str1, str2);
    } else {
        // Each process calculates the complete matrix independently
        local_result = calculate_sequence_distance(str1, str2);
        
        // Collect results (should be identical)
        vector<int> collected_results(total_processes);
        MPI_Gather(&local_result, 1, MPI_INT, collected_results.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        if (process_id == 0) {
            local_result = collected_results[0]; // Use primary result
        }
    }
    
    if (process_id == 0) {
        finish_time = MPI_Wtime();
        cout << "Sequence Distance (Scatter) = " << local_result << ", Duration = " << fixed << setprecision(6) << (finish_time - begin_time) << "s" << endl;
    }
    
    return local_result;
}

void collect_multiple_pairs(const vector<pair<string,string>>& pair_list, int process_id, int total_processes, vector<int>& final_results) {
    double begin_time, finish_time;
    
    if (process_id == 0) {
        begin_time = MPI_Wtime();
    }
    
    // Distribute pairs among processes
    int pairs_per_process = max(1, (int)pair_list.size() / max(1, total_processes));
    int start_index = process_id * pairs_per_process;
    int end_index = min((int)pair_list.size(), (process_id + 1) * pairs_per_process);
    
    vector<int> process_results;
    
    // Calculate assigned pairs
    for (int i = start_index; i < end_index; i++) {
        int distance = calculate_sequence_distance(pair_list[i].first, pair_list[i].second);
        process_results.push_back(distance);
    }
    
    // Collect results using MPI_Gatherv
    vector<int> all_results;
    vector<int> all_counts(total_processes);
    vector<int> all_offsets(total_processes);
    
    // First collect the counts
    int local_count = process_results.size();
    MPI_Gather(&local_count, 1, MPI_INT, all_counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (process_id == 0) {
        // Calculate offsets
        int total_count = 0;
        for (int i = 0; i < total_processes; i++) {
            all_offsets[i] = total_count;
            total_count += all_counts[i];
        }
        all_results.resize(total_count);
    }
    
    // Collect the actual data
    MPI_Gatherv(process_results.data(), local_count, MPI_INT,
                all_results.data(), all_counts.data(), all_offsets.data(), MPI_INT,
                0, MPI_COMM_WORLD);
    
    if (process_id == 0) {
        final_results = all_results;
        finish_time = MPI_Wtime();
        cout << "Sequence Distance (Collect) = [";
        for (size_t i = 0; i < final_results.size(); i++) {
            cout << final_results[i];
            if (i < final_results.size() - 1) cout << ", ";
        }
        cout << "], Duration = " << fixed << setprecision(6) << (finish_time - begin_time) << "s" << endl;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int process_id, total_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    
    string str1, str2;
    
    // Get input from command line arguments or use defaults
    if (argc >= 3) {
        str1 = argv[1];
        str2 = argv[2];
    } else if (argc == 2 && string(argv[1]) == "-i") {
        // Interactive input mode (only for single process)
        if (process_id == 0 && total_processes == 1) {
            cout << "Enter Sequence 1: ";
            getline(cin, str1);
            cout << "Enter Sequence 2: ";
            getline(cin, str2);
        }
    } else {
        // Default sequences if no arguments provided
        str1 = "ACGTAG";
        str2 = "ACTG";
    }
    
    vector<pair<string,string>> pair_list = {
        {str1, str2},
        {"CGTAG", "ATG"},
        {"AGTAG", "ACT"}
    };
    
    // Add longer sequences for better timing measurements
    if (str1.length() < 10) {
        pair_list.push_back({"ACGTAGCTAGCTAGCTAG", "ACTGCTAGCTAGCTAG"});
        pair_list.push_back({"HELLOWORLD", "WORLDHELLO"});
    }
    
    // Display input sequences
    if (process_id == 0) {
        cout << "Sequence 1: " << str1 << endl;
        cout << "Sequence 2: " << str2 << endl;
        cout << endl;
    }
    
    // Execute all three scenarios
    point_to_point_distance(str1, str2, process_id, total_processes);
    scatter_collect_distance(str1, str2, process_id, total_processes);
    
    vector<int> collect_results;
    collect_multiple_pairs(pair_list, process_id, total_processes, collect_results);
    
    MPI_Finalize();
    return 0;
}
