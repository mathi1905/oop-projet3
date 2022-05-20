#include "element.hpp"

using std::string;

Element::Element(string name, const char* frame, int w, int h, int angle){
    this->name = name;
    this->w = w;
    this->h = h;
    this->equiped_element = NULL;
    this->x_speed = 0;
    this->previous_time = 0;
    this->current_time = 0;
    this->angle = angle;
    this->frames.push_back(frame);
}

Element::Element(string name, const char* frame, int x, int y, int w, int h, int angle){
    this->name = name;
    this->x = (float) x;
    this->y = (float) y;
    this->w = w;
    this->h = h;
    this->equiped_element = NULL;
    this->y_speed = 0;
    this->previous_time = 0;
    this->current_time = 0;
    this->angle = angle;
    this->frames.push_back(frame);
}

void Element::equip_element(Element* new_element, int x, int y){
    //We place the element  relatively to the center of the element which will equip this last
    new_element->x = this->x + this->w/2 - new_element->w/2 + x;
    new_element->y = this->y + this->h/2 - new_element->h/2 + y;
    this->equiped_element = new_element;
}

void Element::move(float delta_time){
    //We move the element in function of the x and the y speed of the last but also in function of the elapsed time (given in argument)
    this->x += (float) this->x_speed*delta_time;
    this->y += (float) this->y_speed*delta_time;
    //If the element we are moving has something equiped, we move it too
    if(this->equiped_element != NULL){
        this->equiped_element->x += this->x_speed*delta_time;
        this->equiped_element->y += this->y_speed*delta_time;
    }
}

//This function is used to move an element relatively to it's current position but in one go
void Element::move(int x, int y){
    this->x += x;
    this->y += y;
    if(this->equiped_element != NULL)
        this->equiped_element->move(x, y);
}

//Get the angle of an element
int Element::get_angle(){
    return this->angle;
}

//Get the equiped element (or null if the is no equiped element)
Element* Element::get_equiped_ellement() const{
    return this->equiped_element;
}

//Return true if the element has an equiped element. False otherwise
bool Element::has_equiped_ellement() const{
    if(this->equiped_element != NULL)
        return true;
    return false;
}

//Check the colision (between the center of the element which call the function and the "bordures" of the element given as an argument)
bool Element::colide_with(Element* element) const{
    if(this->get_x_center() >= element->getx() && this->get_x_center() <= element->getx() + element->getw() && this->get_y_center() >= element->gety() && this->get_y_center() <= element->gety() + element->geth())
        return true;
    return false;
}

//All these are getters. There is nothing spcial about them
int Element::getx() const{
    return this->x;
}

int Element::gety() const{
    return this->y;
}

int Element::get_x_center() const{
    return this->x + this->w/2;
}

int Element::get_y_center() const{
    return this->y + this->h/2;
}

int Element::getw() const{
    return this->w;
}

int Element::geth() const{
    return this->h;
}

string Element::get_name() const{
    return this->name;
}

std::vector<const char*> Element::get_frame() const{
    return this->frames;
}

//-------------------------------------------------//

float Element::get_delta_time(){
    //The current time is given by sdl_getTick
    this->current_time = SDL_GetTicks();
    //Delta time is the current time-the previous time
    float delta_time = (float)(this->current_time - this->previous_time)/1000;
    //The current time is now the previous time
    this->previous_time = this->current_time;
    return delta_time;
}

//If we une quipe an element, we don't forget to put NULL instead of the previous equiped element
void Element::unequip_ellement(){
    this->equiped_element = NULL;
}