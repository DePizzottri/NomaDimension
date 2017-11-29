#ifndef NOMA_DIMENSION_FLOYD_INCLUDED
#define NOMA_DIMENSION_FLOYD_INCLUDED

#include <cstring>
#include <limits>

#include <process_graph.hpp>

using namespace std;

const auto INF = numeric_limits<int>::max() / 3;

typedef int* adjacency_matrix;

//void floyd(int* matrix, int n, int threads = 4) {
inline void floyd(adjacency_matrix matrix, int n) {
    for (int k = 0; k < n; ++k) {
#pragma omp parallel for num_threads(4)
        for (int i = 0; i < n; ++i) {
            auto v = matrix[i*n + k];
            for (int j = 0; j < n; ++j) {
                auto val = v + matrix[k*n + j];
                if (matrix[i*n + j] > val) {
                    matrix[i*n + j] = val;
                }
            }
        }
    }
}

//void floyd_advance_vertex(int* matrix, int n, int v, int threads = 4) {
inline void floyd_advance_vertex(adjacency_matrix matrix, int n, int v) {
    auto k = v;
    //for (int k = 0; k < n; ++k) {
#pragma omp parallel for num_threads(4)
    for (int i = 0; i < n; ++i) {
        auto v = matrix[i*n + k];
        for (int j = 0; j < n; ++j) {
            auto val = v + matrix[k*n + j];
            if (matrix[i*n + j] > val) {
                matrix[i*n + j] = val;
            }
        }
    }
    //}
}

//void print(adjacency_matrix matrix, int n) {
//    for (int v = 0; v < n; ++v) {
//        for (int u = 0; u < n; ++u) {
//            if (matrix[v*n + u] == INF) {
//                cout << "I ";
//            }
//            else {
//                cout << matrix[v*n + u] << " ";
//            }
//        }
//        cout << endl;
//    }
//}

inline adjacency_matrix make_graph_matrix(ProcessesGraph const& g) {
    int n = (int)g.graph.size();
    auto matrix = new int[n * n];

    for (int i = 0; i < n*n; ++i) {
        matrix[i] = INF;
    }
    for (int i = 0; i < n; ++i) {
        matrix[i*n + i] = 0;
    }

    for (int v = 0; v < n; ++v) {
        auto sz = g.graph[v].size();
        for (int i = 0; i < sz; ++i) {
            int u = g.graph[v][i];
            matrix[v*n + u] = 1;
        }
    }

    return matrix;
}


#endif //NOMA_DIMENSION_FLOYD_INCLUDED