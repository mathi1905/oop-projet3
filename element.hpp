#ifndef __ELEMENT__

#include <string>
#include <vector>
#include <SDL2/SDL_timer.h>

#define __ELEMENT__

class Element{
    public:
        Element(std::string name, const char* frame, int w, int h, int angle = 0);
        Element(std::string name, const char* frame, int x, int y, int w, int h, int angle = 0);
        void equip_element(Element* new_element, int x, int y);
        void move(float delta_time);
        void move(int x, int y);
        int get_angle();
        Element* get_equiped_ellement() const;
        bool has_equiped_ellement() const;
        void unequip_ellement();
        bool colide_with(Element* element) const;
        int getx() const;
        int gety() const;
        int get_x_center() const;
        int get_y_center() const;
        int getw() const;
        int geth() const;
        std::string get_name() const;
        std::vector<const char*> get_frame() const;

    private:
        std::string name;
        Element* equiped_element;
        Uint32 previous_time;
        Uint32 current_time;
        int w;
        int h;
        float x;
        float y;

    protected:
        float get_delta_time();
        std::vector<const char*> frames;
        float x_speed;
        float y_speed;
        int angle;
};

#endif