#include "provided.h"
#include <string>
#include <vector>
#include <functional>
using namespace std;
#include "ExpandableHashMap.h"
#include <fstream>
#include <sstream>

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
    
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> geoCoordToSteet;
    vector<StreetSegment> uniqueStreets;
};

StreetMapImpl::StreetMapImpl()
{
    //geoCoordToSteet.reset();
}

StreetMapImpl::~StreetMapImpl()
{
    
}


bool StreetMapImpl::load(string mapFile)
{
    // Open the file for input
    ifstream infile(mapFile);
    if (!infile)  // Test for failure to open
    {
        cerr << "Error: Cannot open " << mapFile << endl;
        return false;
    }
    string line;
    while (getline(infile, line))
    {
        uniqueStreets.clear();
        //First Line is the Street Name
        string streetName = line;
        
        //Second Line is number of Street Segments
        getline(infile, line);
        istringstream segmentCount(line);
        int numOfSegments;
        segmentCount >> numOfSegments;
        
        //fill up the vector of street segments
        for (int i = 0; i < numOfSegments; i++)
        {
            //Get line after number of segment line
            getline(infile, line);
            istringstream segment(line);
            string start1, start2, end1, end2;
            segment >> start1 >> start2 >> end1 >> end2;
            GeoCoord starting(start1, start2);
            GeoCoord ending(end1, end2);
            StreetSegment street(starting, ending, streetName);
            uniqueStreets.push_back(street);
        }
        
        vector<StreetSegment> currentStreets;
        for (int i = 0; i < uniqueStreets.size(); i++)
        {
            GeoCoord start = uniqueStreets[i].start;
            vector<StreetSegment>* checkCoord = geoCoordToSteet.find(start);
            if (checkCoord != nullptr)
            {
                checkCoord->push_back(uniqueStreets[i]);
            }else{
                currentStreets.push_back(uniqueStreets[i]);
                geoCoordToSteet.associate(uniqueStreets[i].start, currentStreets);
            }
            GeoCoord end = uniqueStreets[i].end;
            checkCoord = geoCoordToSteet.find(end);
            StreetSegment reverse(uniqueStreets[i].end, uniqueStreets[i].start, uniqueStreets[i].name);
            if (checkCoord != nullptr) {
                checkCoord->push_back(reverse);
            }else{
                currentStreets.clear();
                currentStreets.push_back(reverse);
                geoCoordToSteet.associate(uniqueStreets[i].end, currentStreets);
            }
        }
    }
    return true;  // Delete this line and implement this function correctly
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment>* temp = geoCoordToSteet.find(gc);
    if (temp == nullptr)
    {
        return false;
    }else{
        segs = *temp;
    }
    return true;  // Delete this line and implement this function correctly
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
