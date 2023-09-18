#ifndef WINDOW_H
#define WINDOW_H

#include"Input.hpp"
#include<SFML/Graphics.hpp>
#include"Engine.hpp"
#include"ntris.hpp"
#include"SafeQueue.hpp"
#include"DelayManager.hpp"
#include"ConfigReader.hpp"
#include"ConsoleManager.hpp"
#include"Log.hpp"
#include"ntris.hpp"
#include"MyClock.hpp"
#include"Audio.hpp"

#include<memory>
#include<atomic>
#include<cmath>
#include<thread>
#include<string>
#include<limits>
#include<iostream>
#include<thread>

class ConfigSaver {
public:
  bool _save_on_exit = true;

  void dont_save_on_exit() {
    _save_on_exit = false;
  }

  void save_on_exit() {
    _save_on_exit = true;
  }

  std::size_t getDefaultMusicTheme() {
    std::vector<std::size_t> default_music_theme = cfg.get<std::size_t>(
            "default_music_theme");
    if (default_music_theme.size() < 1) return 4;
    else return default_music_theme[0];
  }

  bool getFourThirds() {
    std::vector<bool> four_thirds = cfg.get<bool>("four_thirds");
    if (four_thirds.size() < 1) return true;
    if (four_thirds[0] == true) return true;
    return false;
  }

  bool getFullscreen() {
    std::vector<bool> fullscreen = cfg.get<bool>("fullscreen");
    if (fullscreen.size() < 1) return false;
    if (fullscreen[0] == true) return true;
    return false;
  }

  bool getShader() {
    std::vector<bool> shader = cfg.get<bool>("shader");
    if (shader.size() < 1) return false;
    if (shader[0] == true) return true;
    return false;
  }

  sf::Vector2<long double> setWindowScale(std::size_t const &width_pixels,
                                          std::size_t const &height_pixels,
                                          bool const &four_thirds) {
    std::vector<double> window_scale = cfg.get<double>("window_scale");
    sf::Vector2<long double> scale{1, 1};
    if (window_scale.size() >= 2) {
      scale.x = window_scale[0];
      scale.y = window_scale[1];
    }
    if (four_thirds) {
      scale.y = 3.L / 4.L * scale.x * width_pixels / height_pixels;
    }
    return scale;
  }

  sf::Vector2i setWindowPosition(std::size_t const &window_width,
                                 std::size_t const &window_height) {
    std::vector<int> window_position = cfg.get<int>("window_position");
    sf::Vector2i window_pos{0, 0};
    if (window_position.size() < 2) {
      window_position.resize(2);
      std::size_t screen_width = sf::VideoMode::getDesktopMode().width;
      std::size_t screen_height = sf::VideoMode::getDesktopMode().height;
      if (window_width <= screen_width) {
        window_pos.x = (screen_width - window_width) / 2;
      }
      if (window_height <= screen_height) {
        window_pos.y = (screen_height - window_height) / 2;
      }
    } else {
      window_pos.x = window_position[0];
      window_pos.y = window_position[1];
    }
    return window_pos;
  }

  void saveWindowScale() {
    saveConfig("window_scale", ntris::window_scale);
  }

  void saveFourThirds() {
    cfg.overwrite("four_thirds", std::vector<bool>(1, ntris::four_thirds));
  }

  void saveFullscreen() {
    cfg.overwrite("fullscreen", std::vector<bool>(1, ntris::fullscreen));
  }

  void saveShader() {
    cfg.overwrite("shader", std::vector<bool>(1, ntris::shader));
  }

  void saveWindowPosition() {
    saveConfig("window_position", ntris::window_position);
  }

  void saveDefaultMusicTheme() {
    std::vector<std::size_t> theme;
    theme.push_back(ntris::default_music_theme);
    cfg.overwrite("default_music_theme", theme);
  }

  ConfigSaver(ConfigReader &_cfg) : cfg(_cfg) {};

  ~ConfigSaver() {
    if (_save_on_exit) {
      saveFullscreen();
      saveFourThirds();
      saveWindowScale();
      saveWindowPosition();
      saveDefaultMusicTheme();
      cfg.save();
    }
  }

private:
  ConfigReader &cfg;

  template<typename T>
  void saveConfig(std::string const &name, sf::Vector2<T> vector) {
    cfg.overwrite(name, std::vector<T>{vector.x, vector.y});
  }
};

class TileRenderer;

struct RenderFrame {
public:

  ConsoleManager cm;
  Audio audio;

  largest_uint timeperframe_odd;
  largest_uint timeperframe_even;
  largest_uint timeperframe;
  bool odd_frame;
  MyClock elapsedtime;
  largest_uint partspersecond;
};

class Window : public sf::NonCopyable {
public:
  enum OPT {
    GENERAL,
    SMALLEST,
    SPAM,
    FULL,
    NOTHING,
    ARRAY,
    ARRAYLOG,
    ARRAYBUCKET
  };

  Window(const std::string &_prepath, const std::size_t &width, const std::size_t &height, const OPT &optimized);

  ~Window() {
    delete engine;
    delete cfg;
    delete config_saver;
  }

  void render();

private:

  sf::View views[ntris::players];

  int cor[4] = {0, 0, 256, 224};

  bool shader = false;

  std::atomic<unsigned int> window_size_x;
  std::atomic<unsigned int> window_size_y;
  std::atomic<bool> close_window;
  std::atomic<bool> hide_cursor;
  std::atomic<bool> toggle_fullscreen;
  std::atomic<bool> fullscreen;
  std::atomic<bool> vertical = false;

  std::atomic<bool> isGame = false;
  std::atomic<bool> game_start = false;
  std::atomic<bool> game_stop = false;

  Engine *engine;
  ConfigReader *cfg;
  ConfigSaver *config_saver;

  std::vector<Input> inputManagers;
  std::unique_ptr<DelayManager> delay_manager;
  SafeQueue<sf::Event> event_queue;
  std::mutex window_mutex;

  bool isWindowOpen();

  sf::RenderWindow window;
  std::string prepath;

  void toggle_fullscreen_func();

  sf::View window_view;
  // TileRenderer for main menu.
  TileRenderer menuTilerend;

  TileRenderer tilerends[ntris::players];
  // Copies previous filed, but keeps containers.
  TileContainer *tilerendsContainers[ntris::players];

// Refactored methods:

// 1 - Methods used in constructor.
  void initWindowConfig(const std::size_t &_width, const std::size_t &_height,
                        const OPT &optimized);

  void initDelayManager(const OPT &optimized);

  ConfigSaver *loadConfig();

  void createWindow(ConfigSaver &config_saver);

  void startEventLoop();

// 2 - Methods used in render loop.
  RenderFrame *initializeRenderComponents();

  void handleRenderLoop(RenderFrame *renderFrame);

  void handleEvents(RenderFrame *renderFrame);

  void initializeViews();

  void initializeView();

  void setWindowedMode();

  bool twoWindowMode() {
    return engine->currentmenu == Engine::PLAYFIELD || engine->currentmenu == Engine::HIGHSCORE;
  }
};

#endif
