#include"Piece.hpp"

Piece::Piece(nes_uchar _gravity)
  :
  x(5),
  y(2),
  piecetype(PieceType::Empty),
  rotation(0) {
  set_gravity_level(_gravity);
}

Piece::Piece(const std::string &name, nes_uchar _gravity)
  : Piece(_gravity) {
  std::cout << name << std::endl;
  set_gravity_level(_gravity);
}

Piece::Piece(PieceType charpiecetype, nes_uchar color, nes_uchar _gravity) {
  piecetype = charpiecetype;
  this->color = color;
  set_gravity_level(_gravity);
}

void Piece::set_color() {
  switch (piecetype) {
    case PieceType::T:
      color = 1;
      break;
    case PieceType::J:
      color = 2;
      break;
    case PieceType::Z:
      color = 3;
      break;
    case PieceType::O:
      color = 1;
      break;
    case PieceType::S:
      color = 2;
      break;
    case PieceType::L:
      color = 3;
      break;
    case PieceType::I:
      color = 1;
      break;
    default:
      color = 0;
      break;
  }
}

nes_uchar Piece::get_color() const {
  return color;
}


std::vector<std::pair<nes_uchar, nes_uchar> > Piece::getPos() const {
  std::vector<std::pair<nes_uchar, nes_uchar> > result;
  if (piecetype == PieceType::Empty) return result;

  if (piecetype == PieceType::C) {
    result.push_back(std::make_pair(x, y));
  } else if (piecetype == PieceType::l) {
    if (rotation == 0 || rotation == 2) {
      result.push_back(std::make_pair(x, y));
      result.push_back(std::make_pair(x, y + 1));
    } else {
      result.push_back(std::make_pair(x, y));
      result.push_back(std::make_pair(x + 1, y));
    }
  } else {
    for (std::vector<std::pair<nes_uchar, nes_uchar> >::size_type i = 0;
         i < 4; ++i) {
      result.push_back(std::make_pair(
        rotationmatrix[static_cast<uint8_t>(piecetype) * 4 +
                       rotation % 4][i][0] + x,
        rotationmatrix[static_cast<uint8_t>(piecetype) * 4 +
                       rotation % 4][i][1] + y));
    }
  }
  return result;
}


std::vector<std::pair<std::size_t, std::size_t> > Piece::nextpiecePos() const {
  std::vector<std::pair<std::size_t, std::size_t> > result;
  if (piecetype == PieceType::Empty) return result;
  for (std::vector<std::pair<nes_uchar, nes_uchar> >::size_type i = 0;
       i < 4; ++i) {
    std::size_t nextpiecex =
      ntris::nextpiece_coords[static_cast<uint8_t>(piecetype)].x +
      rotationmatrix[static_cast<uint8_t>(piecetype) * 4 + rotation % 4][i][0] *
      8;
    std::size_t nextpiecey =
      ntris::nextpiece_coords[static_cast<uint8_t>(piecetype)].y +
      rotationmatrix[static_cast<uint8_t>(piecetype) * 4 + rotation % 4][i][1] *
      8;
    result.push_back(std::make_pair(nextpiecex, nextpiecey));
  }
  return result;
}

int Piece::calculateOrientation() const {
  int minX = INT_MAX;
  int maxX = INT_MIN;
  int minY = INT_MAX;
  int maxY = INT_MIN;

  auto pos = getPos();

  for (const auto& cube : pos) {
    if (cube.first < minX) minX = cube.first;
    if (cube.first > maxX) maxX = cube.first;
    if (cube.second < minY) minY = cube.second;
    if (cube.second > maxY) maxY = cube.second;
  }

  // If there are more cubes horizontally than vertically, return 1.
  // Otherwise, return 0.
  return (maxX - minX) > (maxY - minY) ? 1 : 0;
}


nes_schar Piece::rotationmatrix[28][4][2] = {
  {{-1, 0},  {0,  0},  {1,  0}, {0,  1},},  // 02: T down (spawn)
  {{0,  -1}, {-1, 0},  {0,  0}, {0,  1},},  // 03: T left
  {{-1, 0},  {0,  0},  {1,  0}, {0,  -1},},  // 00: T up
  {{0,  -1}, {0,  0},  {1,  0}, {0,  1},},  // 01: T right

  {{-1, 0},  {0,  0},  {1,  0}, {1,  1},},  // 07: J down (spawn)
  {{0,  -1}, {0,  0},  {-1, 1}, {0,  1},},  // 04: J left
  {{-1, -1}, {-1, 0},  {0,  0}, {1,  0},},  // 05: J up
  {{0,  -1}, {1,  -1}, {0,  0}, {0,  1},},  // 06: J right

  {{-1, 0},  {0,  0},  {0,  1}, {1,  1},},  // 08: Z horizontal (spawn)
  {{1,  -1}, {0,  0},  {1,  0}, {0,  1},},  // 09: Z vertical
  {{-1, 0},  {0,  0},  {0,  1}, {1,  1},},  // 08: Z horizontal (spawn)
  {{1,  -1}, {0,  0},  {1,  0}, {0,  1},},  // 09: Z vertical

  {{-1, 0},  {0,  0},  {-1, 1}, {0,  1},},  // 0A: O (spawn)
  {{-1, 0},  {0,  0},  {-1, 1}, {0,  1},},  // 0A: O (spawn)
  {{-1, 0},  {0,  0},  {-1, 1}, {0,  1},},  // 0A: O (spawn)
  {{-1, 0},  {0,  0},  {-1, 1}, {0,  1},},  // 0A: O (spawn)

  {{0,  0},  {1,  0},  {-1, 1}, {0,  1},},  // 0B: S horizontal (spawn)
  {{0,  -1}, {0,  0},  {1,  0}, {1,  1},},  // 0C: S vertical
  {{0,  0},  {1,  0},  {-1, 1}, {0,  1},},  // 0B: S horizontal (spawn)
  {{0,  -1}, {0,  0},  {1,  0}, {1,  1},},  // 0C: S vertical

  {{-1, 0},  {0,  0},  {1,  0}, {-1, 1},},  // 0E: L down (spawn)
  {{-1, -1}, {0,  -1}, {0,  0}, {0,  1},},  // 0F: L left
  {{1,  -1}, {-1, 0},  {0,  0}, {1,  0},},  // 10: L up
  {{0,  -1}, {0,  0},  {0,  1}, {1,  1},},  // 0D: L right

  {{-2, 0},  {-1, 0},  {0,  0}, {1,  0},},  // 12: I horizontal (spawn)
  {{0,  -2}, {0,  -1}, {0,  0}, {0,  1},},  // 11: I vertical
  {{-2, 0},  {-1, 0},  {0,  0}, {1,  0},},  // 12: I horizontal (spawn)
  {{0,  -2}, {0,  -1}, {0,  0}, {0,  1},},  // 11: I vertical
};
nes_uchar Piece::nextpiecespawn[8][2] = {
  {203, 111},
  {203, 111},
  {203, 111},
  {207, 111},
  {203, 111},
  {203, 111},
  {207, 115},

  // New entry for the 'C' piece
  {207, 115},
};

std::string Piece::pieceTypeToString() const {
  switch (piecetype) {
    case Piece::PieceType::T:
      return "T";
    case Piece::PieceType::J:
      return "J";
    case Piece::PieceType::Z:
      return "Z";
    case Piece::PieceType::O:
      return "O";
    case Piece::PieceType::S:
      return "S";
    case Piece::PieceType::L:
      return "L";
    case Piece::PieceType::I:
      return "I";
    case Piece::PieceType::C:
      return "C";
    case Piece::PieceType::l:
      return "l";
    case Piece::PieceType::Empty:
      return "Empty";
    default:
      return "Unknown";
  }
}