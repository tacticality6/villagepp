#include <mcpp/mcpp.h>

#include "../include/plot.h"

using namespace mcpp;

Plot::Plot(Coordinate l1, Coordinate l2)
    : Plot(l1, l2, ((l1.y-l2.y)/2))
{}
Plot::Plot(Coordinate lA, Coordinate lB, int h)
    : loc1{Coordinate{lA.x, h+1, lA.z}}
    , loc2{Coordinate{lB.x, h+1, lB.z}}
    , height{h+1}
    , length{abs(loc1.x - loc2.x) + 1}
    , width{abs(loc1.z - loc2.z) + 1}
{}