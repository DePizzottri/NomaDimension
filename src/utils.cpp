#include <utils.hpp>

#include <functional>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>

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

vector<ProcessesGraph> generate_all_isomorphic(ProcessesGraph const& pg) {
    vector<int> perm;

    for (int i = 0; i < pg.proc_num; ++i) {
        perm.push_back(i);
    }

    vector<ProcessesGraph> ret;
    do {
        ProcessesGraph npg;
        npg.init(pg.proc_num);
        
        for (auto& sync : pg.syncs) {
            npg.sync((min)(perm[sync.first], perm[sync.second]), (max)(perm[sync.first], perm[sync.second]));
        }

        ret.push_back(npg);
    } while (next_permutation(perm.begin(), perm.end()));

    return ret;
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

adjacency_matrix make_graph_matrix(adjacency_list const& g) {
    int n = (int)g.size();
    auto matrix = new int[n * n];

    for (int i = 0; i < n*n; ++i) {
        matrix[i] = INF;
    }
    for (int i = 0; i < n; ++i) {
        matrix[i*n + i] = 0;
    }

    for (int v = 0; v < n; ++v) {
        auto sz = g[v].size();
        for (int i = 0; i < sz; ++i) {
            int u = g[v][i];
            matrix[v*n + u] = 1;
        }
    }

    return matrix;
}

vector<critical_pair> find_critical_pairs(adjacency_matrix & matrix, int n) {
    vector<critical_pair> critical_pairs;
    //cout << "Critical pairs: "<<endl;
    floyd(matrix, n);
    for (int v = 0; v < n; ++v) {
        for (int u = 0; u < n; ++u) {
            if (matrix[v*n + u] == INF && matrix[u*n + v] == INF) {

                if (check_if_critical(matrix, n, v, u)) {
                    //print(matrix, n);
                    //print(nm, n);
                    critical_pairs.push_back({ v, u });
                    //cout << g.labels[v].proc << " (" << v << "," << u << ") " << g.labels[u].proc << endl;
                }
            }
        }
    }

    return critical_pairs;
}

bool check_critical_pairs_graph(adjacency_list const& poset_graph, vector<critical_pair> const& critical_pairs) {
    adjacency_list icg(critical_pairs.size());

    //generate incompatible critical pairs graph
    for (int i = 0; i < critical_pairs.size(); ++i) {
        for (int j = i + 1; j < critical_pairs.size(); ++j) {
            auto cp1 = critical_pairs[i];
            auto cp2 = critical_pairs[j];

            auto lg = poset_graph;
            lg[cp1.y].push_back(cp1.x);
            lg[cp2.y].push_back(cp2.x);


            if (have_cycle(lg)) {
                icg[i].push_back(j);
                icg[j].push_back(i);
            }
        }
    }

    if (!is_bipartite(icg)) {
        return false;
    }
    else {
        return true;
    }
}

bool network_have_cut_vertice(adjacency_list const& pg) {
    vector<bool> used(pg.size(), false);
    vector<int> tin(pg.size()), fup(pg.size());
    int timer = 0;

    bool have_cut_vertice = false;

    function<void(int, int)> dfs = [&](int v, int p) {
        used[v] = true;
        tin[v] = fup[v] = timer++;
        int children = 0;
        for (auto& to : pg[v]) {
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

void print(adjacency_matrix matrix, int n) {
    for (int v = 0; v < n; ++v) {
        for (int u = 0; u < n; ++u) {
            if (matrix[v*n + u] == INF) {
                cout << "I ";
            }
            else {
                cout << matrix[v*n + u] << " ";
            }
        }
        cout << endl;
    }
}

bool is_poset_2_dimensional(ProcessesGraph const& g) {
    auto matrix = make_graph_matrix(g);

    //calc (weighted) transitive closure
    int n = (int)g.graph.size();
    floyd(matrix, n);

    auto critical_pairs = find_critical_pairs(matrix, n);

    delete[] matrix;

    return check_critical_pairs_graph(g.graph, critical_pairs);
}

adjacency_list generate_fib_poset(int N) {
    //build poset
    //adjacency_list g;
    ////==== rank 1
    ////1 - 0
    //g.push_back({});

    ////==== rank 2
    ////2 - 1
    //g.push_back({});
    //g[0].push_back(1);

    ////11 - 2
    //g.push_back({});
    //g[0].push_back(2);
    //
    ////==== rank 3
    ////12 - 3
    //g.push_back({});
    //g[1].push_back(3);

    ////21 - 4
    //g.push_back({});
    //g[1].push_back(4);
    //g[2].push_back(4);

    ////111 - 5
    //g.push_back({});
    //g[2].push_back(5);

    //==== rank 4
    //112 - 6
    //g.push_back({});
    //g[3].push_back(6);
    ////22 - 7
    //g.push_back({});
    //g[3].push_back(7);
    //g[4].push_back(7);
    ////121 - 8
    //g.push_back({});
    //g[4].push_back(6);
    ////211 - 9
    //g.push_back({});
    //g[4].push_back(9);
    //g[5].push_back(9);
    ////1111 - 10
    //g.push_back({});
    //g[5].push_back(10);

    map<string, int> vname;
    map<int, string> vidx;
    int cur_vert = 0;

    vidx[cur_vert] = "1";
    vname["1"] = cur_vert++;
    set<string> cur_rank_verts, prev_rank_verts;
    prev_rank_verts.insert("1");

    vector<set<int>> g;
    g.push_back({});

    auto connect_verts = [&](string const& vn, int v, string const& un) {
        //if (un == "221")
        //    return;
        int u = -1;
        if (vname.find(un) == vname.end()) {
            g.push_back({});
            vidx[cur_vert] = un;
            u = vname[un] = cur_vert++;
        }
        else {
            u = vname[un];
        }

        g[v].insert(u);
    };

    vector<int> fib({ 1,1 });
    for (int cur_rank = 2; cur_rank <= N; ++cur_rank) {
        for (auto& prev_rank_name : prev_rank_verts) {
            //rule 1.1
            {
                auto pos = prev_rank_name.find_first_of('1');
                if (pos != string::npos) {
                    for (int inc_pos = 0; inc_pos <= pos; ++inc_pos) {
                        auto name = prev_rank_name;
                        name.insert(inc_pos, 1, '1');
                        connect_verts(prev_rank_name, vname[prev_rank_name], name);
                        cur_rank_verts.insert(name);
                    }
                }
            }

            //rule 1.2
            {
                if (prev_rank_name.find('1') == string::npos) {
                    for (int i = 0; i <= prev_rank_name.size(); ++i) {
                        auto name = prev_rank_name;
                        name.insert(i, 1, '1');
                        connect_verts(prev_rank_name, vname[prev_rank_name], name);
                        cur_rank_verts.insert(name);
                    }
                }
            }

            //rule 2
            {
                auto name = prev_rank_name;
                auto pos = name.find_first_of('1');
                if (pos != string::npos) {
                    name[pos] = '2';
                    connect_verts(prev_rank_name, vname[prev_rank_name], name);
                    cur_rank_verts.insert(name);
                }
            }
        }

        fib.push_back(fib[cur_rank - 1] + fib[cur_rank - 2]);
        assert(fib[cur_rank] == cur_rank_verts.size());
        prev_rank_verts.swap(cur_rank_verts);
        cur_rank_verts.clear();
    }

    adjacency_list ret;
    for (int v = 0; v < g.size(); ++v) {
        ret.push_back({});
        for (auto u : g[v]) {
            ret[v].push_back(u);
        }
    }

    return ret;
}