#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

#include <unordered_set>

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

bool check_critical_pair_graph(adjacency_list const& g, vector<critical_pair> const& cp) {
    adjacency_list icg(cp.size());

    for (int i = 0; i < cp.size(); ++i) {
        for (int j = i + 1; j < cp.size(); ++j) {
            auto cp1 = cp[i];
            auto cp2 = cp[j];

            auto lg = g;
            lg[cp1.y].push_back(cp1.x);
            lg[cp2.y].push_back(cp2.x);


            if (have_cycle(lg)) {
                icg[i].push_back(j);
                icg[j].push_back(i);
            }
        }
    }

    //cout << "Bipartite graph" << endl;
    //cout << "=======" << endl;
    //for (int v = 0; v <icg.size(); ++v) {
    //    cout << v <<  ": ";
    //    for (auto u : icg[v]) {
    //        cout << u << ", ";
    //    }
    //    cout << endl;
    //}
    //cout << "=======" << endl;


    if (!is_bipartite(icg)) {
        return false;
    }
    else {
        return true;
    }
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

inline adjacency_matrix make_graph_matrix(adjacency_list const& g) {
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

#include <map>
#include <set>
#include <string>

adjacency_list generate_fib_poset(int N) {
    map<string, int> vname;
    map<int, string> vidx;
    int cur_vert = 0;

    vidx[cur_vert] = "1";
    vname["1"] = cur_vert++;
    set<string> cur_rank_verts, prev_rank_verts;
    prev_rank_verts.insert("1");

    vector<set<int>> g;
    g.push_back({});

    auto connect_verts = [&] (string const& vn, int v, string const& un){
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
        for (auto& prev_rank_name: prev_rank_verts) {
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

    //cout << "=======" << endl;
    //adjacency_list ret;
    //for (int v = 0; v < g.size(); ++v) {
    //    ret.push_back({});
    //    cout << vidx[v] << "("<<v << ")" << ": ";
    //    for (auto u : g[v]) {
    //        ret[v].push_back(u);
    //        cout << vidx[u] << "(" << u << ")" << ", ";
    //    }
    //    cout << endl;
    //}
    //cout << "=======" << endl;


    adjacency_list ret;
    for (int v = 0; v < g.size(); ++v) {
        ret.push_back({});
        for (auto u : g[v]) {
            ret[v].push_back(u);
        }
    }

    return ret;
}

#include <functional>
#include <algorithm>

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

int main(int argc, char* argv[]) {
    cout << "Check some hardcoded poset for been dimention 2" << endl;

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

    //int d = 4;
    for (int d = 3; d < 40; ++d) {
        auto g = generate_fib_poset(d);

        //check poset
        auto matrix = make_graph_matrix(g);
        //print(matrix, g.size());
        auto cps = find_critical_pairs(matrix, g.size());

        //cout << "Critical pairs are: " << endl;
        //for (auto& cp : cps) {
        //    cout << cp.x << "," << cp.y << endl;
        //}

        cout << d << ": ";
        if (check_critical_pair_graph(g, cps)) {
            cout << "HAS dimension 2" << endl;
        }
        else {
            cout << "DO NOT has dimension 2" << endl;
        }
        cout << "HAS cut vertice " << boolalpha << network_have_cut_vertice(g) << endl;
    }

    return EXIT_SUCCESS;
}