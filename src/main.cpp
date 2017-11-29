#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <cstring>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>

using namespace std;

bool check_if_critical(incidence_matrix matrix, int* nm, int n, int x, int y) {
    for (int v = 0; v < n; ++v) {
        for (int u = 0; u < n; ++u) {
            if (v == x && u == y) {
                continue;
            }
            if (matrix[v*n + u] == INF && nm[v*n + u] != INF) {
                return false;
            }
        }
    }

    return true;
}

struct critical_pair {
    int x;
    int y;
};

bool have_cycle(adjacency_list const& g) {
    vector<bool> used(g.size(), false);
    vector<bool> cur_way(g.size(), false);

    function<bool (int)> bfs = [&](int v) -> bool {
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

bool is_bipartite(adjacency_list const& g) {
    vector<int> label(g.size(), -1);

    function<bool(int,int)> bfs = [&](int v, int lbl) {
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

vector<ProcessesGraph> result_processes;
vector<int> iso_count;

//vector<vector<ProcessesGraph>> isov;

bool check_cpg(ProcessesGraph const& pg, adjacency_list const& g, vector<critical_pair> const& cp) {
    //cout << "Graph size: " << pg.graph.size() << endl;
    adjacency_list icg(cp.size());

    for (int i = 0; i < cp.size(); ++i) {
        for (int j = i+1; j < cp.size(); ++j) {
            auto cp1 = cp[i];
            auto cp2 = cp[j];

            auto lg = g;
            lg[cp1.y].push_back(cp1.x);
            lg[cp2.y].push_back(cp2.x);

            //cout << i << " " << j << endl << lg << endl;

            if (have_cycle(lg)) {
                //cout << "Cycle" << endl;
                icg[i].push_back(j);
                icg[j].push_back(i);
            }
        }
    }

    //cout << "Incomparable critical pair graph:" << endl << icg << endl;
    
    if (!is_bipartite(icg)) {
        //cout << endl;
        //cout << "=================================" << endl;
        cout << "WARNING: incompatible critical pair graph is NOT bipartite!" << endl;
        //cout <<  pg << endl;
        //cout << "Critical pairs:" << endl;
        //for (int i = 0; i < cp.size(); ++i) { 
        //    cout << "(" << cp[i].x << "," << cp[i].y <<")"<< endl;
        //}
        //cout << "Bipartite graph:" << endl << icg << endl;
        //cout << "=================================" << endl;
        //cout << "!" << endl;
        return false;
    }
    else {
        cout << "OK: incompatible critical pair graph is bipartite!" << endl;
        //cout << pg << endl;
        bool iso = false;
        //for (auto& p : result_processes) {
        for(int i = 0; i<result_processes.size(); ++i) {
            auto& p = result_processes[i];
            //if (is_isomorphic(p, pg) != is_isomorphic(pg, p)) {
            //    cout << "ERR" << endl;
            //}

            if (is_isomorphic(p, pg)) {
                iso_count[i]++;
                iso = true;
                break;
            }
        }

        if (!iso) {
            //isov.push_back({ pg });
            iso_count.push_back(1);
            result_processes.push_back(pg);
        }
        else {
            //vector<int> idxs;
            //for (int i = 0; i<result_processes.size(); ++i) {
            //    auto& p = result_processes[i];
            //    if (is_isomorphic(p, pg) != is_isomorphic(pg, p)) {
            //        cout << "ERR" << endl;
            //    }
            //    if (is_isomorphic(p, pg)) {
            //        idxs.push_back(i);
            //        isov[i].push_back(pg);
            //    }
            //}

            //if (idxs.size() > 1) {
            //    cout << "AAAA " << endl;
            //    //for (auto& i : idxs) {
            //    //    cout << i << " ";
            //    //}
            //    //cout << endl;
            //    //cout << pg << endl;
            //    //cout << result_processes[idxs[0]] << endl;
            //    //is_isomorphic(result_processes[idxs[0]], pg);
            //    //cout << result_processes[idxs[1]] << endl;
            //    //is_isomorphic(result_processes[idxs[1]], pg);
            //}
        }

        return true;
    }
}

bool process_graph(ProcessesGraph const& g) {
    //cout << "*"<<flush;
    //cout << "================================" << endl;
    //cout << g << endl;

    //prepare graph matrix

    auto matrix = make_graph_matrix(g);

    //calc (weighted) transitive closure
    int n = (int)g.graph.size();
    floyd(matrix, n);

    //check all pairs
    vector<critical_pair> critical_pairs;
    //cout << "Critical pairs: "<<endl;
    auto nm = new int[n * n];
    for (int v = 0; v < n; ++v) {
        for (int u = 0; u < n; ++u) {
            if (matrix[v*n + u] == INF && matrix[u*n + v] == INF) {
                memcpy(nm, matrix, n * n * sizeof(int));
                nm[v*n + u] = 1;
                //floyd(nm, n);
                floyd_advance_vertex(nm, n, v);
                floyd_advance_vertex(nm, n, u);

                if (check_if_critical(matrix, nm, n, v, u)) {
                    //print(matrix, n);
                    //print(nm, n);
                    critical_pairs.push_back({ v, u });
                    //cout << g.labels[v].proc << " (" << v << "," << u << ") " << g.labels[u].proc << endl;
                }
            }
        }
    }

    delete[] matrix;
    delete[] nm;

    return check_cpg(g, g.graph, critical_pairs);

    //cout << "================================" << endl;
}

bool is_full_syncronized(ProcessesGraph const& g) {
    auto graph_matrix = make_graph_matrix(g);
    
    int n = (int)g.graph.size();
    floyd(graph_matrix, n);

    bool all_sync = true;
    for (int start = 0; start < g.proc_num && all_sync; ++start) {
        for (auto last: g.proc_last_vertex) {
            if (graph_matrix[start*n + last] == INF) {
                all_sync = false;
                break;
            }
        }
    }
    
    delete[] graph_matrix;

    if (all_sync) {
        //cout << g << endl;
    }

    return all_sync;
}

void generate_graph(ProcessesGraph const& g, int max_vert_num, int max_vert_in_proc, int sync_num) {
    bool balanced_graph = [&] {
        for (int i = 0; i < g.proc_num; ++i) {
            if (g.proc_verteces[i].size() < max_vert_in_proc)
                return false;
        }

        return true;
    }();

    //if (balanced_graph) {
    //    //process_graph(g);
    //    //return;
    //}

    if (sync_num > max_vert_num) {
        return;
    }

    //if (g.graph.size() >= max_vert_num) {
    //    //process_graph(g);
    //    return;
    //}

    if (is_full_syncronized(g)) {
        process_graph(g);
        return;
    }

    auto proc_num = g.proc_num;
    for (int p1 = 0; p1 < proc_num; ++p1) {
        for (int p2 = p1 + 1; p2 < proc_num; ++p2) {
            auto ng = g;
            ng.sync(p1, p2);
            //ng.update(p1);
            //ng.update(p2);
            generate_graph(ng, max_vert_num, max_vert_in_proc, sync_num + 1);
        }
    }
}

int main(int argc, char* argv[]) {
    //if (argc < 4) {
    //    cout << "usage: " << endl;
    //    cout << "noma_dimention proc_num max_vert_num max_vert_per_proc" << endl;
    //    return 0;
    //}

    //int proc_num = atoi(argv[1]);
    //ProcessesGraph g;
    //g.init(proc_num);
    //generate_graph(g, atoi(argv[2]), 1, 0);

    ////int proc_num = 3;
    ////ProcessesGraph g;
    ////g.init(proc_num);
    ////generate_graph(g, 3, 1, 0);

    //cout << "Count of non isomorphic graphs (of dim 2): " << result_processes.size() << endl;

    //if (iso_count.size() != 0) {
    //    cout << "Graphs with additional symmetricity (0-based indeces):" << endl;

    //    for (int i = 0; i < iso_count.size(); ++i) {
    //        if (iso_count[i] > iso_count[0]) {
    //            cout << i <<' '<< (iso_count[i] / iso_count[0]) << endl;
    //        }
    //    }
    //    cout << endl;
    //}

    //for (auto& p : result_processes) {
    //    cout << p << endl;
    //}


    //vector<vector<int>> perms = {
    //    {0, 3, 2, 1},
    //    {1, 2, 0, 3},
    //    {1, 2, 3, 0},
    //    {2, 1, 0, 3},
    //    {2, 1, 3, 0},
    //    {3, 0, 2, 1}
    //};
    //for (int i = 0; i < perms.size(); ++i) {
    //    for (int j = 0; j < perms.size(); ++j) {
    //        for (int k = 0; k < perms.size(); ++k) {
    //            ProcessesGraph g;
    //            g.init(4);
    //            cout << "===========================" << endl;
    //            cout << i << " " << j << " " << k << endl;
    //            cout << "===========================" << endl;
    //            f45_8(g);
    //            //process_graph(g);
    //            f45_8_p(g, perms[i]);
    //            process_graph(g);
    //            f45_8(g);
    //            //process_graph(g);
    //            f45_8_p(g, perms[j]);
    //            process_graph(g);
    //            f45_8(g);
    //            //process_graph(g);
    //            f45_8_p(g, perms[k]);
    //            process_graph(g);
    //            f45_8(g);
    //            process_graph(g);
    //        }
    //    }
    //}

    //vector<int> perm{0,1,2,3};
    //do {
    //    ProcessesGraph g;
    //    g.init(4);
    //    f45_8(g);
    //    f45_8_p(g, perm);
    //    if (process_graph(g)) {
    //        for (int i = 0; i < perm.size(); ++i) {
    //            cout << perm[i] << " ";
    //        }
    //        cout << endl;
    //    }
    //} while (next_permutation(perm.begin(), perm.end()));

    //vector<function <void(ProcessesGraph &)> > fs = {
    //    //f45_1,
    //    //f45_2,
    //    //f45_3,
    //    //f45_4,
    //    //f45_5,
    //    //f45_6,
    //    //f45_7,
    //    //f45_8,
    //    //f45_9,
    //    //f45_10
    //};
    //for (int i = 0; i < fs.size(); ++i) {
    //    for (int j = 0; j < fs.size(); ++j) {
    //        for (int k = 0; k < fs.size(); ++k) {
    //            ProcessesGraph g;
    //            g.init(4);
    //            cout << i + 8 << " " << j + 8 << " " << k + 8 << endl;
    //            vector<int> perm{ 0,1,2,3 };
    //            fs[i](g, perm);
    //            fs[j](g, perm);
    //            fs[k](g, perm);
    //            process_graph(g);
    //        }
    //    }
    //}

    //ProcessesGraph g;
    //g.init(4);
    //f45_4(g);
    //process_graph(g);
    //f45_6(g);
    //process_graph(g);
    //f45_9(g);
    //process_graph(g);
    //f45_1(g);
    //process_graph(g);
    //f45_9(g);
    //process_graph(g);
    //f45_6(g);
    //process_graph(g);
    //f45_9(g);

    //ProcessesGraph g;
    //g.init(5);
    //f67_36(g);
    //process_graph(g);
    //f67_36(g);
    //process_graph(g);
    //f67_36(g);
    //process_graph(g);
    //f67_36(g);
    //process_graph(g);
    //f67_36(g);
    //process_graph(g);

    //ProcessesGraph g;
    //g.init(6);
    //f69_XX(g);
    //process_graph(g);
    //f69_XX(g);
    //process_graph(g);
    //f69_XX(g);
    //process_graph(g);
    //f69_XX(g);
    //process_graph(g);
    //f69_XX(g);
    //process_graph(g);
    //f69_XX(g);
    //process_graph(g);

    ProcessesGraph g;
    g.init(6);
    for (auto i : {1,2,3,4,5}) {
        f68_2(g);
        process_graph(g);
        cout << is_full_syncronized(g) << endl;
    }

    return 0;
}