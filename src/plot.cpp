#include <mcpp/mcpp.h>

#include "../include/plot.h"

using namespace mcpp;

Plot::Plot(Coordinate l1, Coordinate l2)
    : Plot(l1, l2, ((l1.y-l2.y)/2))
{}
Plot::Plot(Coordinate l1, Coordinate l2, int h)
    : l1{l1}
    , l2{l2}
    , h{h}
    , loc1{this->l1}
    , loc2{this->l2}
    , height{this->h}
    , length{l}
    , width{w}
{
    this->l = abs(loc1.x - loc2.x) + 1;
    this->w = abs(loc1.z - loc2.z) + 1; 
}