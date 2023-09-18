#include"ConsoleManager.hpp"
#include<iostream>
#include<Window.hpp>
#include"TextFormatter.hpp"
#include"Log.hpp"
#include"OptionsMenu.hpp"

void ConsoleManager::open_info_window() {
	if (!info_window.isOpen()) {
		info_window.create(sf::VideoMode((std::size_t) (ntris::ntsc_screen_width * ntris::window_scale.x),(std::size_t) (ntris::ntsc_screen_height * ntris::window_scale.y)), "Information");
		if (menu_stack.empty()) menu_stack.push(MENU::HOME);
	}
}

void ConsoleManager::close_info_window() {
	if (info_window.isOpen()) info_window.close();
}

void ConsoleManager::toggle_info_window()
{
	if (info_window.isOpen()) close_info_window();
	else open_info_window();
}

bool ConsoleManager::is_window_open() {
	return info_window.isOpen();
}

void ConsoleManager::init_unlocked_tree_map() {
	text_to_menu_map["HOME"] = MENU::HOME;
	text_to_menu_map["SETTINGS"] = MENU::SETTINGS;

	//MENU::HOME
	std::unordered_map<MENU, bool> unlocked_map;
	unlocked_map[MENU::SETTINGS] = true;
	unlocked_tree_map[MENU::HOME] = unlocked_map;

}

std::string getFontPathFromConfigFile(const std::string& configFilePath) {
  std::ifstream configFile(configFilePath);

  if (!configFile.is_open()) {
    std::cerr << "Could not open config file: " << configFilePath << std::endl;
    return "";
  }

  std::string line;
  while (std::getline(configFile, line)) {
    // Split line on '=' character
    std::size_t pos = line.find('=');
    if (pos != std::string::npos) {
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1);

      if (key == "font_path") {
        return value;
      }
    }
  }

  std::cerr << "font_path not found in config file: " << configFilePath << std::endl;
  return "";
}

void ConsoleManager::init()
{
  // std::string font_path = getFontPathFromConfigFile("config.txt");

  std::string font_path = ntris::prepath / "Roboto.ttf";
  //info_window_font.loadFromFile(font_path);

	init_unlocked_tree_map();
	//info_window_font.loadFromFile("Roboto.ttf");
  info_window_font.loadFromFile(font_path);
	//open_info_window();
}

bool ConsoleManager::refresh(bool always_print) {
	static nes_uchar counter = 0;
	static std::string text_test = "";
	if (info_window.isOpen()) {
		handle_menu(current_menu);
		info_window.clear();
		sf::Vector2f pos = render_menu(current_menu);
		pos.x += info_window.getSize().x / 2.;
		renderHOME(pos);
		HomeMenu test_menu(info_window_font);
		test_menu.render(info_window, 30);
		info_window.display();
	}

	return info_window.isOpen();
}

void ConsoleManager::handle_menu(MENU const& menu)
{
	sf::Event event;

	switch (menu) {
	case MENU::HOME:

		while (info_window.pollEvent(event))
		{
			bool handled = basic_handler(event);
			if (!handled) handleHOME(event);
		}

	break;

	}
}

void ConsoleManager::handleHOME(sf::Event const& event)
{
	switch (event.type) {
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::F1:
			if (detecting_text_entered && !detecting_hotkey_input) {
				toggle_info_window();
				if (is_window_open())
					info_window.requestFocus();
			}
		break;
		}
	break;
	}
}

bool ConsoleManager::basic_handler(sf::Event const& event) {
	bool handled = false;
	switch (event.type) {
	case sf::Event::Closed:
		close_info_window();
		handled = true;
		break;
	case sf::Event::TextEntered:
		if (info_window.hasFocus()&&detecting_text_entered&&!detecting_hotkey_input) {
			// Handle ASCII characters only
			if (event.text.unicode < 128)
			{
				if (event.text.unicode == 8 && text_entered.length() > 0) {
					text_entered.pop_back();
					handled = true;
				}
				else if (26 < event.text.unicode && text_entered.length()<ntris::max_string_length) {
					text_entered += static_cast<char>(event.text.unicode);
					handled = true;
				}
				std::cout << text_entered << ntris::newline;
			}
		}
	break;
	case sf::Event::KeyPressed:
		if (detecting_text_entered && !detecting_hotkey_input) {
			switch (event.key.code) {
			case sf::Keyboard::F1:
				toggle_info_window();
				if (is_window_open())
					info_window.requestFocus();
				handled = true;
			break;
			case sf::Keyboard::Enter:
				bool transitioned_menu = is_menu_unlocked(current_menu, text_entered);
				if (transitioned_menu) text_entered = "";
				handled = true;
			break;
			}
		}
	break;
	}
	return handled;
}

sf::Vector2f ConsoleManager::render_menu(MENU const& menu) {
	TextFormatter<char> text_formatter(info_window_font);
	sf::Text t = text_formatter.getFormattedText(text_entered, getCharacterSize(), sf::Vector2f( info_window.getSize().x, info_window.getSize().y / 10 ));
	t.setPosition({ 0,0 });
	info_window.draw(t);
	return { 0, text_formatter.getLastFormattedSize().y };
}


void ConsoleManager::renderHOME(sf::Vector2f pos)
{
	TextFormatter<char> text_formatter(info_window_font);
	static nes_uchar counter = 0;

	bool reset = false;
	if (++counter >= 30) {
		counter = 0;
		reset = true;
	}
	for (auto& info : Log::log_vector) {
		std::string outputstring = info->print_complete(reset);			
		sf::Text t = text_formatter.getFormattedText(outputstring, getCharacterSize(), sf::Vector2f( info_window.getSize().x, info_window.getSize().y / 10 ));
		t.setPosition(pos);
		t.setCharacterSize(30);
		t.setFillColor(sf::Color::White);
		t.setOutlineColor(sf::Color::White);
		info_window.draw(t);
		pos.y += text_formatter.getLastFormattedSize().y;
	}
}

ConsoleManager::~ConsoleManager() {
	close_info_window();
}

float ConsoleManager::getCharacterSize() {
	return ntris::info_window_character_size * ntris::window_scale.y;// sqrt((double)ntris::window_scale.x * ntris::window_scale.y);
}

bool ConsoleManager::is_menu_unlocked(MENU const& from_menu, std::string const& to_menu) {
	if (text_to_menu_map.find(to_menu) == text_to_menu_map.end()) return false;
	const MENU MENU_to_menu = text_to_menu_map[to_menu];
	std::unordered_map<MENU, bool>& unlocked_map = unlocked_tree_map[from_menu];
	return unlocked_map[MENU_to_menu];
}