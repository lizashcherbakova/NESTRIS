#include"enums.hpp"

nes_schar glb::FrameCounter::framecountlittle=0;
nes_schar glb::FrameCounter::framecountbig=0;

nes_uchar glb::lineclearframecounter=0;
nes_uchar glb::updatingmatrix=0;
nes_uchar glb::ARE=0;
nes_uchar glb::real_level=0;
nes_uchar glb::shown_level=0;

sf::Vector2f glb::window_scale = { 2,2 };

void glb::lowercase_str(std::string& str) { //TODO make it portable with 16bitchar
	for (auto& c : str)
	{
		c = std::tolower(c);
	}
}
