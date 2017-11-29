#include <process_graph.hpp>

ostream & operator<<(ostream & out, Label const& l) {
    out << "[" << l.proc << ", " << l.num << "]";
    return out;
}

ostream & operator<<(ostream & out, adjacency_list& g) {
    for (int v = 0; v < g.size(); ++v) {
        out << setw(2) << v << ": ";
        for (int i = 0; i < g[v].size(); ++i) {
            out << g[v][i] << " ";
        }
        out << endl;
    }
    return out;
}

ostream& operator << (ostream & out, ProcessesGraph const& g) {
    //out << "Proc = " << g.proc_num << " verteces = " << g.graph.size() << endl;
    //for (int v = 0; v < g.graph.size(); ++v) {
    //    out << setw(2) << v << ": " << g.labels[v] << " -- ";
    //    for (int i = 0; i < g.graph[v].size(); ++i) {
    //        out << g.graph[v][i] << " ";
    //    }
    //    out << endl;
    //}

    out << "Procsses num: " << g.proc_num << endl;
    out << "Syncronizations: " << endl;

    for (auto& s : g.syncs) {
        out << s.first << " " << s.second << endl;
    }
    out << endl;

    return out;
}