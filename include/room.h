#include <mcpp/mcpp.h>

#include <vector>
#include <string>

#pragma once

class Room
{
private:
    mcpp::MinecraftConnection* mc;
    mcpp::Coordinate l1;
    mcpp::Coordinate l2;
    int l;
    int w;
    std::vector<std::vector<int>> layout;
    std::string type;

public:
    Room(mcpp::MinecraftConnection* conn, mcpp::Coordinate c1, mcpp::Coordinate c2);

    bool isStaircaseInside;
    bool isFurnished;

    const mcpp::Coordinate& loc1;
    const mcpp::Coordinate& loc2;
    const int& length;
    const int& width;

    void setLayout();
    void furnish();

};