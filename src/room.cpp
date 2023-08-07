#include <mcpp/mcpp.h>

#include <cassert>
#include <string>

#include "../include/room.h"

using namespace mcpp;

Room::Room(MinecraftConnection* conn, Coordinate c1, Coordinate c2)
    : l1 {c1}
    , l2 {c2}
    , loc1{l1}
    , loc2{l2}
    , length{l}
    , width{w}
{
    assert(conn != nullptr);
    mc = conn;

    l = abs(loc2.x - loc1.x) + 1;
    w = abs(loc2.z - loc1.z) + 1;

    layout = std::vector<std::vector<int>> {
        l,
        std::vector<int> {
            w,
            0
        }
    };
}

void Room::setLayout()
{
    std::vector blockArr = mc->getBlocks(Coordinate(loc1.x, loc1.y-1, loc1.z), Coordinate(loc2.x, loc1.y, loc2.z));

    for (int x=0; x<layout.size(); x++) {
        for (int z=0; z<layout[x].size(); z++)
        {
            BlockType currentBlock = blockArr[1][blockArr[1].size()-1-x][z];
            BlockType underCurrentBlock = blockArr[0][blockArr[0].size()-1-x][z];

            if (currentBlock == Blocks::OAK_DOOR_BLOCK)
                layout[x][z] = 2;
            else if (currentBlock == Blocks::AIR && underCurrentBlock == Blocks::AIR) {
                layout[x][z] = 3;
                isStaircaseInside = true;
            }
            //this is magic, because I'm lazy, if edit stair blocks, fix this
            else if (currentBlock == Blocks::OAK_WOOD_STAIRS
                  || currentBlock == Blocks::COBBLESTONE_STAIRS
                  || currentBlock == Blocks::STONE_BRICK_STAIRS
                  || currentBlock == Blocks::NETHER_BRICK_STAIRS
                  || currentBlock == Blocks::SANDSTONE_STAIRS
                  || currentBlock == Blocks::SPRUCE_WOOD_STAIRS
                  || currentBlock == Blocks::JUNGLE_WOOD_STAIRS
                  || currentBlock == Blocks::ACACIA_WOOD_STAIRS
            ) {
                layout[x][z] = 3;
                isStaircaseInside = true;
            }
            else if (!(currentBlock == Blocks::AIR))
                layout[x][z] = 1;
        }
    }
}

void Room::furnish()
{
    if (isFurnished) return;
    
    int xMid = loc1.x + l/2;
    int zMid = loc1.z + w/2;
    int y = loc2.y -1;

    if (isStaircaseInside)
        mc->doCommand("setblock " + std::to_string(xMid) + " " + std::to_string(y) + " " + std::to_string(zMid) + " minecraft:lantern");
    else
        mc->doCommand("setblock " + std::to_string(xMid) + " " + std::to_string(y) + " " + std::to_string(loc1.z+4) + " minecraft:lantern");
    
    int x = loc2.x;
    int z = loc1.z;
    for (int l=0; l<layout.size(); l++) {
        for (int w=0; w<layout[l].size(); w++) {
            if (layout[l][w] == 0)
            {
                if (type == "small bedroom")
                {

                }
                else if (type == "brewing")
                {

                }
                else if (type == "medium bedroom")
                {

                }
                else if (type == "study")
                {

                }
                else if (type == "large dining")
                {

                }
                else if (type == "living")
                {

                }
            }
            z++;
        }
        z = loc1.z;
        x--;
    }
}