#include "logic.hpp"

using std::vector;

//This function will be used to associate each wolf with it's respective baloon (it's very usefull when dealing with the collision)
struct Wolf_baloon{
    Wolf* wolf;
    Element* baloon;
};

//This is the callback used to spawn a wolf. It's called every 3 sec
static Uint32 spawn_callback(Uint32 interval, void* param){
    Logic* arg = (Logic*) param;
    arg->will_spawn();
    return interval;
}

//This is the callback used to spawn a meat. It's called every 10 sec
static Uint32 spawn_meat_callback(Uint32 interval, void* param){
    Logic* arg = (Logic*) param;
    arg->will_spawn_meat();
    interval = 0;
    return interval;
}

Logic::Logic(World* world){
    this->score = 0;
    this->world = world;
    //We initiate the pig at the right position
    this->pig = new Pig(400, 240);
    this->meat = NULL;
    //We add the element to the world (the class responsible of generating and dealing with tetures)
    this->world->add_ellement(this->pig);
    //We initiate the timer for spawning the wolf
    this->wolf_event = SDL_AddTimer(3000, spawn_callback, this);
    //We initiate the timer for spawning the meat
    this->meat_event = SDL_AddTimer(10000, spawn_meat_callback, this);
    //We initiate the timer to reload (the time value is set to 0 because we want to have an arrow in the bow when we spawn)
    this->spawn_ready = false;
    this->spawn_meat_ready = false;
    this->next_reload = 0;

    //We initiate the random values (used to decide where the wolfs will jump)
    srand(time(NULL));
}

int Logic::get_score() const{
    return score;
}

bool Logic::test_collisions(){
    //This variable will be used to "revalidate" the iterators when we erase something from a vector
    int i = 0;
    //For each shooted arrow in the scene
    for(vector<Projectile*>::iterator current_arrow=this->arrows.begin(); current_arrow != this->arrows.end(); ){
        //For each wolf in the scene
        for(vector<Wolf_baloon*>::iterator current_wolf=this->flying_wolfs.begin(); current_wolf != this->flying_wolfs.end(); current_wolf++){
            //If the wolf has a baloon
            if((*current_wolf)->baloon != NULL){
                //We check if the baloon equiped by the wolf colide with the arrow
                if((*current_arrow)->colide_with((*current_wolf)->baloon) || ((*current_arrow)->colide_with((*current_wolf)->wolf) && (*current_arrow)->get_name() == "meat")){
                    //If it's the case, we remove the baloon from the scene
                    this->world->remove_ellement((*current_wolf)->baloon);
                    //We thene delete the "touched" baloon
                    delete (*current_wolf)->baloon;
                    //We make sure the baloon field is set to NULL (as there is no baloon anymore)
                    (*current_wolf)->baloon = NULL;
                    //We ask the wolf to unequip his baloon
                    (*current_wolf)->wolf->unequip_ellement();
                    //The poor wolf die :'(
                    (*current_wolf)->wolf->die();
                    //The "old" texture of the world changed while he was dying. So we musst tell this to the world
                    this->world->replace_texture((*current_wolf)->wolf);
                }
            }
            //If an arrow touch a wolf
            if((*current_arrow)->colide_with((*current_wolf)->wolf) && (*current_arrow)->get_name() != "meat"){
                    //We musst remove the arrow from the scene
                    this->world->remove_ellement(*current_arrow);
                    //delete it
                    delete *current_arrow;
                    //And erase it from the arrows vector
                    this->arrows.erase(current_arrow);
                    //At the end of the loop, we will increment i. We don't want it this time (we removed an ellement, so the position i will be the same in the lext loop and in this one)
                    i--;
                    //We "revalidate" the pointer. We will use the decremented value of i (for the same reason than before)
                    current_arrow = this->arrows.begin() + i;
                    break;
            }
        }
        i++;
        current_arrow++;
    }
    //We reset i
    i = 0;
    //For each rock thronw by the wolf
    for(vector<Projectile*>::iterator current_rock=this->rocks_list.begin(); current_rock != this->rocks_list.end(); ){
        //If the rock colide with mamapig, then the game is loose
        if((*current_rock)->colide_with(this->pig))
            //we return false to tell this information to the calling function
            return false;
        //If we didn't colide with the pig, we still can colide with the arrays
        for(vector<Projectile*>::iterator current_arrow=this->arrows.begin(); current_arrow != this->arrows.end(); current_arrow++){
            //If a rock will colid with an arrow
            if((*current_arrow)->colide_with(*current_rock)){
                //We will need to remove the rock as well as the arrow from the scene and from the vector
                this->world->remove_ellement(*current_arrow);
                this->world->remove_ellement(*current_rock);
                delete *current_arrow;
                delete *current_rock;
                this->arrows.erase(current_arrow);
                this->rocks_list.erase(current_rock);
                i--;
                current_rock = this->rocks_list.begin() + i;
                //All the things we will do there are basically the same as what we did when dealing with the wolf arrow colision (except we will remove two objecto this time)
                break;
            }
        }
        i++;
        current_rock++;
    }
    //If the pig colide with the meat
    if(this->meat != NULL){
        if((this->meat)->colide_with(this->pig)){
            if(this->pig->has_equiped_ellement()){
                this->world->remove_ellement(this->pig->get_equiped_ellement());
                delete this->pig->get_equiped_ellement();
            }
            this->pig->load(this->meat);
            this->meat = NULL;
            this->meat_event = SDL_AddTimer(10000, spawn_meat_callback, this);
        }
    }
    return true;
}

void Logic::make_turn(){
    this->pig->play_turn();
    //Used to revalidate the iterators
    int i = 0;
    //We allways spaw the wolfs here to avoid some very weird segfault (as callbacks are insynchronous, beeing able to call wolf everywhere can lead to strange behaviours)
    if(this->spawn_ready){
        this->spawn_ready = false;
        this->spawn_wolf();
    }
    //We allways spaw the meat here to avoid some very weird segfault
    if(this->spawn_meat_ready){
        this->spawn_meat_ready = false;
        this->spawn_meat();
    }
    if(SDL_TICKS_PASSED(SDL_GetTicks(), this->next_reload))
        this->reload();
    //We first deal with the arrows
    for (vector<Projectile*>::iterator current_arrow=this->arrows.begin(); current_arrow != this->arrows.end(); ){
        //each arrow will play it's turn (they will move and compute all the informations relative to projectiles movement)
        (*current_arrow)->play_turn();
        //If the arrow colide with the trunk or the ground (it can't colide with anything else)
        if((*current_arrow)->get_x_center() <= 70 || (*current_arrow)->get_y_center() >= 480 || (*current_arrow)->get_y_center() <= 170){
            //We remove the arrow from the world
            world->remove_ellement(*current_arrow);
            //We delete the arrow (as we don't need it anymore)
            delete *current_arrow;
            //We remove the arrow from the vector
            this->arrows.erase(current_arrow);
            //We revalidate the iterator
            current_arrow = this->arrows.begin() + i;
        }
        else{
            i++;
            current_arrow++;
        }
    }
    i = 0;
    //We then move the walking wolfs (the one who arn't falling from the ledge)
    for (vector<Wolf_baloon*>::iterator current_wolf=this->walking_wolfs.begin(); current_wolf != this->walking_wolfs.end(); ){
        (*current_wolf)->wolf->play_turn();
        //If the wolf start to fall
        if((*current_wolf)->wolf->get_is_falling()){
            //We will add it to the list of flying wolfs
            this->flying_wolfs.push_back(*current_wolf);
            //The texture will change, we need to tell it to the world
            this->world->replace_texture((*current_wolf)->wolf);
            //We remove the wolf from the walking wolfs list
            this->walking_wolfs.erase(current_wolf);
            //We revalidate the iterator
            current_wolf = this->walking_wolfs.begin() + i;
        }
        //If the wolf is out of the bounds
        else if ((*current_wolf) -> wolf -> getx() >= 450){
            //We remove it from the scene
            world->remove_ellement((*current_wolf)->wolf);
            delete (*current_wolf)->wolf;
            //We don't forget to free the struc used to store the wolf and his baloon
            delete (*current_wolf);
            this->walking_wolfs.erase(current_wolf);
            current_wolf = this->walking_wolfs.begin() + i;
            this->score++;

        }
        else{
            i++;
            current_wolf++;
        }
    }
    i = 0;
    //We deal with the flying wolfs
    for (vector<Wolf_baloon*>::iterator current_wolf=this->flying_wolfs.begin(); current_wolf != this->flying_wolfs.end(); ){
        //If the wolf need to shoot, he play_turn will return a projectile. Otherwise, it will return NULL
        Projectile* projectile = (*current_wolf)->wolf->play_turn();
        //If the wolf is behind 230 px (and is not dead), he will start to launch some rock
        if((*current_wolf)->wolf->gety()+(*current_wolf)->wolf->geth() >= 230 && !(*current_wolf)->wolf->get_is_shooting() && !(*current_wolf)->wolf->dead())
            (*current_wolf)->wolf->start_shooting();
        //If we have a projectile
        if(projectile != NULL){
            //We need to add it to the scene
            this->world->add_ellement(projectile);
            this->rocks_list.push_back(projectile);
        }
        //If the wolf touched the ground
        if(!(*current_wolf)->wolf->get_is_falling()){
            //If he is alive
            if(!(*current_wolf)->wolf->dead()){
                //We will remove his baloon to the struct (and the delete it)
                this->world->remove_ellement((*current_wolf)->baloon);
                delete (*current_wolf)->baloon;
                //We reput this wolf inside the walking wolfs vector
                this->walking_wolfs.push_back(*current_wolf);
                //We change it's texture
                this->world->replace_texture((*current_wolf)->wolf, 2);
                this->flying_wolfs.erase(current_wolf);
                current_wolf = this->flying_wolfs.begin() + i;
            }
            else{
                //In the other case, the poor wolf is dead. We remove it from the world
                this->world->remove_ellement((*current_wolf)->wolf);
                //We delete this wolf
                delete (*current_wolf)->wolf;
                //We delete the struct (the wolf is dead so it couldn't store any baloon)
                delete (*current_wolf);
                this->flying_wolfs.erase(current_wolf);
                current_wolf = this->flying_wolfs.begin() + i;
            }
        }
        else{
            i++;
            current_wolf++;
        }
    }
    i = 0;
    //We deal with the rocks thrown by the wolves
    for (vector<Projectile*>::iterator current_rock=this->rocks_list.begin(); current_rock != this->rocks_list.end(); ){
        (*current_rock)->play_turn();
        //If the rock is out of the bounds
        if((*current_rock)->get_x_center() >= 550 || (*current_rock)->get_y_center() >= 480){
            //We remove it from the scene
            this->world->remove_ellement(*current_rock);
            delete (*current_rock);
            this->rocks_list.erase(current_rock);
            current_rock = this->rocks_list.begin() + i;
        }
        else{
            i++;
            current_rock++;
        }
    }
}

//This function is called when the reload_callback is called
void Logic::reload(){
    //We ask the pig to reload
    if(this->pig->reload())
        //We add the newly equipped arrow to the scene
        this->world->add_ellement(this->pig->get_equiped_ellement());
}

//This function is called when we want to spawn a wolf (not directly by the spawn callback to avoid sync problems)
void Logic::spawn_wolf(){
    //We create a struct for the new wolf
    struct Wolf_baloon* new_wolf = new Wolf_baloon;
    //We initiate the wolf stored inside the struct containing the wolf and it's baloon
    new_wolf->wolf = new Wolf(82, 50, 50, 50, this->falling_positions[rand() % 5], 480);
    //We fill the baloon field with the actual baloon equiped by the wolf
    new_wolf->baloon = new_wolf->wolf->get_equiped_ellement();
    this->walking_wolfs.push_back(new_wolf);
    //We add the wolf to the scene (the baloon will be automatically added as he is equiped by the wolf)
    this->world->add_ellement(new_wolf->wolf);
}

//This function is called when we want to spawn meat
void Logic::spawn_meat(){
    Projectile* meat = new Projectile("meat", "./sprites/Meat.bmp", 400, 210, 20, 10, 30, 10);
    this->meat = meat;
    //We add the meat to the scene
    this->world->add_ellement(this->meat);
}

bool Logic::compute_command(){
    //While there is some input to deal with
    while(SDL_PollEvent(&this->event)){
        switch (this->event.type){
            //If we click the cross
            case SDL_QUIT:
                //We return false to warn the calling function the quit button has been clicked on
                return false;
            //Event called when you push on a key
            case SDL_KEYDOWN:
                //We look at whick key has been pressed
                switch (event.key.keysym.scancode){
                    case SDL_SCANCODE_UP:
                        //If this is the up key, we se the speed of the pig to (-100) (it will move to the top of the screen)
                        this->pig->set_speed(-100);
                        break;
                    case SDL_SCANCODE_DOWN:
                        //If this is the down key, we se the speed of the pig to (100) (it will move to the bottom of the screen)
                        this->pig->set_speed(100);
                        break;
                    case SDL_SCANCODE_SPACE:
                        //If this is the space key, we check if this is not a repeted event (to avoid executing the code multiple time if the user hold it's finger on the key)
                        if(event.key.repeat == 0){
                            //If we where able to shoot
                            if(this->shoot())
                                this->next_reload = SDL_GetTicks() + 1000;
                        }
                    default:
                        break;
                }
                return true;
            //If the user stop pressing a key, the pig stop mooving
            case SDL_KEYUP:
                //We don't want to stop moving each time the player push (and release) the spacebar, so we also check if it's not the spacebar which is released
                if(event.key.keysym.scancode != SDL_SCANCODE_SPACE)
                    this->pig->set_speed(0);
            //It's used to make sure the program will continue to deal with input even if it's a mouse movement (to avoid calling default and returning each time the player move the mouse)
            case SDL_MOUSEMOTION:
                break;
            default:
                return true;
        }
    }
    return true;
}

bool Logic::click_cross(){
    while(SDL_PollEvent(&this->event)){
        switch (this->event.type){
            //If we click the cross
            case SDL_QUIT:
                //We return false to warn the calling function the quit button has been clicked on
                return false;
            case SDL_MOUSEMOTION:
                break;
            default:
                return true;
        }
    }
    return true;
}

//This function is used at the end of the programm to delete everything (and avoid memory leaks)
void Logic::destroy_logic(){
    //We first stop the three timers
    SDL_RemoveTimer(this->wolf_event);
    SDL_RemoveTimer(this->meat_event);

    //If the pig has an equiped arrow
    if(this->pig->has_equiped_ellement())
        //We delete it
        delete this->pig->get_equiped_ellement();
    //We delete the pig
    delete this->pig;
    //While there is some wolves in the walking wolf vector
    while(this->walking_wolfs.size() != 0){
        //If the last wolf has an equiped element
        if(this->walking_wolfs.back()->wolf->has_equiped_ellement())
            //We remove it
            delete this->walking_wolfs.back()->wolf->get_equiped_ellement();
        //we delete the last wolf in the vector
        delete this->walking_wolfs.back()->wolf;
        //We delete the struc containing the wolf and the baloon
        delete this->walking_wolfs.back();
        //We remove the last element of the vector (the one we just freed)
        this->walking_wolfs.pop_back();
    }
    //Same process than for walkink wolves
    while(this->flying_wolfs.size() != 0){
        if(this->flying_wolfs.back()->wolf->has_equiped_ellement())
            delete this->flying_wolfs.back()->wolf->get_equiped_ellement();
        delete this->flying_wolfs.back()->wolf;
        delete this->flying_wolfs.back();
        this->flying_wolfs.pop_back();
    }
    //We remove each arrow and each rock from their corresponding list (almost the same way we removed the wolves)
    while(this->arrows.size() != 0){
        delete this->arrows.back();
        this->arrows.pop_back();
    }
    while(this->rocks_list.size() != 0){
        delete this->rocks_list.back();
        this->rocks_list.pop_back();
    }
}

//This function is called by the spawning_callback
void Logic::will_spawn(){
    //We tell the class a wolf is ready to spawn. He will spawn next time we will call make_turn
    this->spawn_ready = true;
}

//This function is called by the spawning_meat_callback
void Logic::will_spawn_meat(){
    this->spawn_meat_ready = true;
}

//-------------------------------------------------------------------------//

bool Logic::shoot(){
    //The shoot function will return a launched projectile (the one he was equiped with)
    Projectile* new_arrow = this->pig->shoot(500);
    if(new_arrow != NULL){
        //If there is a projectile, we add it to the list of arrows
        this->arrows.push_back(new_arrow);
        return true;
    }
    else
        //If there is not (the pig hasn't reloaded yet), we return false to notify the failure
        return false;
}
