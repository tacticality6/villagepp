#include <mcpp/mcpp.h>
#include <mcpp/block.h>

#include <numeric>
#include <cassert>
#include <cmath>

#include "../include/terraformer.h"


/*
Terraformer
    Class to handle all methods required to 
    shape the minecraft world
*/
using namespace mcpp;


Terraformer::Terraformer(MinecraftConnection* conn)
{
    assert(conn != nullptr);
    mc = conn;
}

int Terraformer::flattenPlot(const Coordinate& loc1, const Coordinate& loc2)
{
    //first remove trees from the plot
    this->purgeTrees(loc1, loc2);

    //get the average height
    std::vector heights = this->mc->getHeights(loc1, loc2);
    int avgHeight = 0;
    int numBlocks = 0;
    for (std::vector row : heights)
        for (int height : row)
        {
            avgHeight += height;
            numBlocks += 1;
        }
    
    avgHeight = (int)(((double)avgHeight / numBlocks) + 0.5);
    std::vector surfaceBlocks = this->getSurfaceBlocks(loc1, loc2);

    for (int x=0; x < surfaceBlocks.size(); x++) {
        for(int z=0; z < surfaceBlocks[x].size(); z++)
        {
            BlockType surfaceBlock = surfaceBlocks[x][z];

            //set blocks above to air
            this->mc->setBlocks(Coordinate(loc1.x+x, avgHeight, loc1.z+z), 
                                Coordinate(loc1.x+x, MAX_HEIGHT, loc1.z+z),
                                Blocks::AIR);
            
            //set top block to surface block
            this->mc->setBlock(Coordinate(loc1.x+x, avgHeight, loc1.z+z), 
                               surfaceBlock);
            
            //set blocks below
            //if top block is grass, set below blocks to dirt
            if (surfaceBlock.id == Blocks::GRASS.id)
                this->mc->setBlocks(Coordinate(loc1.x+x, 0, loc1.z+z),
                                    Coordinate(loc1.x+x, avgHeight-1, loc1.z+z),
                                    Blocks::DIRT);
            else
                this->mc->setBlocks(Coordinate(loc1.x+x, 0, loc1.z+z),
                                    Coordinate(loc1.x+x, avgHeight-1, loc1.z+z),
                                    surfaceBlock);
            
        }
    }
    return avgHeight;
}

std::vector<std::vector<BlockType>> Terraformer::getSurfaceBlocks(const Coordinate& loc1, const Coordinate& loc2)
{
    std::vector heights = this->mc->getHeights(loc1, loc2);
    std::vector blocks = this->mc->getBlocks(Coordinate(loc1.x, 0, loc1.z), 
                                             Coordinate(loc2.x, MAX_HEIGHT, loc2.z));

    std::vector<std::vector<BlockType>> surfaceBlocks (
        heights.size(),
        std::vector<BlockType> (
            heights[0].size(),
            Blocks::DIRT
        )
    );
    for (int x=0; x < heights.size(); x++) {
        for (int z=0; z < heights[x].size(); z++)
        {
            int y = heights[x][z];
            surfaceBlocks[x][z] = blocks[y][x][z];
        }
    }

    return surfaceBlocks;
}

void Terraformer::purgeTrees(const Coordinate& loc1, const Coordinate& loc2)
{
    std::vector blocks = this->mc->getBlocks(Coordinate(loc1.x, 0, loc1.z),
                                             Coordinate(loc2.x, MAX_HEIGHT, loc2.z));
    
    for (int y=0; y < blocks.size(); y++) {
        for (int x=0; x < blocks[y].size(); x++) {
            for (int z=0; z < blocks[x].size(); z++) 
            {
                BlockType block = blocks[y][x][z];
                if (block.id == Blocks::OAK_WOOD.id 
                ||  block.id == Blocks::OAK_LEAVES.id)
                {
                    this->mc->setBlock(Coordinate(loc1.x+x, y, loc1.z+z),
                                      Blocks::AIR);
                }
            }
        }
    }
}

int Terraformer::placePlotAndSmoothSurroundings(const Coordinate& loc1, const Coordinate& loc2, int SEARCH_RANGE)
{
    int plotHeight = this->flattenPlot(loc1, loc2);

    Coordinate search{SEARCH_RANGE,0,SEARCH_RANGE};
    Coordinate extendedLoc1 = loc1 - search;
    Coordinate extendedLoc2 = loc2 + search;

    this->purgeTrees(extendedLoc1, extendedLoc2);

    std::vector surfaceBlocks = this->getSurfaceBlocks(extendedLoc1, extendedLoc2);
    std::vector heights = this->mc->getHeights(extendedLoc1, extendedLoc2);
    double weightScale = 1.0 / SEARCH_RANGE;

    for (int i=0; i<SEARCH_RANGE+1; i++) {
        for (int x=0; x<surfaceBlocks.size(); x++) {
            for (int z=0; z<surfaceBlocks[x].size(); z++)
            {
                BlockType surfaceBlock = surfaceBlocks[x][z];

                //dont touch water
                switch (surfaceBlock.id)
                {
                    case Blocks::STILL_WATER.id:
                    case Blocks::FLOWING_WATER.id:
                    case Blocks::STILL_LAVA.id:
                    case Blocks::FLOWING_LAVA.id:
                        continue;
                }

                //get correct slice
                //this mess selects a rectangle from the 2d array, with corners i from the edge
                if ( ! (
                    (
                        ((x==i) || (x == (surfaceBlocks.size()-1-i)))
                        &&
                        ((z>=i) && (z <= (surfaceBlocks[x].size()-1-i)))
                    )
                    ||
                    (
                        ((z==i) || (z == (surfaceBlocks[x].size()-1-i)))
                        &&
                        ((x>=i) && (x <= (surfaceBlocks.size()-1-i)))
                    )
                    )) {
                    continue;
                }

                int realX = extendedLoc1.x+x;
                int realZ = extendedLoc1.z+z;

                int targetHeight = heights[x][z] + ceil((plotHeight-heights[x][z])*(weightScale*i));

                //if building up
                if (heights[x][z] < plotHeight)
                {
                    //top block
                    this->mc->setBlock(Coordinate(realX, targetHeight, realZ), surfaceBlock);

                    //set below
                    //if top block is grass, set elow blocks to dirt
                    if (surfaceBlock.id == Blocks::GRASS.id)
                        this->mc->setBlocks(Coordinate(realX, targetHeight-1, realZ),
                                            Coordinate(realX, targetHeight-10, realZ),
                                            Blocks::DIRT);
                    else
                        this->mc->setBlocks(Coordinate(realX, targetHeight-1, realZ),
                                            Coordinate(realX, targetHeight-10, realZ),
                                            surfaceBlock);
                }
                else if (heights[x][z] > plotHeight)
                {
                    //build down
                    //top block
                    this->mc->setBlock(Coordinate(realX, targetHeight, realZ), surfaceBlock);
                    //clear above
                    this->mc->setBlocks(Coordinate(realX, targetHeight+1, realZ),
                                        Coordinate(realX, MAX_HEIGHT, realZ),
                                        Blocks::AIR);

                }
            }
        }
    }
    return plotHeight;
}