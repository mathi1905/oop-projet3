#ifndef __WOLF__

#include <SDL2/SDL.h>

#include "element.hpp"
#include "projectile.hpp"

#define __WOLF__

class Wolf : public Element{
    public:
        Wolf(int x, int y, int runing_speed, int falling_speed, int falling_position, int ground_position);
        Projectile* play_turn();
        bool get_is_falling();
        bool get_is_shooting();
        void reload();
        void can_shoot();
        void start_shooting();
        void stop_shooting();
        void die();
        bool dead();
    private:
        void start_falling();
        void stop_falling();
        SDL_TimerID shoot_timer;
        bool will_shoot;
        int falling_position;
        int ground_position;
        int falling_speed;
        int runing_speed;
        bool is_falling;
        bool is_shooting;
        bool is_dead;
};

#endif
