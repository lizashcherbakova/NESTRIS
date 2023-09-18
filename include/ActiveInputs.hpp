#ifndef ACTIVEINPUTS_H
#define ACTIVEINPUTS_H

#include"ntris.hpp"
class ActiveInputs {
    public:
        ActiveInputs(bool *_prevactiveinputs, bool *_activeinputs, bool _leftandright=false);
        bool getPress(const std::size_t& _button) const;
        bool getHold(const std::size_t& _button) const;
        bool getLAR() const;
		bool getHideMouse() const;

    private:

        bool leftandright;

        bool prevactiveinputs[ntris::maxbuttons];
        bool activeinputs[ntris::maxbuttons];

		bool hide_mouse = false;
};

#endif
