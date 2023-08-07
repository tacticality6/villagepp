#include <mcpp/mcpp.h>

#include <vector>
#include <string>

#pragma once

class Room
{
private:
    mcpp::Coordinate l1;
    mcpp::Coordinate l2;
    int l;
    int w;
    std::vector<int> layout;
    std::string type;
    mcpp::Coordinate drCrd;

public:
    bool isStaircaseInside;
    bool isFurnished;
    
    const mcpp::Coordinate& loc1;
    const mcpp::Coordinate& loc2;
    const int& length;
    const int& width;
    const mcpp::Coordinate& doorCoord;

    void setLayout();
    void furnish();

};