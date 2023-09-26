#include <set>
#include "Berge.h"
#include "BZ.h"
#include "BZU.h"
#include "BZUStar.h"
#include "zdd.h"
#include <fstream>
#include <string>
#include <iostream>
#include <chrono>

std::set<std::set<int>> import_family(char* file_name)
{
    std::ifstream file;
    std::string line, val;
    std::set<std::set<int>> family = {};
    int rows = 0;
    std::set<int> current_set = {};
    file.open(file_name);
    if (file.is_open()) {
        while ( std::getline(file, line)) {
            rows++;
            std::stringstream s(line);
            while (std::getline(s, val, ',')) {
                current_set.insert(std::stoi(val));
            }
            family.insert(current_set);
            current_set.clear();
        }
    }
    else
    {
        std::cout << "Invalid file input" << std::endl;
    }

    return family;
}

void export_family(std::set<std::set<int>>* family, char* file_name)
{
    std::ofstream output;
    output.open(file_name);
    for(auto s : *family)
    {
        for(int i : s)
        {
            output << i << ",";
        }
        output << std::endl;
    }
}



int main(int argc, char** argv)
{

    std::set<std::set<int>> problem = import_family(argv[2]);
    if (problem.size() == 0)
        return 0;
    std::set<std::set<int>> solution;
    zdd* zdd_solution = empty();    


    auto start = std::chrono::high_resolution_clock::now();
    
    if(std::string(argv[1]) == std::string("Berge"))
    {
        solution = Berge(&problem);
    }
    else if(std::string(argv[1]) == "BZ")
    {
        zdd_solution = BZ(&problem);
    }
    else if(std::string(argv[1]) == "BZU")
    {
        zdd_solution = BZU(&problem);
    }   
    else if(std::string(argv[1]) == "BZUs")
    {
        zdd_solution = BZUStar(&problem);
    }   
    else
    {
        std::cout << "Invalid algorithm input" << std::endl;
    }

    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Duration: " << duration.count() << "ms" << std::endl;

    if(!(zdd_solution == empty()))
        solution = to_set_of_sets(zdd_solution);
    export_family(&solution, argv[3]);
}