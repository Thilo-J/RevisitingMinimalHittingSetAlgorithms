#include <set>
#include "BZU.h"
#include "zdd.h"
#include "zdd_generators.h"


zdd* BZUIteration(ZddManager* m, zdd* minimal_diagnoses, std::set<int> new_conflict)
{
    if(minimal_diagnoses == empty())
        return empty();

    zdd* old = minimal_diagnoses;
    zdd* add = empty();

    std::set<int> first_loop_set_location;

    auto loop1 = zdd_sets(old, &first_loop_set_location);

    while(loop1.move_next()){
        std::set<int> intersection;
        set_intersection(first_loop_set_location.begin(), first_loop_set_location.end(), new_conflict.begin(), new_conflict.end(), inserter(intersection, intersection.end()));
        if (intersection.size() == 0)
        {
            old = zdd_difference(m, old, single_set_to_zdd(m, first_loop_set_location));
            for (int c : new_conflict)
            {
                std::set<int> new_set = std::set<int>(first_loop_set_location);
                new_set.insert(c);
                bool add_new_set = true;
                std::set<int> second_loop_location;
                auto loop2 = zdd_sets(minimal_diagnoses, &second_loop_location);
                while(loop2.move_next())
                {
                    if(second_loop_location != first_loop_set_location)
                    {
                        if (includes(new_set.begin(), new_set.end(), second_loop_location.begin(), second_loop_location.end()))
                        {
                            add_new_set = false;
                            break;
                        }
                    }
                }
                if(add_new_set)
                {
                    add = zdd_union(m, add, single_set_to_zdd(m, new_set));
                }
            } 
        }
    }
    zdd* result = zdd_union(m, old, add);
    mark_zdd_tree(m, result);
    delete_unmarked_nodes(m);
    m->cache.clear(); //TODO Check if possible to remove. Currently not because of delete_unmarked_nodes
    return result;
}

zdd* BZU(std::set<std::set<int>> problem)
{
    ZddManager m = ZddManager(100, 1000);
    zdd* MHSes = empty();
    for (auto set : problem)
    {
        if(MHSes == empty()){
            for(int i : set)
                MHSes = zdd_union(&m, MHSes, single_set_to_zdd(&m, {i}));
        }
        else{
            MHSes = BZUIteration(&m, MHSes, set);
        }
    }
    return MHSes;
}