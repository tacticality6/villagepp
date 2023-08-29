#include <mcpp/mcpp.h>

#include <cassert>

#include "../include/decoration.h"
#include "../include/plot.h"

using namespace mcpp;

Decoration::Decoration(MinecraftConnection* conn, Plot plot)
: plot{plot}
{
    assert(conn != nullptr);
    mc = conn;

    x0 = plot.loc1.x;
    y0 = plot.height;
    z0 = plot.loc1.z;
}

void Decoration::build()
{
    enum DecorationType {POND, GARDEN, WINDMILL};
    DecorationType decType = static_cast<DecorationType>(rand() % 3);

    switch (decType)
    {
        case POND: buildPond(); break;
        case GARDEN: buildGarden(); break;
        case WINDMILL: buildWindmill(); break;
    }

}

void Decoration::buildPond()
{
    constexpr int pondSize = 8;
    constexpr int pondDepth = 3;
    int x1 = x0 + 2*pondSize;
    int z1 = z0 + 2*pondSize;
    
    //frame
    mc->setBlocks(Coordinate(x0, y0, z0), Coordinate(x1, y0, z1), Blocks::STONE_BRICK_SLAB);
    mc->setBlocks(Coordinate(x0+1, y0-pondDepth, z0+1), Coordinate(x1-1, y0-1, z1-1), Blocks::STILL_WATER);
    mc->setBlocks(Coordinate(x0+1, y0, z0+1), Coordinate(x1-1, y0, z1-1), Blocks::AIR);

    mc->setBlocks(Coordinate(x0+6, y0+4, z0+6), Coordinate(x0+10, y0+4, z0+10), Blocks::STONE_BRICKS);

    //TODO::
}

void Decoration::buildGarden()
{
    
}

void Decoration::buildWindmill()
{
    
}