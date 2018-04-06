#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>
#include <unordered_set>
#include <set>

int out_count = 0;

unordered_set<hash<ProcessesGraph>::result_type> cache;

vector<ProcessesGraph> result_processes;

void generate_graph(ProcessesGraph const& g, int max_sync_num, int sync_num) {
    if (sync_num > max_sync_num) {
        return;
    }

    hash<ProcessesGraph> hsh;

    if (cache.find(hsh(g)) != cache.end()) {
        return;
    }

    auto iso_gs = generate_all_isomorphic(g);

    for (auto& iso_g: iso_gs) {
        cache.insert(hsh(iso_g));
    }

    if (is_full_syncronized(g)) {
        if (is_poset_2_dimensional(g)) {
            bool iso = false;
            for (int i = 0; i<result_processes.size(); ++i) {
                auto& p = result_processes[i];
                if (is_isomorphic(p, g)) {
                    iso = true;
                    break;
                }
            }

            if (!iso) {
                result_processes.push_back(g);

                cout << "-" << out_count++ << "-" << endl;
                cout << g;
                cout << "Result network has cut vertice: " << std::boolalpha << network_have_cut_vertice(g) << endl;
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
    cout << "Enumerate of isomorphic executions of N processes and K syncronizations (with unbounded cache optimization)" << endl;

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

    cout << "Count of non isomorphic graphs (of dim 2): " << out_count << endl;

    //ProcessesGraph a, b;
    //a.init(4);
    //b.init(4);

    //a.sync(0, 1);
    //a.sync(2, 3);
    //a.sync(0, 2);
    //a.sync(0, 1);
    //a.sync(2, 3);
    //
    //b.sync(0, 1);
    //b.sync(2, 3);
    //b.sync(0, 2);
    //b.sync(2, 3);
    //b.sync(0, 1);

    //for (auto& c : generate_all_isomorphic(a)) {
    //    hash<ProcessesGraph> h;
    //    //cout << c << endl;
    //    //cout << boolalpha << (h(c) == h(b)) << endl;
    //    cout << boolalpha << is_isomorphic(c, b) << endl;
    //}

    return EXIT_SUCCESS;
}