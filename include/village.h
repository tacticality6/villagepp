#include <mcpp/mcpp.h>

#include <vector>

#include "terraformer.h"
#include "plot.h"

#pragma once

class Village
{
private:
    enum ArrayFeatures {NONE, PLOT, DOOR, TREE, WATER, TEMP_ISOLATE};
    mcpp::MinecraftConnection* mc;
    static constexpr int maxSize{150};
    static constexpr int plotSize{15};
    mcpp::Coordinate centrepoint;
    std::pair<mcpp::Coordinate, mcpp::Coordinate> villageDomain;
    std::vector<std::vector<ArrayFeatures>> arrayRepresentation;
    Terraformer terraformer;
    int numPlots;
    std::vector<Plot> plots;



public:
    explicit Village(mcpp::MinecraftConnection* conn);

    void build();

    void updateArrayRepresentationNaturalFeatures();
    std::vector<Plot> setPlots();

    mcpp::Coordinate worldToVillage(const mcpp::Coordinate& loc);
    mcpp::Coordinate villageToWorld(const mcpp::Coordinate& loc);
};