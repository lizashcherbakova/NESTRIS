#ifndef MATRIXCONTAINER_H
#define MATRIXCONTAINER_H

#include"ntris.hpp"
#include"Renderer.hpp"
#include"PieceContainer.hpp"
#include"PFMatrix.hpp"
#include"Piece.hpp"
#include<cstdio>

const int kMatrixContainerWidth = 10;
const int kMatrixContainerHeight = 22;

class MatrixContainer : public Renderer {

public:
  MatrixContainer(TileContainer *_tilecont, const nes_ushort &_frameappearance);

  bool collision(const Piece &_piece) const;

  nes_uchar lockpiece(const Piece &_piece, const nes_ushort &_framecounter);

  nes_uchar clearlines();

  const PFMatrix &getMatrix() const { return matrix; };

  nes_uchar getBlock(const nes_uchar &x, const nes_uchar &y);

  // old version render
  // void render(const nes_uchar &_level);
  // new version render
  void render(const nes_uchar &_level, nes_uchar blinkBorder, ntris::GameStatus& _gamestatus);

  void reset();

  // New version methods.
  std::vector<std::pair<nes_uchar, nes_uchar>> getFlyCubes() const;
  void setBlinkCubes(std::vector<std::pair<nes_uchar, nes_uchar>> &&);
  void clearCubes(const std::vector<std::pair<nes_uchar, nes_uchar>> &);

protected:

private:
  nes_uchar linesclearedarray[22]{0};
  nes_uchar linescleared{0};

  nes_uchar framesBlinkCounter = 0;
  bool blinkHiddenState = false;
  std::vector<std::pair<nes_uchar, nes_uchar>> cubesToBlink;

  PFMatrix newmatrix;
  PFMatrix matrix;
};

#endif // MATRIXCONTAINER_H
