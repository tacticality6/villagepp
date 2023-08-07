#include <mcpp/mcpp.h>

#include <vector>

#include "./plot.h"
#include "./room.h"

#pragma once

class House 
{
private:
    enum Orientation { NORTH, EAST, SOUTH, WEST};

    mcpp::MinecraftConnection* mc;
    Plot plot;
    mcpp::Coordinate doorCoordinate;
    Orientation orientation;
    std::vector<Room> rooms;
    int numRooms;
    int numFloors;

    //dimensions
    int l;
    int h;
    int w;
    int finalH;

    //house corners (supplementary)
    mcpp::Coordinate loc1;
    mcpp::Coordinate loc2;

    //material blocks
    mcpp::BlockType wallMat;
    mcpp::BlockType floorMat;
    mcpp::BlockType roofMat;

    mcpp::BlockType roofSlabMat;
    mcpp::BlockType pillarMat;
    mcpp::BlockType doorType;
    mcpp::BlockType stairMat;

public:
    explicit House(mcpp::MinecraftConnection* conn, Plot plot);

    const mcpp::Coordinate& getDoorCoordinate();
    
    //build methods
    void build();

    void buildFrame();
    void setFrontDoor();
    void placeFrontDoor();

    void buildRoof();

    void buildRoofNS(const mcpp::Coordinate& locA, const mcpp::Coordinate& locB, int increment=0);
    void buildRoofEW(const mcpp::Coordinate& locA, const mcpp::Coordinate& locB, int increment=0);

    void roomSplit(const Room& room, int choiceAxis); //maybe edit this one

    void buildFloors(const mcpp::Coordinate& locA, const mcpp::Coordinate& locB, bool anotherStorey);

    void buildStairs(const mcpp::Coordinate& loc);

    //void furnishHouse(); //I'm going to put this code in Room

    const Plot& getPlot();

};