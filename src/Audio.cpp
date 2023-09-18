#include"Audio.hpp"

void Audio::init(const std::filesystem::path &path)
{
  sfx.setVolume(50.f);
  music_theme.setVolume(50.f);

  std::filesystem::path prepath = path / "sounds";
  if (!menu_move.loadFromFile((prepath / "12sq1.wav").string()))
    Log::update_error("Couldn't load 12sq1.wav - menu_move");

  if (!menu_select.loadFromFile((prepath / "13sq1.wav").string()))
    Log::update_error("Couldn't load 13sq1.wav - menu_select");

  if (!menu_select_short.loadFromFile((prepath / "26sq1.wav").string()))
    Log::update_error("Couldn't load 26sq1.wav - menu_select_short");

  if (!piece_move.loadFromFile((prepath / "14sq1.wav").string()))
    Log::update_error("Couldn't load 14sq1.wav - piece_move");

  if (!piece_rotate.loadFromFile((prepath / "16sq1.wav").string()))
    Log::update_error("Couldn't load 16sq1.wav - piece_rotate");

  if (!level_up.loadFromFile((prepath / "17sq1.wav").string()))
    Log::update_error("Couldn't load 17sq1.wav - level_up");

  if (!piece_drop.loadFromFile((prepath / "18sq1.wav").string()))
    Log::update_error("Couldn't load 18sq1.wav - piece_drop");

  if (!tetris.loadFromFile((prepath / "27sq1.wav").string()))
    Log::update_error("Couldn't load 27sq1.wav - tetris");

  if (!clear_lines.loadFromFile((prepath / "21sq1.wav").string()))
    Log::update_error("Couldn't load 21sq1.wav - clear_lines");

  if (!top_out.loadFromFile((prepath / "24noi.wav").string()))
    Log::update_error("Couldn't load 24noi.wav - top_out");

  for (std::size_t i = 1; i <= 3; ++i) {
    //theme music 1 to 3
    std::filesystem::path music_theme_src = prepath / ("music theme " + std::to_string(i) + ".ogg");
    if (!music_theme.openFromFile(music_theme_src.string()))
      Log::update_error("Failed to load" + music_theme_src.string() + " - Music theme " + std::to_string(i));

    //fast theme music 1 to 3, played when the stack is too high
    music_theme_src = prepath / ("music theme " + std::to_string(i) + " fast.ogg");
    if (!music_theme.openFromFile(music_theme_src.string()))
      Log::update_error("Failed to load" + music_theme_src.string() + " - Music theme " + std::to_string(i) +" fast");
  }
}

void Audio::stopMusic() {
	current_theme = 0;
	music_theme.stop();
}

void Audio::playPieceMove()
{
	if (!check_if_level_up() && !check_if_tetris()) {
		sfx.setBuffer(piece_move);
		sfx.play();
	}
}

void Audio::playPieceRotate()
{
	if (!check_if_level_up() && !check_if_tetris()) {
		sfx.setBuffer(piece_rotate);
		sfx.play();
	}
}

void Audio::playPieceDrop()
{
	if (!check_if_level_up() && !check_if_tetris()) {
		sfx.setBuffer(piece_drop);
		sfx.play();
	}
}

void Audio::playTopOut()
{
	//idk the priority on this one
	//if (!check_if_level_up() && !check_if_tetris()) {
		sfx.setBuffer(top_out);
		sfx.play();
	//}
}

void Audio::playLevelUp()
{
	level_up_active = true;
	sfx.setBuffer(level_up);
	sfx.play();
}

void Audio::playTetris()
{
	if (!check_if_level_up()) {
		sfx.setBuffer(tetris);
		sfx.play();
		tetris_active = true;
	}
}

void Audio::playClearLines()
{
	if (!check_if_level_up() && !check_if_tetris()) {
		sfx.setBuffer(clear_lines);
		sfx.play();
	}
}

void Audio::playMenuMove()
{
	sfx.setBuffer(menu_move);
	sfx.play();
}

void Audio::playMenuSelect()
{
	sfx.setBuffer(menu_move);
	sfx.play();
}

void Audio::playMenuSelectShort()
{
	sfx.setBuffer(menu_select_short);
	sfx.play();
}
bool Audio::check_if_level_up()
{
	if (level_up_active) {
		if (sfx.getStatus() == sf::SoundSource::Status::Stopped) {
			level_up_active = false;
			return false;
		}
		else if (sfx.getStatus() == sf::SoundSource::Status::Playing) {
			return true;
		}
		level_up_active = false;
		return false;
	}
	else
		return false;
}

bool Audio::check_if_tetris()
{
	if (tetris_active) {
		if (sfx.getStatus() == sf::SoundSource::Status::Stopped) {
			tetris_active = false;
			return false;
		}
		else if (sfx.getStatus() == sf::SoundSource::Status::Playing) {
			return true;
		}
		tetris_active = false;
		return false;
	}
	else
		return false;
}
void Audio::playMusic(std::size_t const& _theme_number, bool const& _fast, const std::filesystem::path &path)
{
  std::filesystem::path prepath = path / "sounds";

  if (_theme_number < 0 || _theme_number>3) return;
	if (current_theme == _theme_number && is_music_playing &&_fast==music_fast) return;
	using namespace std::string_literals;
	current_theme = _theme_number;
	music_fast = _fast;
	if (_theme_number == 0) {
		is_music_playing = false;
		stopMusic(); 
		return;
	}
	is_music_playing = true;
	if (!_fast) {
		std::string music_theme_src = prepath / ("music theme "s + std::to_string(_theme_number) + ".ogg");
    if (!music_theme.openFromFile(music_theme_src))
			Log::update_error("Failed to load"s + music_theme_src + " - Music theme "s + ntris::to_string(_theme_number));
		else {
			music_theme.setLoop(true);
			music_theme.play();
		}
	}
	else {
		std::string music_theme_src = prepath / ("music theme "s + std::to_string(_theme_number) + " fast.ogg");
    if (!music_theme.openFromFile(music_theme_src))
			Log::update_error("Failed to load"s + music_theme_src + " - Music theme "s + ntris::to_string(_theme_number) + " fast"s);
		else {
			music_theme.setLoop(true);
			music_theme.play();
		}
	}
}


void Audio::muteSFX() {
	muted_SFX = true;
	sfx.setVolume(0.f);
}

void Audio::unMuteSFX() {
	muted_SFX = false;
	sfx.setVolume(volume_SFX);
}

void Audio::muteMusic() {
	muted_music = true;
	music_theme.setVolume(0.f);
}

void Audio::unMuteMusic() {
	muted_music = false;
	music_theme.setVolume(volume_music);
}

void Audio::setVolumeSFX(float const& _volume) {
	volume_SFX = _volume;
	if (muted_SFX)
		unMuteSFX();
	sfx.setVolume(_volume);
}

void Audio::setVolumeMusic(float const& _volume) {
	volume_music= _volume;
	if (muted_music) 
		unMuteMusic();
	music_theme.setVolume(_volume);
}