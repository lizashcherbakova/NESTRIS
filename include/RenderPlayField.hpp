#pragma once
#ifndef RENDERPLAYFIELD_H
#define RENDERPLAYFIELD_H
#include"TileContainer.hpp"
#include"Renderer.hpp"
#include"ActiveInputs.hpp"
#include"Score.hpp"
#include"PieceContainer.hpp"
#include"MatrixContainer.hpp"
#include"LevelLines.hpp"
#include"Statistics.hpp"
#include"ntris.hpp"
#include"Audio.hpp"
#include"GameplayContainer.hpp"

//class Score; //todo fwd decl
//class LevelLines;
//class PieceContainer;
class RenderPlayField : public Renderer
{
    public:
        RenderPlayField(TileContainer* _tilecont, const nes_ushort& _frameappearance, nes_uchar _level, ntris::GameStatus *gameStatus);
        void update(const ActiveInputs& _input, const nes_ushort& _framecounter, GameplayContainer& _gameplay_container, Audio& _audio) ;
        void render(const nes_ushort& framecounter, GameplayContainer& _gameplay_container, Audio& _audio) ;
        void resetPlayField(const nes_uchar& _level_select, GameplayContainer& _gameplay_container, const nes_ushort& _framecounter);
        bool gameOver();
    private:
        void renderimage(bool blink);
        void renderBackground(const nes_uchar& _color1, const nes_uchar& _color2);
        bool tetris;
        bool tetris_sound_on = false;

        nes_uchar level;
        nes_uchar blinkingRate = 15;

        bool firstframeis4;
        bool paused;
        bool playfield_blink=false;
        bool game_over=false;
        nes_uchar pausecounter;

        void init_assets();
        std::vector<tiletype> renderblink;
        ntris::GameStatus *gamestatus;
        // std::vector<std::pair<nes_uchar, nes_uchar>> flyCubes;
        /*
        int lockpiece();
        int updatePlayField(const ActiveInputs& _input) ;
        int** pfmatrix;
        void renderSquare(const Uint8& red, const Uint8& green, const Uint8& blue, const Uint8& alpha, const std::size_t& x, const std::size_t& y, const std::size_t& w, const std::size_t& h);
        unsigned int das;
        unsigned int fallcounter;
        int spawnpiececounter;
        int blinkscreencounter;
        short spawnCount;
        void spawnPiece();
        int clearlines();
        int ** newmatrix;
        std::size_t linescleared;
        int linesclearedarray[22];




        void renderPiece(PieceDetails _piece);
        bool checkcollision(PieceDetails& _piece, PieceDetails& _lastgoodpos);

        enum BUTTONS {
            Left,
            Up,
            Right,
            Down,
            Select,
            Start,
            B,
            A
        };
        static int rotationmatrix[28][4][2];
        static char spawnTable[7];*/
};

#endif // RENDERPLAYFIELD_H
