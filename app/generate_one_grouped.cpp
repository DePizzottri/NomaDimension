#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

#include <unordered_set>

bool generate_one_graph(ProcessesGraph const& g, int group_size, int group_num, int sync_num) {
    auto proc_num = g.proc_num;

    if (!is_poset_2_dimensional(g))
        return false;

    if (sync_num <= 0) {
        if (is_full_syncronized(g)) {
            if (is_poset_2_dimensional(g)) {
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