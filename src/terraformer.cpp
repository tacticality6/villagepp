#include <mcpp/mcpp.h>
#include <mcpp/block.h>

#include <numeric>


#include "terraformer.h"


/*
Terraformer
    Class to handle all methods required to 
    shape the minecraft world
*/
using namespace mcpp;


explicit Terraformer::Terraformer(MinecraftConnection* conn)
{
    if (conn)
    {
        mc = conn;
    }
    else
    {
        mc = &MinecraftConnection();
    }
}


int Terraformer::flattenPlot(const Coordinate& loc1, const Coordinate& loc2)
{
    this->purgeTrees(loc1, loc2);

    auto heights = mc->getHeights(loc1, loc2);

    int avg_height = static_cast<int>(std::reduce(heights.begin(), heights.end(), 0.0) / heights.size());

    ///TODO: get some other data
    auto surfaceBlocks = this->getSurfaceBlockMap(loc1, loc2);

    //iterate and flatten
    for (auto el : surfaceBlocks)
    {
        const Coordinate& loc = el.first;
        const BlockType surfaceBlock = el.second;

        //set blocks above to air
        mc->setBlocks(Coordinate(loc.x, avg_height+1, loc.z), Coordinate(loc.x, MAX_HEIGHT, loc.z), Blocks::AIR);
        //set surface block
        mc->setBlock(Coordinate(loc.x, avg_height, loc.z), surfaceBlock);
        //set blocks below
        mc->setBlocks(Coordinate(loc.x, avg_height-15, loc.z), Coordinate(loc.x, avg_height-1, loc.z), (surfaceBlock == Blocks::GRASS ? Blocks::DIRT : surfaceBlock));

    }

    return avg_height;
}


int Terraformer::placePlotAndSmoothSurroundings(const Coordinate& loc1, const Coordinate& loc2, int SEARCH_RANGE=5)
{
    Coordinate scaledLoc1 = Coordinate(loc1.x-SEARCH_RANGE, loc1.y, loc1.z-SEARCH_RANGE);
    Coordinate scaledLoc2 = Coordinate(loc2.x-SEARCH_RANGE, loc2.y, loc2.z-SEARCH_RANGE);
    //first, flatten the plot
    int plotHeight = this->flattenPlot(loc1, loc2);
    //remove trees from search range
    this->purgeTrees(scaledLoc1, scaledLoc2);

    //get data
    auto heightMap = this->getHeightMap(scaledLoc1, scaledLoc2);
    auto surfaceMap = this->getSurfaceBlockMap(scaledLoc1, scaledLoc2);

    //minimize unnatural jumps in height by using a scaled value based on distance
    double weightScale = 1.0 / SEARCH_RANGE;
    //iterate
    for (int i=0; i<SEARCH_RANGE+1; i++)
    {
        for (auto el : heightMap)
        {
            const Coordinate loc = el.first;
            BlockType surfaceBlock = surfaceMap[loc];
            int blockHeight = heightMap[loc];
            //skip over fluid/ice blocks
            if (surfaceBlock == Blocks::STILL_WATER || surfaceBlock == Blocks::FLOWING_WATER ||
                surfaceBlock == Blocks::STILL_LAVA  || surfaceBlock == Blocks::FLOWING_LAVA  ||
                surfaceBlock == Blocks::ICE
            )
                continue;

            //this large logic block selects a rectangle from the height map
            //the rectangle starts from the outermost and moves in, based on i
            if (!(
                (
                    ((loc.x == loc1.x+i) || (loc.x == (loc2.x-i)))
                    &&
                    ((loc.z >= loc1.z+1) && (loc.z <= (loc2.z-i)))
                )
                ||
                (
                    ((loc.z == loc1.z+i) || (loc.z == (loc2.z-i)))
                    &&
                    ((loc.x >= loc1.x+1) && (loc.x <= (loc2.x-i)))
            )))
                continue;

            //if we get to this stage, we have selected the correct rectangle
            int targetHeight = static_cast<int>(blockHeight + std::ceil((plotHeight-blockHeight)*(weightScale*i)));

            //surface block
            mc->setBlock(Coordinate(loc.x, targetHeight, loc.z), surfaceBlock);
            if (blockHeight < plotHeight)
            {
                //build up 
                //set below
                mc->setBlocks(Coordinate(loc.x, targetHeight-10, loc.z), Coordinate(loc.x, targetHeight-1, loc.z), (surfaceBlock == Blocks::GRASS ? Blocks::DIRT : surfaceBlock));
            }
            else
            {
                //build down
                //clear above
                mc->setBlocks(Coordinate(loc.x, targetHeight+1, loc.z), Coordinate(loc.x, MAX_HEIGHT, loc.z), Blocks::AIR);
            }
        }
    }

    return plotHeight;

}


void Terraformer::purgeTrees(const mcpp::Coordinate& loc1, const mcpp::Coordinate& loc2)
{
    int lowestHeight;
    int greatestHeight;
}