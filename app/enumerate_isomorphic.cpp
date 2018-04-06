#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

vector<ProcessesGraph> result_processes;
vector<int> iso_count;

int out_count = 0;

void generate_graph(ProcessesGraph const& g, int max_sync_num, int sync_num) {
    if (sync_num > max_sync_num) {
        return;
    }

    if (is_full_syncronized(g)) {
        if (is_poset_2_dimensional(g)) {
            bool iso = false;
            //for (auto& p : result_processes) {
            for (int i = 0; i<result_processes.size(); ++i) {
                auto& p = result_processes[i];
                //if (is_isomorphic(p, pg) != is_isomorphic(pg, p)) {
                //    cout << "ERR" << endl;
                //}
        
                if (is_isomorphic(p, g)) {
                    iso_count[i]++;
                    iso = true;
                    break;
                }
            }
        
            if (!iso) {
                iso_count.push_back(1);
                result_processes.push_back(g);
        
                cout << "-" << out_count++ << "-" << endl;
                cout << g;
                cout << "Result network has cut vertice: "<< std::boolalpha << network_have_cut_vertice(g) << endl;
                cout << endl;
            }
        }
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
    cout << "Enumerate of isomorphic executions of N processes and K syncronizations" << endl;

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