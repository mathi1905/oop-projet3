#include "pig.hpp"


//the pig (like nearly everything else in our scene is an element. And inherit from this class)
Pig::Pig(int x, int y): Element("pig", "./sprites/PigMom.bmp", x, y, 30, 50){
    this->x_speed = 0;
    this->y_speed = 0;
}

//this function is used to change the speed of the pig
void Pig::set_speed(int y){
    this->y_speed = y;
}

void Pig::play_turn(){
    //each turn, we must check if the pig still inside the bounds
    if(this->gety() < 170)
        //If the pig is under the bound, we will move the pig in function of the speed of the last and of the ellapsed time (we in fact reverse the movement made by the pig the previous turn so the pig will not be able to cross the bounds)
        this->move(0, abs(this->y_speed*get_delta_time()));
    else if(this->gety() > 405)
        //If the pig is over the bound, we do basically the same thing than before but in the other direction
        this->move(0, -abs(this->y_speed*get_delta_time()));
    else
        //If the pig is inside the bound, we will move it
        this->move(this->get_delta_time());
}

bool Pig::reload(){
    if(!this->has_equiped_ellement()){
        //We create a new arrow for the pig to equip
        Projectile* arrow = new Projectile("Arrow", "./sprites/Arrow.bmp", 20, 10, 5, 1);
        //we equip the new arrow
        this->equip_element(arrow, -10, 10);
        return true;
    }
    return false;
}

void Pig::load(Element* munition){
    this->equip_element(munition, -10, 10);
}

Projectile* Pig::shoot(float speed){
    //We check if the pig can shoot (if there is an arrow loaded)
    if(!this->has_equiped_ellement())
        return NULL;
    //We create a copy of the arrow we will shoot
    Projectile* shooted_arrow = (Projectile*) this->get_equiped_ellement();
    //If it's the case, we unequip the arrow
    this->unequip_ellement();
    //We compute the speed as well as the angle the arrow will be shooted with
    shooted_arrow->lauch(speed, 185);
    //We return the arrow (for the game engine to deal with it)
    return shooted_arrow;
}
