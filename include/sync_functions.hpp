#ifndef NOMA_DIMENSION_SYNC_FUNCTIONS_INCLUDED
#define NOMA_DIMENSION_SYNC_FUNCTIONS_INCLUDED

#include <process_graph.hpp>

//4 process executions with its number

void f45_10(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 3);
    g.sync(1, 2);
}

void f45_9(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 1);
    g.sync(2, 3);
}

void f45_9_p(ProcessesGraph & g, vector<int> const& perm) {
    assert(g.proc_num == 4);
    assert(perm.size() == 4);
    g.sync(perm[0], perm[1]);
    g.sync(perm[2], perm[3]);
    g.sync(perm[0], perm[2]);
    g.sync(perm[0], perm[1]);
    g.sync(perm[2], perm[3]);
}

void f45_8(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(1, 2);
    g.sync(1, 3);
    g.sync(2, 3);
    g.sync(0, 3);
}

void f45_8_p(ProcessesGraph & g, vector<int> const& perm) {
    assert(g.proc_num == 4);
    assert(perm.size() == 4);
    g.sync(perm[0], perm[1]);
    g.sync(perm[1], perm[2]);
    g.sync(perm[1], perm[3]);
    g.sync(perm[2], perm[3]);
    g.sync(perm[0], perm[3]);
}

void f45_7(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(2, 3);
    g.sync(0, 3);
    g.sync(1, 3);
}

void f45_6(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(2, 3);
    g.sync(0, 3);
    g.sync(0, 1);
}

void f45_5(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(1, 2);
}

void f45_4(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 1);
}

void f45_3(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(0, 3);
    g.sync(2, 3);
    g.sync(1, 2);
}

void f45_2(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(0, 3);
    g.sync(0, 2);
    g.sync(1, 2);
}

void f45_1(ProcessesGraph & g) {
    assert(g.proc_num == 4);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(0, 3);
    g.sync(0, 2);
    g.sync(0, 1);
}

//5-process executions with its number

void f57_36(ProcessesGraph & g) {
    assert(g.proc_num == 5);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(3, 4);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 1);
    g.sync(3, 4);
}

//grouped executions

void f69_XX(ProcessesGraph & g) {
    //two 3-groups with bridge
    assert(g.proc_num == 6);
    g.sync(4, 5);
    g.sync(0, 1);
    g.sync(0, 2);
    g.sync(3, 4);
    g.sync(2, 3);
    g.sync(0, 2);
    g.sync(0, 1);
    g.sync(3, 4);
    g.sync(4, 5);
}

void f68_1(ProcessesGraph & g) {
    //three two groups with 2 briges (1-2, 3-4)
    assert(g.proc_num == 6);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
    //1 brige
    g.sync(1, 2);
    //2 brige
    g.sync(3, 4);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
}

void f68_2(ProcessesGraph & g) {
    //three two groups with 3 briges (1-2, 1-4, 2-4)
    assert(g.proc_num == 6);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
    //1 brige from (0,1) to (2,3)
    g.sync(1, 2);
    //and fully sync(0,1) and (2,3)
    g.sync(0, 1);
    g.sync(2, 3);
    //2 brige from (0,1) to (4,5)
    //g.sync(1, 4);
    //and sync (4,5) and (0,1)
    //g.sync(0, 1);
    //g.sync(4, 5);

    //3 brige from (2,3) to (4,5)
    g.sync(2, 4);
    g.sync(2, 3);
    g.sync(4, 5);

    //2 brige from (0,1) to (4,5)
    //g.sync(1, 4);

    //parallel sync
    //g.sync(0, 1);
    //g.sync(2, 3);
    //g.sync(4, 5);
}


#endif //NOMA_DIMENSION_SYNC_FUNCTIONS_INCLUDED