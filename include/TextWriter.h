#ifndef TEXTWRITER_H
#define TEXTWRITER_H
#include"enums.h"
class TileContainer;

class TextWriter
{
public:
    static void init();
    static void write(const std::string& text, TileContainer *_tilecont, sf::Vector2u _position);
    static void write_hex(unsigned int _hex_number, TileContainer *_tilecont, sf::Vector2u _position, unsigned int fill_zeros);
    static void write_int(unsigned int _int_number, TileContainer *_tilecont, sf::Vector2u _position, unsigned int fill_zeros);
    static std::size_t char_lookup[255];
};

#endif // TEXTWRITER_H
