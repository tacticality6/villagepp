#include <mcpp/mcpp.h>

#include <algorithm>
#include <iostream>
#include <random>
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

    //material blocks
    mcpp::BlockType wallMat;
    mcpp::BlockType floorMat;
    mcpp::BlockType roofMat;

    mcpp::BlockType roofSlabMat;
    mcpp::BlockType pillarMat;
    mcpp::BlockType doorType;
    mcpp::BlockType stairMat;

public:
    explicit House(mcpp::MinecraftConnection* mc, Plot plot, Orientation orientation);

    //build methods
    void build();

    void buildFrame();
    void setFrontDoor();
    void placeFrontDoor();

    void buildRoof();

    void buildRoofNS(int increment=0);
    void buildRoofEW(int increment=0);
    void buildRoofPyramid();
    void buildRoofFlat();

    void roomSplit(Room room, int choiceAxis); //maybe edit this one

    void buildFloors(bool anotherStorey);

    void buildStairs(mcpp::Coordinate loc);

    //void furnishHouse(); //I'm going to put this code in Room

    mcpp::Coordinate getDoorCoordinate();
};