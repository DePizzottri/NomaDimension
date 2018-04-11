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
        auto cps = find_critical_pairs(matrix, (int)g.size());

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

bool check_poset(ProcessesGraph const& g) {
    auto matrix = make_graph_matrix(g);

    //calc (weighted) transitive closure
    int n = (int)g.graph.size();
    floyd(matrix, n);

    auto critical_pairs = find_critical_pairs(matrix, n);

    for(int i = 0; i<critical_pairs.size(); ++i) {
        auto& cp = critical_pairs[i];
        cout <<i<< ": "<< cp.x << " " << cp.y << endl;
    }

    delete[] matrix;

    return check_critical_pairs_graph(g.graph, critical_pairs);
}

ostream& operator<<(ostream & out, Label const& l) {
    out << "[" << l.proc << ", " << l.num << "]";
    return out;
}

void print_pg(ostream & out, ProcessesGraph const& g) {
    out << "Proc = " << g.proc_num << " verteces = " << g.graph.size() << endl;
    for (int v = 0; v < g.graph.size(); ++v) {
        out << setw(2) << v << ": " << g.labels[v] << " -- ";
        for (int i = 0; i < g.graph[v].size(); ++i) {
            out << g.graph[v][i] << " ";
        }
        out << endl;
    }
}

int main(int argc, char* argv[]) {
    cout << "Check some hardcoded poset for been dimension 2" << endl;

    ProcessesGraph g;
    g.init(4);

    g.sync(0, 1);
    g.sync(1, 2);
    g.sync(2, 3);
    g.sync(0, 2);

    print_pg(cout, g);

    if (!check_poset(g)) {
        cout <<"Poset is not 2 dimentsional"<<endl;
    }

    return EXIT_SUCCESS;
}