#ifndef ENGINE_H
#define ENGINE_H

#include"Input.hpp"
#include"ActiveInputs.hpp"
#include<SFML/Graphics.hpp>
#include"TileContainer.hpp"
#include"TileRenderer.hpp"
//#include"Renderer.hpp"
#include"RenderLevelSelect.hpp"
#include"RenderPlayField.hpp"
#include"RenderHighScore.hpp"
#include"RenderGameModeSelect.hpp"
#include"Audio.hpp"
#include"GameplayContainer.hpp"

class Engine {
public:

  enum MenuType {
    GAMEMODESELECT,
    LEVELSELECT,
    PLAYFIELD,
    HIGHSCORE,
  };

  Engine(TileContainer *menu_tile_container, TileContainer **_tilecont, const MenuType &_startingmenu);

  void frame(const ActiveInputs **_inputs, Audio &_audio);

  MenuType currentmenu;

private:
  void handlePlayFieldCase(Audio & _audio, const ActiveInputs **_inputs);

private:
  // * multiple
  std::vector<GameplayContainer> gameplay_containers;
  TileContainer **tileconts;
  TileContainer *menu_tile_container;

  RenderLevelSelect RLS;
  // * multiple
  std::vector<RenderPlayField> RPFs;
  std::vector<RenderHighScore> RHSs;

  RenderGameModeSelect RGMS;

  //levelselect
  int levelselect = 0;
  bool levelselectreload = true;
  bool game_mode_select_reload = true;

  //game
  unsigned long long framecounter = 0;
  //statistics
  int piececount[7];
};

#endif // ENGINE_H
