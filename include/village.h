#include <mcpp/mcpp.h>

#include <vector>

#include "terraformer.h"

#pragma once

class Village
{
private:
    enum ArrayRepresentation {NONE, PLOT, DOOR, WATER};
    static constexpr int maxSize{150};
    static constexpr int plotSize{15};
    mcpp::MinecraftConnection* mc;
    Terraformer* terraformer;
    int numPlots;
    mcpp::Coordinate centrepoint;
    std::vector<int> villageDomain;



public:
    explicit Village(mcpp::MinecraftConnection* conn = nullptr);

    
};