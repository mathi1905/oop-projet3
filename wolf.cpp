#include "wolf.hpp"

//This is the callback called when the wolf will shoot womething (every 3 sec from the moment start shooting is called)
static Uint32 reload_callback(Uint32 interval, void* param){
    Wolf* arg = (Wolf*) param;
    arg->can_shoot();
    return interval;
}

Wolf::Wolf(int x, int y, int runing_speed, int falling_speed, int falling_position, int ground_position) : Element("wolf", "./sprites/Wolf_WalkingLeft_Balloon_Step1.bmp", x - 30/2, y + 35/2, 30, 35){
    //We don't forget to "reset" delta time before starting to move
    this->get_delta_time();
    this->runing_speed = runing_speed;
    this->x_speed = runing_speed;
    this->frames.push_back("./sprites/Wolf_WalkingLeft_Balloon_Step2.bmp");
    Element* baloon = new Element("baloon", "./sprites/Balloon.bmp", 20, 30);
    this->equip_element(baloon, 5, -20);
    this->falling_position = falling_position;
    this->falling_speed = falling_speed;
    this->ground_position = ground_position;
    this->is_falling = false;
    this->will_shoot = false;
    this->is_shooting = false;
    this->is_dead = false;
}

Projectile* Wolf::play_turn(){
    //Each turn, we move the wolf
    this->move(this->get_delta_time());
    //If the world is behing the falling position (and is not falling right now)
    if(this->getx() >= this->falling_position && !this->is_falling)
        //The wolf will start to fall instead of walking
        this->start_falling();
    //If the world touche the ground and is flying (we use this parameter to be sure to ewxecute this only once)
    if(this->gety() >= this->ground_position - this->geth() && this->is_falling){
        //We stop falling
        this->stop_falling();
        //If we where shooting (if we where alive) we stop shooting as well
        if(this->is_shooting)
            this->stop_shooting();
        //We unequip our ellement (if there is no, it's not a problem as unequip ellement will only replace NULL by NULL)
        this->unequip_ellement();
    }
    //If the wolf is ready to shoot
    if(this->will_shoot){
        //we create a new rock
        Projectile* rock = new Projectile("Rock", "./sprites/Rock.bmp", this->get_x_center(), this->get_y_center(), 15, 15, 30, 1);
        //We lauch the new rock
        rock->lauch(250, -50);
        //We prevent the wolf to shoot again next turn
        this->will_shoot = false;
        //We give the shooted rock to the calling function
        return rock;
    }
    //If we didn't shoot this turn, we don't need to return anything
    return NULL;
}

//This function is called when we start to fall
void Wolf::start_falling(){
    this->is_falling = true;
    this->x_speed = 0;
    this->y_speed = this->falling_speed;
    //We will need to change the current list of frame. So we clear the old one
    this->frames.clear();
    //We add the new frame in the previously cleared vector
    this->frames.push_back("./sprites/Wolf_Falling_Balloon.bmp");
}

//This function is called when we stop falling (when we touch the ground)
void Wolf::stop_falling(){
    this->is_falling = false;
    this->x_speed = this->runing_speed;
    this->y_speed = 0;
    //We will (the same way than before) change the frames making the animation
    this->frames.clear();
    this->frames.push_back("./sprites/Wolf_WalkingLeft_Step1.bmp");
    this->frames.push_back("./sprites/Wolf_WalkingLeft_Step2.bmp");
}

bool Wolf::get_is_falling(){
    return this->is_falling;
}

bool Wolf::get_is_shooting(){
    return this->is_shooting;
}

void Wolf::start_shooting(){
    //We tell the class we want to shoot at the next turn
    this->can_shoot();
    //We start a timer (to shoot every 3 sec)
    this->shoot_timer = SDL_AddTimer(3000, reload_callback, this);
    //We tell the class the wolf that we started to shoot
    this->is_shooting = true;
}

void Wolf::stop_shooting(){
    //We stop the timer (to avoid shooting again)
    SDL_RemoveTimer(this->shoot_timer);
    //If the wolf where ready to shoot, we make sure he will not
    this->is_shooting = false;
    //We tell the struct we aren't shooting anymore
    this->will_shoot = false;
}

void Wolf::can_shoot(){
    this->will_shoot = true;
}

void Wolf::die(){
    this->is_dead = true;
    //We change the current frames
    this->frames.clear();
    this->frames.push_back("./sprites/Wolf_Free_Falling.bmp");
    this->stop_shooting();
    //The y_speed will increase
    this->y_speed = 100;
}

bool Wolf::dead(){
    return this->is_dead;
}
