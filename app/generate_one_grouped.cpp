#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

#include <unordered_set>

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

    if (!is_bipartite(icg)) {
        return false;
    }
    else {
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

bool generate_one_graph(ProcessesGraph const& g, int group_size, int group_num, int sync_num) {
    auto proc_num = g.proc_num;

    if (!process_graph(g))
        return false;

    if (sync_num <= 0) {
        if (is_full_syncronized(g)) {
            if (process_graph(g)) {
                cout << g << endl;
                return true;
            }
        }

        return false;
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
                    if (generate_one_graph(ng, group_size, group_num, sync_num - 1)) {
                        return true;
                    }
                }
            }
        }

        for (auto p2 : ingroup_processes) {
            auto ng = g;
            if (p2 != p1) {
                ng.sync(p1, p2);
                if (generate_one_graph(ng, group_size, group_num, sync_num - 1)) {
                    return true;
                }
            }
        }
    }

    return false;
}

int main(int argc, char* argv[]) {
    cout << "Generate one groped execution" << endl;

    if (argc < 3) {
        cerr << "usage: " << endl;
        cerr << "generate_one_grouped <group size> <group num> <sync num>" << endl;
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
    generate_one_graph(g, group_size, group_num, sync_num);

    return EXIT_SUCCESS;
}