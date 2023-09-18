#ifndef INPUT_H
#define INPUT_H
#include"ActiveInputs.hpp"
#include"ntris.hpp"
#include<vector>
#include<array>
#include<utility>
#include<SFML/Window/Event.hpp>
#include<SFML/Window/Keyboard.hpp>
#include<SFML/Window/Joystick.hpp>
#include<unordered_map>

struct input_union {
    union {
        sf::Keyboard::Key keyboard_input;
        int joystick_button;
        sf::Joystick::Axis joystick_axis;
    };
    enum  {
        KEYBOARD_INPUT,
        JOYSTICK_BUTTON,
        JOYSTICK_AXIS
    } tag;
    double axis_sign;
};

class Input
{
    public:
        Input(const std::string& keybinding_file);
        ActiveInputs getInput();
    private:
        void update(const std::size_t& _buttons);
        void setup(const std::string& keybinding_file);
        void initMap();
        sf::Event event;
		bool prevactiveinputs[ntris::maxbuttons] = { false };
		bool activeinputs[ntris::maxbuttons] = { false };
		bool leftandright = false;
        bool isActive(const input_union& _input_union) const;
        std::array<std::vector<input_union>, ntris::maxbuttons >inputdependancies;
        std::size_t maxbuttons;
        std::vector<unsigned int> active_joysticks;
        std::array<std::array<float, sf::Joystick::AxisCount>, sf::Joystick::Count > joystick_axis_deadzone;
        std::unordered_map<std::string, input_union> keybinds_lookup_table;
};
#endif
