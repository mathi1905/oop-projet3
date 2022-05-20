#ifndef __WORLD__

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#include "element.hpp"

#define __WORLD__

typedef struct Sprite* Sprite_t;

class World{
    public:
        World();
        void draw_world(int score);
        void draw_game_over();
        void destroy_world();
        void add_ellement(Element* new_element, int repeat_frames = 1);
        void remove_ellement(Element* element);
        void replace_texture(Element* element, int repeat_frames = 1);
    private:
        void draw_rectangle(int x, int y, int w, int h, int r = 0, int g = 0, int b = 0);
        void draw_elements();
        SDL_Renderer* renderer;
        SDL_Window* window;
        std::vector<Sprite_t> scene;
};

#endif
