#if !defined(BZ_H)
#define BZ_H

#include <set>
#include "zdd.h"

zdd* BZ(std::set<std::set<int>> problem);
zdd* BZIteration(ZddManager* m, zdd* minimal_diagnoses, std::set<int> new_conflict);

#endif // BZ_H