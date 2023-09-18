#include"Engine.hpp"
#include<cstdio>
#include"random.hpp"
#include"Log.hpp"
#include<string>

Engine::Engine(TileContainer *_menu_tilecont, TileContainer **_tileconts, const MenuType &_startingmenu) :
        menu_tile_container(_menu_tilecont),
        tileconts(_tileconts),
        currentmenu(_startingmenu),
        RLS(_menu_tilecont, 0, 0), // LEVEL_SELECTION
        RGMS(_menu_tilecont, framecounter) // GAM
{
  framecounter = 0;
  Log::update<std::string>("system", std::string("Engine init"));

  for (int i = 0; i < ntris::players; ++i) {
    gameplay_containers.emplace_back(GameplayContainer(_tileconts[i], framecounter));
    gameplay_containers[i].music_theme = ntris::default_music_theme;
  }

  for (int i = 0; i < ntris::players; ++i) {
    RPFs.emplace_back(RenderPlayField(_tileconts[i], framecounter, 0, &gameplay_containers[i].gamestatus));
  }

  for (int i = 0; i < ntris::players; ++i) {
    RHSs.emplace_back(_tileconts[i], framecounter);
  }

}

// TODO: change all with [0]
void Engine::frame(const ActiveInputs **_inputs, Audio &_audio) {
  ++framecounter;
  ntris::incframe();
  random::prng();
  switch (currentmenu) {

    case GAMEMODESELECT:
      for(auto &game_container : gameplay_containers) {
        RGMS.renderGameModeSelect(game_mode_select_reload, game_container);
        // TODO: is okay to kepp inptus[0]
        RGMS.updateGameModeSelect(*_inputs[0], game_container, _audio);
      }

      game_mode_select_reload = false;
      if (RGMS.getSelectedEvent()) {
        currentmenu = LEVELSELECT;
        levelselectreload = true;
      }
      break;

    case LEVELSELECT:
      _audio.stopMusic();
      for(auto &game_container : gameplay_containers) {
        // TODO: is okay to kepp inptus[0]
        RLS.renderLevelSelect(levelselectreload, game_container);
        levelselect = RLS.updateLevelSelect(*_inputs[0], game_container,
                                            _audio);
      }

      levelselectreload = false;
      if (levelselect >= 0) {
        currentmenu = PLAYFIELD;
        for (int i = 0; i < ntris::players; ++i) {
          RPFs[i].resetPlayField(levelselect, gameplay_containers[i],
                                 framecounter);
        }
      } else if (RLS.exitMenu()) {
        currentmenu = GAMEMODESELECT;
        game_mode_select_reload = true;
      }
      break;

    case PLAYFIELD:
      handlePlayFieldCase(_audio, _inputs);
      break;

    case HIGHSCORE:
      _audio.stopMusic();
      // TODO: assemble results from all game modes.
      for (int i = 0; i < ntris::players; ++i) {
        RHSs[i].update(*_inputs[i], framecounter, gameplay_containers[i], _audio);
        RHSs[i].render(framecounter, gameplay_containers[i]);
        if (RHSs[i].submitted) {
          currentmenu = LEVELSELECT;
          levelselectreload = true;
          RHSs[i].submitted = false;
        }
      }
      break;

    default:
      Log::update_error("ERROR default frame case in engine::frame switch");
      break;
  }
}

void Engine::handlePlayFieldCase(Audio &_audio, const ActiveInputs **_inputs) {
  // For each player
  for (int i = 0; i < ntris::players; ++i) {
    // You will need to handle input for each player separately.
    // Assuming you have inputs for each player separately in _inputs[i]
    // If your audio class needs to handle both players, you will also need to modify it
    _audio.playMusic(gameplay_containers[i].music_theme, false, ntris::prepath);
    RPFs[i].update(*_inputs[i], framecounter, gameplay_containers[i], _audio);
    RPFs[i].render(framecounter, gameplay_containers[i], _audio);

    if (RPFs[i].gameOver()) {
      _audio.playTopOut();
      // The highscore handling here seems to be commented out. If you wish to enable it, it would also have to be updated to handle both players
      // RHS.resetHighScore(RPF[i].matrixhandler, RPF[i].piecehandler, RPF[i].scorehandler, RPF[i].levellineshandler, RPF[i].statisticshandler);
      currentmenu = HIGHSCORE;
    } else if(gameplay_containers[i].piecehandler.punch) {
      std::cout << "PUNCH==========" << std::endl;
      auto& first = gameplay_containers[i].piecehandler;
      auto& second = gameplay_containers[ntris::players - i -1].piecehandler;
      second.addPunch(first.getPunch());
    }
  }

}

