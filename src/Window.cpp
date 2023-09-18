#include"Window.hpp"

bool Window::isWindowOpen() {
  std::lock_guard<std::mutex> fullscreen_lock(window_mutex);
  return window.isOpen();
}

void Window::toggle_fullscreen_func() {
  std::cout << "TOGGLE" << std::endl;
  std::lock_guard fullscreen_lock(window_mutex);

  if (fullscreen.load()) {
    ntris::fullscreen = false;
    fullscreen.store(false);
    window.setActive();
    window.close();
    window.create(sf::VideoMode(window_size_x.load(), window_size_y.load()),
                  "NESTRIS");
    window.setPosition(ntris::window_position);
    window.setView(window_view);
    window.setActive(false);
  } else {
    ntris::fullscreen = true;
    fullscreen.store(true);
    window.setActive();
    ntris::window_position = window.getPosition();
    window_view = window.getView();
    window.close();
    window.create(sf::VideoMode::getFullscreenModes()[0], "NESTRIS",
                  sf::Style::Fullscreen);
    sf::View fullscreen_view(sf::FloatRect(0, 0, menuTilerend.getWidthPixels(),
                                           menuTilerend.getHeightPixels()));
    sf::Vector2f size = {window_view.getViewport().width,
                         window_view.getViewport().height};
    sf::Vector2f fullscreen_size = {
            (float) sf::VideoMode::getFullscreenModes()[0].width,
            (float) sf::VideoMode::getFullscreenModes()[0].height};
    sf::Vector2f window_position = {(float) ntris::window_position.x + 5,
                                    (float) ntris::window_position.y +
                                    30}; //Idk how to get the title bar height
    fullscreen_view.setViewport(
            sf::FloatRect(window_position.x / fullscreen_size.x,
                          window_position.y / fullscreen_size.y,
                          window_size_x.load() / fullscreen_size.x,
                          window_size_y.load() / fullscreen_size.y));
    window.setView(fullscreen_view);
    window.setActive(false);
  }
}

void Window::setWindowedMode() {

  std::size_t window_width = menuTilerend.getWidthPixels() * ntris::window_scale.x;
  std::size_t window_height = menuTilerend.getHeightPixels() * ntris::window_scale.y;

  if (isGame.load()) {
    // Creating window for game mode (twice as big)

    if (!vertical.load()) {
      window.setSize(sf::Vector2u(window_width * 2, window_height));

      // TODO: split for 2 game screens should be added.
    } else {
      window.setSize(sf::Vector2u(window_width, window_height * 2));

      // TODO: split for 2 game screens should be added.
    }
  } else {
    // TODO: exiting from 2 screens mode.
    // std::cerr << "Not implemented!" << std::endl;
    window.setSize(sf::Vector2u(window_width, window_height));
  }

  ntris::window_position = config_saver->setWindowPosition(window_width, window_height);

  window.setPosition(ntris::window_position);

  //window.setActive(false);
}

//----- Refactored code --------//

// ----- Window rendering --------//

void Window::render() {

  RenderFrame *renderFrame = initializeRenderComponents();

  handleRenderLoop(renderFrame);

  delete renderFrame;
}

///////

RenderFrame *Window::initializeRenderComponents() {

  // Used for managing between methods.
  RenderFrame *renderFrame = new RenderFrame();

  renderFrame->partspersecond = MyClock::getPartsPerSecond();

  renderFrame->timeperframe_odd =
          (long double) (renderFrame->partspersecond) / ntris::ntsc_fps_odd;
  renderFrame->timeperframe_even =
          (long double) (renderFrame->partspersecond) / ntris::ntsc_fps_even;
  renderFrame->timeperframe = renderFrame->timeperframe_odd;

  renderFrame->odd_frame = false;

  renderFrame->audio.init(ntris::prepath);

  window.setActive();

  return renderFrame;
}


void Window::handleRenderLoop(RenderFrame *renderFrame) {
  // Main rendering loop
  while (!close_window.load() && isWindowOpen()) {

    // Skipping that moment because window is not created yet.
    if (game_start.load() || game_stop.load()) {
      continue;
    }

    if (isGame.load()) {
      if (!twoWindowMode()) {
        // should handle game stop
        std::cout << "Game stop" << std::endl;
        isGame.store(false);
        game_stop.store(true);
      }
    } else if (twoWindowMode()) {
      // should handle game start.
      std::cout << "Game start" << std::endl;
      //toggle_double_size_func(true);
      // toggle_fullscreen_func();
      isGame.store(true);
      game_start.store(true);
    }
    // Toggle between odd and even frames
    renderFrame->odd_frame = !renderFrame->odd_frame;
    renderFrame->timeperframe = renderFrame->odd_frame ? renderFrame->timeperframe_odd : renderFrame->timeperframe_even;

    // Check if enough time has passed for the next frame
    if (renderFrame->elapsedtime.elapsedTime() >= renderFrame->timeperframe) {

      // Log the frames-per-second (FPS) if enough time has passed for the next frame
      if (renderFrame->elapsedtime.elapsedTime() > 0)
        Log::update<long double>("fps",
                                 renderFrame->partspersecond / (long double) renderFrame->elapsedtime.elapsedTime());

      // Reset the clock tracking the elapsed time for the next frame
      renderFrame->elapsedtime.restart();
      sf::Int64 delaycalc = 0;

      const ActiveInputs temp_inputs[ntris::players] = {inputManagers[0].getInput(), inputManagers[1].getInput()};

      const ActiveInputs *ais[ntris::players];

      for (int i = 0; i < ntris::players; ++i) {
        ais[i] = &(temp_inputs[i]);
      }
      // Update the game engine with the current active inputs and audio
      engine->frame(ais, renderFrame->audio);

      // Hide or show the mouse cursor based on the active inputs
      for (int i = 0; i < ntris::players; ++i) {
        if (!hide_cursor.load() && temp_inputs[i].getHideMouse()) {
          hide_cursor.store(true);
          break;  // Stop the loop as soon as we find a player who wants to hide the cursor
        } else if (hide_cursor.load() && !temp_inputs[i].getHideMouse()) {
          hide_cursor.store(false);
        }
      }

      // Log the delay due to processing (getting active inputs and updating the game engine)
      Log::update<sf::Int64>("processing delay", renderFrame->elapsedtime.elapsedTime() - delaycalc);
      delaycalc = renderFrame->elapsedtime.elapsedTime();

      // Lock the window for drawing
      {
        std::lock_guard<std::mutex> fullscreen_lock(window_mutex);

        Log::update<sf::Int64>("draw delay", renderFrame->elapsedtime.elapsedTime() - delaycalc);
        delaycalc = renderFrame->elapsedtime.elapsedTime();


        if (twoWindowMode() && isGame.load()) {
          window.clear();
          for (int i = 0; i < ntris::players; ++i) {

            window.setView(views[i]);
            tilerends[i].drawmod(window);
            Log::update<sf::Int64>("draw delay",
                                   renderFrame->elapsedtime.elapsedTime() -
                                   delaycalc);
            delaycalc = renderFrame->elapsedtime.elapsedTime();

          }
          window.display();

        } else {
          // Clear the window for the next frame
          window.clear();
          // Draw the next frame on the window
          menuTilerend.drawmod(window);

          // Log the delay due to drawing on the window
          Log::update<sf::Int64>("draw delay",
                                 renderFrame->elapsedtime.elapsedTime() -
                                 delaycalc);
          delaycalc = renderFrame->elapsedtime.elapsedTime();

          // Display the window with the new frame
          window.display();
        }
      }

      // Log the delay due to displaying the window
      Log::update<sf::Int64>("display delay", renderFrame->elapsedtime.elapsedTime() - delaycalc);
      delaycalc = renderFrame->elapsedtime.elapsedTime();

      // Handle window events, such as user input and window resizing
      handleEvents(renderFrame);

    } else {
      // Introduce delay to maintain the target frame rate
      delay_manager->delay(renderFrame->timeperframe - renderFrame->elapsedtime.elapsedTime());
    }
  }

  // Save the current window position if not in fullscreen mode
  if (!fullscreen.load()) {
    std::lock_guard<std::mutex> fullscreen_lock(window_mutex);
    ntris::window_position = window.getPosition();
  }

  // Log the termination of the window
  Log::update<std::string>("system", std::string("Window terminating"));

  // Refresh the console manager and close the info window
  renderFrame->cm.refresh(true);
  renderFrame->cm.close_info_window();
}


void Window::handleEvents(RenderFrame *renderFrame) {

  // Add the keycodes for the keys you want to use.
  // I'm using A, S, D, and W as placeholders here.
  sf::Keyboard::Key keys[4] = {sf::Keyboard::Q, sf::Keyboard::W, sf::Keyboard::E, sf::Keyboard::R};
  sf::Keyboard::Key decrementKeys[4] = {sf::Keyboard::Z, sf::Keyboard::X, sf::Keyboard::C, sf::Keyboard::V};

  while (auto event = event_queue.pop_if_not_empty()) {
    //sf::Event event = event_queue.pop();
    switch (event->type) {
      case sf::Event::Closed:
        close_window.store(true);
        break;
      case sf::Event::KeyPressed: {

        // Decrement the array elements when corresponding key is pressed.
        for (int i = 0; i < 4; i++) {
          if (event->key.code == decrementKeys[i]) {
            cor[i] -= 1;


            for (int i = 0; i < 4; i++) {
              std::cout << "cor[" << i << "] = " << cor[i] << std::endl;
            }
            std::cout << '\n';
            break;
          }

        }

        // Increment/decrement the array elements when corresponding key is pressed.
        for (int i = 0; i < 4; i++) {
          if (event->key.code == keys[i]) {
            cor[i] += 1;
            // Display all array elements each time a key is pressed.
            for (int i = 0; i < 4; i++) {
              std::cout << "cor[" << i << "] = " << cor[i] << std::endl;
            }
            std::cout << '\n';
            break;
          }
        }


        bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
        std::lock_guard<std::mutex> fullscreen_lock(window_mutex);
        if (window.hasFocus()) {
          if (event->key.code == sf::Keyboard::F1) {
            renderFrame->cm.toggle_info_window();
            if (!renderFrame->cm.is_window_open())
              window.requestFocus();
          }
          if ((event->key.code == sf::Keyboard::F &&
               ctrl)/* || Input::getInput()*/) {
            toggle_fullscreen.store(true);
          }
        }
      }
        break;
      case sf::Event::Resized:
        if (!fullscreen.load()) {
          std::lock_guard<std::mutex> fullscreen_lock(window_mutex);
          window_size_x.store(window.getSize().x);
          window_size_y.store(window.getSize().y);
        }
        break;
    }
  }
  renderFrame->cm.refresh();
}

//----- Window initializing --------//

Window::Window(const std::string &_prepath, const std::size_t &width, const std::size_t &height,
               const OPT &optimized) : prepath(_prepath) {
  initWindowConfig(width, height, optimized);

  // Loading Config
  config_saver = loadConfig();

  for (int i = 0; i < ntris::players; ++i) {
    tilerendsContainers[i] = tilerends[i].getTileContainer();
    std::string filename = prepath + "settings/keybinds" + std::to_string(i) + ".ini";
    inputManagers.emplace_back(Input(filename));
  }

  engine = new Engine(menuTilerend.getTileContainer(), tilerendsContainers, Engine::GAMEMODESELECT);
  // Create Window
  createWindow(*config_saver);

  // Event Loop
  startEventLoop();
}

///////

void
Window::initWindowConfig(const std::size_t &_width, const std::size_t &_height,
                         const OPT &optimized) {
  // Initialize atomic variables and DelayManager
  close_window.store(false);
  hide_cursor.store(false);
  toggle_fullscreen.store(false);
  fullscreen.store(false);
  window_size_x.store(1);
  window_size_y.store(1);
  initDelayManager(optimized);
}

void Window::initDelayManager(const OPT &optimized) {
  // Initialize DelayManager based on the 'optimized' parameter
  switch (optimized) {
    case GENERAL:
      delay_manager = std::make_unique<GeneralDelayManager>();
      break;
    case SMALLEST:
      delay_manager = std::make_unique<SmallestDelayManager>();
      break;
    case SPAM:
      delay_manager = std::make_unique<SpamDelayManager>();
      break;
    case FULL:
      delay_manager = std::make_unique<FullThreadDelayManager>();
      break;
    case NOTHING:
      delay_manager = std::make_unique<NothingDelayManager>();
      break;
    case ARRAY:
      delay_manager = std::make_unique<ArrayDelayManager>();
      break;
    case ARRAYLOG:
      delay_manager = std::make_unique<ArrayLogDelayManager>();
      break;
    case ARRAYBUCKET:
      delay_manager = std::make_unique<BucketArrayDelayManager>();
      break;
    default:
      delay_manager = std::make_unique<GeneralDelayManager>();
  }
}

void initializeTileRand(TileRenderer &tilerend,
                        const std::pair<largest_uint, largest_uint> &tilesize,
                        const sf::Vector3<std::size_t> &extra_render,
                        const std::string &prepath) {
  tilerend.create(ntris::ntsc_tiles_x, ntris::ntsc_tiles_y, tilesize,
                  TileRenderer::DRAWTEXTURE, extra_render);

  tilerend.load(prepath + "texturesprite/sprites.txt");

  if (ntris::shader)
    tilerend.set_shader(prepath + "shaders/crt.glsl", sf::Shader::Fragment);
}

ConfigSaver *Window::loadConfig() {
  // Load configuration related to window and game settings
  // ... existing load config code ...
  cfg = new ConfigReader(prepath + std::string("settings/config.txt"));
  ConfigSaver *configSaver = new ConfigSaver(*cfg);

  std::pair<largest_uint, largest_uint> tilesize = {ntris::tilesize.first,
                                                    ntris::tilesize.second};
  const sf::Vector3<std::size_t> extra_render(16, 16, 64);

  for (auto &tilerend: tilerends) {
    initializeTileRand(tilerend, tilesize, extra_render, prepath);
  }
  initializeTileRand(menuTilerend, tilesize, extra_render, prepath);

  ntris::shader = configSaver->getShader();

  ntris::four_thirds = configSaver->getFourThirds();
  ntris::fullscreen = configSaver->getFullscreen();
  fullscreen.store(ntris::fullscreen);
  ntris::window_scale = configSaver->setWindowScale(menuTilerend.getWidthPixels(),
                                                    menuTilerend.getHeightPixels(),
                                                    ntris::four_thirds);
  return configSaver;
}

void Window::createWindow(ConfigSaver &config_saver) {
  // Create and setup Window
  std::size_t window_width = menuTilerend.getWidthPixels() * ntris::window_scale.x;
  std::size_t window_height =
          menuTilerend.getHeightPixels() * ntris::window_scale.y;

  window.create(
          sf::VideoMode(menuTilerend.getWidthPixels(), menuTilerend.getHeightPixels()),
          "NESTRIS");

  ntris::window_position = config_saver.setWindowPosition(window_width, window_height);

  window.setPosition(ntris::window_position);

  window.setSize(sf::Vector2u(window_width, window_height));

  window_view = window.getView();
  //tilerend.load("texturesprite/sprites.txtupdated");

  window.setActive(false);
  fullscreen.store(false);

  ntris::default_music_theme = config_saver.getDefaultMusicTheme();
}

// TODO: warning! Method is supposed to work with 2 players only.
void Window::initializeViews() {
  // Get the size of the window in pixels

  views[1] = sf::View(sf::FloatRect(cor[0], cor[1], cor[2], cor[3]));
  views[0] = sf::View(sf::FloatRect(cor[0], cor[1], cor[2], cor[3]));

  // views[0].setViewport(sf::FloatRect(0.f, 0.f, 0.5f, 1.f)); // for vertical

  if (vertical.load()) {
    // If the flag is true, split the window vertically
    views[1].setViewport(sf::FloatRect(0, 0, 1, 0.5f));
    views[0].setViewport(sf::FloatRect(0, 0.5f, 1, 0.5f));
  } else {
    // If the flag is false, split the window horizontally
    views[1].setViewport(sf::FloatRect(0, 0, 0.5f, 1));
    views[0].setViewport(sf::FloatRect(0.5f, 0, 0.5f, 1));
  }
}

void Window::initializeView() {
    window_view.setViewport(sf::FloatRect(0, 0, 1, 1));
    window.setView(window_view);
}

void Window::startEventLoop() {
  std::thread render_thread(&Window::render, this);

  sf::Event event;
  bool is_mouse_hidden = false;

  while (isWindowOpen() && !close_window.load()) {

    if (window.waitEvent(event)) {
      if (!is_mouse_hidden && hide_cursor.load()) {
        window.setMouseCursorVisible(false);
        is_mouse_hidden = true;
      } else if (is_mouse_hidden && event.type == sf::Event::MouseMoved) {
        window.setMouseCursorVisible(true);
        is_mouse_hidden = false;
      }
      event_queue.push(event);
    }
    if (toggle_fullscreen.load()) {

      toggle_fullscreen.store(false);
      toggle_fullscreen_func();
    } else if (game_start.load()) {
      setWindowedMode();
      initializeViews();
      game_start.store(false);
    } else if (game_stop.load()) {
      setWindowedMode();
      initializeView();
      game_stop.store(false);
    }
  }
  if (render_thread.joinable())
    render_thread.join();
  window.setActive(true);
  window.close();
}

		