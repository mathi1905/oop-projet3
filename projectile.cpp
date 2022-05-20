#include "projectile.hpp"

#define PI 3.14159265

using std::string;

Projectile::Projectile(string name, const char* path, int x, int y, int w, int h, int masse, int cross_sectional_area, float air_density, float drag_coefficent, float grav) : Element(name, path, x, y, w, h){
    this->masse = masse;
    this->cross_sectional_area = cross_sectional_area;
    this->air_density = air_density;
    this->drag_coefficient = drag_coefficent;
    this->grav = grav;
}

Projectile::Projectile(string name, const char* path, int w, int h, int masse, int cross_sectional_area) : Element(name, path, w, h){
    this->masse = masse;
    this->cross_sectional_area = cross_sectional_area;
    this->air_density = 1;
    this->drag_coefficient = 0.001;
    //We used 300 instead of 500 for the gravity. We don't know why, but everything was falling way too fast in 500 and it was impossible to figure out why. With a gravity of 300 everything act just like in the demo
    this->grav = 300;
}

void Projectile::lauch(float speed, int angle){
    //We reinitiate the delta time (to avoid having some very big value when we the projectile is lauched after a long time)
    this->get_delta_time();
    //We get the x and the y speed with the well known physic formula
    this->x_speed = speed*cos(angle*(PI/180));
    this->y_speed = speed*sin(angle*(PI/180));
    this->angle = angle;
}

void Projectile::play_turn(){
    //Each turn, we get the delta time and store it to a variable (otherwise, he will be "reset" everytime we will call it)
    float delta_time = get_delta_time();
    //We compute the work of the y_speed (x_speed had no acceleration, so it's work is null)
    float work = this->masse*this->grav;
    //We compute the speed using the work as well as the drag forces
    this->y_speed += (float) ((work - this->get_ydrag())/this->masse)*delta_time;
    //We do the same for the x_speed (except the work is 0, so no need to use it in the formula)
    this->x_speed -= (this->get_xdrag()/this->masse)*delta_time;
    //We change the position of the projectile
    this->move(delta_time);
    //We compute the new angle
    this->angle = atan2(-this->y_speed, -this->x_speed) * 180/PI;
}

//----------------------------------------------------------------------------------------//

float Projectile::get_xdrag(){
    //We compute the x drag force using the "classic" formula
    float drag_force = (0.5)*this->air_density*this->cross_sectional_area*this->drag_coefficient*pow(this->x_speed, 2)*(-1*cos(this->angle*(PI/180)));
    return drag_force;
}

float Projectile::get_ydrag(){
    //We compute the y drag force using the "classic" formula
    float drag_force = (0.5)*this->air_density*this->cross_sectional_area*this->drag_coefficient*pow(this->y_speed, 2)*(-1*sin(this->angle*(PI/180)));
    return drag_force;
}
