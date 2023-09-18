#include"TextWriter.hpp"
#include<string>
#include<sstream>
#include"TileContainer.hpp"
#include<SFML/System/Vector2.hpp>
#include<iomanip>

std::size_t TextWriter::char_lookup[255]={};

void TextWriter::init() {
    char_lookup[(unsigned char)' ']=87;
    constexpr std::size_t number_offset=1;
    for (std::size_t i=0; i<10; ++i) {
        TextWriter::char_lookup[(unsigned char)('0'+i)]=number_offset+i;
    }
    constexpr std::size_t letter_offset=11;
    for (std::size_t i=0; i<26; ++i) {
        char_lookup[(unsigned char)('a'+i)]=letter_offset+i;
    }
    char_lookup[(unsigned char)'-']=37;
    char_lookup[(unsigned char)',']=38;
    char_lookup[(unsigned char)'\'']=39;
    char_lookup[(unsigned char)':'] = 103; //260
    char_lookup[(unsigned char)'%'] = 83; 
    char_lookup[(unsigned char)'�'] = 620;
}
void TextWriter::write(const std::string& text, TileContainer *_tilecont, sf::Vector2u _position, nes_uchar color) {
    for (std::size_t i=0; i<text.length(); ++i) {
        const char ith_char=std::tolower(text[i]);
        if (_position.x<_tilecont->getWidth() && _position.y<_tilecont->getHeight()) {
            _tilecont->at(_position.x,_position.y) = tiletype(char_lookup[(unsigned char)ith_char], 0x0d,color,color,color);
            _position.x++;
        }
        else break;
    }
}
void TextWriter::write_hex(unsigned int _hex_number, TileContainer *_tilecont, sf::Vector2u _position, unsigned int fill_zeros, nes_uchar color) {
    std::stringstream _tmpstream;
    _tmpstream<<std::setfill('0')<<std::setw(fill_zeros)<<std::hex<<_hex_number<<std::dec;
    TextWriter::write(_tmpstream.str(),_tilecont,_position, color);
}
void TextWriter::write_int(unsigned int _int_number, TileContainer *_tilecont, sf::Vector2u _position, unsigned int fill_zeros) {
    std::stringstream _tmpstream;
    _tmpstream<<std::setfill('0')<<std::setw(fill_zeros)<<std::dec<<_int_number<<std::dec;
    TextWriter::write(_tmpstream.str(),_tilecont,_position);
}
