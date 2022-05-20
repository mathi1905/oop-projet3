#include "world.hpp"


using std::vector;

//A sprite is a struc containing all the world will need to draw an element on the screen
struct Sprite {
    Element* element;
    vector<SDL_Texture*> texture;
    vector<SDL_Texture*>::iterator current_frame;
    SDL_Rect hitbox;
};

World::World(){
    //We create the windows
    this->window = SDL_CreateWindow("POOYAN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 550, 580, 0);
    //We initiate the renderer
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    //We hide the cursor (we will not need it in our game)
    SDL_ShowCursor(SDL_DISABLE);
}

void World::draw_world(int score){
    TTF_Init();
    //Each time we want to render the scene, we need to clean it
    SDL_RenderClear(this->renderer);
    //The we draw the different rectangle making the decor
    this->draw_rectangle(0, 100, 350, 70, 0, 255, 0);
    this->draw_rectangle(0, 170, 70, 310, 128, 96, 60);
    this->draw_rectangle(0, 480, 550, 100, 128, 96, 20);
    //We the draw each Element present inside the scene
    this->draw_elements();
    //And finaly we draw the house (it will be at the foreground, like so we can see the wolf walking inside)
    this->draw_rectangle(450, 405, 100, 75, 220, 220, 220);
    //We draw the sky
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255);

    //this opens a font style and sets a size
    TTF_Font* font = TTF_OpenFont("standard.ttf", 30);

    SDL_Color black = {0, 0, 0, 255};

    /* as TTF_RenderText_Blended could only be used on
    SDL_Surface then you have to create the surface first */
    SDL_Surface* surfaceMessage = NULL;

    switch (score){
        case 0:
            surfaceMessage = TTF_RenderText_Blended(font, "0/5", black);
            break;
        case 1:
            surfaceMessage = TTF_RenderText_Blended(font, "1/5", black);
            break;
        case 2:
            surfaceMessage = TTF_RenderText_Blended(font, "2/5", black);
            break;
        case 3:
            surfaceMessage = TTF_RenderText_Blended(font, "3/5", black);
            break;
        case 4:
            surfaceMessage = TTF_RenderText_Blended(font, "4/5", black);
            break;
        case 5:
            surfaceMessage = TTF_RenderText_Blended(font, "5/5", black);
            break;
    }

    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 450;  //controls the rect's x coordinate
    Message_rect.y = 405; // controls the rect's y coordinte
    Message_rect.w = 100; // controls the width of the rect
    Message_rect.h = 75; // controls the height of the rect

    // NULL correspond to the crop size (we ignore this)
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    //We show the rendered window to the user
    SDL_RenderPresent(this->renderer);

    //We free surface and texture
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);

    TTF_CloseFont(font);
    TTF_Quit();
}

void World::draw_game_over(){
    TTF_Init();
    //Each time we want to render the scene, we need to clean it
    SDL_RenderClear(this->renderer);

    //We draw the black screen
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);

    //this opens a font style and sets a size
    TTF_Font* font = TTF_OpenFont("standard.ttf", 30);

    SDL_Color white = {255, 255, 255, 255};

    SDL_Color black = {0, 0, 0, 255};

    // as TTF_RenderText_Shaded could only be used on
    // SDL_Surface then you have to create the surface first
    SDL_Surface* surfaceMessage = TTF_RenderText_Shaded(font, "GAME OVER", white, black);

    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 100;  //controls the rect's x coordinate
    Message_rect.y = 120; // controls the rect's y coordinte
    Message_rect.w = 350; // controls the width of the rect
    Message_rect.h = 350; // controls the height of the rect

    // NULL correspond to the crop size (we ignore this)
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

    //We show the rendered window to the user
    SDL_RenderPresent(this->renderer);

    //We free surface and texture
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);

    TTF_CloseFont(font);
    TTF_Quit();
}

void World::destroy_world(){
    //We remove every ellement from the scene using the same technic than inside destroy logic (in destroy logic.cpp)
    while(this->scene.size() != 0){
        for(auto textures: this->scene.back()->texture)
            //This function is called to remove a texture from the memory
            SDL_DestroyTexture(textures);
        delete this->scene.back();
        this->scene.pop_back();
    }
    //We destory the renderer
    SDL_DestroyRenderer(this->renderer);
    //We close the window
    SDL_DestroyWindow(this->window);
}

//The repeat_frame parameter is used to slow down a bit som of the animations (wich are playing to fast)
void World::add_ellement(Element* new_element, int repeat_frames){
    //Each time we want to add an element to the scene, we neet to create a new sprite struct
    Sprite* new_sprite = new Sprite;
    new_sprite->element = new_element;
    //We will generate a texture for each path contained in the vector given by get_frame (sometimes more than one time if repeat_frames is not one)
    for(auto textures: new_element->get_frame()){
        for(int i = 0; i < repeat_frames; i++)
            new_sprite->texture.push_back(IMG_LoadTexture(this->renderer, textures));
    }
    //We set the iterator to the begining of the vector of textures
    new_sprite->current_frame = new_sprite->texture.begin();
    //We recover the parameters of the texture (to be able to modify them)
    SDL_QueryTexture(*new_sprite->texture.begin(), NULL, NULL, &new_sprite->hitbox.w, &new_sprite->hitbox.h);
    //We set the sprite height and weight
    new_sprite->hitbox.w = new_element->getw();
    new_sprite->hitbox.h = new_element->geth();
    //We add the newly created sprite to the end of the scene
    this->scene.push_back(new_sprite);
    //If this element has an equiped element, we add this ellement to the scene too
    if(new_element->has_equiped_ellement())
        this->add_ellement(new_element->get_equiped_ellement(), repeat_frames);
}

void World::remove_ellement(Element* element){
    int i = 0;
    //We will iterate over all of the elements of the scene until we found the one we are searching for
    for (auto current_element : this->scene){
        //Il we found it
        if(current_element->element == element){
            for(auto textures: current_element->texture)
                //We destroy all the textures loaded inside the sprite struct
                SDL_DestroyTexture(textures);
            //We delete the sprite struct
            delete current_element;
            //We erase the i th sprite of the vector (we know because of the i variable we are at the position i of the vector)
            this->scene.erase(this->scene.begin() + i);
            return;
        }
        i++;
    }
}

void World::replace_texture(Element* element, int repeat_frames){
    //We search for the sprite containing the element we want the texture to be replaced
    for (auto current_element : this->scene){
        if(current_element->element == element){
            //We destroy all the textures contained in the texture vector
            for(auto textures: current_element->texture)
                SDL_DestroyTexture(textures);
            //we clear the vector
            current_element->texture.clear();
            //We add will (as before) load all the path contained in the element. If they have changed, the textures will change as well
            for(auto textures: element->get_frame()){
                for(int i = 0; i < repeat_frames; i++)
                    current_element->texture.push_back(IMG_LoadTexture(this->renderer, textures));
            }
            //We replace the iterator at the begining of the vector
            current_element->current_frame = current_element->texture.begin();
            //We will update the informations of the texture (we only use the first element as the weight and the height will allways be the same for all the frames of the animation)
            SDL_QueryTexture(*current_element->texture.begin(), NULL, NULL, &current_element->hitbox.w, &current_element->hitbox.h);
            current_element->hitbox.w = element->getw();
            current_element->hitbox.h = element->geth();
            return;
        }
    }
}

//------------------------------------------------------------------------------------------------//

void World::draw_rectangle(int x, int y, int w, int h, int r, int g, int b){
    //We use SDL to draw one rectance
    SDL_Rect rect;
    //We set the x position of the rectangle
    rect.x = x;
    //We set the y position of the rectangle
    rect.y = y;
    //We set the weight of the rectangle
    rect.w = w;
    //We seet the height of the rectangle
    rect.h = h;
    //We set the color of the rectangle
    SDL_SetRenderDrawColor(this->renderer, r, g, b, 255);
    //We add draw the rectangle using the renderer
    SDL_RenderDrawRect(this->renderer, &rect);
    //We fill the rectangle
    SDL_RenderFillRect(this->renderer, &rect);
}

void World::draw_elements(){
    //We will draw all the scene, so we iterate over each sprite of the scene
    for (auto current_element : this->scene){
        //We set the position of the element in the scene
        current_element->hitbox.x = current_element->element->getx();
        current_element->hitbox.y = current_element->element->gety();
        //We use this function to apply some modification over the sprite (in our case, we want to modiffy the angle)
        SDL_RenderCopyEx(this->renderer, *current_element->current_frame, NULL, &current_element->hitbox, current_element->element->get_angle(), NULL, SDL_FLIP_NONE);
        current_element->current_frame++;
        //If the current frame reached an end (we played all the frames), we will put it to the begining again so we will play the animation one more time
        if(current_element->current_frame == current_element->texture.end())
            current_element->current_frame = current_element->texture.begin();
    }
}
