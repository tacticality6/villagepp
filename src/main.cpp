#include "../include/village.h"
#include "../include/terraformer.h"
#include <mcpp/mcpp.h>
#include <mcpp/util.h>
#include <mcpp/block.h>

using namespace mcpp;

int main()
{
    MinecraftConnection mc;
    Terraformer terraformer{&mc};

    Coordinate playerCoordinate = mc.getPlayerPosition();
    Coordinate lowCoord = playerCoordinate - Coordinate(8,8,8);
    Coordinate highCoord = playerCoordinate + Coordinate(8,8,8);
    terraformer.placePlotAndSmoothSurroundings(lowCoord, highCoord);

    return 0;
}