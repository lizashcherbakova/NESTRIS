#ifndef LEVELLINES_H
#define LEVELLINES_H
#include"Renderer.hpp"
#include"TileContainer.hpp"
#include"ntris.hpp"
#include"Audio.hpp"

class LinesContainer {
public:
    LinesContainer() {
        lines[0]=lines[1]=0;
    }
    explicit LinesContainer(nes_uchar _lines[]) {
        lines[0]=_lines[0];
        lines[1]=_lines[1];
    }
    explicit LinesContainer(unsigned int _lines) {
        lines[0]=(_lines&0x0f)+((_lines/10)%10)*0x0f;
        _lines/=100;
        lines[1]=(_lines&0x0f)+((_lines/10)%10)*0x0f;
        _lines/=100;
    }
    nes_uchar lines[2];
    unsigned int reallines() const {
        unsigned int result=(lines[0]&0x0f)+((lines[0]&0xf0)/0x0f)*10;
        result+=(lines[1]&0x0f)*100+((lines[1]&0xf0)/0x0f)*1000;
        return result;
    }
    bool operator>(const LinesContainer& _sc) const {
        return this->reallines()>_sc.reallines();
    }
    const nes_uchar& operator[](const std::size_t& i) const {
        return lines[i];
    }
    nes_uchar& operator[](const std::size_t& i) {
        return lines[i];
    }
    LinesContainer& addLines(const nes_uchar& _added_lines) { //TODO breaks for more than 6 lines added
        lines[0]+=_added_lines;
        if (lines[0]%16u>9u) {
            lines[0]+=0x06;
        }
        if (lines[0]/16u>9u) {
            lines[0]&=0x0f;
            lines[1]++;
        }
        return *this;
    }
};

class LevelLines : public Renderer //DOESN'T WORK LIKE THE NES AFTER LEVEL 137
{
    public:
        LevelLines() {};
        LevelLines(TileContainer * _tilecont, const nes_ushort& _frameappearance, const nes_uchar& _level, ntris::GameStatus* _gamestatus);
        const nes_uchar& get_real_level() const;
        const nes_uchar& get_shown_level() const;
        const nes_uchar& get_starting_level() const;
        unsigned int getLines() const;
        nes_uchar getTetrisPercentage() const;
        void addlines(const nes_uchar& _clearedlines);
        void render(Audio& _audio);
    private:
        LinesContainer lines{};
        LinesContainer linestemp{};
        nes_uchar starting_level = 0;
        nes_uchar real_level=0;
        nes_uchar shown_level=0;
        nes_ushort linestolevelup=0;
        std::size_t total_lines_cleared=0;
        std::size_t tetris_lines_cleared=0;
        static nes_uchar level_hex[256]; //TODO
        // static field form ntris is turned into a struct.
        ntris::GameStatus* gamestatus;
};

#endif // LEVELLINES_H
