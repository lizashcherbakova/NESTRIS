#ifndef PIECECONTAINER_H
#define PIECECONTAINER_H

#include"ntris.hpp"
#include"PFMatrix.hpp"
#include"Piece.hpp"
#include"Renderer.hpp"
#include"ActiveInputs.hpp"
#include"TileContainer.hpp"
#include"Audio.hpp"
#include "LevelLines.hpp"
#include"random.hpp"

#include<vector>
#include<utility>

struct PunchData {
  Piece piece;
};

class PieceContainer : public Renderer {

  enum class CollisionAction {
    Continue,
    Transform,
    Demolish
  };

public:
  PieceContainer() {};

  PieceContainer(TileContainer *_tilecont, const nes_ushort &_frameappearance, const LevelLines &levelLines);

  /*Piece tryMove(const ActiveInputs& _inputs);
  Piece tryRotate(const ActiveInputs& _inputs);
  Piece tryDrop(const ActiveInputs& _inputs, const nes_uchar& _gravity);*/
  void inputManager(const ActiveInputs &_inputs, const PFMatrix &pfmatrix,
                    Audio &_audio, const ntris::GameStatus *gamestatus,
                    const LevelLines& levelLines);

  //void doMove(const bool& _collision);
  //void doRotate(const bool& _collision);
  //void doDrop();
  void lockpiece(nes_uchar _gravity);

  void spawnPiece(const LevelLines &levelLines);

  const Piece &getPiece() const;

  void render(const nes_ushort &_framecounter, const nes_uchar &_level, const ntris::GameStatus *gamestatus);

  void rendernextpiece(const nes_uchar &_level);

  Piece lastdroppedpiece = Piece("lastdroppedpiece", 0);
  nes_uchar holddownpoints{0};

  bool gameOver(const PFMatrix &pfmatrix);

  nes_uchar getDrought() const;

  bool dropped_event = false;
  bool spawned_event = false;
  bool punch = false;

  // New version:
  // Fields that handles launch cubes logic.
  bool launch_event = false;
  bool cube_collision_event = false;
  std::vector<size_t> collided_cubes;

  // Method to add new fly cube.
  void addPiece(const Piece &newPiece);
  void addPunch(const PunchData& punchData);
  PunchData getPunch();
private:

  nes_uchar hidecountercurrentpiece{0};

  bool collision(const PFMatrix &_pfmatrix, const Piece &_piece);
  // New version: for better separation of code logic.
  void down_event(const PFMatrix &_pfmatrix, const LevelLines& levelLines);

  std::vector<std::pair<nes_uchar, nes_uchar> > lastrenderedpos;
  std::size_t spawncount = 0; //TODO check spawncount
  nes_uchar spawnpiececounter{0};

  bool downinterrupted{false};
  bool hidenextpiece{false};//TODO does select carry after new games?
  bool just_spawned{false};

  nes_uchar das{0};
  nes_uchar holddowncounter{0};
  nes_uchar init_delay{96};
  nes_uchar current_drought{0};
  nes_uchar max_drought{0};
  Piece currentpiece = Piece("currentpiece" , 0);
  Piece nextpiece  = Piece("nextpiece",0);

  // T J Z O S L I
  static nes_uchar spawn_table[7];

  // New version: New field to store fly cubes.
  std::vector<Piece> flyCubes;
  PunchData punchData;
  // New version: Collision for 2 pieces.
  bool collision(const Piece& _piece1, const Piece& _piece2) const;
  std::pair<Piece, PieceContainer::CollisionAction>
  on_collide(const Piece& current_piece, const std::vector<size_t>& collided_cubes) const;
  // Method that returns a list of cubes that collided with the current piece.
  bool checkCubesCollision(const Piece& current_piece);
  std::pair<Piece, PieceContainer::CollisionAction>
  handleCubesCollision(Piece &piece, const LevelLines& levelLines);
  bool isOutOfBounds(const Piece &piece) const;

  // temp
  void printPieceTypes() const;
};

#endif // PIECE_H
