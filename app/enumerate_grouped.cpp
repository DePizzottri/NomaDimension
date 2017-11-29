#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

#include <unordered_set>

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
            //isov.push_back({ pg });
            iso_count.push_back(1);
            result_processes.push_back(pg);

            cout << pg << endl;
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

    //find all critical pairs
    vector<critical_pair> critical_pairs;
    //cout << "Critical pairs: "<<endl;
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

    delete[] matrix;

    return check_cpg(g, g.graph, critical_pairs);

    //cout << "================================" << endl;
}

void generate_grouped_graph(ProcessesGraph const& g, int group_size, int group_num, int sync_num) {
    auto proc_num = g.proc_num;

    if (sync_num <= 0) {
        if (is_full_syncronized(g)) {
            process_graph(g);
            return;
        }

        return;
    }

    auto group_leaders = [&] {
        unordered_set<int> ret;
        for (int i = 0; i < proc_num; i += group_size) {
            ret.insert(i);
        }

        return ret;
    }();

    for (int p1 = 0; p1 < proc_num; ++p1) {

        auto ingroup_processes = [&] {
            vector<int> ret;

            for (int p2 = group_size*(p1 / group_size); p2 < group_size*(p1 / group_size) + group_size; ++p2) {
                ret.push_back(p2);
            }

            return ret;
        }();

        if (group_leaders.find(p1) != group_leaders.end()) {
            for (auto p2 : group_leaders) {
                auto ng = g;
                if (p2 != p1) {
                    ng.sync(p1, p2);
                    generate_grouped_graph(ng, group_size, group_num, sync_num - 1);
                }
            }
        }

        for (auto p2 : ingroup_processes) {
            auto ng = g;
            if (p2 != p1) {
                ng.sync(p1, p2);
                generate_grouped_graph(ng, group_size, group_num, sync_num - 1);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    cout << "Enumerate all grouped executions" << endl;

    if (argc < 3) {
        cerr << "usage: " << endl;
        cerr << "enumerate_grouped <group size> <group num> <sync num>" << endl;
        return EXIT_FAILURE;
    }

    int group_size = atoi(argv[1]);
    int group_num = atoi(argv[2]);
    int sync_num = atoi(argv[3]);
    //int group_size = 2;
    //int group_num = 2;
    //int sync_num = 6;
    cout << "Group size: " << group_size << endl;
    cout << "Groups num: " << group_num << endl;
    cout << "Sync num: " << sync_num << endl;

    ProcessesGraph g;
    g.init(group_size*group_num);
    generate_grouped_graph(g, group_size, group_num, sync_num);

    cout << "Count of non isomorphic graphs (of dim 2): " << result_processes.size() << endl;

    return EXIT_SUCCESS;
}