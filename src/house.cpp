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
#include "../include/blockMods.h"

using namespace mcpp;

House::House(mcpp::MinecraftConnection* conn, Plot plot)
    : plot{plot}
    , wallMat{Blocks::AIR}
    , floorMat{Blocks::AIR}
    , roofMat{Blocks::AIR}
    , roofSlabMat{Blocks::AIR}
    , pillarMat{Blocks::AIR}
    , doorType{Blocks::AIR}
    , stairMat{Blocks::AIR}
{
    assert(conn != nullptr);
    this->mc = conn;

    this->numRooms = 0;
    this->numFloors = 0;

    this->l = rand() % (plot.length / 2) + ((plot.length / 2)+1);
    this->h = 5;
    this->finalH = h;
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
    this->buildFloors(Coordinate(loc1.x, loc2.y+1, loc1.z), Coordinate(loc2.x, loc2.y+h, loc2.z), isMultistorey);

    this->buildRoof();
}

void House::buildFrame()
{
    mc->setBlocks(loc1, loc2, wallMat);

    mc->setBlocks(Coordinate(loc1.x+2, loc1.y+1, loc1.z), Coordinate(loc2.x-2, loc2.y-1, loc2.z), Blocks::GLASS_PANE);
    mc->setBlocks(Coordinate(loc1.x, loc1.y+1, loc1.z+2), Coordinate(loc2.x, loc2.y-1, loc2.z-2), Blocks::GLASS_PANE);

    mc->setBlocks(Coordinate(loc1.x+2, loc2.y-1, loc1.z-1), Coordinate(loc2.x-2, loc2.y-1, loc1.z-1), roofSlabMat.withMod(8));
    mc->setBlocks(Coordinate(loc1.x+2, loc2.y-1, loc2.z+1), Coordinate(loc2.x-2, loc2.y-1, loc2.z+1), roofSlabMat.withMod(8));
    mc->setBlocks(Coordinate(loc1.x-1, loc2.y-1, loc1.z+2), Coordinate(loc1.x-1, loc2.y-1, loc2.z-2), roofSlabMat.withMod(8));
    mc->setBlocks(Coordinate(loc2.x+1, loc2.y-1, loc1.z+2), Coordinate(loc2.x+1, loc2.y-1, loc2.z-2), roofSlabMat.withMod(8));

    mc->setBlocks(Coordinate(loc1.x+1, loc1.y, loc1.z+1), Coordinate(loc2.x-1, loc2.y-1, loc2.z-1), Blocks::AIR);
    
    mc->setBlocks(Coordinate(loc1.x, loc1.y-1, loc1.z), Coordinate(loc2.x, loc1.y-1, loc2.z), floorMat);

    std::vector<int> xCoords = {loc1.x, loc2.x};
    std::vector<int> zCoords = {loc1.z, loc2.z};
    for (int x : xCoords) {
        for (int z: zCoords) {
            mc->setBlocks(Coordinate(x, loc1.y-1, z), Coordinate(x, loc2.y, z), pillarMat);
        }
    }

    rooms.resize(numRooms+1); // dodgy way of making it work like the python code
    rooms[numRooms] = Room(); //TODO:: FIX

}

void House::setFrontDoor()
{
    orientation = (Orientation)(rand() % 4); //unfortunate magic number, will work in this circumstance

    if (orientation == Orientation::NORTH || orientation == Orientation::SOUTH)
    {
        int xDoorCoord = (rand() % ((loc2.x - 3) - (loc1.x + 3))) + loc1.x+3;
        if (orientation == Orientation::NORTH)
            this->doorCoordinate = Coordinate(xDoorCoord, loc1.y, loc1.z);
        else
            this->doorCoordinate = Coordinate(xDoorCoord, loc1.y, loc2.z);
    } else
    {
        int zDoorCoord = (rand() % ((loc2.z - 3) - (loc1.z + 3))) + loc1.z+3;
        if (orientation = Orientation::EAST)
            this->doorCoordinate = Coordinate(loc2.x, loc1.y, zDoorCoord);
        else
            this->doorCoordinate = Coordinate(loc1.x, loc1.y, zDoorCoord);
    }
}

void House::placeFrontDoor()
{
    int x = doorCoordinate.x;
    int y = doorCoordinate.y;
    int z = doorCoordinate.z;

    switch(orientation)
    {
        case Orientation::NORTH: 
        {
            mc->setBlock(Coordinate(x,y+2,loc1.z), stairMat.withMod(BlockMods::StairOrient::NORTH_DESCENDING));
            
            mc->setBlocks(Coordinate(x-1,y-1,loc1.z), Coordinate(x-1,loc2.y,loc1.z), pillarMat);
            mc->setBlocks(Coordinate(x-2,y,loc1.z), Coordinate(x-2,loc2.y,loc1.z), wallMat);
            mc->setBlocks(Coordinate(x+1,y-1,loc1.z), Coordinate(x+1,loc2.y,loc1.z), pillarMat);
            mc->setBlocks(Coordinate(x+2,y,loc1.z), Coordinate(x+2,loc2.y,loc1.z), wallMat);

            mc->setBlock(Coordinate(x, loc2.y+1, loc1.z-1), stairMat.withMod(BlockMods::StairOrient::NORTH_ASCENDING));
            mc->setBlock(Coordinate(x+1, loc2.y, loc1.z-1), stairMat.withMod(BlockMods::StairOrient::NORTH_ASCENDING));
            mc->setBlock(Coordinate(x-1, loc2.y, loc1.z-1), stairMat.withMod(BlockMods::StairOrient::NORTH_ASCENDING));

            mc->setBlock(Coordinate(x, loc2.y, loc1.z-1), stairMat.withMod(BlockMods::StairOrient::NORTH_DESCENDING));
            
            mc->setBlock(Coordinate(x, y+1, z), doorType.withMod(BlockMods::DoorOrient::SOUTH_TOP));
            mc->setBlock(Coordinate(x, y, z), doorType.withMod(BlockMods::DoorOrient::SOUTH_BOTTOM));
            
            break;
        }
        case Orientation::EAST: 
        {
            mc->setBlock(Coordinate(loc2.x,y+2,z), stairMat.withMod(BlockMods::StairOrient::WEST_DESCENDING));
            
            mc->setBlocks(Coordinate(loc2.x,y-1,z-1), Coordinate(loc2.x,loc2.y,z-1), pillarMat);
            mc->setBlocks(Coordinate(loc2.x,y,z-2), Coordinate(loc2.x,loc2.y,z-2), wallMat);
            mc->setBlocks(Coordinate(loc2.x,y-1,z+1), Coordinate(loc2.x,loc2.y,z+1), pillarMat);
            mc->setBlocks(Coordinate(loc2.x,y,loc1.z+2), Coordinate(loc2.x,loc2.y,loc1.z+2), wallMat);

            mc->setBlock(Coordinate(loc2.x+1, loc2.y+1, z), stairMat.withMod(BlockMods::StairOrient::WEST_ASCENDING));
            mc->setBlock(Coordinate(loc2.x+1, loc2.y, z+1), stairMat.withMod(BlockMods::StairOrient::WEST_ASCENDING));
            mc->setBlock(Coordinate(loc2.x+1, loc2.y, z-1), stairMat.withMod(BlockMods::StairOrient::WEST_ASCENDING));

            mc->setBlock(Coordinate(loc2.x+1, loc2.y, z-1), stairMat.withMod(BlockMods::StairOrient::WEST_DESCENDING));
            
            mc->setBlock(Coordinate(x, y+1, z), doorType.withMod(BlockMods::DoorOrient::WEST_TOP));
            mc->setBlock(Coordinate(x, y, z), doorType.withMod(BlockMods::DoorOrient::WEST_BOTTOM));
            
            break;
        }
        case Orientation::SOUTH: 
        {
            mc->setBlock(Coordinate(x,y+2,loc2.z), stairMat.withMod(BlockMods::StairOrient::SOUTH_DESCENDING));
            
            mc->setBlocks(Coordinate(x-1,y-1,loc2.z), Coordinate(x-1,loc2.y,loc2.z), pillarMat);
            mc->setBlocks(Coordinate(x-2,y,loc2.z), Coordinate(x-2,loc2.y,loc2.z), wallMat);
            mc->setBlocks(Coordinate(x+1,y-1,loc2.z), Coordinate(x+1,loc2.y,loc2.z), pillarMat);
            mc->setBlocks(Coordinate(x+2,y,loc2.z), Coordinate(x+2,loc2.y,loc2.z), wallMat);

            mc->setBlock(Coordinate(x, loc2.y+1, loc2.z-1), stairMat.withMod(BlockMods::StairOrient::SOUTH_ASCENDING));
            mc->setBlock(Coordinate(x+1, loc2.y, loc2.z-1), stairMat.withMod(BlockMods::StairOrient::SOUTH_ASCENDING));
            mc->setBlock(Coordinate(x-1, loc2.y, loc2.z-1), stairMat.withMod(BlockMods::StairOrient::SOUTH_ASCENDING));

            mc->setBlock(Coordinate(x, loc2.y, loc2.z-1), stairMat.withMod(BlockMods::StairOrient::SOUTH_DESCENDING));
            
            mc->setBlock(Coordinate(x, y+1, z), doorType.withMod(BlockMods::DoorOrient::NORTH_TOP));
            mc->setBlock(Coordinate(x, y, z), doorType.withMod(BlockMods::DoorOrient::NORTH_BOTTOM));
            
            break;
        }
        case Orientation::WEST: 
        {
            mc->setBlock(Coordinate(loc1.x,y+2,z), stairMat.withMod(BlockMods::StairOrient::EAST_DESCENDING));
            
            mc->setBlocks(Coordinate(loc1.x,y-1,z-1), Coordinate(loc1.x,loc2.y,z-1), pillarMat);
            mc->setBlocks(Coordinate(loc1.x,y,z-2), Coordinate(loc1.x,loc2.y,z-2), wallMat);
            mc->setBlocks(Coordinate(loc1.x,y-1,z+1), Coordinate(loc1.x,loc2.y,z+1), pillarMat);
            mc->setBlocks(Coordinate(loc1.x,y,loc1.z+2), Coordinate(loc1.x,loc2.y,loc1.z+2), wallMat);

            mc->setBlock(Coordinate(loc1.x+1, loc2.y+1, z), stairMat.withMod(BlockMods::StairOrient::EAST_ASCENDING));
            mc->setBlock(Coordinate(loc1.x+1, loc2.y, z+1), stairMat.withMod(BlockMods::StairOrient::EAST_ASCENDING));
            mc->setBlock(Coordinate(loc1.x+1, loc2.y, z-1), stairMat.withMod(BlockMods::StairOrient::EAST_ASCENDING));

            mc->setBlock(Coordinate(loc1.x+1, loc2.y, z-1), stairMat.withMod(BlockMods::StairOrient::EAST_DESCENDING));
            
            mc->setBlock(Coordinate(x, y+1, z), doorType.withMod(BlockMods::DoorOrient::EAST_TOP));
            mc->setBlock(Coordinate(x, y, z), doorType.withMod(BlockMods::DoorOrient::EAST_BOTTOM));
            
            break;
        }
    }
}

void House::buildRoofNS(const mcpp::Coordinate& locA, const mcpp::Coordinate& locB, int increment=0)
{
    if (this->l %2 == 0)
    {
        if (locA.x == floor(locA.x + l/2.0))
        {
            mc->setBlocks(Coordinate(locA.x-1, locA.y-1, locA.z-1), Coordinate(locA.x-1, locA.y-1, locB.z+1), wallMat);
            mc->setBlocks(Coordinate(locB.x+1, locA.y-1, locA.z-1), Coordinate(locB.x+1, locA.y-1, locB.z+1), wallMat);
            return;
        }
    }
    else
    {
        if (locA.x == ceil(locA.x + l/2.0))
        {
            mc->setBlocks(Coordinate(locA.x-1, locA.y-1, locA.z-1), Coordinate(locA.x-1, locA.y-1, locB.z+1), wallMat);
            return;
        }
    }

    mc->setBlocks(Coordinate(locA.x-1, locA.y, locA.z-1), Coordinate(locA.x-1, locA.y, locB.z+1), stairMat.withMod(BlockMods::StairOrient::EAST_ASCENDING));
    mc->setBlocks(Coordinate(locB.x+1, locA.y, locA.z-1), Coordinate(locB.x+1, locA.y, locB.z+1), stairMat.withMod(BlockMods::StairOrient::WEST_ASCENDING));

    mc->setBlock(Coordinate(locA.x, locA.y, locA.z-1), stairMat.withMod(BlockMods::StairOrient::WEST_DESCENDING));
    mc->setBlock(Coordinate(locB.x, locA.y, locA.z-1), stairMat.withMod(BlockMods::StairOrient::EAST_DESCENDING));
    mc->setBlock(Coordinate(locA.x, locA.y, locB.z+1), stairMat.withMod(BlockMods::StairOrient::WEST_DESCENDING));
    mc->setBlock(Coordinate(locB.x, locA.y, locB.z+1), stairMat.withMod(BlockMods::StairOrient::EAST_DESCENDING));

    if (increment >= 1)
    {
        mc->setBlocks(locA, Coordinate(locB.x, locA.y, locA.z), wallMat);
        mc->setBlocks(Coordinate(locA.x, locA.y, locB.z), Coordinate(locB.x, locA.y, locB.z), wallMat);
    }

    increment++;

    return buildRoofNS(Coordinate(locA.x+1, locA.y+1, locA.z), Coordinate(locB.x-1, locA.y+1, locB.z), increment);
}

void House::buildRoofEW(const Coordinate& locA, const Coordinate& locB, int increment=0)
{
    if (this->w %2 == 0)
    {
        if (locA.z == floor(locA.z + w/2.0))
        {
            mc->setBlocks(Coordinate(locA.x-1, locA.y-1, locA.z-1), Coordinate(locB.x+1, locA.y-1, locA.z-1), wallMat);
            mc->setBlocks(Coordinate(locA.x-1, locA.y-1, locA.z+1), Coordinate(locB.x+1, locA.y-1, locB.z+1), wallMat);
            return;
        }
    }
    else
    {
        if (locA.z == ceil(locA.z + w/2.0))
        {
            mc->setBlocks(Coordinate(locA.x-1, locA.y-1, locA.z-1), Coordinate(locB.x+1, locA.y-1, locA.z-1), wallMat);
            return;
        }
    }

    mc->setBlocks(Coordinate(locA.x-1, locA.y, locA.z-1), Coordinate(locB.x+1, locA.y, locA.z-1), stairMat.withMod(BlockMods::StairOrient::NORTH_ASCENDING));
    mc->setBlocks(Coordinate(locA.x-1, locA.y, locB.z+1), Coordinate(locB.x+1, locA.y, locB.z+1), stairMat.withMod(BlockMods::StairOrient::SOUTH_ASCENDING));

    mc->setBlock(Coordinate(locA.x-1, locA.y, locA.z), stairMat.withMod(BlockMods::StairOrient::SOUTH_DESCENDING));
    mc->setBlock(Coordinate(locA.x-1, locA.y, locB.z), stairMat.withMod(BlockMods::StairOrient::NORTH_DESCENDING));
    mc->setBlock(Coordinate(locB.x+1, locA.y, locA.z), stairMat.withMod(BlockMods::StairOrient::SOUTH_DESCENDING));
    mc->setBlock(Coordinate(locB.x+1, locA.y, locB.z), stairMat.withMod(BlockMods::StairOrient::NORTH_DESCENDING));

    if (increment >= 1)
    {
        mc->setBlocks(locA, Coordinate(locA.x, locA.y, locB.z), wallMat);
        mc->setBlocks(Coordinate(locB.x, locA.y, locA.z), Coordinate(locB.x, locA.y, locB.z), wallMat);
    }

    increment++;

    return buildRoofNS(Coordinate(locA.x, locA.y+1, locA.z+1), Coordinate(locB.x, locA.y+1, locB.z-1), increment);
}

void House::buildRoof()
{
    if (orientation == Orientation::NORTH || orientation == Orientation::SOUTH)
        buildRoofNS(Coordinate(loc1.x, loc1.y+finalH-1, loc1.z), Coordinate(loc2.x, loc1.y+finalH-1, loc2.z));
    else
        buildRoofEW(Coordinate(loc1.x, loc1.y+finalH-1, loc1.z), Coordinate(loc2.x, loc1.y+finalH-1, loc2.z));
}

void House::roomSplit(const Room& room, int choiceAxis)
{
    Coordinate locA = room.loc1;
    Coordinate locB = room.loc2;

    constexpr int MINROOMSIZE = 10;
    if (abs(locB.x-locA.x)+1 < MINROOMSIZE)
        return;

    int xMid = locA.x + room.length/2;
    int zMid = locA.z + room.width/2;

    if (orientation == Orientation::NORTH || orientation == Orientation::SOUTH) {
        if (xMid == this->doorCoordinate.x)
            xMid += 1;
    } else {
        if (zMid = this->doorCoordinate.z)
            zMid += 1;
    }

    
    if (choiceAxis == 0)
    {
        mc->setBlocks(Coordinate(xMid, locA.y, locA.z), Coordinate(xMid, locB.y, locB.z), wallMat);

        int zDoorCoord;
        if (locA.z+1 >= zMid-1)
            zDoorCoord = locA.z+1;
        else
            zDoorCoord = (rand() % ((zMid-1)-(locA.z+1))) + (locA.z+1);
        
        mc->setBlock(Coordinate(xMid, locA.y+1, zDoorCoord), doorType.withMod(BlockMods::DoorOrient::WEST_TOP));
        mc->setBlock(Coordinate(xMid, locA.y, zDoorCoord), doorType.withMod(BlockMods::DoorOrient::WEST_BOTTOM));
        
        this->rooms[this->numRooms] = Room();

        bool splitFirstRoom = (bool)(rand() % 2);
        if (splitFirstRoom == 0)
            return;
        else
            roomSplit(rooms[numRooms], 1);
        
        this->numRooms++;
        this->rooms.resize(numRooms+1);
        this->rooms[numRooms] = Room();

        bool splitSecondRoom = (bool)(rand() % 2);
        if (splitSecondRoom == 0)
            return;
        else
            roomSplit(rooms[numRooms], 1);
    }
    else
    {
        mc->setBlocks(Coordinate(locA.x, locA.y, zMid), Coordinate(locB.x, locB.y, zMid), wallMat);

        int xDoorCoord;
        if (locA.x+1 >= xMid-1)
            xDoorCoord = locA.x+1;
        else
            xDoorCoord = (rand() % ((xMid-1)-(locA.x+1))) + (locA.x+1);
        
        mc->setBlock(Coordinate(xDoorCoord, locA.y+1, zMid), doorType.withMod(BlockMods::DoorOrient::NORTH_TOP));
        mc->setBlock(Coordinate(xDoorCoord, locA.y, zMid), doorType.withMod(BlockMods::DoorOrient::NORTH_BOTTOM));
        
        this->rooms[this->numRooms] = Room();

        bool splitFirstRoom = (bool)(rand() % 2);
        if (splitFirstRoom == 0)
            return;
        else
            roomSplit(rooms[numRooms], 1);
        
        this->numRooms++;
        this->rooms.resize(numRooms+1);
        this->rooms[numRooms] = Room();

        bool splitSecondRoom = (bool)(rand() % 2);
        if (splitSecondRoom == 0)
            return;
        else
            roomSplit(rooms[numRooms], 1);  
    }

}

void House::buildFloors(const Coordinate& locA, const Coordinate& locB, bool anotherStorey)
{
    if (!anotherStorey)
        return;
    else if (this->numFloors == 2)
        return;


    mc->setBlocks(locA, locB, wallMat);

    mc->setBlocks(Coordinate(locA.x+2, locA.y+1, locA.z), Coordinate(locB.x-2, locB.y-1, locB.z), Blocks::GLASS_PANE);
    mc->setBlocks(Coordinate(locA.x, locA.y+1, locA.z+2), Coordinate(locB.x, locB.y-1, locB.z-2), Blocks::GLASS_PANE);

    mc->setBlocks(Coordinate(locA.x+2, locB.y-1, locA.z-1), Coordinate(locB.x-2, locB.y-1, locA.z-1), roofSlabMat.withMod(8));
    mc->setBlocks(Coordinate(locA.x+2, locB.y-1, locB.z+1), Coordinate(locB.x-2, locB.y-1, locB.z+1), roofSlabMat.withMod(8));
    mc->setBlocks(Coordinate(locA.x-1, locB.y-1, locA.z+2), Coordinate(locA.x-1, locB.y-1, locB.z-2), roofSlabMat.withMod(8));
    mc->setBlocks(Coordinate(locB.x+1, locB.y-1, locA.z+2), Coordinate(locB.x+1, locB.y-1, locB.z-2), roofSlabMat.withMod(8));

    mc->setBlocks(Coordinate(locA.x+1, locA.y, locA.z+1), Coordinate(locB.x-1, locB.y-1, locB.z-1), Blocks::AIR);
    
    mc->setBlocks(Coordinate(locA.x, locA.y-1, locA.z), Coordinate(locB.x, locA.y-1, locB.z), floorMat);

    std::vector<int> xCoords = {locA.x, locB.x};
    std::vector<int> zCoords = {locA.z, locB.z};
    for (int x : xCoords) {
        for (int z: zCoords) {
            mc->setBlocks(Coordinate(x, locA.y-1, z), Coordinate(x, locB.y, z), pillarMat);
        }
    }


    this->finalH += h;
    this->numRooms++;
    this->rooms.resize(numRooms+1);
    this->numFloors++;

    this->rooms[numRooms] = Room();

    int choiceSplit = rand() % 2;
    if (choiceSplit == 1)
    {
        int choiceAxis = rand() % 2;
        roomSplit(rooms[numRooms], choiceAxis);
    }

    buildStairs(Coordinate(locA.x, locA.y-5, locA.z));

    bool anotherStorey = (bool)(rand() % 2);
    if (anotherStorey)
        buildFloors(Coordinate(locA.x, locA.y+1, locA.z), Coordinate(locB.x, locB.y+this->h, locB.z), anotherStorey);
}

void House::buildStairs(const Coordinate& loc)
{
    mc->setBlocks(Coordinate(loc.x+1,loc.y,loc.z+1), Coordinate(loc.x+4, loc.y+4, loc.z+1), Blocks::AIR);

    mc->setBlock(Coordinate(loc.x+3, loc.y, loc.z+2), stairMat.withMod(BlockMods::StairOrient::WEST_ASCENDING));
    mc->setBlock(Coordinate(loc.x+2, loc.y+1, loc.z+2), stairMat.withMod(BlockMods::StairOrient::WEST_ASCENDING));
    mc->setBlock(Coordinate(loc.x+1, loc.y+1, loc.z+2), stairMat.withMod(BlockMods::StairOrient::EAST_DESCENDING));
    mc->setBlock(Coordinate(loc.x+1, loc.y+1, loc.z+1), stairMat.withMod(BlockMods::StairOrient::EAST_DESCENDING));
    mc->setBlock(Coordinate(loc.x+2, loc.y+2, loc.z+1), stairMat.withMod(BlockMods::StairOrient::EAST_ASCENDING));
    mc->setBlock(Coordinate(loc.x+3, loc.y+3, loc.z+1), stairMat.withMod(BlockMods::StairOrient::EAST_ASCENDING));
    mc->setBlock(Coordinate(loc.x+4, loc.y+4, loc.z+1), stairMat.withMod(BlockMods::StairOrient::EAST_ASCENDING));

    mc->setBlocks(Coordinate(loc.x+5, loc.y+4, loc.z+1), Coordinate(loc.x+5, loc.y+4, loc.z+3), floorMat);
    mc->setBlocks(Coordinate(loc.x+4, loc.y+4, loc.z+2), Coordinate(loc.x+1, loc.y+4, loc.z+3), floorMat);
    mc->setBlock(Coordinate(loc.x+4, loc.y+4, loc.z+2), floorMat);
}

const Plot& House::getPlot()
{
    return plot;
}