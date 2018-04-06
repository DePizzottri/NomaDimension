#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>
#include <unordered_set>
#include <set>
#include <mutex>

#include <job_managment.h>

atomic_int out_count{0};
atomic_int iso_count{0};
unordered_set<hash<ProcessesGraph>::result_type> cache;
vector<ProcessesGraph> result_processes;
mutex rpmut;
mutex out_mut;

void generate_graph(Queue & queue, ProcessesGraph const& g, int max_sync_num, int sync_num) {
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
        Job job = [rp = result_processes, g]{
            if (is_poset_2_dimensional(g)) {
                bool iso = false;
                for (int i = 0; i < rp.size(); ++i) {
                    auto& p = rp[i];
                    if (is_isomorphic(p, g)) {
                        iso = true;
                        iso_count++;
                        break;
                    }
                }

                if (!iso) {
                    {
                        //TODO: may dublicate
                        lock_guard<mutex> lock(rpmut);
                        result_processes.push_back(g);
                    }

                    {
                        lock_guard<mutex> lock(out_mut);
                        cout << "-" << out_count++ << "-" << endl;
                        cout << g;
                        cout << "Result network has cut vertice: " << std::boolalpha << network_have_cut_vertice(g) << endl;
                        cout << endl;
                    }
                }
            }
        };

        queue.enqueue(job);
        return;
    }

    auto proc_num = g.proc_num;
    for (int p1 = 0; p1 < proc_num; ++p1) {
        for (int p2 = p1 + 1; p2 < proc_num; ++p2) {
            auto ng = g;
            ng.sync(p1, p2);
            generate_graph(queue, ng, max_sync_num, sync_num + 1);
        }
    }
}

int main(int argc, char* argv[]) {
    cout << "Enumerate of isomorphic executions of N processes and K syncronizations (with unbounded cache opt) (with job queue opt)" << endl;

    if (argc < 3) {
        cerr << "usage: " << endl;
        cerr << "enumerate_isomorphic proc_num max_sync_num" << endl;
        return EXIT_FAILURE;
    }

    int proc_num = atoi(argv[1]);
    ProcessesGraph g;
    g.init(proc_num);

    cout << "Processes num: " << proc_num << endl;

    atomic_bool stopped{false};
    vector<thread> threads;

    Queue queue;
    for (int i = 0; i < 3; ++i)
        threads.emplace_back(
            thread([&queue, &stopped] {
                consumer(queue, stopped);
            })
        );


    generate_graph(queue, g, atoi(argv[2]), 0);

    stopped = true;

    for (auto& t : threads)
        t.join();

    {
        lock_guard<mutex> lock(out_mut);
        cout << "Count of non isomorphic graphs (of dim 2): " << out_count << " isomorphic count: " << iso_count << endl;
    }

    return EXIT_SUCCESS;
}
