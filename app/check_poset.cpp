#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

#include <unordered_set>

#include <map>
#include <set>
#include <string>

#include <functional>
#include <algorithm>

void check_fib_poset() {
    cout << "Check fibonacci poset" << endl;

    //int d = 4;
    for (int d = 3; d < 40; ++d) {
        auto g = generate_fib_poset(d);

        //check poset
        auto matrix = make_graph_matrix(g);
        //print(matrix, g.size());
        auto cps = find_critical_pairs(matrix, g.size());

        cout << d << ": ";
        if (check_critical_pairs_graph(g, cps)) {
            cout << "HAS dimension 2" << endl;
        }
        else {
            cout << "DO NOT has dimension 2" << endl;
        }
        cout << "HAS cut vertice " << boolalpha << network_have_cut_vertice(g) << endl;
    }
}

int main(int argc, char* argv[]) {
    //cout << "Check some hardcoded poset for been dimension 2" << endl;
    cout << "Generate flat grid" << endl;

    //const auto offs0 = 0;

    //const auto offs1 = 3;
    //const auto offs2 = offs1 + 3;

    if (argc < 3) {
        cerr << "Usage: exec <proc_num> <max_depth>" << endl;
        return 1;
    }

    auto proc_num = stoi(argv[1]);
    auto max_depth = stoi(argv[2]);

    auto f331 = [](ProcessesGraph & g, vector<int> const& p) {
        g.sync(p[0], p[1]);
        g.sync(p[0], p[2]);
        g.sync(p[0], p[1]);
    };

    auto f332 = [](ProcessesGraph & g, vector<int> const& p) {
        g.sync(p[0], p[1]);
        g.sync(p[0], p[2]);
        g.sync(p[1], p[2]);
    };

    //auto trans = { f331, f332 };

    vector<ProcessesGraph> result_processes;
    int isocount = 0;

    function<void(ProcessesGraph & g, int depth)> generate = [&](ProcessesGraph & g, int depth) {
        if (depth == max_depth) {
            if (is_full_syncronized(g) && is_poset_2_dimensional(g)) {
                bool iso = false;
                for (int i = 0; i<result_processes.size(); ++i) {
                    auto& p = result_processes[i];
                    if (is_isomorphic(p, g)) {
                        iso = true;
                        break;
                    }
                }

                if (!iso) {
                    isocount++;
                    result_processes.push_back(g);
                    cout << g << endl;
                }
            }
            return;
        }

        for (int i = 0; i < g.proc_num - 1; ++i) {
            auto ng = g;
            ng.sync(i, i + 1);
            generate(ng, depth + 1);
        }
    };

    ProcessesGraph g;
    g.init(proc_num);

    generate(g, 0);

    cout << "Count of nonisomorphic executions: " << isocount << endl;


    //auto sync = [&](int a, int b) mutable {
    //    g.sync(a, b);
    //};

    //for (int i = 0; i < 3; ++i) {
    //    for (int j = 3; j < 6; ++j) {
    //        for (int i1 = 7; i1 < 9; ++i1) {
    //        for (int k = 7; k < 9; ++k) {

    //            auto ng = g;
    //            for (int t = 0; t < 1; ++t) {
    //                ng.sync(i, j);
    //                ng.sync(, j);

    //                f33_1(ng);
    //                f33_1(ng, 3);
    //            }


    //            cout << i << " " << j << endl;

    //        }
    //    }
    //}

   

    return EXIT_SUCCESS;
}