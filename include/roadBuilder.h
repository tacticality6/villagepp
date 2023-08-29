#include <mcpp/mcpp.h>
#include <vector>

#include "village.h"
#include "terraformer.h"


class RoadBuilder
{
private:
    mcpp::MinecraftConnection* mc;
    std::vector<std::vector<ArrayFeatures>> arrayRepresentation;
    std::vector<std::vector<int>> heightArr;
    std::pair<mcpp::Coordinate, mcpp::Coordinate> villageDomain;
    std::vector<mcpp::Coordinate> doorCoordinates;

public:
    RoadBuilder(mcpp::MinecraftConnection* conn, 
        const std::vector<std::vector<ArrayFeatures>>& arrayRepresentation, 
        const std::pair<mcpp::Coordinate, mcpp::Coordinate>& villageDomain, 
        const std::vector<mcpp::Coordinate>& doorCoordinates);
    
    mcpp::Coordinate worldToVillage(const mcpp::Coordinate& loc);
    mcpp::Coordinate villageToWorld(const mcpp::Coordinate& loc);

    void placeLampPost(const mcpp::Coordinate& locVillage);
    
};