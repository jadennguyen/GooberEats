#include "provided.h"
#include <vector>
using namespace std;
#include <limits>
#include <algorithm>
#include <random>
#include <utility>
#include <unordered_set>
class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* m_streetmap;
    struct Node{
        GeoCoord location;
        string item;
    };
    double costFunction(const vector<Node>& path) const;
};

inline
int randInt(int min, int max)
{
    if (max < min)
        std::swap(max, min);
    static std::random_device rd;
    static std::default_random_engine generator(rd());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}
double DeliveryOptimizerImpl::costFunction(const vector<Node> &path) const
{
    double costDistance = 0;
    for (int i = 0; i < path.size() - 1; i++)
    {
        costDistance += distanceEarthMiles(path[i].location, path[i + 1].location);
    }
    return costDistance;
}
DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_streetmap = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    /*
    vector<GeoCoord> vertices;
    vertices.push_back(depot);
    for (int i = 0; i < deliveries.size(); i++)
    {
        vertices.push_back(deliveries[i].location);
    }
    
    vector<pair<pair<GeoCoord, GeoCoord>, double>> graph;
    for (int i = 0; i < vertices.size(); i++)
    {
        for (int j = i + 1; j < vertices.size(); j++)
        {
            pair<GeoCoord, GeoCoord> edge;
            edge.first = vertices[i];
            edge.second = vertices[j];
            pair<pair<GeoCoord, GeoCoord>, double> weightedEdges;
            weightedEdges.first = edge;
            double weight = distanceEarthMiles(vertices[i], vertices[j]);
            weightedEdges.second = weight;
            graph.push_back(weightedEdges);
        }
    }
    sort(graph.begin(), graph.end(), [](const pair<pair<GeoCoord, GeoCoord>, double>& lhs, const pair<pair<GeoCoord, GeoCoord>, double>& rhs){return lhs.second < rhs.second;});
    
    vector<pair<GeoCoord, GeoCoord>> spanTree;
    spanTree.push_back(graph[0].first);
    for (int i = 1; i < graph.size(); i++)
    {
        
    }
    */
    
    PointToPointRouter getDeliveries(m_streetmap);
    GeoCoord currentLocation = depot;
    
    //Distance from depot to each successive deliveries and back to the depot
    double distanceTravelled = distanceEarthMiles(depot, deliveries[0].location);
    for (int i = 0; i < deliveries.size() - 1; i++)
    {
        distanceTravelled += distanceEarthMiles(deliveries[i].location,deliveries[i + 1].location);
    }
    distanceTravelled += distanceEarthMiles(deliveries[deliveries.size() - 1].location, depot);
    
    //Simulated Annealing Attempt
    double bestScore = numeric_limits<double>::max();
    double q = 0.9;  //90% of the time, we will accept a new p’ even if its score is. worse than the current score
    vector<Node> bestSolution;
    vector<Node> path; //Some initial path that start and end at the depot
    Node startNode;
    startNode.location = depot;

    path.push_back(startNode);
    for (int i = 0; i < deliveries.size(); i++) {
        Node newNode;
        newNode.location = deliveries[i].location;
        newNode.item = deliveries[i].item;
        path.push_back(newNode);
    }
    path.push_back(startNode);
    
    //Method to generate random double (Might work??)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    //Number of iterations
    int iteration = 40;
    while(iteration > 0)
    {
        double currentScore = costFunction(path);
        int randIndex = randInt(1, path.size() - 1);
        vector<Node> pathPrime;
        pathPrime = path;
        iter_swap(pathPrime.begin() + randIndex, pathPrime.end() - 1 - randIndex);  //New ordering of path, swap 2 random items
        double newScore = costFunction(pathPrime);
        if (newScore < currentScore)
        {
            path = pathPrime;   //Accepts our solution!
            if (newScore < bestScore)
            {
                bestScore = newScore;
                bestSolution = path;
            }
        }else if (newScore >= currentScore)
        {
            //With probability q, accept our worse solution because it might help us in the long run
            double randDouble = dis(gen);
            if(randDouble < q)
            {
                bestSolution = pathPrime; //Accept our new solution
            }
        }
        q *= 0.9;
        iteration--;
    }
    deliveries.clear();
    for (int i = 1 ; i < bestSolution.size() - 1; i++) {
        DeliveryRequest request(bestSolution[i].item, bestSolution[i].location);
        deliveries.push_back(request);
    }
    oldCrowDistance = distanceTravelled;  // Delete these lines and implement this function correctly
    
    newCrowDistance = costFunction(bestSolution);
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
