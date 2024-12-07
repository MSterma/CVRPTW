#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <random>
#include <__charconv/chars_format.h>
#include <__random/random_device.h>

#include "Client.h"
#include "Vehicle.h"
#define  RCLSIZE 1
#define TIME 300
#define PSIZE 60
double grasp(std::vector<Vehicle> &vehicles,std::vector<Client> clients, int maxCap,Client depot) {
    double solLen=0;
    srand(time(NULL));
    std::vector<int> rcl;
    std::vector<Client> unserved=std::move(clients);
    int xd=depot.getX();
    int yd=depot.getY();
    int depoDue=depot.getDueDate();
    int k=0;
    std::sort(unserved.begin(),unserved.end(),[](Client& lhs, Client& rhs) {
             //double l= lhs.getDistance(x,y)+t>lhs.getReadyTime()?lhs.getDistance(x,y):lhs.getReadyTime()-static_cast<double>(t);
           // double r=rhs.getDistance(x,y)+t>rhs.getReadyTime()?rhs.getDistance(x,y):rhs.getReadyTime()-static_cast<double>(t);
             return lhs.getDemand()>rhs.getDemand();
         });
    while(!unserved.empty()) {
        vehicles.emplace_back(Vehicle(xd,yd,maxCap));
        while(1) {
            int x=vehicles.at(k).getX();
            int y=vehicles.at(k).getY();
            double t=vehicles.at(k).getTime();
            Vehicle& vehicle=vehicles.at(k);
            int cap=vehicle.getCapacity();


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
    while (i<v.size()) {
        Client clientI=clients.at(v.at(i)-1);
        if(x==depot.getX() && y==depot.getY() && clientI.getX()!= depot.getX() && clientI.getY()!= depot.getY()) {
            double distance=clientI.getDistance(x,y);
            double finalDistance=distance>clientI.getReadyTime()? distance+clientI.getServiceTime():clientI.getReadyTime()+clientI.getServiceTime();
            bool canReturn=finalDistance+depot.getDistance(clientI.getX(),clientI.getY())<=depot.getDueDate();
            if(!canReturn || clientI.getDemand() >maxCap) {
                return {-1,-1};
            }
                x=clientI.getX();
                y=clientI.getY();
                t+=finalDistance;
                cap+=clientI.getDemand();
                i++;
            //std::cout<<clientI.getId()<<" ";
        }else {
            double distance=clientI.getDistance(x,y);
            double finalDistance=distance+t>=clientI.getReadyTime()? distance+clientI.getServiceTime():clientI.getReadyTime()+clientI.getServiceTime()-t;
            bool canReturn=t+finalDistance+depot.getDistance(clientI.getX(),clientI.getY())<=depot.getDueDate();
            if(canReturn && cap+clientI.getDemand()<=maxCap && t+finalDistance-clientI.getServiceTime()<=clientI.getDueDate()) {
                x=clientI.getX();
                y=clientI.getY();
                t+=finalDistance;
                cap+=clientI.getDemand();
                i++;
                //std::cout<<clientI.getId()<<" ";
            }else {
                t+=depot.getDistance(x,y);
                x=depot.getX();
                y=depot.getY();
                maxt+=t;
                t=0;
                cap=0;
                masz+=1;

            }

        }

    }
    maxt+=t;
    t=0;
    cap=0;
    masz+=1;
    return {masz,maxt};
}
double genetic(std::vector<std::vector<int>> &population,std::vector<Client> clients, int maxCap,Client depot) {
    std::vector<Vehicle> vehicles;
    std::vector<std::vector<int>> crossovers;
    std::vector<int> winners;
    for (int i = 0; i < PSIZE; i+=2) {
        std::pair<int,double> a=checkSolution(clients,maxCap,depot,population.at(i));
        std::pair<int,double> b=checkSolution(clients,maxCap,depot,population.at(i+1));
        if(a.first*100+a.second*0.001>100*b.first+0.001+b.second) {
            winners.push_back(i);
        }else {
            winners.push_back(i+1);
        }
    }
    for(int z=1; z<winners.size(); z++) {
        std::vector<int> first;
        int fir=winners.at(z-1);
        int sec=winners.at(z);

        int i=rand()%population.at(0).size();
        int j=rand()%population.at(0).size();
        while (j<i) {
            j=rand()%population.at(0).size();
        }
        //std::cout<<j<<std::endl;
        for(int w=0;w<i;w++) {
            first.push_back(-1);
        }
        for(int k=i;k<j;k++) {
                first.push_back(population.at(fir).at(k));
        }
        int circ=j;
        while(first.size()<clients.size()) {
            if(std::find(first.begin(), first.end(), population.at(sec).at(circ)) != first.end()) {
                if(circ==clients.size()-1) {
                    circ=0;
                }else {
                    circ++;
                }
            }else {
                first.push_back(population.at(sec).at(circ));
            }
        }
        int w=0;
        while(w<i) {
            if(std::find(first.begin(), first.end(), population.at(sec).at(circ)) != first.end()) {
                if(circ==clients.size()-1) {
                    circ=0;
                }else {
                    circ++;
                }
            }else {
                first.at(w)=population.at(sec).at(circ);
                w++;
            }

        }

      if(rand()%100<5) {
          for (int sw = 1; sw < first.size(); sw++) {
              std::pair<int,double>  prev=checkSolution(clients,maxCap,depot,first);
              int buff=first.at(sw-1);
              first.at(sw-1)= first.at(sw);
              first.at(sw) = buff;
              std::pair<int,double>  nsol=checkSolution(clients,maxCap,depot,first);
              if(prev.first<nsol.first || (prev.first==nsol.first && prev.second<nsol.second)) {
                  buff=first.at(sw-1);
                  first.at(sw-1)= first.at(sw);
                  first.at(sw) = buff;
              }
              // std::cout << sw<< std::endl;
          }

      }else  if(rand()%100<=0) {

            int  firstL=rand()%clients.size();
            int  secondL=rand()%clients.size();
            while (firstL==secondL) {
                secondL=rand()%clients.size();
            }
            int buff=first.at(firstL);
            first.at(firstL)= first.at(secondL);
            first.at(secondL) = buff;



      }
        crossovers.push_back(first);
    }
    while (population.size()>PSIZE/2+1) {
        //std::cout << population.size()<<std::endl;
        int actsol=0;
        std::pair<int,double> max=checkSolution(clients,  maxCap, depot,population.at(0));
        int out=0;
        for(auto v : population) {
            std::pair<int,double> act=checkSolution(clients, maxCap, depot, v);
            if(act.second*act.first>max.second*max.first) {
                out=actsol;
                max=act;
            }
            actsol++;
        }
        population.erase(population.begin()+out);
    }
    for(auto v : crossovers) {
        population.push_back(v);

    }
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc!=3) {
        argv[1]=(char*)"input/m2kvrptw-0.txt";
        argv[2]=(char*)"solution.txt";


    }


    std::string testFile=argv[1];
    std::ifstream file (testFile);
        std::vector<Vehicle> vehicles;
        std::vector<Client> clients;
        int vehicles_num;

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
        for(int i = 0; i < 4; i++) {
            getline(file,buf);
        }
        int vals[7];
        file>>vals[0]>>vals[1]>>vals[2]>>vals[3]>>vals[4]>>vals[5]>>vals[6];
         Client depot= Client(vals[0],vals[1],vals[2],vals[3],vals[4],vals[5],vals[6]);
         std::ofstream Ofile (argv[2]);
        while (file>>vals[0]>>vals[1]>>vals[2]>>vals[3]>>vals[4]>>vals[5]>>vals[6]) {
            client_num++;
            clients.emplace_back(vals[0],vals[1],vals[2],vals[3],vals[4],vals[5],vals[6]);
        }// end of parsing
    double max=5576016.77462;

    int z=0;
    int min=0;
     std::vector<std::vector<int>>population;
    long beg =time(NULL);
 /*while(population.size()<PSIZE) {
        std::vector<int> sol;
        for(int i=1; i<=clients.size(); i++) {
            sol.push_back(i );
        }
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(sol.begin(),sol.end(),g);

        population.push_back(sol);

    }*/
  srand(time(NULL));
   while(z<PSIZE) {
        std::vector<int> sl;
        double l=grasp(vehicles,clients,maxCap,depot);
       for(auto vehicle: vehicles) {
            for (int route: vehicle.getRoute()) {
                sl.emplace_back(route);
            }
        }
        population.emplace_back(sl);
        vehicles.clear();
        z++;
    }
    /*for(int i=0; i<PSIZE; i++) {
        for(int j=0; j<clients.size(); j++) {
            std::cout << " " << population.at(i).at(j);
        }
        std::cout << std::endl;

    }*/
    std::cout<<"done"<<time(NULL)-beg<<std::endl;
    int iters=0;
    double mm=20000000;
    int point=1400;
    while(mm>=1048 || max>3976016) {
       // std::cout<<iters<<std::endl{
        for(auto v: population) {
            std::pair<int,double> sol=checkSolution(clients,maxCap,depot,v);
            if(sol.first<mm || (sol.first==mm && sol.second<max)) {

               max=sol.second;
                mm=sol.first;
                if(mm<point) {
                    point-=50;
                    std::cout<<time(NULL)-beg<<" ";
                    std::cout<<std::fixed<<std::setprecision(5)<<sol.second<<" "<<sol.first<<" "<<iters<<std::endl;
                }
               // std::cout<<std::fixed<<std::setprecision(5)<<sol.second<<" "<<sol.first<<" "<<iters<<std::endl;


            }
        }
        /*std::cout<<std::endl;
        for(int i=0; i<PSIZE; i++) {
        for(int j=0; j<clients.size(); j++) {
        std::cout << " " << population.at(i).at(j);
        }
        std::cout << std::endl;

        }
        std::cout<<std::endl;*/
        genetic(population,clients,maxCap,depot);
        iters++;
    }
    std::cout<<"done"<<time(NULL)-beg<<std::endl;
    std::cout<<std::fixed<<std::setprecision(5)<<max<<" "<<mm<<" "<<iters<<std::endl;
    file.close();
    Ofile.close();

    return 0;
}
