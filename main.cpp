#include <SDL2/SDL_timer.h>


#include "world.hpp"
#include "logic.hpp"
#include "element.hpp"
#include "pig.hpp"

int main(){
    //We init everything we will need in SDL
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
        return 1;
    //We create the world and the logic of the world
    World world;
    Logic logic(&world);
    bool lost = false;
    int score = 0;


    //While the player havn't pushed the cross and havn't loss
    while(logic.compute_command() && !lost && score < 5 ){
        score = logic.get_score();
        //We play one "turn" (we make everything perfom their "actions" for the turn)
        logic.make_turn();
        //We then check all the colision (it will return false if the pis is touched by a rock)
        lost = !logic.test_collisions();
        //We draw the world
        world.draw_world(score);

        //We wait 1 sec/30 to make sure we are in 30 fps
        SDL_Delay(1000 / 30);
    }

    if(lost || score >= 5){
        while(logic.click_cross()){
            world.draw_game_over();
            SDL_Delay(1000 / 30);
        }
    }
    //If we are here, the game is over. So we free the world as well as the logic
    world.destroy_world();
    logic.destroy_logic();
    //We close SDL
    SDL_Quit();

    return 0;
}
