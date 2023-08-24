#include <mcpp/mcpp.h>

#include <cassert>
#include <random>

#include "../include/village.h"
#include "../include/house.h"

using namespace mcpp;

Village::Village(MinecraftConnection* conn)
: terraformer{conn}
{
    assert(conn != nullptr);
    this->mc = conn;

    this->terraformer = Terraformer(this->mc);

    this->numPlots = (rand() % 6) + 5; // 5-10

    this->centrepoint = mc->getPlayerPosition();

    Coordinate halfMax {maxSize/2, maxSize/2, maxSize/2};
    this->villageDomain = {centrepoint-halfMax, centrepoint+halfMax - Coordinate{1,1,1}};

    this->arrayRepresentation = {
        maxSize,
        std::vector<ArrayFeatures> {
            maxSize,
            ArrayFeatures::NONE
        }
    };
    this->updateArrayRepresentationNaturalFeatures();
}

void Village::updateArrayRepresentationNaturalFeatures()
{
    std::vector<std::vector<BlockType>> surfaceBlocks = this->terraformer.getSurfaceBlocks(
        villageDomain.first, villageDomain.second);
    
    for (int x=0; x < surfaceBlocks.size(); x++) {
        for (int z=0; z < surfaceBlocks[x].size(); z++)
        {
            BlockType block = surfaceBlocks[x][z];

            if (block.id == Blocks::STILL_WATER.id
            || block.id == Blocks::FLOWING_WATER.id
            || block.id == Blocks::STILL_LAVA.id
            || block.id == Blocks::FLOWING_LAVA.id) //water
                arrayRepresentation[x][z] = ArrayFeatures::WATER;
            else if (arrayRepresentation[x][z] == ArrayFeatures::NONE &&
            (block.id == Blocks::OAK_LEAVES.id
             || block.id == Blocks::OAK_WOOD.id)) //tree
                arrayRepresentation[x][z] = ArrayFeatures::TREE;
        }
    }
}

std::vector<Plot> Village::setPlots()
{
    constexpr int isolationRadius = 10;
    constexpr int tolerance = 2;

    std::vector<Plot> plotsInternal;

    const std::vector heightArr = this->mc->getHeights(villageDomain.first, villageDomain.second);

    std::vector<std::pair<Coordinate, Coordinate>> interimPlots;

    int flatness = 0;
    
    while (interimPlots.size() < numPlots && flatness < 50) {
        for (int x=tolerance; x < arrayRepresentation.size()-tolerance-plotSize; x++) {
            for (int z=tolerance; z < arrayRepresentation[x].size()-tolerance-plotSize; z++)
            {
                bool valid = true;
                for(int l=x-tolerance; l < x+plotSize+tolerance; l++) {
                    for (int w=z-tolerance; w < z+plotSize+tolerance; w++)
                    {
                        if (!(
                        (
                            (arrayRepresentation[l][w] == ArrayFeatures::NONE)
                            ||
                            (arrayRepresentation[l][w] == ArrayFeatures::TREE)
                        )
                        &&
                        (abs(heightArr[x][z] - heightArr[l][w]) <= flatness)
                        ))
                        {
                            valid = false;
                            break;
                        }
                    }
                    if (!valid)
                        break;
                }
                if (!valid)
                    continue;
                
                //if you get here, plot is valid
                interimPlots.push_back (
                    std::pair<Coordinate, Coordinate> {
                        Coordinate(x,0,z),
                        Coordinate(x+plotSize,0,z+plotSize)
                    }
                );

                //get isolation bounds
                const int xLBound = std::max(0, x-isolationRadius);
                const int zLBound = std::max(0, z-isolationRadius);
                const int xHBound = std::min(x+plotSize+isolationRadius, (int)arrayRepresentation.size()-1);
                const int zHBound = std::min(z+plotSize+isolationRadius, (int)arrayRepresentation[0].size()-1);


                //mark array representation
                //mark isolation radius (temp)
                for (int l=xLBound; l < xHBound; l++) {
                    for (int w=zLBound; w < zHBound; w++) {
                        arrayRepresentation[l][w] = ArrayFeatures::TEMP_ISOLATE;
                    }
                }

                //mark plot
                for (int l=x; l < x+plotSize-1; l++) {
                    for (int w=z; w < z+plotSize-1; w++) {
                        arrayRepresentation[l][w] = ArrayFeatures::PLOT;
                    }
                }

                //exit early if sufficient plots
                if (interimPlots.size() == numPlots)
                    break;
            }
            //also exit here
            if (interimPlots.size() == numPlots)
                break;
        }
        //increase flatness tolerance
        if (interimPlots.size() == numPlots)
            break;
        flatness++;
    }
    //once all plots are place, remove isolation markers
    for (int x=0; x < arrayRepresentation.size(); x++) {
        for (int z=0; z < arrayRepresentation[x].size(); z++) {
            if (arrayRepresentation[x][z] == ArrayFeatures::TEMP_ISOLATE) {
                arrayRepresentation[x][z] = ArrayFeatures::NONE;
            }
        }
    }

    //plots.reserve(numPlots);
    for (auto p : interimPlots)
    {
        Coordinate a = villageToWorld(p.first);
        Coordinate b = villageToWorld(p.second);
        std::cout << a.x << " " << a.z << "--" << b.x << " " << b.z << std::endl;
        int height = this->terraformer.placePlotAndSmoothSurroundings(a, b);
        Plot plot {a, b, height};

        plotsInternal.push_back(plot);
    }
    //update the natural features of the array we just erased with the plots
    this->updateArrayRepresentationNaturalFeatures();
    return std::move(plotsInternal);
}

void Village::build()
{
    this->plots = this->setPlots();

    std::vector<Coordinate> doorCoordinates;

    for (int index=0; index < plots.size(); index++)
    {
        if (index<5 || (bool)(rand()%2))
        {
            House house = House(mc, plots[index]);
            //get current far corner
            Coordinate plotFarCorner = plots[index].loc2;
            house.build();
            Coordinate doorCoordinate = house.getDoorCoordinate();

            //update array representation with shrunken plot, and door coordinate
            Coordinate plotCorner = worldToVillage(plots[index].loc1);
            for (int x=plotCorner.x; x < plotFarCorner.x; x++) {
                for (int z=plotCorner.z; z < plotFarCorner.z; z++)
                {
                    if (x > plotCorner.x + house.getPlot().length || z > plotCorner.z + house.getPlot().width)
                        arrayRepresentation[x][z] == ArrayFeatures::NONE;
                    if (doorCoordinate == villageToWorld(Coordinate(x,plots[index].height,z)))
                        arrayRepresentation[x][z] == ArrayFeatures::DOOR;
                }
            }
        }
        else 
        {
            //build decoration TODO:
        }
    }

    //do roads

}

Coordinate Village::worldToVillage(const Coordinate& loc)
{
    return Coordinate(loc.x - villageDomain.first.x, 
                      loc.y, 
                      loc.z - villageDomain.first.z);
}

Coordinate Village::villageToWorld(const Coordinate& loc)
{
    return Coordinate(loc.x + villageDomain.first.x, 
                      loc.y, 
                      loc.z + villageDomain.first.z);
}