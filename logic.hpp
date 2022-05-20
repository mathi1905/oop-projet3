#ifndef __COMMAND__

#include <SDL2/SDL.h>
#include <vector>

#include "world.hpp"
#include "pig.hpp"
#include "element.hpp"
#include "wolf.hpp"

#define __COMMAND__

typedef struct Wolf_baloon* Wolf_baloon_t;

class Logic{
    public:
        Logic(World* world);
        bool compute_command();
        bool click_cross();
        void make_turn();
        void reload();
        void spawn_wolf();
        void spawn_meat();
        void destroy_logic();
        void will_spawn();
        void will_spawn_meat();
        bool test_collisions();
        int get_score() const;

    private:
        Element* meat;
        int score;
        bool shoot();
        bool spawn_ready;
        bool spawn_meat_ready;
        SDL_Event event;
        World* world;
        const int falling_positions[5] = {75, 125, 150, 200, 250};
        Pig* pig;
        SDL_TimerID wolf_event;
        SDL_TimerID meat_event;
        Uint32 next_reload;
        std::vector<Projectile*> arrows;
        std::vector<Wolf_baloon_t> walking_wolfs;
        std::vector<Wolf_baloon_t> flying_wolfs;
        std::vector<Projectile*> rocks_list;
};

#endif
