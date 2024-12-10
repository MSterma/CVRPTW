#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <random>
#include <iomanip>
#include <filesystem>

#include "Client.h"
#include "Solution.h"
#include "Vehicle.h"
#define  RCLSIZE 3
#define TIME 180
#define PSIZE 15
#define MUTRATE 50
#define AlPHA 0.001
#define BETA 100
double grasp(std::vector<Vehicle> &vehicles,std::vector<Client> clients, int maxCap,Client depot) {
    double solLen=0;
    srand(time(NULL));
    std::vector<int> rcl;
    std::vector<Client> unserved=std::move(clients);
    int xd=depot.getX();
    int yd=depot.getY();
    int depoDue=depot.getDueDate();
    int k=0;

    while(!unserved.empty()) {
        vehicles.emplace_back(Vehicle(xd,yd,maxCap));
        while(1) {
            int x=vehicles.at(k).getX();
            int y=vehicles.at(k).getY();
            double t=vehicles.at(k).getTime();
            Vehicle& vehicle=vehicles.at(k);
            int cap=vehicle.getCapacity();
            std::sort(unserved.begin(),unserved.end(),[x, y, t](Client& lhs, Client& rhs) {
                double l= lhs.getDistance(x,y)+t>lhs.getReadyTime()?lhs.getDistance(x,y):lhs.getReadyTime()-static_cast<double>(t);
                double r=rhs.getDistance(x,y)+t>rhs.getReadyTime()?rhs.getDistance(x,y):rhs.getReadyTime()-static_cast<double>(t);
                return l<r;
            });

            int i=0;

            while (rcl.size()<RCLSIZE && i<unserved.size()) {
                double dis=unserved.at(i).getDistance(x,y)+t>=unserved.at(i).getReadyTime()?unserved.at(i).getDistance(x,y):static_cast<double>(unserved.at(i).getReadyTime())-t;
                double ndsist=unserved.at(i).getDistance(xd,yd)+vehicles.at(k).getTime()+unserved.at(i).getServiceTime()+dis;
                if (cap>=unserved.at(i).getDemand() && dis+t<=unserved.at(i).getDueDate() && ndsist<=static_cast<double>(depoDue)) {
                    rcl.push_back(i);
                }
                i++;
            }
            if(rcl.size()>0) {
                int randNum = rand()%(rcl.size());
                Client cl=unserved.at(rcl.at(randNum));
                double d=cl.getDistance(x,y)+t>=static_cast<double>(cl.getReadyTime())?cl.getDistance(x,y):(static_cast<double>(cl.getReadyTime())-t);
               vehicle.add(cl.getId());
                vehicle.setX(cl.getX());
                vehicle.setY(cl.getY());
                vehicle.load(cl.getDemand());
                vehicle.incTime(d+static_cast<double>(cl.getServiceTime()));
                unserved.erase(unserved.begin()+rcl.at(randNum));
                rcl.clear();
            }else if (x==xd && y==yd && vehicle.getRoute().empty()) {
                return -1;
            }else{
                vehicle.incTime(depot.getDistance(vehicles.at(k).getX(),vehicles.at(k).getY()));
                break;
            }
        }
        k++;
    }
    for (auto vehicle: vehicles) {
        solLen+=vehicle.getTime();
    }
    return solLen;
}
std::pair<int,double> checkSolution(std::vector<Client> clients, int maxCap, Client depot, std::vector<int> v) {
    int x=depot.getX();
    int y=depot.getY();
    int cap=0;
    double t=0;
    double maxt=0;
    int masz=0;
    int i=0;
    while (i < v.size()) {
        Client clientI = clients.at(v.at(i) - 1);
        if (cap + clientI.getDemand() > maxCap) {
            t += depot.getDistance(x, y);
            x = depot.getX();
            y = depot.getY();
            maxt += t;
            t = 0;
            cap = 0;
            masz += 1;
            continue;
        }

        double distance = clientI.getDistance(x, y);
        double finalDistance = std::max(distance, clientI.getReadyTime() - t) + clientI.getServiceTime();

        bool canReturn = t + finalDistance + depot.getDistance(clientI.getX(), clientI.getY()) <= depot.getDueDate();

        if (canReturn && t + finalDistance - clientI.getServiceTime() <= clientI.getDueDate()) {
            x = clientI.getX();
            y = clientI.getY();
            t += finalDistance;
            cap += clientI.getDemand();
            i++;
        } else {
            t += depot.getDistance(x, y);
            x = depot.getX();
            y = depot.getY();
            maxt += t;
            t = 0;
            cap = 0;
            masz += 1;
        }
    }

    maxt += t + depot.getDistance(x, y);
    t=0;
    cap=0;
    masz+=1;
    return {masz,maxt};
}
std::string getSolution(std::vector<Client> clients, int maxCap, Client depot, std::vector<int> v) {
    std::string sol = "";
    int x = depot.getX();
    int y = depot.getY();
    int cap = 0;
    double t = 0;
    double maxt = 0;
    int masz = 0;
    int i = 0;

    while (i < v.size()) {
        Client clientI = clients.at(v.at(i) - 1);


        if (cap + clientI.getDemand() > maxCap) {

            sol += "\n";
            t += depot.getDistance(x, y);
            x = depot.getX();
            y = depot.getY();
            maxt += t;
            t = 0;
            cap = 0;
            masz += 1;
            continue;
        }


        double distance = clientI.getDistance(x, y);
        double finalDistance = std::max(distance, clientI.getReadyTime() - t) + clientI.getServiceTime();
        bool canReturn = t + finalDistance + depot.getDistance(clientI.getX(), clientI.getY()) <= depot.getDueDate();

        if (canReturn && t + finalDistance - clientI.getServiceTime() <= clientI.getDueDate()) {

            x = clientI.getX();
            y = clientI.getY();
            t += finalDistance;
            cap += clientI.getDemand();
            sol += std::to_string(clientI.getId()) + " ";
            i++;
        } else {
            sol += "\n";
            t += depot.getDistance(x, y);
            x = depot.getX();
            y = depot.getY();
            maxt += t;
            t = 0;
            cap = 0;
            masz += 1;
        }
    }

    maxt += t + depot.getDistance(x, y);
    return sol;
}

void genetic(std::vector<Solution> &population,std::vector<Client> clients, int maxCap,Client depot) {
    std::vector<std::vector<int>> crossovers;
    std::sort(population.begin(),population.end(), [](Solution& lhs , Solution& rhs ) {
        if(lhs.getRouteNumber()<rhs.getRouteNumber()) {
            return false;
        }
        if(lhs.getRouteNumber()==rhs.getRouteNumber() || lhs.getValue()<rhs.getValue()) {
            return false;
        }
        return true;
    });
    for(int z=1; z<PSIZE; z++) {
        std::vector<int> child;
        int rs=population.at(0).getRoute().size();
        int i=rand()%rs;
        int j=rand()%rs;
        while (j<i) {
            j=rand()%rs;
        }
        std::vector<int> p1=population.at(z-1).getRoute();
        std::vector<int> p2=population.at(z).getRoute();
        //std::cout<<j<<std::endl;
        for(int w=0;w<i;w++) {
            child.push_back(-1);
        }
        for(int k=i;k<j;k++) {
                child.push_back(p1.at(k));
        }
        int circ=j;
        while(child.size()<clients.size()) {
            if(std::find(child.begin(), child.end(), p2.at(circ)) != child.end()) {
                if(circ==clients.size()-1) {
                    circ=0;
                }else {
                    circ++;
                }
            }else {
                child.push_back(p2.at(circ));
            }
        }
        int w=0;
        while(w<i) {
            if(std::find(child.begin(), child.end(), p2.at(circ)) != child.end()) {
                if(circ==clients.size()-1) {
                    circ=0;
                }else {
                    circ++;
                }
            }else {
                child.at(w)=p2.at(circ);
                w++;
            }

        }if(rand()%100<=MUTRATE) {
          auto nfirst=child;
            int  firstL=rand()%clients.size();
            int  secondL=rand()%clients.size();
            while (firstL==secondL) {
                secondL=rand()%clients.size();
            }
            int buff=nfirst.at(firstL);
            nfirst.at(firstL)= nfirst.at(secondL);
            nfirst.at(secondL) = buff;
            crossovers.push_back(nfirst);
      }
        crossovers.push_back(child);
    }
    for(auto v : crossovers) {
        std::pair<int,double> vals=checkSolution(clients, maxCap, depot, v);
        population.emplace_back(vals.second,vals.first, v);

    }
    while (population.size()>PSIZE) {
        //std::cout << population.size()<<std::endl;
        int actsol=0;
        int out=0;
        auto max=population.at(0);
        for(auto v : population) {
            if(v.getValue()*AlPHA+v.getRouteNumber()*BETA>max.getValue()*AlPHA+max.getRouteNumber()*BETA) {
                out=actsol;
                max=v;
            }
            actsol++;
        }
        population.erase(population.begin()+out);
    }
}


int main(int argc, char *argv[])
{
    if(argc!=3) {
        argv[1]=(char*)"input/C101.txt";
        argv[2]=(char*)"solution.txt";
    }
    std::string path = "input/1000";
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path().string() << std::endl;
        std::ifstream file(entry.path().string());
        std::string testFile = argv[1];
//        std::ifstream file(testFile);
        std::vector<Vehicle> vehicles;
        std::vector<Client> clients;
        int vehicles_num;
        std::ofstream Ofile(entry.path().string() + "sol.txt");
        int maxCap;
        int client_num = 0;
        std::string fileName;
        std::string buf;
        //file parsing:
        file >> fileName;
        for (int i = 0; i < 3; ++i) {
            file >> buf;
        }
        file >> vehicles_num;
        file >> maxCap;
        for (int i = 0; i < 4; i++) {
            getline(file, buf);
        }
        int vals[7];
        file >> vals[0] >> vals[1] >> vals[2] >> vals[3] >> vals[4] >> vals[5] >> vals[6];
        Client depot = Client(vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6]);
//        std::ofstream Ofile(argv[2]);
        while (file >> vals[0] >> vals[1] >> vals[2] >> vals[3] >> vals[4] >> vals[5] >> vals[6]) {
            client_num++;
            clients.emplace_back(vals[0], vals[1], vals[2], vals[3], vals[4], vals[5], vals[6]);
        }// end of parsing

        int z = 0;
        std::vector<Solution> population;
        long beg = time(NULL);
        bool flag = false;
        while (z < PSIZE) {
            std::vector<int> sl;;
            double l = grasp(vehicles, clients, maxCap, depot);
//            std::cout<<std::fixed<<std::setprecision(5)<<l<<" "<<vehicles.size()<<std::endl;
            if (l == -1) {
                flag = true;
                break;
            }
            for (auto veh: vehicles) {
                for (auto r: veh.getRoute()) {
                    sl.push_back(r);

                }
            }
            population.emplace_back(l, vehicles.size(), sl);

            vehicles.clear();
            z++;
        }
        if (flag) {
            std::string output = "-1 -1\n";
            Ofile << output;
            file.close();
            Ofile.close();
            return 0;
        }
        std::cout<<"done "<< time(NULL)-beg<<std::endl;
        int iters = 0;
        int bid = 0;

        auto actbest = population.at(0);
        while (time(NULL) - beg < TIME) {
            genetic(population, clients, maxCap, depot);
            iters++;
            int i = 0;
            for (auto v: population) {
                if (BETA * v.getRouteNumber() + v.getValue() * AlPHA <
                    actbest.getValue() * AlPHA + actbest.getRouteNumber() * BETA) {
                    actbest = v;
                    bid = i;
                }
                i++;
            }


        }
        std::pair<int, double> bst = checkSolution(clients, maxCap, depot, population.at(bid).getRoute());
        std::string output = std::to_string(bst.first) + " " + std::to_string(bst.second) + "\n";
        output += getSolution(clients, maxCap, depot, population.at(bid).getRoute());
        Ofile << output;
        file.close();
        Ofile.close();
    }
    return 0;
}
