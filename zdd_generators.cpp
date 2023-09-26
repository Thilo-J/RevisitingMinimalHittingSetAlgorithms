#include <set>
#include "zdd_generators.h"
#include "zdd.h"
#include "generator.hpp"

generator<std::set<int>*> zdd_sets(zdd* node, std::set<int>* current_set)
{
    if (node == empty()) {

    }
    else if (node == base())
    {
        co_yield current_set;
    }
    else {
        co_yield zdd_sets(node->left, current_set);
        current_set->insert(node->value);
        co_yield zdd_sets(node->right, current_set);
        current_set->erase(node->value);
    }
}

generator<std::set<int>*> zdd_with_constraint_1(zdd* node, std::set<int>* current_set, std::set<int>* new_conflict)
{
    if (node == empty()) {

    }
    else if (node == base())
    {
        co_yield current_set;
    }
    else {
        co_yield zdd_with_constraint_1(node->left, current_set, new_conflict);       
        if(!new_conflict->contains(node->value)){
            current_set->insert(node->value);
            co_yield zdd_with_constraint_1(node->right, current_set, new_conflict);
            current_set->erase(node->value);
        }
    }
}

generator<std::set<int>* > zdd_with_constraint_2(zdd* node, std::set<int>* current_set, std::set<int>* new_set)
{
    if (node == empty()) {

    }
    else if (node == base())
    {
        co_yield current_set;
    }
    else {
        co_yield zdd_with_constraint_2(node->left, current_set, new_set);
        if(new_set->contains(node->value)){
            current_set->insert(node->value);
            co_yield zdd_with_constraint_2(node->right, current_set, new_set);
            current_set->erase(node->value);
        }
    }
}
