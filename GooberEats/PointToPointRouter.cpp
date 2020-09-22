#include "provided.h"
#include <list>
using namespace std;
#include <limits>
#include <stack>
class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
        
private:
    struct Node{
        GeoCoord coord;
        Node* parent = nullptr;
        string streetname;
        double local = numeric_limits<double>::max();
        double global = numeric_limits<double>::max();
        bool visited = false;
        vector<Node*> vecNeighbors;
    };
    bool isInVector(const vector<Node*>& closedList,const Node* node) const
    {
        for (int i = 0; i < closedList.size(); i++)
        {
            if (closedList[i]->coord.latitude == node->coord.latitude && closedList[i]->coord.longitude == node->coord.longitude)
            {
                return true;
            }
        }
        return false;
    }
    const StreetMap* m_streetmap;

};



PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_streetmap = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    //Check whether the start and end Geocoord is valid, if not return BAD_COORD
    vector<StreetSegment> startStreets, endStreets;
    if (!m_streetmap->getSegmentsThatStartWith(start, startStreets)) return BAD_COORD;
    if (!m_streetmap->getSegmentsThatStartWith(end, endStreets)) return BAD_COORD;
    
    //If the start and end geocoord are the same, clear the route and return delivery success because there's always a route to itself
    if (start == end)
    {
        route.clear();
        return DELIVERY_SUCCESS;
    }
    
    vector<Node*> nodesLookedAt;
    list<Node*> uncheckedNodes;
    
    //Initialize the starting node
    Node* currentNode = new Node;
    currentNode->coord = start;
    currentNode->local = 0;
    currentNode->global = distanceEarthMiles(start, end);
    currentNode->streetname = startStreets[0].name;
    
    //Find all the neighboring GeoCoord of the starting coord and add it to the neighbors vector
    for (int i = 0; i < startStreets.size(); i++)
    {
        Node* neighbors = new Node;
        neighbors->coord = startStreets[i].end;
        neighbors->parent = currentNode;
        neighbors->streetname = startStreets[i].name;
        currentNode->vecNeighbors.push_back(neighbors);
    }
    
    //Push back the starting coord to the list of Nodes that haven't been checked
    uncheckedNodes.push_back(currentNode);
    
    while (!uncheckedNodes.empty() && currentNode->coord != end)
    {
        //sort nodes that haven't been checked yet by their global goal, lowest is first
        //STL list sort function that compare node pointers based on their global value
        uncheckedNodes.sort([](const Node* lhs, const Node* rhs){return lhs->global < rhs->global;});
        
        /*
        //Run through the list and take out the nodes that have been visited
        while (!uncheckedNodes.empty() && uncheckedNodes.front()->visited)
        {
            uncheckedNodes.pop_front();
        }
        
        //Check if popping the list make the list empty, if so then break out of the while loop
        if (uncheckedNodes.empty())
        {
            break;
        }
        */
        
        currentNode = uncheckedNodes.front();
        uncheckedNodes.pop_front();
        currentNode->visited = true;
        nodesLookedAt.push_back(currentNode);
              
        //Check surrounding neighbors and calculate their local and global values
        for (auto neighborNodes : currentNode->vecNeighbors)
        {
            
            //If the neighbor node have not been visited and have not been looked at then add it to the list of nodes to check
            if (!neighborNodes->visited && !isInVector(nodesLookedAt, neighborNodes))
            {
                uncheckedNodes.push_back(neighborNodes);
            }
            
            //Calculate the neighbor nodes distance from the current node
            double possibleLowerGoal = currentNode->local + distanceEarthMiles(currentNode->coord, neighborNodes->coord);
            
            //If going down this node would lead to a shorter distance than what the neighbor node currently have set then update the node accordingly
            if (possibleLowerGoal < neighborNodes->local)
            {
                neighborNodes->parent = currentNode;    //Set the neighbor node's parent node to be the current node
                neighborNodes->local = possibleLowerGoal;   //Set the neighbor node's local value to be the new lower local value
                
                //Essentially a heuristic value that let the algorithm know the next best path to search along
                neighborNodes->global = neighborNodes->local + distanceEarthMiles(neighborNodes->coord, end);
            }
            
            //Get neighbor Geocoords of each neighbors of the current Node by getting a vector of the street segments that it is mapped
            vector<StreetSegment> neighborStreets;
            m_streetmap->getSegmentsThatStartWith(neighborNodes->coord, neighborStreets);
            
            //Loop through the vector to get the end GeoCoord of each street segment and add it to the neighbor nodes' vector of neighbor nodes
            for (int i = 0; i < neighborStreets.size(); i++)
            {
                Node* neighbors = new Node; //Dynamically allocate a new node pointer
                neighbors->coord = neighborStreets[i].end;  //Keep track of  the GeoCoord of the neighbors
                neighbors->parent = neighborNodes;      //Update the neighbor node's neighbors to point back to its parent
                neighbors->streetname = neighborStreets[i].name;    //Get the street name of the neighbor node's neighbors
                neighborNodes->vecNeighbors.push_back(neighbors);
            }
        }
    }
    
    //If the current node reach the end, then the while loop will stop and it will still point to the end node
    if(currentNode->coord == end)
    {
        //stack<StreetSegment> startToEnd;
        double distanceTraveled = 0;
        Node * parent = currentNode;
        while (parent != nullptr)
        {
            GeoCoord coord = parent->coord;
            if (parent->parent != nullptr)
            {
                distanceTraveled += distanceEarthMiles(coord, parent->parent->coord);
                StreetSegment segment(parent->parent->coord, coord, parent->streetname);
                route.push_front(segment);
            }
            parent = parent->parent;
        }
        totalDistanceTravelled = distanceTraveled;
        return DELIVERY_SUCCESS;
    }
    //Return NO_ROUTE if there is no path
    return NO_ROUTE;  // Delete this line and implement this function correctly
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
