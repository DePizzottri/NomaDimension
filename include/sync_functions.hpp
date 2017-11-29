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
    //IS OK (LINE-GRID)
    //three two-process groups (0,1) (2,3) (4,5) with 2 briges (0-2, 3-4)
    assert(g.proc_num == 6);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
    //1 brige
    g.sync(0, 2);
    //2 brige
    g.sync(3, 4);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
}

void f68_1_2(ProcessesGraph & g) {
    //NOT OK (GROUP LINE-GRID)
    //three two-process groups (0,1) (2,3) (4,5) with 3 briges (0-2, 0-4)
    //assert(g.proc_num == 6);
    //parallel sync
    g.sync(0, 1);
    g.sync(1, 2);
    g.sync(2, 3);
    g.sync(3, 0);


    //g.sync(4, 5);
    //g.sync(5, 0);
    //1 brige
    //g.sync(1, 2);
    //2 brige
    //g.sync(3, 4);
    //3 brige
    //g.sync(5, 0);
    //parallel sync
    //g.sync(0, 1);
    //g.sync(2, 3);
    //g.sync(4, 5);
}

void f68_2_1(ProcessesGraph & g) {
    //OK - group GRID
    //three two-process groups (0,1) (2,3) (4,5), with 3 briges (0-2), (0-4), (0-2)
    assert(g.proc_num == 6);
    //parallel sync
    g.sync(0, 1);
    g.sync(2, 3);
    g.sync(4, 5);
    //1st brige from (0,1) to (2,3)
    g.sync(0, 2);

    //inner sync 1st group
    g.sync(0, 1);
    //g.sync(2, 3);

    //2nd brige from (0,1) to (4,5)
    g.sync(0, 4);

    //inner sync 1st group
    g.sync(0, 1);
    //g.sync(4, 5);

    //3rd brige from (0,1) to (2,3)
    g.sync(0, 2);

    g.sync(4, 5);
    g.sync(2, 3);
}

//void f68_2_2(ProcessesGraph & g) {
//    //NOT OK ???
//    //three two-process groups (0,1) (2,3) (4,5), with 3 briges (0-2, 0-4, 2-4)
//    assert(g.proc_num == 6);
//    //parallel sync
//    //g.sync(0, 1);
//    //g.sync(2, 3);
//    //g.sync(4, 5);
//    //1st brige from (0,1) to (2,3)
//
//    //g.sync(0, 4);
//    //g.sync(2, 4);
//    //g.sync(0, 2);
//
//    //inner sync 1st and 2nd group
//    //g.sync(0, 1);
//    //g.sync(2, 3);
//
//    //2rd bridge
//    //g.sync(2, 4);   
//
//    //g.sync(2, 3); 
//    //g.sync(4, 5);
//
//    //3nd brige from (0,1) to (4,5)
//    //g.sync(0, 4);
//
//    //g.sync(4, 5);
//    //g.sync(0, 1);
//
//    //g.sync(0, 1);
//    //g.sync(2, 3);
//    //g.sync(4, 5);
//
//    vector<pair<int, int>> s = {
//        { 0, 1 },
//        { 0, 1 },
//        { 0, 2 },
//        { 0, 4 },
//        { 2, 3 },
//        { 2, 3 },
//        { 2, 4 },
//        { 4, 5 },
//        { 4, 5 }
//    };
//
//    //for (auto i = s.rbegin(); i != s.rend(); ++i)
//    //    g.sync(i->first, i->second);
//    //process_graph(g);
//    do {
//        auto ng = g;
//        for (auto& p : s)
//            ng.sync(p.first, p.second);
//        process_graph(ng);
//    } while (next_permutation(s.begin(), s.end()));
//}

void f68_3(ProcessesGraph & g) {
    //NOT OK
    //LINE-GRID - OK
    //BUT OK IF "BAD"-3 group start from inner sync 
    //two 3-process groupes (0,1,2) (3,4,5) with one bridge (0,3)
    assert(g.proc_num == 6);
    //full sync group 1
    g.sync(0, 1);
    g.sync(1, 2);
    g.sync(0, 1);
    //full sync group 2
    g.sync(3, 5);
    g.sync(4, 5);
    g.sync(3, 4);

    //bridge
    g.sync(0, 3);

    //full sync group 1
    g.sync(1, 2);
    g.sync(0, 1);
    g.sync(0, 2);
    ////full sync group 2
    g.sync(4, 5);
    g.sync(3, 5);
    //g.sync(3, 4);
}

void f5_3_2(ProcessesGraph & g) {
    //OK, if 3-group start from inner sync
    //two 2-3-process groupes (0,1) (2,3,4) with one bridge (0,3)
    assert(g.proc_num == 5);
    //full sync group 1
    g.sync(0, 1);
    //full sync group 2
    g.sync(3, 4);
    g.sync(2, 3);
    g.sync(2, 4);

    //bridge
    g.sync(0, 2);

    //full sync group 1
    g.sync(0, 1);
    //full sync group 2
    g.sync(3, 4);
    g.sync(2, 3);
    g.sync(2, 4);
}

void f6_2_2_2_g(ProcessesGraph & g) {
    //2x3 grid (0,1) (2,3) (4,5) - (0,2,4) (1,3,5)
    //three two-process groups, connects in 3x grid
    //l-grid only

    assert(g.proc_num == 6);
    //full connect 3-p group 1
    g.sync(0, 2);
    g.sync(2, 4);
    g.sync(0, 2);
}

#endif //NOMA_DIMENSION_SYNC_FUNCTIONS_INCLUDED