#include <mcpp/mcpp.h>

#include "plot.h"



class Decoration
{
private:
    mcpp::MinecraftConnection* mc;
    Plot plot;
    int x0;
    int y0;
    int z0;

public:
    Decoration(mcpp::MinecraftConnection* conn, Plot plot);

    void build();
    void buildPond();
    void buildGarden();
    void buildWindmill();
};