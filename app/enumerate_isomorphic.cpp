#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

vector<ProcessesGraph> result_processes;
vector<int> iso_count;

bool check_cpg(ProcessesGraph const& pg, adjacency_list const& g, vector<critical_pair> const& cp) {
    //cout << "Graph size: " << pg.graph.size() << endl;
    adjacency_list icg(cp.size());

    for (int i = 0; i < cp.size(); ++i) {
        for (int j = i + 1; j < cp.size(); ++j) {
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
        //cout << "WARNING: incompatible critical pair graph is NOT bipartite!" << endl;
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
        //cout << "OK: incompatible critical pair graph is bipartite!" << endl;
        //cout << pg << endl;
        bool iso = false;
        //for (auto& p : result_processes) {
        for (int i = 0; i<result_processes.size(); ++i) {
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
            iso_count.push_back(1);
            result_processes.push_back(pg);

            cout << pg << endl;
        }

        return true;
    }
}

bool process_graph(ProcessesGraph const& g) {
    //prepare graph matrix
    auto matrix = make_graph_matrix(g);
    //calc (weighted) transitive closure
    int n = (int)g.graph.size();
    floyd(matrix, n);

    //find all critical pairs
    vector<critical_pair> critical_pairs;
    //cout << "Critical pairs: "<<endl;
    for (int v = 0; v < n; ++v) {
        for (int u = 0; u < n; ++u) {
            if (matrix[v*n + u] == INF && matrix[u*n + v] == INF) {

                if (check_if_critical(matrix, n, v, u)) {
                    critical_pairs.push_back({ v, u });
                }
            }
        }
    }

    delete[] matrix;

    return check_cpg(g, g.graph, critical_pairs);

    //cout << "================================" << endl;
}

void generate_graph(ProcessesGraph const& g, int max_sync_num, int sync_num) {
    if (sync_num > max_sync_num) {
        return;
    }

    if (is_full_syncronized(g)) {
        process_graph(g);
        return;
    }

    auto proc_num = g.proc_num;
    for (int p1 = 0; p1 < proc_num; ++p1) {
        for (int p2 = p1 + 1; p2 < proc_num; ++p2) {
            auto ng = g;
            ng.sync(p1, p2);
            generate_graph(ng, max_sync_num, sync_num + 1);
        }
    }
}

int main(int argc, char* argv[]) {
    cout << "Enumerate of isomorphic executions" << endl;

    if (argc < 3) {
        cerr << "usage: " << endl;
        cerr << "enumerate_isomorphic proc_num max_sync_num" << endl;
        return EXIT_FAILURE;
    }

    int proc_num = atoi(argv[1]);
    ProcessesGraph g;
    g.init(proc_num);

    cout << "Processes num: " << proc_num << endl;

    generate_graph(g, atoi(argv[2]), 0);

    cout << "Count of non isomorphic graphs (of dim 2): " << result_processes.size() << endl;

    if (iso_count.size() != 0) {
        cout << "Graphs with additional symmetricity (0-based indeces):" << endl;

        for (int i = 0; i < iso_count.size(); ++i) {
            if (iso_count[i] > iso_count[0]) {
                cout << i <<' '<< (iso_count[i] / iso_count[0]) << endl;
            }
        }
        cout << endl;
    }

    return EXIT_SUCCESS;
}