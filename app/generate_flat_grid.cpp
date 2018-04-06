#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

#include <functional>
#include <algorithm>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "Generate flat grid" << endl;

    if (argc < 3) {
        cerr << "Usage: exec <proc_num> <max_depth>" << endl;
        return 1;
    }

    auto proc_num = stoi(argv[1]);
    auto max_depth = stoi(argv[2]);

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
 
    return EXIT_SUCCESS;
}