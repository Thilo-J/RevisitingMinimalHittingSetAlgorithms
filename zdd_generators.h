#if !defined(ZDDGENERATORS_H)
#define ZDDGENERATORS_H

#include <set>
#include "generator.hpp"
#include "zdd.h"

generator<std::set<int>*> zdd_sets(zdd* node, std::set<int>* current_set);
generator<std::set<int>*> zdd_with_constraint_1(zdd* node, std::set<int>* current_set, std::set<int>* new_conflict);
generator<std::set<int>* > zdd_with_constraint_2(zdd* node, std::set<int>* current_set, std::set<int>* new_set);


#endif // ZDDGENERATORS_H