#include <iostream>
#include <string>
#include <vector>
#include <fstream>
int main()
{
    std::vector<std::string> testFiles({ "input/cvrptw1.txt","input/cvrptw2.txt"});
    for (const auto& test_file: testFiles) {
        std::ifstream file (test_file);
        std::string buf;
        file >> buf;
        std::cout <<"File name:"<< buf << std::endl;
    };
    return 0;
}
