#ifndef __PROJECTILE__

#include <math.h>
#include <string>

#include "element.hpp"

#define __PROJECTILE__

class Projectile : public Element{
    public:
        Projectile(std::string name, const char* path, int x, int y, int w, int h, int masse, int cross_sectional_area, float air_density = 1.0, float drag_coefficent = 0.001, float grav = 300);
        Projectile(std::string name, const char* path, int w, int h, int masse, int cross_sectional_area);
        void lauch(float speed, int angle);
        void play_turn();
    private:
        float get_xdrag();
        float get_ydrag();
        float grav;
        float air_density;
        int cross_sectional_area;
        float drag_coefficient;
        int masse;
};

#endif