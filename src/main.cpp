#include "../include/village.h"
#include <mcpp/mcpp.h>
#include <mcpp/util.h>
#include <mcpp/block.h>

using namespace mcpp;

int main()
{
    mcpp::MinecraftConnection mc;

    mc.postToChat("TEST");
    Coordinate playerPos = mc.getPlayerPosition();
    mc.setBlocks(playerPos, playerPos + Coordinate(5, 5, 5), Blocks::STONE);
}