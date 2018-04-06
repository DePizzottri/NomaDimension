#ifndef NOMA_DIMENSION_utils_INCLUDED
#define NOMA_DIMENSION_utils_INCLUDED

#include <process_graph.hpp>
#include <floyd.hpp>

bool have_cycle(adjacency_list const& g);

struct critical_pair {
    int x;
    int y;
};

bool check_if_critical(adjacency_matrix matrix, int n, int x, int y);

bool is_bipartite(adjacency_list const& g);

bool is_full_syncronized(ProcessesGraph const& g);

bool is_isomorphic(ProcessesGraph const& pgl, ProcessesGraph const& pgr);

vector<ProcessesGraph> generate_all_isomorphic(ProcessesGraph const& pg);

//O(V+E)
bool network_have_cut_vertice(ProcessesGraph const& pg);

bool network_have_cut_vertice(adjacency_list const& pg);

void print(adjacency_matrix matrix, int n);

//manually delete returned val
adjacency_matrix make_graph_matrix(adjacency_list const& g);

//finds all critical pairs in poset represented by braph (adjacency matrix)
vector<critical_pair> find_critical_pairs(adjacency_matrix & graph, int n);

//check if incopatible critical pair graph is bipartite
bool check_critical_pairs_graph(adjacency_list const& poset_graph, vector<critical_pair> const& critical_pairs);

bool is_poset_2_dimensional(ProcessesGraph const& g);

adjacency_list generate_fib_poset(int N);

#endif //NOMA_DIMENSION_utils_INCLUDED
