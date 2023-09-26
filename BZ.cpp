#include <set>
#include "BZ.h"
#include "zdd.h"    
#include "zdd_generators.h"



zdd* BZIteration(ZddManager* m, zdd* minimal_diagnoses, std::set<int> new_conflict)
{
    if(minimal_diagnoses == empty())
        return empty();

    zdd* old = minimal_diagnoses;
    zdd* add = empty();

    std::set<int> first_loop_set_location;

    auto gen_loop1 = zdd_with_constraint_1(old, &first_loop_set_location, &new_conflict);

    while(gen_loop1.move_next()){
        old = zdd_difference(m, old, single_set_to_zdd(m, first_loop_set_location));
        for (int c : new_conflict)
        {
            std::set<int> new_set = std::set<int>(first_loop_set_location);
            new_set.insert(c);
            bool add_new_set = true;
            std::set<int> second_loop_location;
            auto loop2_gen = zdd_with_constraint_2(minimal_diagnoses, &second_loop_location, &new_set);
            while(loop2_gen.move_next())
            {
                if(second_loop_location != first_loop_set_location)
                {
                    add_new_set = false;
                    break;
                }
            }
            if(add_new_set)
            {
                add = zdd_union(m, add, single_set_to_zdd(m, new_set));
            }
        } 
    
    }
    zdd* result = zdd_union(m, old, add);
    mark_zdd_tree(m, result);
    delete_unmarked_nodes(m);
    m->cache.clear();
    return result;
}

zdd* BZ(std::set<std::set<int>>* problem)
{
    ZddManager m = ZddManager(100, 1000);
    zdd* MHSes = empty();
    for (auto set : *problem)
    {
        if(MHSes == empty()){
            for(int i : set)
                MHSes = zdd_union(&m, MHSes, single_set_to_zdd(&m, {i}));
        }
        else{
            MHSes = BZIteration(&m, MHSes, set);
        }
    }
    return MHSes;
}