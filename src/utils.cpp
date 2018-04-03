#include <utils.hpp>

#include <functional>
#include <algorithm>
#include <iostream>

using namespace std;

bool have_cycle(adjacency_list const& g) {
    vector<bool> used(g.size(), false);
    vector<bool> cur_way(g.size(), false);

    function<bool(int)> bfs = [&](int v) -> bool {
        used[v] = true;
        cur_way[v] = true;
        for (int i = 0; i < g[v].size(); ++i) {
            auto u = g[v][i];
            if (cur_way[u]) {
                //cout << "Cycle on " << v << " " << u << endl;
                return true;
            }
            if (!used[u] && bfs(u))
                return true;
        }

        cur_way[v] = false;
        return false;
    };

    for (int i = 0; i < g.size(); ++i) {
        if (!used[i] && bfs(i))
            return true;
    }

    return false;
}

bool check_if_critical(adjacency_matrix matrix, int n, int x, int y) {
    auto nm = new int[n * n];
    memcpy(nm, matrix, n * n * sizeof(int));
    nm[x*n + y] = 1;
    //floyd(nm, n);
    floyd_advance_vertex(nm, n, x);
    floyd_advance_vertex(nm, n, y);


    for (int v = 0; v < n; ++v) {
        for (int u = 0; u < n; ++u) {
            if (v == x && u == y) {
                continue;
            }
            if (matrix[v*n + u] == INF && nm[v*n + u] != INF) {
                delete[] nm;
                return false;
            }
        }
    }

    delete[] nm;

    return true;
}

bool is_bipartite(adjacency_list const& g) {
    vector<int> label(g.size(), -1);

    function<bool(int, int)> bfs = [&](int v, int lbl) {
        label[v] = lbl;

        auto nl = lbl == 0 ? 1 : 0;

        for (int i = 0; i < g[v].size(); ++i) {
            auto u = g[v][i];

            if (label[u] != -1 && label[u] != nl) {
                //cout << "NOT bipartite at " << v << " " << u << endl;
                return false;
            }

            if (label[u] == -1 && !bfs(u, nl)) {
                return false;
            }
        }

        return true;
    };

    for (int i = 0; i < g.size(); ++i) {
        if (label[i] == -1 && !bfs(i, 0))
            return false;
    }

    return true;
}

bool is_full_syncronized(ProcessesGraph const& g) {
    auto graph_matrix = make_graph_matrix(g);

    int n = (int)g.graph.size();
    floyd(graph_matrix, n);

    bool all_sync = true;
    for (int start = 0; start < g.proc_num && all_sync; ++start) {
        for (auto last : g.proc_last_vertex) {
            if (graph_matrix[start*n + last] == INF) {
                all_sync = false;
                break;
            }
        }
    }

    delete[] graph_matrix;

    return all_sync;
}

bool is_isomorphic(ProcessesGraph const& pgl, ProcessesGraph const& pgr) {
    if (pgl.proc_num != pgr.proc_num)
        return false;

    if (pgl.graph.size() != pgr.graph.size())
        return false;

    if (pgl.proc_verteces.size() != pgr.proc_verteces.size())
        return false;

    if (pgl.labels.size() != pgr.labels.size())
        return false;

    vector<int> perm;

    for (int i = 0; i < pgr.proc_num; ++i) {
        perm.push_back(i);
    }

    bool isomorphic = false;
    do {
        bool proc_vert_sizes = [&] {
            for (int i = 0; i < pgl.proc_verteces.size(); ++i) {
                if (pgl.proc_verteces[i].size() != pgr.proc_verteces[perm[i]].size())
                    return false;
            }

            return true;
        }();

        if (!proc_vert_sizes)
            continue;

        bool proc_vert_labels = [&] {
            //for each process
            for (int p = 0; p < pgl.proc_verteces.size(); ++p) {
                //for each vertex in process
                for (int i = 0; i < pgl.proc_verteces[p].size(); ++i) {
                    //current vertex
                    auto vl = pgl.proc_verteces[p][i];
                    auto vr = pgr.proc_verteces[perm[p]][i];

                    //labels not
                    if (pgl.labels[vl].num != pgr.labels[vr].num)
                        return false;

                    if (pgl.graph[vl].size() != pgr.graph[vr].size())
                        return false;

                    //next vertex is sync
                    if (pgl.graph[vl].size() != 0) {
                        //sync vertex
                        auto vls = pgl.graph[vl][0];
                        auto vrs = pgr.graph[vr][0];

                        //if (vls != vrs)
                        //    return false;

                        //verteces after sync
                        auto vlas1 = pgl.graph[vls][0];
                        auto vlas2 = pgl.graph[vls][1];

                        auto vras1 = pgr.graph[vrs][0];
                        auto vras2 = pgr.graph[vrs][1];

                        //other vertex after sync
                        auto vlo = [&] {
                            if (pgl.labels[vlas1].proc == p)
                                return vlas2;
                            return vlas1;
                        }();

                        auto vro = [&] {
                            if (pgr.labels[vras1].proc == perm[p])
                                return vras2;
                            return vras1;
                        }();

                        //must mutch process and event
                        if (perm[pgl.labels[vlo].proc] != pgr.labels[vro].proc ||
                            pgl.labels[vlo].num != pgr.labels[vro].num
                            )
                            return false;
                    }
                }
            }

            return true;
        }();

        if (!proc_vert_labels)
            continue;

        isomorphic = true;
    } while (!isomorphic && next_permutation(perm.begin(), perm.end()));

    return isomorphic;
}

bool network_have_cut_vertice(ProcessesGraph const& pg) {
    vector<bool> used(pg.network.size(), false);
    vector<int> tin(pg.network.size()), fup(pg.network.size());
    int timer = 0;

    bool have_cut_vertice = false;

    function<void(int, int)> dfs = [&](int v, int p) {
        used[v] = true;
        tin[v] = fup[v] = timer++;
        int children = 0;
        for(auto& to: pg.network[v]) {
            if (to == p)  continue;
            if (used[to])
                fup[v] = min(fup[v], tin[to]);
            else {
                dfs(to, v);
                fup[v] = min(fup[v], fup[to]);
                if (fup[to] >= tin[v] && p != -1)
                    have_cut_vertice = true;
                ++children;
            }
        }
        if (p == -1 && children > 1)
            have_cut_vertice = true;
    };

    dfs(0, -1);

    return have_cut_vertice;
}