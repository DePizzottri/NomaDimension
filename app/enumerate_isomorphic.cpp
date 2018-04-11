#include <iostream>

#include <process_graph.hpp>
#include <floyd.hpp>
#include <sync_functions.hpp>
#include <utils.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

#include <boost/fiber/all.hpp>

namespace bf = boost::fibers;

using allocator_type = boost::fibers::fixedsize_stack;

//const uint32_t max_cache_size =   1000000; //~295Mb
const uint32_t max_cache_size =     40000000; //~Mb
//const uint32_t max_cache_size =     100000000; //>30~Gb
//const uint32_t max_cache_size =     400000000; //Gb

atomic_int out_count{ 0 };
atomic_int iso_count{ 0 };
atomic_int fork_count{ 0 };

//unordered_set<hash<ProcessesGraph>::result_type> cache;

struct CacheEntry{
    hash<ProcessesGraph>::result_type   hash;
    uint32_t time;
};

atomic_uint32_t current_time{ 0 };

//typedef boost::multi_index_container<
//    CacheEntry,
//    boost::multi_index::indexed_by<
//        boost::multi_index::hashed_unique<BOOST_MULTI_INDEX_MEMBER(CacheEntry, hash<ProcessesGraph>::result_type, hash)>
//        ,boost::multi_index::ordered_unique<BOOST_MULTI_INDEX_MEMBER(CacheEntry, uint32_t, time), std::less<uint32_t>>
//    >
//> cache_t;
//
//cache_t cache;

using PEHash = ProcessesGraph::PEHash;
struct CacheEntryPE {
    PEHash hash;
    uint32_t time;
};

typedef boost::multi_index_container<
    CacheEntryPE,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<BOOST_MULTI_INDEX_MEMBER(CacheEntryPE, PEHash, hash), std::hash<PEHash>>,
        boost::multi_index::ordered_unique<BOOST_MULTI_INDEX_MEMBER(CacheEntryPE, uint32_t, time), std::less<uint32_t>>
    >
> cache_pe_t;

cache_pe_t cache_pe;

//unordered_set<ProcessesGraph::PEHash> cache_pe;

vector<ProcessesGraph> result_processes;
bf::mutex cache_mut;
bf::mutex rp_mut;
bf::mutex out_mut;

void generate_graph(allocator_type & salloc, ProcessesGraph g, int max_sync_num, int sync_num) {
    if (sync_num > max_sync_num) {
        return;
    }

    auto sorted = [](auto a) {
        sort(a.begin(), a.end());
        return a;
    };


    //hash<ProcessesGraph> hsh;
    {
        lock_guard<bf::mutex> lock(cache_mut);

        if (cache_pe.get<0>().find(sorted(g.proc_sync_name)) != cache_pe.get<0>().end()) {
            return;
        }

        //if (cache.get<0>().find(hsh(g)) != cache.get<0>().end()) {
        //    return;
        //}

        //if(cache.get<0>().find(hsh(g)) != cache.get<0>().end()) {
        //    auto s = sorted(g.proc_sync_name);
        //    if (cache_pe.get<0>().find(s) == cache_pe.get<0>().end()) {
        //        cerr << "Cache PE error - not found" << endl;
        //        for (auto& a : cache_pe.get<0>()) {
        //            for (auto& b : a.hash) {
        //                cerr << b << " ";
        //            }
        //            cerr << endl;
        //        }
        //    }

        //    return;
        //}

        //if (cache_pe.get<0>().find(g.proc_sync_name) != cache_pe.get<0>().end()) {
        //    cerr << "Cache PE error - found" << endl;
        //}
    }

    //auto iso_gs = generate_all_isomorphic(g);
    
    {
        lock_guard<bf::mutex> lock(cache_mut);

        //for (auto& iso_g : iso_gs) {
        //    cache.insert({ hsh(iso_g), current_time++ });
        //}

        //if (cache.size() + iso_gs.size() > max_cache_size) {
        //    auto& time_idx = cache.get<1>();
        //    for (int i = 0; i < iso_gs.size(); ++i) {
        //        time_idx.erase(time_idx.begin());
        //    }
        //}

        //for (auto& iso_g : iso_gs) {
        //    if (!is_equal_by_sync_name(g, iso_g)) {
        //        cerr << "Not equal by sync name!" << endl;
        //    }

        //    if (!is_equal_by_sync_name(iso_g, g)) {
        //        cerr << "Not equal by sync name!" << endl;
        //    }

        //    cache.insert({ hsh(iso_g), current_time++ });
        //}

        if (cache_pe.size() + 1 > max_cache_size) {
            auto& time_idx = cache_pe.get<1>();
            for(int i = 0; i<5; ++i)
                time_idx.erase(time_idx.begin());
        }

        cache_pe.insert({ sorted(g.proc_sync_name), current_time++ });
    }

    if (is_full_syncronized(g)) {
        //bf::fiber{
        //    bf::launch::dispatch,
        //    [rp = result_processes, g = std::move(g)]() {
                if (is_poset_2_dimensional(g)) {
                    bool iso = false;
                    for (int i = 0; i < result_processes.size(); ++i) {
                        auto& p = result_processes[i];
                        if (is_isomorphic(p, g)) {
                            iso = true;
                            iso_count++;
                            break;
                        }
                    }

                    if (!iso) {
                        {
                            //TODO: there are dublicates may appear
                            lock_guard<bf::mutex> lock(rp_mut);
                            result_processes.push_back(g);
                        }

                        {
                            lock_guard<bf::mutex> lock(out_mut);
                            cout << "-" << out_count++ << "-" << endl;
                            cout << g;
                            cout << "Result network has cut vertice: " << std::boolalpha << network_have_cut_vertice(g) << endl;
                            cout << endl;
                        }
                    }
                }
        //    }
        //}.join();
      
        return;
    }

    auto proc_num = g.proc_num;
    for (int p1 = 0; p1 < proc_num; ++p1) {
        for (int p2 = p1 + 1; p2 < proc_num; ++p2) {
            auto ng = g;
            ng.sync(p1, p2);

            if (fork_count < 4) {
                //cerr << "Fork" << endl;
                fork_count++;
                bf::fiber{
                    bf::launch::dispatch,
                    std::allocator_arg, salloc,
                    generate_graph,
                    std::ref(salloc), std::move(ng), max_sync_num, sync_num + 1
                }.join();
                fork_count--;
            }
            else {
                generate_graph(salloc, std::move(ng), max_sync_num, sync_num + 1);
            }
        }
    }
}

#include <barrier.hpp>

int main(int argc, char* argv[]) {
    try {
        cout << "Enumerate of isomorphic executions of N processes and K syncronizations (with bounded cache opt) (with fibers opt)" << endl;

        if (argc < 3) {
            cerr << "usage: " << endl;
            cerr << "enumerate_isomorphic proc_num max_sync_num" << endl;
            return EXIT_FAILURE;
        }

        auto proc_num = atoi(argv[1]);
        auto sync_num = atoi(argv[2]);

        ProcessesGraph g;
        g.init(proc_num);
        cout << "Processes num: " << proc_num << " Sycncronizations num: " << sync_num << endl;

        //start fibers working threads
        std::uint32_t thread_count = std::thread::hardware_concurrency();
        boost::fibers::use_scheduling_algorithm< boost::fibers::algo::work_stealing >(thread_count);
        barrier b{ thread_count };

#if BOOST_OS_WINDOWS || BOOST_OS_BSD
        allocator_type salloc{ 2 * allocator_type::traits_type::page_size() * 4};
#else
        allocator_type salloc{ allocator_type::traits_type::page_size() * 4};
#endif
        bool done = false;
        std::mutex mtx{};
        boost::fibers::condition_variable_any cnd{};

        std::vector< std::thread > threads;
        for (std::uint32_t i = 1; i < thread_count; ++i) {
            // spawn thread
            threads.emplace_back([thread_count, &b, &mtx, &done, &cnd] {
                boost::fibers::use_scheduling_algorithm< boost::fibers::algo::work_stealing >(thread_count);
                b.wait();
                unique_lock<std::mutex> lk(mtx);
                cnd.wait(lk, [&done]() { return done; });
            });
        }
        b.wait();

        //generate_graph(salloc, g, sync_num, 0);

        bf::fiber{
            bf::launch::dispatch,
            std::allocator_arg, salloc,
            generate_graph,
            std::ref(salloc), std::move(g), sync_num, 0
        }.join();

        unique_lock<std::mutex> lk(mtx);
        done = true;
        lk.unlock();
        cnd.notify_all();
        for (std::thread & t : threads) {
            t.join();
        }

        {
            lock_guard<bf::mutex> lock(out_mut);
            cout << "Count of non isomorphic graphs (of dim 2): " << out_count << " isomorphic count: " << iso_count << endl;
        }

        return EXIT_SUCCESS;
    }
    catch (std::exception const& e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "unhandled exception" << std::endl;
    }

    return EXIT_FAILURE;
}
