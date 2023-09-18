#ifndef PIECE_H
#define PIECE_H

#include "ntris.hpp"
#include "TileContainer.hpp"

#include <vector>
#include <utility>
#include <iostream>

class Piece {
public:

  // TODO delete:
  void print_info() const {
    std::cout << "====================Piece Information:\n";
    std::cout << "Piece Type: " << pieceTypeToString() << "\n";
    //std::cout << "Color: " << static_cast<int>(get_color()) << "\n";
    std::cout << "Gravity Level: " << static_cast<int>(gravity_level) << "\n";
    std::cout << "Counter: " << static_cast<int>(counter) << "\n";
    std::cout << "Rotation: " << static_cast<int>(rotation) << "\n";
    std::cout << "X Position: " << static_cast<int>(get_x()) << "\n";
    std::cout << "Y Position: " << static_cast<int>(get_y()) << "\n\n";
    //std::cout << "Orientation: " << calculateOrientation() << "\n";
  }


  enum class PieceType : uint8_t {
    T,  //0
    J,  //1
    Z,  //2
    O,  //3
    S,  //4
    L,  //5
    I,  //6
    C,  //7
    l,  //8
    Empty
  };

  Piece() {};

  Piece(nes_uchar _gravity);

  Piece(const std::string &name, nes_uchar _gravity);

  Piece(PieceType charpiecetype, nes_uchar color, nes_uchar _gravity);

  nes_uchar rotation, color;

  nes_uchar gravity_level = 0;
  nes_uchar counter{0};
  PieceType piecetype;

  nes_uchar get_color() const;

  std::vector<std::pair<nes_uchar, nes_uchar> > getPos() const;

  std::vector<std::pair<std::size_t, std::size_t> > nextpiecePos() const;

  static nes_schar rotationmatrix[28][4][2];
  static nes_uchar nextpiecespawn[8][2];

  // new methods
  void set_color();

  void set_gravity_level(nes_uchar _gravity) {
    if (_gravity >= 255) {
      gravity_level = 254;
    } else {
      gravity_level = _gravity;
    }
  }

  void increment_step_counter() {
    ++counter;
    //std::cout << "increment_step_counter " <<  static_cast<int>(counter) << std::endl;
  }

  void increment_x() {
    ++x;
  }

  void increment_y() {
    ++y;
    std::cout << "increment_y " << static_cast<int>(y) << std::endl;
  }

  void decrement_x() {
    --x;
  }

  void decrement_y() {
    --y;
    std::cout << "decrement_y " << static_cast<int>(y) << std::endl;
  }

  int get_x() const { return x; }

  int get_y() const { return y; }

  void set_x(nes_uchar x_new) { x = x_new; }

  void set_y(nes_uchar y_new) { y = y_new; }


  bool gravity_check() {
    return counter >= ntris::gravity[gravity_level];
  }

  // Returns 0 if more cubes are distributed vertically,
  // Returns 1 if vertically.
  int calculateOrientation() const;

  std::string pieceTypeToString() const;

protected:

private:
  nes_uchar x, y;
};


#endif // PIECE_H
