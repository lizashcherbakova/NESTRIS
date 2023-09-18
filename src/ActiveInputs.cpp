#include"ActiveInputs.hpp"

ActiveInputs::ActiveInputs(bool *_prevactiveinputs, bool *_activeinputs, bool _leftandright):
	leftandright(_leftandright),
	hide_mouse(false)
{
    //todo use std::copy
    //opposing axis handling
    for (std::size_t buttoninit=0; buttoninit<ntris::maxbuttons; ++buttoninit)
        prevactiveinputs[buttoninit]=_prevactiveinputs[buttoninit];
	
	for (std::size_t buttoninit = 0; buttoninit <ntris::maxbuttons; ++buttoninit) {
		activeinputs[buttoninit] = _activeinputs[buttoninit];
		if (activeinputs[buttoninit]) hide_mouse = true;
	}

}

bool ActiveInputs::getHold(const std::size_t& _button) const{ //todo maybe && prevactiveinputs?
    return activeinputs[_button];
}

bool ActiveInputs::getPress(const std::size_t& _button) const{
    return activeinputs[_button]&&!prevactiveinputs[_button];
}

bool ActiveInputs::getLAR() const {
    return leftandright;
}

bool ActiveInputs::getHideMouse() const {
	return hide_mouse;
}