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

//O(V+E)
bool network_have_cut_vertice(ProcessesGraph const& pg);

#endif //NOMA_DIMENSION_utils_INCLUDED
