#include <set>
#include "Berge.h"
#include "BZ.h"
#include "BZU.h"
#include "BZUStar.h"
#include "zdd.h"


int main(int argc, char** argv)
{
    
    std::set<std::set<int>> problem = {{1,2},{3,4},{2,3,7}};
    auto s1 = Berge(problem);
    auto s2 = zdd_count(BZ(problem));
    auto s3 = zdd_count(BZU(problem));
    auto s4 = zdd_count(BZUStar(problem));
}