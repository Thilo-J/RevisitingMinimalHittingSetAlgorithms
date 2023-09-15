#if !defined(BERGE_H)
#define BERGE_H

#include <set>

std::set<std::set<int>> Berge(std::set<std::set<int>> problem);
std::set<std::set<int>> BergeIteration(std::set<std::set<int>> minimal_diagnoses, std::set<int> new_conflict);

#endif // BERGE_H


