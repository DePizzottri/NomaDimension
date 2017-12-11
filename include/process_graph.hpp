#ifndef NOMA_DIMENSION_GRAPH_INCLUDED
#define NOMA_DIMENSION_GRAPH_INCLUDED

#include <vector>
#include <ostream>
#include <cassert>
#include <iomanip>
#include <unordered_set>

using namespace std;

struct Label {
    int proc;
    int num;
};

inline ostream & operator<<(ostream & out, Label const& l);

typedef vector<vector<int>> adjacency_list;

inline ostream & operator<<(ostream & out, adjacency_list& g);

class ProcessesGraph {
public:
    int proc_num;

    vector<vector<int>> graph;
    vector<int> proc_last_vertex;
    int next_vertex;

    vector<unordered_set<int>> network;
    
    vector<Label> labels;

    vector<vector<int>> proc_verteces;

    vector<pair<int, int>> syncs;

    void init(int proccess_num) {
        graph.clear();
        proc_num = proccess_num;
        proc_last_vertex.clear();
        next_vertex = 0;
        labels.clear();
        proc_verteces.clear();

        proc_last_vertex.resize(proc_num);
        proc_verteces.resize(proc_num);

        //auto zero_vertex = next_vertex++;
        //graph.push_back({});
        //labels.push_back({ -1, -1 });

        for (int i = 0; i < proc_num; ++i) {
            //create first vertex
            auto cur_vert = next_vertex++;
            graph.push_back({});
            proc_last_vertex[i] = cur_vert;
            labels.push_back({i, 0});
            proc_verteces[i].push_back(cur_vert);
            //initial vertex
            //update(i);

            //graph[zero_vertex].push_back(cur_vert);
        }

        network.clear();
        network.resize(proc_num);
    }

    void update(int proc) {
        add_vertex_to_proc(proc);
    }

    void sync(int proc1, int proc2) {
        assert(proc1 < proc_num);
        assert(proc1 >= 0);

        assert(proc2 < proc_num);
        assert(proc2 >= 0);

        syncs.emplace_back(proc1, proc2);

        int p1 = proc_last_vertex[proc1];
        int p2 = proc_last_vertex[proc2];

        ////add vertex to first process
        //auto v1 = add_vertex_to_proc(proc1);

        ////add vertex to second process
        //auto v2 = add_vertex_to_proc(proc2);

        ////sync
        //graph[p1].push_back(p2);
        //graph[p2].push_back(p1);
        
        auto new_vertex = next_vertex++;
        graph.push_back({});

        auto p1nv = next_vertex++;
        graph.push_back({});

        auto p2nv = next_vertex++;
        graph.push_back({});

        graph[p1].push_back(new_vertex);
        graph[p2].push_back(new_vertex);
        
        graph[new_vertex].push_back(p1nv);
        graph[new_vertex].push_back(p2nv);

        proc_last_vertex[proc1] = p1nv;
        proc_last_vertex[proc2] = p2nv;

        proc_verteces[proc1].push_back(p1nv);
        proc_verteces[proc2].push_back(p2nv);

        labels.push_back({ -1, -1 });

        labels.push_back({ proc1, labels[p1].num + 1 });
        labels.push_back({ proc2, labels[p2].num + 1 });

        network[proc1].insert(proc2);
        network[proc2].insert(proc1);
    }

private:

    int add_vertex_to_proc(int proc) {
        assert(proc < proc_num);
        assert(proc >= 0);

        auto cur_vert = proc_last_vertex[proc];
        auto cur_vert_label = labels[cur_vert];

        auto new_vertex = next_vertex++;
        graph.push_back({});
        graph[cur_vert].push_back(new_vertex);
        proc_last_vertex[proc] = new_vertex;
        labels.push_back({ proc, cur_vert_label.num + 1 });
        
        proc_verteces[proc].push_back(new_vertex);

        return new_vertex;
    }
};

ostream& operator << (ostream & out, ProcessesGraph const& g);

//void draw(ProcessesGraph const& g) {
//    int factor = 4;
//    int proc_num = g.proc_num;
//    char** m = new char*[(proc_num-1)*factor];
//    for (int i = 0; i < (proc_num - 1) * factor; ++i) {
//        m[i] = new char[g.graph.size() * factor];
//    }
//
//    vector<pair<int, int>> coordinates(g.graph.size());
//
//    for (int p = 0; p < proc_num; ++p) {
//        int cnt = 0;
//        for (auto& v : g.proc_verteces[p]) {
//            m[p*factor][cnt] = '*';
//            coordinates[v] = { p*factor, cnt };
//            for (int i = 0; i < factor; ++i, ++cnt) {
//                m[p*factor][cnt] = '-';
//            }
//        }
//    }
//
//    for (int v = 0; v < g.graph.size(); ++v) {
//        if (g.graph[v].size()) {
//            for (auto& u : g.graph[v]) {
//                if (g.labels[v].proc != g.labels[u].proc) {
//                    auto c1 = coordinates[v];
//                    auto c2 = coordinates[u];
//                }
//            }
//        }
//    }
//
//    for (int i = 0; i < (proc_num - 1) * 5; ++i) {
//        delete[] m[i];
//    }
//    delete[]m;
//}


#endif //NOMA_DIMENSION_GRAPH_INCLUDED