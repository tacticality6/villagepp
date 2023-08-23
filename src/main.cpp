#include "../include/village.h"
#include "../include/terraformer.h"
#include "../include/house.h"
#include <mcpp/mcpp.h>
#include <mcpp/util.h>
#include <mcpp/block.h>

#include <cstdlib>

using namespace mcpp;

int main()
{
    try
    {
        
        srand( time(0) );
        MinecraftConnection mc = MinecraftConnection{};
        
        /*
        Village village{&mc};
        village.build();
        */
        Coordinate pc = mc.getPlayerPosition() + Coordinate{2,2,2};
        Coordinate pc2 = pc + Coordinate{14, 14, 14};
        Terraformer t = Terraformer{&mc};
        int h = t.flattenPlot(pc, pc2);
        Plot hp {pc, pc2, h};
        House house {&mc, hp};
        house.build();

        return 0;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    
}