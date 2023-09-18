#include"MatrixContainer.hpp"
#include"ntris.hpp"
#include<vector>
#include<utility>
#include<cstdio>
#include <iostream>


MatrixContainer::MatrixContainer(TileContainer *_tilecont,
                                 const nes_ushort &_frameappearance)
  : Renderer(_tilecont, _frameappearance) {
  for (int i = 0; i < 9; ++i) {
    for (int j = 10; j < 22; ++j) {
      //matrix(i,j)=1; //staring wtih a tetris
    }
  }
}

nes_uchar MatrixContainer::getBlock(const nes_uchar &x, const nes_uchar &y) {
  return matrix(x, y);
}

bool MatrixContainer::collision(const Piece &_piece) const {
  bool collision = false;
  std::vector<std::pair<nes_uchar, nes_uchar> > piecepositions = _piece.getPos();
  for (std::vector<std::pair<nes_uchar, nes_uchar> >::size_type i = 0;
       i < piecepositions.size(); ++i) {
    std::size_t _xx = piecepositions[i].first;
    std::size_t _yy = piecepositions[i].second;
    if (!PFMatrix::inbounds(_xx, _yy)) {
      collision = true;
      break;
    }
    if (matrix(_xx, _yy)) {
      collision = true;
      break;
    }
  }
  return collision;
}

nes_uchar MatrixContainer::lockpiece(const Piece &_piece,
                                     const nes_ushort &_framecounter) {
  std::vector<std::pair<nes_uchar, nes_uchar> > piecepositions = _piece.getPos();
  for (std::vector<std::pair<nes_uchar, nes_uchar> >::size_type i = 0;
       i < piecepositions.size(); ++i) {
    std::size_t _xx = piecepositions[i].first;
    std::size_t _yy = piecepositions[i].second;
    matrix(_xx, _yy) = _piece.get_color();
  }
  char _tempclearedlines = clearlines();
  return _tempclearedlines;
}


nes_uchar MatrixContainer::clearlines() {
  bool whichlines[22];
  //TODO if row 2 is cleared also row 21 is cleared (bug)
  std::size_t lowestline = 0;
  linescleared = 0;
  for (std::size_t row = 0; row < 22; ++row) {
    bool clearedline = true;
    for (std::size_t column = 0; column < 10; ++column) {
      if (matrix(column, row) == 0) {
        clearedline = false;
        column = 10;
      }
    }
    whichlines[row] = clearedline;
    if (whichlines[row]) {
      lowestline = row;
      linesclearedarray[linescleared++] = row;
    }
  }
  newmatrix = matrix;
  for (std::size_t i = lowestline, rowcounter = lowestline; i >= 1 &&
                                                            rowcounter >=
                                                            1; --i, --rowcounter) { //20 because ntris::playfield isn't saved over 20
    while (rowcounter >= 1 && whichlines[rowcounter]) --rowcounter;
    for (std::size_t j = 0; j < 10; ++j) {
      newmatrix(j, i) = newmatrix(j, rowcounter);
    }
  }
  return linescleared;
}

void MatrixContainer::reset() {
  for (std::size_t i = 0; i < 10; ++i) {
    for (std::size_t j = 0; j < 22; ++j) {
      matrix(i, j) = 0;
    }
  }
}

// ---------------------------- New version methods ------------------------- //

std::vector<std::pair<nes_uchar, nes_uchar>>
MatrixContainer::getFlyCubes() const {

  std::vector<std::pair<nes_uchar, nes_uchar>> rez;

  // Iterate over the matrix

  for (int x = 0; x < kMatrixContainerWidth - 2; x++) {
    for (int y = 0; y < kMatrixContainerHeight; y++) {

      // Check for the 3 cubes in a row
      if (matrix(x, y + 2) && matrix(x + 1, y + 2)
          && matrix(x + 2, y + 2)) {

        // Check for free space of at least 2 cubes above the side cubes
        if (y + 2 < kMatrixContainerHeight
            && !matrix(x, y + 1) && !matrix(x + 2, y + 1)
            && !matrix(x, y) && !matrix(x + 2, y)) {

          // Check for exactly one cube above the middle cube and free space above it
          if (matrix(x + 1, y + 1) && !matrix(x + 1, y)) {
            rez.emplace_back(x + 1, y + 1);
          }
        }
      }
    }
  }

  return rez;
}

void MatrixContainer::render(const nes_uchar &_level, nes_uchar blinkBorder, ntris::GameStatus& _gamestatus) {
  ++framesBlinkCounter;

  if(blinkBorder <= framesBlinkCounter) {
    framesBlinkCounter = 0;
    blinkHiddenState = !blinkHiddenState;
  }

  if (hidecounter > 0) {
    --hidecounter;
    return;
  }
  if (_gamestatus.lineclearframecounter > 0) {
    if (ntris::getframemod4() == 0) {
      for (std::size_t i = 0; i < linescleared; ++i) {
        std::size_t x = _gamestatus.lineclearframecounter - 1;
        std::size_t y = linesclearedarray[i];
        matrix(x, y) = 0;

        x = 10 - _gamestatus.lineclearframecounter;
        matrix(x, y) = 0;
      }
    }
  } else if (_gamestatus.updatingmatrix > 0) {
    std::size_t update_iter = 5 - _gamestatus.updatingmatrix;
    for (std::size_t y = 0; y < 4; ++y) {
      for (std::size_t x = 0; x < 10; ++x) {
        matrix(x, y + update_iter * 4 + 2) = newmatrix(x,
                                                       y + update_iter * 4 + 2);
      }
    }
    setBlinkCubes(getFlyCubes());
  }
  for (nes_uchar x = 0; x < kMatrixContainerWidth; ++x) {
    for (nes_uchar y = 2; y < 22; ++y) {
      // New lines of code:
      // First check if the current cube is in the list of cubes to blink.
      bool shouldBlink = false;
      for (const auto& cube : cubesToBlink) {
        if (cube.first == x && cube.second == y) {
          shouldBlink = true;
          break;
        }
      }
      // Then, if it's time to blink and this cube should blink, set it to a different tile type.
      // Otherwise, draw it normally.
      if (shouldBlink && blinkHiddenState) {
        tilecont->at(ntris::playfieldx + x, ntris::playfieldy + y - 2) = tiletype(_level, 0);  // assuming 0 is the "blink" tile type
      } else {
        tilecont->at(ntris::playfieldx + x, ntris::playfieldy + y - 2) = tiletype(_level, matrix(x, y));
      }
    }
  }
}

void MatrixContainer::setBlinkCubes(
  std::vector<std::pair<nes_uchar, nes_uchar>> &&newBlink) {

  std::cout << "setBlinkCubes! " << newBlink.size() <<  std::endl;
   cubesToBlink = std::move(newBlink);
}

void MatrixContainer::clearCubes(const std::vector<std::pair<nes_uchar, nes_uchar>> &cubes) {
  for (auto& cube : cubes) {
    matrix.matrix[cube.first][cube.second] = 0;
  }
}



