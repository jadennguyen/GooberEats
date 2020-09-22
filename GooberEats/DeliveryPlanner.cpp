#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
    
    
private:
    const StreetMap* m_streetmap;
    string proceedAngle(const StreetSegment& street) const;
    string turnAngle(const StreetSegment& street1, const StreetSegment& street2) const;
};

string DeliveryPlannerImpl::proceedAngle(const StreetSegment& street) const
{
    double angle = angleOfLine(street);
    if (angle >= 0 && angle < 22.5)
    {
        return "east";
    }else if (angle >= 22.5 && angle < 67.5)
    {
        return "northeast";
    }else if (angle >= 67.5 && angle < 112.5)
    {
        return "north";
    }else if (angle >= 112.5 && angle < 157.5)
    {
        return "northwest";
    }else if (angle >= 157.5 && angle < 202.5)
    {
        return "west";
    }else if (angle >= 202.5 && angle < 247.5)
    {
        return "southwest";
    }else if (angle >= 247.5 && angle < 292.5)
    {
        return "south";
    }else if (angle >= 292.5 && angle < 337.5)
    {
        return "southeast";
    }else{
        return "east";
    }
}

string DeliveryPlannerImpl::turnAngle(const StreetSegment& street1, const StreetSegment& street2) const
{
    double angle = angleBetween2Lines(street1, street2);
    if (angle < 1 || angle > 359)
    {
        return "NO TURN";
    }else if (angle >= 1 && angle < 180)
    {
        return "left";
    }else
    {
        return "right";
    }
}
DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_streetmap = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    PointToPointRouter getDeliveries(m_streetmap);
    double distanceTravelled;
    list<StreetSegment> route;
    GeoCoord currentLocation = depot;
    
    //Check if the depot is a valid GeoCoord location
    if (getDeliveries.generatePointToPointRoute(depot, depot, route, distanceTravelled) == BAD_COORD)
    {
        return BAD_COORD;
    }
    route.clear();
    
    string startStreetName;
    string direction;
    distanceTravelled = 0;
    double proceedDistance;
    double depotToDelivery;
    for (int i = 0; i < deliveries.size(); i++)
    {
        //Check if a path is possible from the depot to the first delivery or from a delivery to a succesive delivery
        if(getDeliveries.generatePointToPointRoute(currentLocation, deliveries[i].location, route, depotToDelivery) == NO_ROUTE)
        {
            return NO_ROUTE;
        }

        distanceTravelled += depotToDelivery;   //Add up the total distance traveled from depot to delivery and from deliveries to its succesive deliveries
    
        auto routeIT = route.begin();
        while(routeIT != route.end())   //Run through each street segments
        {
            startStreetName = routeIT->name;
            direction = proceedAngle(*routeIT);
            StreetSegment lastStreet;
            proceedDistance = 0;
            
            //While the street segment is the same street and the end of the vector haven't been reached, calculate the proceed distance
            while (startStreetName == routeIT->name && routeIT != route.end())
            {
                proceedDistance += distanceEarthMiles(routeIT->start, routeIT->end);
                lastStreet = *routeIT;
                routeIT++;
            }
            
            //Initialize a proceed command
            DeliveryCommand proceedDC;
            proceedDC.initAsProceedCommand(direction, startStreetName, proceedDistance);
            commands.push_back(proceedDC);  //Add it to the vector of delivery commands
            if (routeIT == route.end())     //If we reached the delivery, then initialize a delivered command and add it to the vector of delivery commands, then break out of the loop
            {
                DeliveryCommand deliveredDC;
                deliveredDC.initAsDeliverCommand(deliveries[i].item);
                commands.push_back(deliveredDC);
                break;
            }else{
                direction = turnAngle(lastStreet, *routeIT);
                DeliveryCommand turnDC;
                turnDC.initAsTurnCommand(direction, routeIT->name);
                commands.push_back(turnDC);
            }
        }
        route.clear();  //Clear the route at the end before resuing the vector
        currentLocation = deliveries[i].location;   //Set the current location to the current delivery so a path can be found to the successive delivery
    }
    GeoCoord endDelivery = deliveries[deliveries.size() - 1].location;  //Get the last delivery location
    getDeliveries.generatePointToPointRoute(endDelivery, depot, route, depotToDelivery);   //get the route from the last delivery back to the depot
    distanceTravelled += depotToDelivery;   //increase the distance traveled again from the last delivery back to the depot
    totalDistanceTravelled = distanceTravelled;
    auto it = route.begin();
    while(it != route.end())   //loop through the segments from the last delivery to the depot, same thing as earlier but without the delivery command
    {
        startStreetName = it->name;
        direction = proceedAngle(*it);
        StreetSegment lastStreet;
        proceedDistance = 0;
        while (startStreetName == it->name && it != route.end())
        {
            proceedDistance += distanceEarthMiles(it->start, it->end);
            lastStreet = *it;
            it++;
        }
        DeliveryCommand proceedDC;
        proceedDC.initAsProceedCommand(direction, startStreetName, proceedDistance);
        commands.push_back(proceedDC);
        if (it == route.end())
        {
            /*
            DeliveryCommand testEnd;
            testEnd.initAsDeliverCommand("Reached DEPOT");
            commands.push_back(testEnd);
            */
            return DELIVERY_SUCCESS;
        }else{
            direction = turnAngle(lastStreet, *it);
            DeliveryCommand turnDC;
            turnDC.initAsTurnCommand(direction, it->name);
            commands.push_back(turnDC);
        }
    }
    return NO_ROUTE;  // Delete this line and implement this function correctly
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
