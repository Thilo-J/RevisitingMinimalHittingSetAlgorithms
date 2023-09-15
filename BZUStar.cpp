#include <set>
#include "BZUStar.h"
#include "zdd.h"
#include "generator.hpp"

template<class T> // Intelisens not working properly. This is a workaround
generator<bool> zdd_sets(zdd* node, std::set<int>* current_set)
{
    if (node == empty()) {

    }
    else if (node == base())
    {
        co_yield true;
    }
    else {
        co_yield zdd_sets<T>(node->left, current_set);
        current_set->insert(node->value);
        co_yield zdd_sets<T>(node->right, current_set);
        current_set->erase(node->value);
    }
}

template<class T> // Intelisens not working properly. This is a workaround
generator<bool> zdd_with_constraint_1(zdd* node, std::set<int>* current_set, std::set<int>* new_conflict)
{
    if (node == empty()) {

    }
    else if (node == base())
    {
        co_yield true;
    }
    else {
        co_yield zdd_with_constraint_1<T>(node->left, current_set, new_conflict);       
        if(!new_conflict->contains(node->value)){
            current_set->insert(node->value);
            co_yield zdd_with_constraint_1<T>(node->right, current_set, new_conflict);
            current_set->erase(node->value);
        }
    }
}


zdd* BZUStarIteration(ZddManager* m, zdd* minimal_diagnoses, std::set<int> new_conflict)
{
    if(minimal_diagnoses == empty())
        return empty();

    zdd* old = minimal_diagnoses;
    zdd* add = empty();

    std::set<int> first_loop_set_location;

    auto loop1 = zdd_with_constraint_1<char>(old, &first_loop_set_location, &new_conflict);

    while(loop1.move_next()){
        old = zdd_difference(m, old, single_set_to_zdd(m, first_loop_set_location));
        for (int c : new_conflict)
        {
            std::set<int> new_set = std::set<int>(first_loop_set_location);
            new_set.insert(c);
            bool add_new_set = true;
            std::set<int> second_loop_location;
            auto loop2 = zdd_sets<char>(minimal_diagnoses, &second_loop_location);
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
    zdd* result = zdd_union(m, old, add);
    mark_zdd_tree(m, result);
    delete_unmarked_nodes(m);
    m->cache.clear(); //TODO Check if possible to remove. Currently not because of delete_unmarked_nodes
    return result;
}

zdd* BZUStar(std::set<std::set<int>> problem)
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
            MHSes = BZUStarIteration(&m, MHSes, set);
        }
    }
    return MHSes;
}