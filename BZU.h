#if !defined(BZU_H)
#define BZU_H

#include <set>
#include "zdd.h"

zdd* BZU(std::set<std::set<int>> problem);
zdd* BZUIteration(ZddManager* m, zdd* minimal_diagnoses, std::set<int> new_conflict);

#endif // BZU_H