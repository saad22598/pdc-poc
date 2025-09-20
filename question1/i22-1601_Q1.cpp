#include <mpi.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <iomanip>

// Name: Syed Saad Mohsin
// Roll Number: 221-1601
// Assignment 1 for PDC

using namespace std;

int min3(int a, int b, int c) {
    return min(a, min(b, c));
}

int compute_edit_distance(const string& seq1, const string& seq2) {
    int m = seq1.length();
    int n = seq2.length();
    
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    
    // Initialize base cases
    for (int i = 0; i <= m; i++) {
        dp[i][0] = i;
    }
    for (int j = 0; j <= n; j++) {
        dp[0][j] = j;
    }
    
    // Fill the DP table
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (seq1[i-1] == seq2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = min3(
                    dp[i-1][j] + 1,      // deletion
                    dp[i][j-1] + 1,      // insertion
                    dp[i-1][j-1] + 1     // substitution
                );
            }
        }
    }
    
    return dp[m][n];
}

int p2p_edit_distance(const string& seq1, const string& seq2, int rank, int size) {
    double start_time, end_time;
    
    if (rank == 0) {
        start_time = MPI_Wtime();
    }
    
    int result_p2p;
    
    if (size == 1) {
        result_p2p = compute_edit_distance(seq1, seq2);
    } else {
        // Simplified P2P: Each process computes independently, then gather results
        result_p2p = compute_edit_distance(seq1, seq2);
        
        // Gather results using point-to-point communication
        if (rank == 0) {
            vector<int> all_results(size);
            all_results[0] = result_p2p;
            
            // Receive results from other processes
            for (int proc = 1; proc < size; proc++) {
                MPI_Recv(&all_results[proc], 1, MPI_INT, proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            
            result_p2p = all_results[0]; // Use first result
        } else {
            // Send result to process 0
            MPI_Send(&result_p2p, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    
    if (rank == 0) {
        end_time = MPI_Wtime();
        cout << "Edit Distance (P2P) = " << result_p2p << ", Time = " << fixed << setprecision(6) << (end_time - start_time) << "s" << endl;
    }
    
    return result_p2p;
}

int scatter_gather_edit_distance(const string& seq1, const string& seq2, int rank, int size) {
    double start_time, end_time;
    
    if (rank == 0) {
        start_time = MPI_Wtime();
    }
    
    int result_scatter;
    
    if (size == 1) {
        result_scatter = compute_edit_distance(seq1, seq2);
    } else {
        // Each process computes the full matrix independently
        result_scatter = compute_edit_distance(seq1, seq2);
        
        // Gather results (though they should be the same)
        vector<int> all_results(size);
        MPI_Gather(&result_scatter, 1, MPI_INT, all_results.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        if (rank == 0) {
            result_scatter = all_results[0]; // Use first result
        }
    }
    
    if (rank == 0) {
        end_time = MPI_Wtime();
        cout << "Edit Distance (Scatter) = " << result_scatter << ", Time = " << fixed << setprecision(6) << (end_time - start_time) << "s" << endl;
    }
    
    return result_scatter;
}

void gather_multiple_pairs(const vector<pair<string,string>>& sequence_pairs, int rank, int size, vector<int>& results) {
    double start_time, end_time;
    
    if (rank == 0) {
        start_time = MPI_Wtime();
    }
    
    // Distribute pairs among processes
    int pairs_per_proc = max(1, (int)sequence_pairs.size() / max(1, size));
    int start_idx = rank * pairs_per_proc;
    int end_idx = min((int)sequence_pairs.size(), (rank + 1) * pairs_per_proc);
    
    vector<int> local_results;
    
    // Compute assigned pairs
    for (int i = start_idx; i < end_idx; i++) {
        int dist = compute_edit_distance(sequence_pairs[i].first, sequence_pairs[i].second);
        local_results.push_back(dist);
    }
    
    // Gather results using MPI_Gatherv
    vector<int> all_results;
    vector<int> all_sizes(size);
    vector<int> all_displs(size);
    
    // First gather the sizes
    int local_size = local_results.size();
    MPI_Gather(&local_size, 1, MPI_INT, all_sizes.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        // Calculate displacements
        int total_size = 0;
        for (int i = 0; i < size; i++) {
            all_displs[i] = total_size;
            total_size += all_sizes[i];
        }
        all_results.resize(total_size);
    }
    
    // Gather the actual data
    MPI_Gatherv(local_results.data(), local_size, MPI_INT,
                all_results.data(), all_sizes.data(), all_displs.data(), MPI_INT,
                0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        results = all_results;
        end_time = MPI_Wtime();
        cout << "Edit Distance (Gather) = [";
        for (size_t i = 0; i < results.size(); i++) {
            cout << results[i];
            if (i < results.size() - 1) cout << ", ";
        }
        cout << "], Time = " << fixed << setprecision(6) << (end_time - start_time) << "s" << endl;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    string seq1, seq2;
    
    // Get input from command line arguments or use defaults
    if (argc >= 3) {
        seq1 = argv[1];
        seq2 = argv[2];
    } else if (argc == 2 && string(argv[1]) == "-i") {
        // Interactive input mode (only for single process)
        if (rank == 0 && size == 1) {
            cout << "Enter Sequence 1: ";
            getline(cin, seq1);
            cout << "Enter Sequence 2: ";
            getline(cin, seq2);
        }
    } else {
        // Default sequences if no arguments provided
        seq1 = "ACGTAG";
        seq2 = "ACTG";
    }
    
    vector<pair<string,string>> sequence_pairs = {
        {seq1, seq2},
        {"CGTAG", "ATG"},
        {"AGTAG", "ACT"}
    };
    
    // Add longer sequences for better timing measurements
    if (seq1.length() < 10) {
        sequence_pairs.push_back({"ACGTAGCTAGCTAGCTAG", "ACTGCTAGCTAGCTAG"});
        sequence_pairs.push_back({"HELLOWORLD", "WORLDHELLO"});
    }
    
    // Display input sequences
    if (rank == 0) {
        cout << "Sequence 1: " << seq1 << endl;
        cout << "Sequence 2: " << seq2 << endl;
        cout << endl;
    }
    
    // Run all three scenarios
    p2p_edit_distance(seq1, seq2, rank, size);
    scatter_gather_edit_distance(seq1, seq2, rank, size);
    
    vector<int> gather_results;
    gather_multiple_pairs(sequence_pairs, rank, size, gather_results);
    
    MPI_Finalize();
    return 0;
}
