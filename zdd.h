#if !defined(ZDD_H)
#define ZDD_H

#include <set>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <tuple>
#include <boost/unordered_map.hpp>
#include <boost/compute/detail/lru_cache.hpp>

struct zdd {
    int value;
    bool marked;
    zdd* left;
    zdd* right;

    bool operator==(const zdd a) const
    {
        return (value == a.value && left == a.left && right == a.right);
    }
};

enum cache_function {union_cache, diff_cache, inter_cache, sub0_cache, sub1_cache, change_cache};
class ZddManager {
    public:
        ZddManager(size_t init_unique_list_size=5000, size_t cache_size=5000){
            cache = boost::compute::detail::lru_cache<std::tuple<intptr_t,intptr_t,cache_function>,zdd*>(cache_size);
            unique_nodes.reserve(init_unique_list_size);
            unique_nodes = {};
        };
        boost::unordered_map<std::tuple<int,zdd*,zdd*>,zdd*> unique_nodes = {};
        boost::compute::detail::lru_cache<std::tuple<intptr_t,intptr_t,cache_function>,zdd*> cache = boost::compute::detail::lru_cache<std::tuple<intptr_t,intptr_t,cache_function>,zdd*>(1000000);
        std::unordered_set<zdd*> mark_tree_visited = {};
};

zdd* empty();
zdd* base();
zdd* get_node(ZddManager* m, int value, zdd* P0, zdd* P1);
void mark_zdd_tree(ZddManager* m, zdd* P);
void delete_unmarked_nodes(ZddManager* m);
zdd* zdd_subset1(ZddManager* m, zdd* P, int var);
zdd* zdd_subset0(ZddManager* m, zdd* P, int var);
zdd* zdd_change(ZddManager* m, zdd* P, int var);
zdd* zdd_union(ZddManager* m, zdd* P, zdd* Q);
zdd* zdd_intersection(ZddManager* m, zdd* P, zdd* Q);
zdd* zdd_difference(ZddManager* m, zdd* P, zdd* Q);
int zdd_count(zdd* P);
std::set<std::set<int>> to_set_of_sets(zdd* P);
zdd* to_zdd(ZddManager* m, std::set<std::set<int>> set_of_sets);
zdd* single_set_to_zdd(ZddManager* m, std::set<int> set_of_ints);
std::set<int> smallest_set(ZddManager* m, zdd* P);

#endif // ZDD_H

