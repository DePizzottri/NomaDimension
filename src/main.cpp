#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <cstring>

using namespace std;

struct Label {
    int proc;
    int num;
};

ostream & operator<<(ostream & out, Label const& l) {
    out << "[" << l.proc << ", " << l.num << "]";
    return out;
}

ostream & operator<<(ostream & out, vector<vector<int>>& g) {
    for (int v = 0; v < g.size(); ++v) {
        out << setw(2) << v << ": ";
        for (int i = 0; i < g[v].size(); ++i) {
            out << g[v][i] << " ";
        }
        out << endl;
    }
    return out;
}


class ProcessesGraph {
public:
    int proc_num;

    vector<vector<int>> graph;
    vector<int> proc_last_vertex;
    int next_vertex;
    
    vector<Label> labels;

    vector<vector<int>> proc_verteces;

    void init(int proccess_num) {
        graph.clear();
        proc_num = proccess_num;
        proc_last_vertex.clear();
        next_vertex = 0;
        labels.clear();
        proc_verteces.clear();

        proc_last_vertex.resize(proc_num);
        proc_verteces.resize(proc_num);

        //auto zero_vertex = next_vertex++;
        //graph.push_back({});
        //labels.push_back({ -1, -1 });

        for (int i = 0; i < proc_num; ++i) {
            //create first vertex
            auto cur_vert = next_vertex++;
            graph.push_back({});
            proc_last_vertex[i] = cur_vert;
            labels.push_back({i, 0});
            proc_verteces[i].push_back(cur_vert);
            //initial vertex
            //update(i);

            //graph[zero_vertex].push_back(cur_vert);
        }
    }

    void update(int proc) {
        add_vertex_to_proc(proc);
    }

    void sync(int proc1, int proc2) {
        assert(proc1 < proc_num);
        assert(proc1 >= 0);

        assert(proc2 < proc_num);
        assert(proc2 >= 0);

        int p1 = proc_last_vertex[proc1];
        int p2 = proc_last_vertex[proc2];

        ////add vertex to first process
        //auto v1 = add_vertex_to_proc(proc1);

        ////add vertex to second process
        //auto v2 = add_vertex_to_proc(proc2);

        ////sync
        //graph[p1].push_back(p2);
        //graph[p2].push_back(p1);
        
        auto new_vertex = next_vertex++;
        graph.push_back({});

        auto p1nv = next_vertex++;
        graph.push_back({});

        auto p2nv = next_vertex++;
        graph.push_back({});

        graph[p1].push_back(new_vertex);
        graph[p2].push_back(new_vertex);
        
        graph[new_vertex].push_back(p1nv);
        graph[new_vertex].push_back(p2nv);

        proc_last_vertex[proc1] = p1nv;
        proc_last_vertex[proc2] = p2nv;

        proc_verteces[proc1].push_back(p1nv);
        proc_verteces[proc2].push_back(p2nv);

        labels.push_back({ -1, -1 });

        labels.push_back({ proc1, labels[p1].num + 1 });
        labels.push_back({ proc2, labels[p2].num + 1 });
    }

private:

    int add_vertex_to_proc(int proc) {
        assert(proc < proc_num);
        assert(proc >= 0);

        auto cur_vert = proc_last_vertex[proc];
        auto cur_vert_label = labels[cur_vert];

        auto new_vertex = next_vertex++;
        graph.push_back({});
        graph[cur_vert].push_back(new_vertex);
        proc_last_vertex[proc] = new_vertex;
        labels.push_back({ proc, cur_vert_label.num + 1 });
        
        proc_verteces[proc].push_back(new_vertex);

        return new_vertex;
    }
};

ostream& operator << (ostream & out, ProcessesGraph const& g) {
    out << "Proc = " << g.proc_num << " verteces = " << g.graph.size() << endl;
    for (int v = 0; v < g.graph.size(); ++v) {
        out << setw(2) << v << ": " << g.labels[v] << " -- ";
        for (int i = 0; i < g.graph[v].size(); ++i) {
            out << g.graph[v][i] << " ";
        }
        out << endl;
    }

    return out;
}

void draw(ProcessesGraph const& g) {
    int factor = 4;
    int proc_num = g.proc_num;
    char** m = new char*[(proc_num-1)*factor];
    for (int i = 0; i < (proc_num - 1) * factor; ++i) {
        m[i] = new char[g.graph.size() * factor];
    }

    vector<pair<int, int>> coordinates(g.graph.size());

    for (int p = 0; p < proc_num; ++p) {
        int cnt = 0;
        for (auto& v : g.proc_verteces[p]) {
            m[p*factor][cnt] = '*';
            coordinates[v] = { p*factor, cnt };
            for (int i = 0; i < factor; ++i, ++cnt) {
                m[p*factor][cnt] = '-';
            }
        }
    }

    for (int v = 0; v < g.graph.size(); ++v) {
        if (g.graph[v].size()) {
            for (auto& u : g.graph[v]) {
                if (g.labels[v].proc != g.labels[u].proc) {
                    auto c1 = coordinates[v];
                    auto c2 = coordinates[u];
                }
            }
        }
    }

    for (int i = 0; i < (proc_num - 1) * 5; ++i) {
        delete[] m[i];
    }
    delete[]m;
}

const auto INF = numeric_limits<int>::max() / 3;

void floyd(int* matrix, int n) {
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

void floyd_advance_vertex(int* matrix, int n, int v) {
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

void print(int* matrix, int n) {
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

bool check_if_critical(int* matrix, int* nm, int n, int x, int y) {
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

bool have_cycle(vector<vector<int>> const& g) {
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

bool is_bipartite(vector<vector<int>> const& g) {
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

//bool __is_isomorphic1(ProcessesGraph const& pgl, ProcessesGraph const& pgr) {
//    if (pgl.proc_num != pgl.proc_num)
//        return false;
//
//    if (pgl.graph.size() != pgr.graph.size())
//        return false;
//
//    if (pgl.proc_verteces.size() != pgr.proc_verteces.size())
//        return false;
//
//    if (pgl.labels.size() != pgr.labels.size())
//        return false;
//
//    vector<int> perm{ 2,0,1 };
//
//    //for (int i = 0; i < pgr.proc_num; ++i) {
//    //    perm.push_back(i);
//    //}
//
//    vector<int> aperm(pgl.proc_num);
//    for (int i = 0; i < pgr.proc_num; ++i) {
//        aperm[perm[i]] = i;
//    }
//
//    bool isomorphic = false;
//    do {
//        bool proc_vert_sizes = [&] {
//            for (int i = 0; i < pgl.proc_verteces.size(); ++i) {
//                if (pgl.proc_verteces[i].size() != pgr.proc_verteces[perm[i]].size())
//                    return false;
//            }
//
//            return true;
//        }();
//
//        if (!proc_vert_sizes)
//            continue;
//
//        bool proc_vert_labels = [&] {
//            for (int p = 0; p < pgl.proc_verteces.size(); ++p) {
//                for (int i = 0; i < pgl.proc_verteces[p].size(); ++i) {
//                    auto vl = pgl.proc_verteces[p][i];
//                    auto vr = pgr.proc_verteces[perm[p]][i];
//
//                    if (pgl.labels[vl].num != pgr.labels[vr].num)
//                        return false;
//
//                    if (pgl.graph[vl].size() != pgr.graph[vr].size())
//                        return false;
//
//                    //next vertex is sync
//                    if (pgl.graph[vl].size() != 0) {
//                        //sync vertex
//                        auto vls = pgl.graph[vl][0];
//                        auto vrs = pgr.graph[vr][0];
//
//                        //verteses after sycn
//                        auto vlas1 = pgl.graph[vls][0];
//                        auto vlas2 = pgl.graph[vls][1];
//
//                        auto vras1 = pgr.graph[vrs][0];
//                        auto vras2 = pgr.graph[vrs][1];
//
//                        //other vertex after sync
//                        auto vlo = [&] {
//                            if (pgl.labels[vlas1].proc == p)
//                                return vlas2;
//                            return vlas1;
//                        }();
//
//                        auto vro = [&] {
//                            if (pgr.labels[vras1].proc == aperm[p])
//                                return vras2;
//                            return vras1;
//                        }();
//
//                        if (pgl.labels[vlo].proc != aperm[pgr.labels[vro].proc] ||
//                            pgl.labels[vlo].num != pgr.labels[vro].num
//                            )
//                            return false;
//                    }
//                }
//            }
//
//            return true;
//        }();
//
//        if (!proc_vert_labels)
//            continue;
//
//        isomorphic = true;
//
//        //bool sync_verteces = [&] {
//        //    for (int v = 0; v < pgl.graph.size(); ++v) {
//        //        if (pgl.labels[v].num == -1) {
//        //            if(pgr.labels[])
//        //        }
//        //    }
//        //}
//    } while (!isomorphic && next_permutation(perm.begin(), perm.end()));
//
//    return isomorphic;
//}

vector<ProcessesGraph> result_processes;
vector<int> iso_count;

//vector<vector<ProcessesGraph>> isov;

bool check_cpg(ProcessesGraph const& pg, vector<vector<int>> const& g, vector<critical_pair> const& cp) {
    //cout << "Graph size: " << pg.graph.size() << endl;
    vector<vector<int>> icg(cp.size());

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

int* make_graph_matrix(ProcessesGraph const& g) {
    int n = (int)g.graph.size();
    auto matrix = new int[n * n];

    for (int i = 0; i < n*n; ++i) {
        matrix[i] = INF;
    }
    for (int i = 0; i < n; ++i) {
        matrix[i*n + i] = 0;
    }

    for (int v = 0; v < n; ++v) {
        int sz = g.graph[v].size();
        for (int i = 0; i < sz; ++i) {
            int u = g.graph[v][i];
            matrix[v*n + u] = 1;
        }
    }

    return matrix;
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

void f45_10(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 3);
    g.sync(1, 2);
}

void f45_9(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 1);
    g.sync(2, 3);
}

void f45_9_p(ProcessesGraph & g, vector<int> const& perm) {
    assert(g.proc_num == 4);
    assert(perm.size() == 4);
    g.sync(perm[0], perm[1]);
    g.sync(perm[2], perm[3]);
    g.sync(perm[0], perm[2]);
    g.sync(perm[0], perm[1]);
    g.sync(perm[2], perm[3]);
}

void f45_8(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(1, 2);
    g.sync(1, 3);
    g.sync(2, 3);
    g.sync(0, 3);
}

void f45_8_p(ProcessesGraph & g, vector<int> const& perm) {
    assert(g.proc_num == 4);
    assert(perm.size() == 4);
    g.sync(perm[0], perm[1]);
    g.sync(perm[1], perm[2]);
    g.sync(perm[1], perm[3]);
    g.sync(perm[2], perm[3]);
    g.sync(perm[0], perm[3]);
}

void f45_7(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(2, 3);
    g.sync(0, 3);
    g.sync(1, 3);
}

void f45_6(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(2, 3);
    g.sync(0, 3);
    g.sync(0, 1);
}

void f45_5(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(1, 2);
}

void f45_4(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 1);
}

void f45_3(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(0, 3);
    g.sync(2, 3);
    g.sync(1, 2);
}

void f45_2(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(0, 3);
    g.sync(0, 2);
    g.sync(1, 2);
}

void f45_1(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(0, 3);
    g.sync(0, 2);
    g.sync(0, 1);
}

//void f45_(ProcessesGraph & g) {
//    assert(g.proc_num == 4);
//    g.sync(0, 1);
//    g.sync();
//    g.sync();
//    g.sync();
//    g.sync();
//}

void f57_36(ProcessesGraph & g) {
    assert(g.proc_num == 5);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(3, 4);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 1);
    g.sync(3, 4);
}

void f69_XX(ProcessesGraph & g) {
    //two 3-groups with bridge
    assert(g.proc_num == 6);
    g.sync(4, 5);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(3, 4);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 1);
    g.sync(3, 4);
    g.sync(4, 5);
}

void f68_1(ProcessesGraph & g) {
    //three two groups with 2 briges (1-2, 3-4)
    assert(g.proc_num == 6);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
    //1 brige
    g.sync(1, 2);
    //2 brige
    g.sync(3, 4);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
}

void f68_2(ProcessesGraph & g) {
    //three two groups with 3 briges (1-2, 1-4, 2-4)
    assert(g.proc_num == 6);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
    //1 brige from (0,1) to (2,3)
    g.sync(1, 2);
    //and fully sync(0,1) and (2,3)
    g.sync(0, 1);
    g.sync(2, 3);
    //2 brige from (0,1) to (4,5)
    //g.sync(1, 4);
    //and sync (4,5) and (0,1)
    //g.sync(0, 1);
    //g.sync(4, 5);

    //3 brige from (2,3) to (4,5)
    g.sync(2, 4);
    g.sync(2, 3);
    g.sync(4, 5);

    //2 brige from (0,1) to (4,5)
    //g.sync(1, 4);

    //parallel sync
    //g.sync(0, 1);
    //g.sync(2, 3);
    //g.sync(4, 5);
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