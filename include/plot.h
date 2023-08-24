#include <mcpp/mcpp.h>


#pragma once

class Plot
{
public:
    Plot(mcpp::Coordinate a, mcpp::Coordinate b);
    Plot(mcpp::Coordinate a, mcpp::Coordinate b, int h);

    const mcpp::Coordinate loc1;
    const mcpp::Coordinate loc2;
    const int height;
    const int length;
    const int width;

};