#if !defined(BZUSTAR_H)
#define BZUSTAR_H

#include <set>
#include "zdd.h"

zdd* BZUStar(std::set<std::set<int>>* problem);
zdd* BZUStarIteration(ZddManager* m, zdd* minimal_diagnoses, std::set<int> new_conflict);

#endif // BZUSTAR_H