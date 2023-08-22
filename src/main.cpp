#include "../include/village.h"
#include "../include/terraformer.h"
#include <mcpp/mcpp.h>
#include <mcpp/util.h>
#include <mcpp/block.h>

#include <cstdlib>

using namespace mcpp;

int main()
{
    srand( time(0) );
    MinecraftConnection mc = MinecraftConnection{};
    
    Village village{&mc};
    village.build();

    return 0;
}