#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <__ranges/split_view.h>

#include "Client.h"
#include "Vehicle.h"

int main()
{
    std::vector<std::string> testFiles({ "input/cvrptw1.txt","input/cvrptw2.txt","input/rc21010.txt"});
    for (const auto& test_file: testFiles) {
        std::ifstream file (test_file);
        std::vector<Vehicle> vehicles;
        std::vector<Client> clients;
        int vehicles_num;

        int maxCap;
        int client_num = 0;
        std::string fileName;
        std::string buf;
        //file parsing:
        file >> fileName;
        std::cout <<"File name:"<< fileName << std::endl;
        for (int i = 0; i < 3; ++i) {
            file >> buf;
        }
        file >> vehicles_num;
        std::cout <<"veh_num "<< vehicles_num  << std::endl;
        file >> maxCap;
        std::cout <<"veh_cap "<< maxCap << std::endl;
        for (int i = 0; i < vehicles_num; i++) {
            vehicles.emplace_back(maxCap);
        }
        for(int i = 0; i < 4; i++) {
            getline(file,buf);
        }
        int vals[7];

        while (file>>vals[0]>>vals[1]>>vals[2]>>vals[3]>>vals[4]>>vals[5]>>vals[6]) {
            client_num++;
            clients.emplace_back(vals[0],vals[1],vals[2],vals[3],vals[4],vals[5],vals[6]);
        }// end of parsing
        std::cout << "vechs" << std::endl;
       /* for (int i = 0; i < vehicles_num; i++) {
            std::cout<<vehicles.at(i).toString()<<std::endl;
        }*/
        std::cout << "clients: "<< std::endl;
        std::sort(clients.begin(),clients.end());
        for ( auto& client : clients) {
            std::cout << client.toString() << std::endl;
        }
    };
    return 0;
}
