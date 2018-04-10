#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

#include <unordered_set>
#include <set>
#include <string>

vector<ProcessesGraph> result_processes;
vector<int> iso_count;
int out_count = 0;

unordered_set<hash<ProcessesGraph>::result_type> cache;

unordered_set<ProcessesGraph::PEHash> cache_pe;

auto sorted = [](auto a) {
    sort(a.begin(), a.end());
    return a;
};

void generate_grouped_graph(ProcessesGraph const& g, int group_size, int group_num, int sync_num) {
    auto proc_num = g.proc_num;

    //hash<ProcessesGraph> hsh;

    //if (cache.find(hsh(g)) != cache.end()) {
    //    cerr << "bb" << endl;

    //    auto s = sorted(g.proc_sync_name);
    //    if (cache_pe.find(s) == cache_pe.end()) {
    //        cerr << "Cache PE error - not found" << endl;
    //        for (auto& a : cache_pe) {
    //            for (auto& b : a) {
    //                cerr << b << " ";
    //            }
    //            cerr << endl;
    //        }
    //    }

    //    return;
    //}

    if (cache_pe.find(sorted(g.proc_sync_name)) != cache_pe.end()) {
        return;
        //cerr << "Cache PE error - found" << endl;
        //for (auto& a : cache_pe) {
        //    for (auto& b : a) {
        //        cerr << b << " ";
        //    }
        //    cerr << endl;
        //}
    }

    //auto iso_gs = generate_all_isomorphic(g);

    //for (auto& iso_g : iso_gs) {
    //    cache.insert(hsh(iso_g));
    //}

    cache_pe.insert(sorted(g.proc_sync_name));

    //cerr << cache.size() << endl;

    if (sync_num <= 0) {
        if (is_full_syncronized(g)) {
            if (is_poset_2_dimensional(g)) {
                bool iso = false;
                for (int i = 0; i<result_processes.size(); ++i) {
                    auto& p = result_processes[i];

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
                    cout << "Result network has cut vertice: " << std::boolalpha << network_have_cut_vertice(g) << endl;
                    cout << endl;
                }
            }
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

        if (group_leaders.find(p1) != group_leaders.end()) { //p1 is group leader
            for (auto p2 : group_leaders) { //sync with others group leaders
                auto ng = g;
                if (p2 != p1) {
                    ng.sync(p1, p2);
                    generate_grouped_graph(ng, group_size, group_num, sync_num - 1);
                }
            }
        }

        //sync in group
        for (auto p2 : ingroup_processes) {
            if (p2 != p1) {
                auto ng = g;
                ng.sync(p1, p2);
                generate_grouped_graph(ng, group_size, group_num, sync_num - 1);
            }
        }

        //sync by divisibility by group size
        //for (int i = 0; i < group_num; ++i) {
        //    auto p2 = (p1 + i*group_size) % (group_size*group_num);
        //    if (p1 != p2) {
        //        auto ng = g;
        //        ng.sync(p1, p2);
        //        generate_grouped_graph(ng, group_size, group_num, sync_num - 1);
        //    }
        //}
    }
}

int main(int argc, char* argv[]) {
    cout << "Enumerate all grouped executions (network cut vertice) (with unbounded cache optimization)" << endl;

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