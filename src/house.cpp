#include <mcpp/mcpp.h>
#include <mcpp/block.h>

#include <vector>
#include <random>
#include <cassert>
#include <algorithm>
#include <iterator>

#include "../include/house.h"

#include "../include/plot.h"
#include "../include/room.h"

using namespace mcpp;

House::House(mcpp::MinecraftConnection* conn, Plot plot)
    : plot{plot}
{
    assert(conn != nullptr);
    this->mc = conn;

    this->numRooms = 0;
    this->numFloors = 0;

    this->l = rand() % (plot.length / 2) + ((plot.length / 2)+1);
    this->h = 5;
    this->w = rand() % (plot.width / 2) + ((plot.width / 2));

    this->loc1 = plot.loc1;
    this->loc2 = Coordinate(plot.loc1.x+l-1, plot.height-1, plot.loc1.z+w-1);


    //block types arrays
    const std::vector<BlockType> structureMats {
        Blocks::OAK_WOOD_PLANK,
        Blocks::SPRUCE_WOOD_PLANK,
        Blocks::BIRCH_WOOD_PLANK,
        Blocks::JUNGLE_WOOD_PLANK,
        Blocks::COBBLESTONE,
        Blocks::STONE_BRICKS,
        Blocks::NETHER_BRICK,
        Blocks::SANDSTONE
    };
    const std::vector<BlockType> roofSlabMats {
        Blocks::STONE_SLAB,
        Blocks::SANDSTONE_SLAB,
        Blocks::WOODEN_SLAB,
        Blocks::COBBLESTONE_SLAB,
        Blocks::BRICK_SLAB,
        Blocks::STONE_BRICK_SLAB,
        Blocks::NETHER_BRICK_SLAB,
        Blocks::QUARTZ_SLAB,
        Blocks::OAK_WOOD_SLAB,
        Blocks::SPRUCE_WOOD_SLAB,
        Blocks::JUNGLE_WOOD_SLAB,
        Blocks::ACACIA_WOOD_SLAB,
        Blocks::DARK_OAK_WOOD_SLAB
    };
    const std::vector<BlockType> pillarMats {
        Blocks::OAK_WOOD,
        Blocks::SPRUCE_WOOD,
        Blocks::BIRCH_WOOD,
        Blocks::JUNGLE_WOOD,
        Blocks::STONE_BRICKS,
        Blocks::SANDSTONE
    };
    const std::vector<BlockType> stairMats {
        Blocks::OAK_WOOD_STAIRS,
        Blocks::COBBLESTONE_STAIRS,
        Blocks::STONE_BRICK_STAIRS,
        Blocks::NETHER_BRICK_STAIRS,
        Blocks::SANDSTONE_STAIRS,
        Blocks::SPRUCE_WOOD_STAIRS,
        Blocks::JUNGLE_WOOD_STAIRS,
        Blocks::ACACIA_WOOD_STAIRS
    };

    //set block types
    //structure blocks
    std::vector<BlockType> selectedVec;
    std::sample(structureMats.begin(), 
                structureMats.end(), 
                std::back_inserter(selectedVec), 
                3, 
                std::mt19937 {std::random_device{}()}
               );
    this->wallMat = selectedVec[0];
    this->floorMat = selectedVec[1];
    this->roofMat = selectedVec[2];

    int index = rand() % roofSlabMats.size();
    this->roofSlabMat = roofSlabMats[index];

    index = rand() % pillarMats.size();
    this->pillarMat = pillarMats[index];

    index = rand() % stairMats.size();
    this->stairMat = stairMats[index];

    this->doorType = Blocks::OAK_DOOR_BLOCK;
}


void House::build()
{
    this->buildFrame();
    this->setFrontDoor();
    this->placeFrontDoor();

    int choiceAxis = (plot.length >= plot.width) ? 0 : 1;
    this->roomSplit(this->rooms[this->numRooms], choiceAxis);

    bool isMultistorey = (bool)(rand()%2);
    this->buildFloors(isMultistorey);

    this->buildRoof();
}

void House::buildFrame()
{
    this->mc->setBlocks(loc1, loc2, wallMat);
}

const Plot& House::getPlot()
{
    return plot;
}