#include <mcpp/mcpp.h>
#include <mcpp/block.h>
#include <mcpp/util.h>

#include <vector>
#include <map>

#pragma once


class Terraformer
{
private:
    static constexpr int MAX_HEIGHT = 255;
    mcpp::MinecraftConnection* mc;
public:
    /*
     
    */
    explicit Terraformer(mcpp::MinecraftConnection* conn = nullptr);

    int flattenPlot(const mcpp::Coordinate& loc1, const mcpp::Coordinate& loc2);

    int placePlotAndSmoothSurroundings(const mcpp::Coordinate& loc1, const mcpp::Coordinate& loc2, int SEARCH_RANGE);

    void purgeTrees(const mcpp::Coordinate& loc1, const mcpp::Coordinate& loc2);

    const std::map<const mcpp::Coordinate, const int> getHeightMap(const mcpp::Coordinate& loc1, const mcpp::Coordinate& loc2);

    const std::map<const mcpp::Coordinate, const BlockType> getBlockMap(const mcpp::Coordinate& loc1, const mcpp::Coordinate& loc2);

    const std::map<const mcpp::Coordinate, const BlockType> getSurfaceBlockMap(const mcpp::Coordinate& loc1, const mcpp::Coordinate& loc2);
};