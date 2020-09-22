#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

unsigned int hasher(const std::string& s)
{
    return std::hash<std::string>()(s);
}

int main(int argc, char *argv[])
{
    /*
    StreetMap currentMap;
    GeoCoord depot("34.0625329", "-118.4470263");
    GeoCoord seg1("34.0636533", "-118.4470480");
    GeoCoord seg2("34.0636625", "-118.4464708");
    GeoCoord seg3("34.0636671", "-118.4461842");
    DeliveryRequest first("1st", seg1);
    DeliveryRequest second("2nd", seg2);
    DeliveryRequest third("3rd", seg3);
    vector<DeliveryRequest> DR;
    DR.push_back(second);
    DR.push_back(first);
    DR.push_back(third);
    DeliveryOptimizer optimize(&currentMap);
    double oldCrow, newCrow;
    optimize.optimizeDeliveryOrder(depot, DR, oldCrow, newCrow);
    for (int i = 0; i < DR.size(); i++)
    {
        cout << DR[i].item << endl;
    }
    cout << DR.size() << endl;
    cout << "Old Crow: " << oldCrow << endl;
    cout << distanceEarthMiles(depot, seg2) + distanceEarthMiles(seg2, seg1) + distanceEarthMiles(seg1, seg3) + distanceEarthMiles(seg3, depot)<< endl;
    cout << "New Crow: " << newCrow << endl;
    */
    
    
    StreetMap currentMap;
    currentMap.load("/Users/JadenNguyen/Desktop/mapdatatest.txt");
    vector<StreetSegment> newVector;
    
    cout << fixed;
    cout << setprecision(7);
    
    
    GeoCoord newCoord("34.0636671", "-118.4461842");
    if(currentMap.getSegmentsThatStartWith(newCoord, newVector))
    {
        cout << "In Map" << endl;
        for (int i = 0; i < newVector.size(); i++)
        {
            cout << newVector[i].start.latitude << " " << newVector[i].start.longitude << " " << newVector[i].end.latitude << " " << newVector[i].end.longitude <<  " " << newVector[i].name << endl;
        }
    }
    if (!currentMap.getSegmentsThatStartWith(newCoord, newVector))
    {
        cout << "Not In Map" << endl;
    }

    cout << "====================" << endl;
    cout << "PointToPoint Testing: " << endl;
    GeoCoord start("34.0636671", "-118.4461842");
    GeoCoord end("34.0625329" ,"-118.4470263");
    PointToPointRouter route(&currentMap);
    list<StreetSegment> listOfStreets;
    double distance = 0;
    
    if(route.generatePointToPointRoute(start, end, listOfStreets, distance) == BAD_COORD)
    {
        cout << "Bad Coord" << endl;
    }
    listOfStreets.clear();
    if (route.generatePointToPointRoute(start, end, listOfStreets, distance) == NO_ROUTE)
    {
        cout << "No Route" << endl;
    }else{
        for (auto p = listOfStreets.begin(); p != listOfStreets.end(); p++)
        {
            cout << p->start.latitude << " " << p->start.longitude << " " << p->end.latitude << " " << p->end.longitude << " " << p->name << endl;
        }
        cout << "Distance: " << distance << " miles" << endl;
    }
    
    GeoCoord g1("34.0625329" ,"-118.4470263");
    GeoCoord g2("34.0632405" ,"-118.4470467");
    GeoCoord g3("34.0636533", "-118.4470480");
    GeoCoord g4("34.0636625", "-118.4464708");
    GeoCoord g5("34.0636671", "-118.4461842");
    cout << "Actual Distance: " << distanceEarthMiles(g1, g2) + distanceEarthMiles(g2, g3) + distanceEarthMiles(g3, g4) + distanceEarthMiles(g4, g5)<< endl;
    //cout << newVector.size() << endl;
    //cout << newVector[0].end.longitude << endl;
    cout << "====================" << endl;
    DeliveryPlanner test(&currentMap);
    GeoCoord depot("34.0625329", "-118.4470263");
    vector<DeliveryRequest> requests;
    GeoCoord firstStop("34.0636671", "-118.4461842");
    DeliveryRequest oneRequest("Sardines",firstStop);
    GeoCoord secondStop("34.0636533", "-118.4470480");
    DeliveryRequest anotherRequest("Beer", secondStop);
    requests.push_back(oneRequest);
    //requests.push_back(anotherRequest);
    vector<DeliveryCommand> commands;
    double totalDistanceTravelled = 0;
    
    if(test.generateDeliveryPlan(depot, requests, commands, totalDistanceTravelled) == NO_ROUTE)
    {
        cout << "NO ROUTE" << endl;
    }
    
    for (int i = 0; i < commands.size(); i++)
    {
        cout << commands[i].description() << endl;
    }
    cout << totalDistanceTravelled << " miles travelled for all deliveries." << endl;
    
    
    /*
    ExpandableHashMap<string, int> test;
    
    test.associate("hi", 3);
    test.associate("lol", 4);
    test.associate("omg", 5);
    test.associate("bye", 5);
    test.dump();
    //cout << test.size() << endl;
    //cout << test.getBucketNumber() << endl;
    cout << "=======" << endl;
    test.associate("hey", 2);
    //test.associate("gay", 1);
    test.dump();
    cout << test.size() << endl;
    cout << test.getBucketNumber() << endl;
    */
    
    //Real code
    /*
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " mapdata.txt deliveries.txt" << endl;
        return 1;
    }

    StreetMap sm;
        
    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord depot;
    vector<DeliveryRequest> deliveries;
    if (!loadDeliveryRequests(argv[2], depot, deliveries))
    {
        cout << "Unable to load delivery request file " << argv[2] << endl;
        return 1;
    }

    cout << "Generating route...\n\n";

    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;
    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl;
    */
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}
