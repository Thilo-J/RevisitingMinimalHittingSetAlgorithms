#include <set>
#include <algorithm>
#include "Berge.h"

using namespace std;

bool empty_intersection(const set<int>& x, const set<int>& y)
{
    set<int>::const_iterator i = x.begin();
    set<int>::const_iterator j = y.begin();
    while (i != x.end() && j != y.end())
    {
      if (*i == *j)
        return false;
      else if (*i < *j)
        ++i;
      else
        ++j;
    }
    return true;
}

set<set<int>> BergeIteration(set<set<int>>* minimal_diagnoses, set<int> new_conflict)
{
    set<set<int>> old = set<set<int>>(*minimal_diagnoses);
    set<set<int>> add = set<set<int>>();

    for (set<int> minimal_diagnosis : *minimal_diagnoses)
    {
        if (empty_intersection(minimal_diagnosis, new_conflict)) 
        {
            old.erase(minimal_diagnosis);
            for (int c : new_conflict)
            {
                set<int> new_set = set<int>(minimal_diagnosis);
                new_set.insert(c);
                bool add_new_set = true;
                for (set<int> k : *minimal_diagnoses)
                {
                    if(k != minimal_diagnosis)
                    {
                        if (includes(new_set.begin(), new_set.end(), k.begin(), k.end())) 
                        {
                            add_new_set = false;
                            break;
                        }
                    }
                }
                if(add_new_set)
                {
                    add.insert(new_set);
                }
            }
        }
    }
    set_union(old.begin(), old.end(), add.begin(), add.end(), inserter(old, old.end())); // Union between old and add
    return old;
}

std::set<std::set<int>> Berge(std::set<std::set<int>>* problem)
{
    std::set<std::set<int>> MHSes = {};
    for (auto set : *problem)
    {
        if(MHSes.size() == 0){
            for(int i : set)
                MHSes.insert({i});
        }
        else{
            MHSes = BergeIteration(&MHSes, set);
        }
    }
    return MHSes;
}