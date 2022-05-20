#ifndef __PIG__

#include <string>
#include <cstdlib>

#include "element.hpp"
#include "projectile.hpp"

#define __PIG__

class Pig : public Element{
    public:
        Pig(int x, int y);
        void set_speed(int y);
        void play_turn();
        bool reload();
        void load(Element* munition);
        Projectile* shoot(float speed);

};

#endif
