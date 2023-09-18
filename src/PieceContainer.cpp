#include"PieceContainer.hpp"

#include <iostream>

PieceContainer::PieceContainer(TileContainer *_tilecont,
                               const nes_ushort &_frameappearance,
                               const LevelLines &_levelLines)
        : Renderer(_tilecont, _frameappearance),
          downinterrupted(false),
          das(0),
          spawncount(0),
          hidenextpiece(false) {
  spawnPiece(_levelLines);
  spawnPiece(_levelLines);
  hidecounter = sleepcounter = 0;
}

bool PieceContainer::collision(const PFMatrix &_pfmatrix, const Piece &_piece) {
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
    if (_pfmatrix(_xx, _yy)) {
      collision = true;
      break;
    }
  }
  return collision;
}

//TODO change nes_uchar in slower game modes
void PieceContainer::inputManager(const ActiveInputs &_inputs,
                                  const PFMatrix &pfmatrix,
                                  Audio &_audio,
                                  const ntris::GameStatus *gamestatus,
                                  const LevelLines& levelLines) {

  auto _gravity = ntris::gravity[levelLines.get_real_level()];
  dropped_event = false;
  punch = false;
  bool piece_changed = false;
  //96 frames of ARE when the game starts
  if (init_delay > 0) {
    init_delay--; //TODO before or after, frame discrepancy?
  }
  //Select hides the next piece
  if (_inputs.getPress(ntris::Select)) hidenextpiece = !hidenextpiece;
  //if sleepcounter>0 the piececontainer won't work for sleepcounter frames
  if (sleepcounter > 0) {
    --sleepcounter;
    return;
  }
  //if the lines are being cleared, or the matrix is being shifted down, or ARE>0, the piececontainer sleeps
  if (gamestatus->lineclearframecounter > 0 || gamestatus->updatingmatrix > 0 ||
      gamestatus->ARE > 0)
    return; //TODO 1 frame error? updating>1?

  currentpiece.increment_step_counter();

  //MOVE
  if (!_inputs.getHold(ntris::Down)) {
    //resets how many frames down has been held
    holddowncounter = holddownpoints = 0;
  }
  Piece temppiece = currentpiece;
  if (_inputs.getPress(ntris::Down)) {
    //holding down stops working if left or right are pressed
    downinterrupted = false;
    //forces the piece to fall down during the beginning of the game
    init_delay = 0;
  }
  if (_inputs.getHold(ntris::Down)) {
    //redudant?
    init_delay = 0;
    //interrupts holding down
    if (_inputs.getPress(ntris::Right) || _inputs.getPress(ntris::Left) ||
        _inputs.getHold(ntris::Right) || _inputs.getHold(ntris::Left))
      downinterrupted = true;
  } else {
    if (_inputs.getPress(ntris::Right)) {
      piece_changed = true;
      das = 0;
      temppiece.increment_x();
    } else if (_inputs.getPress(ntris::Left)) {
      piece_changed = true;
      das = 0;
      temppiece.decrement_x();
    } else if (_inputs.getHold(ntris::Right)) {
      ++das;
      if (das >= 16) {
        piece_changed = true;
        temppiece.increment_x();
        das = 10;
      }
    } else if (_inputs.getHold(ntris::Left)) {
      ++das;
      if (das >= 16) {
        piece_changed = true;
        temppiece.decrement_x();
        das = 10;
      }
    }
  }
  if (collision(pfmatrix, temppiece)) {
    das = 16;
  } else if (checkCubesCollision(temppiece)) {
    handleCubesCollision(temppiece, levelLines);
  } else {
    if (piece_changed)
      _audio.playPieceMove(); //HAS TO BE PLAYED BEFORE PLAYPIECEROTATE BECAUSE IF A PIECE IS ROTATED AND MOVE IN THE SAME FRAME THIS SOUND DOESNT PLAY
    currentpiece = temppiece;
  }
  //ROTATE
  temppiece = currentpiece;
  piece_changed = false;
  if (_inputs.getPress(ntris::A)) {
    piece_changed = true;
    temppiece.rotation = (temppiece.rotation + 1) % 4;
  } else if (_inputs.getPress(ntris::B)) {
    piece_changed = true;
    temppiece.rotation = (temppiece.rotation - 1) % 4;
  }
  if (!collision(pfmatrix, temppiece)) {
    if (piece_changed)
      _audio.playPieceRotate(); //HAS TO BE PLAYED AFTER PLAYPIECEMOVE BECAUSE IF A PIECE IS ROTATED AND MOVE IN THE SAME FRAME THIS SOUND TAKES PRIORITY
    currentpiece = temppiece;
  }

  //if not holding down or have been holding down
  //DROP
  if (_inputs.getHold(ntris::Down) && !downinterrupted) {
    ++holddowncounter;
    if (holddowncounter >= 3) {
      ++holddownpoints;
      holddowncounter -= 2;
      down_event(pfmatrix, levelLines);
    }
  }

  if (init_delay == 0) {
    // Checking that enough frames have passed for a figure to go down.
    if (currentpiece.gravity_check()) {
      down_event(pfmatrix, levelLines);
    }
  }

  if (!flyCubes.empty()) {
    // Check if it's time to move the flying cubes
    for (size_t i = 0; i < flyCubes.size(); ++i) {
      flyCubes[i].increment_step_counter();
      if (flyCubes[i].gravity_check()) {
        // Moving the cube up, decrement the y value
        flyCubes[i].decrement_y();
        if (collision(currentpiece, flyCubes[i])) {
          cube_collision_event = true;
          collided_cubes.emplace_back(i);
          std::cout << "COLLISION" << std::endl;
        }
        flyCubes[i].counter = 0;
      }
    }
  }

  if (cube_collision_event) {
    handleCubesCollision(currentpiece, levelLines);
  }

  //FLY
  if (_inputs.getPress(ntris::Space)) {
    this->launch_event = true;
  }

  if (dropped_event)
    _audio.playPieceDrop();

  // Remove out-of-bounds cubes
  for (auto it = flyCubes.begin(); it != flyCubes.end();) {
    if (isOutOfBounds(*it)) {
      it = flyCubes.erase(it);
    } else {
      ++it;
    }
  }

  // std::cout << "Cubes number " << flyCubes.size() << std::endl;
}

const Piece &PieceContainer::getPiece() const {
  return currentpiece;
}

//TODO change both nes_ushort and nes_uchar with bigger game modes
void PieceContainer::rendernextpiece(const nes_uchar &_level) {
  if (!hidenextpiece) {
    std::vector<std::pair<std::size_t, std::size_t> > nextpiecepos = nextpiece.nextpiecePos();
    for (const auto &i: nextpiecepos) {
      tilecont->renderExtra(i.first, i.second, tiletype(_level,
                                                        nextpiece.get_color()),
                            0.6);
    }
  }
}

//TODO change both nes_ushort and nes_uchar with bigger game modes
void PieceContainer::render(const nes_ushort &_framecounter,
                            const nes_uchar &_level,
                            const ntris::GameStatus *gamestatus) { //TODO the first piece renders a little bit late
  if (hidecounter > 0) {
    --hidecounter;
    return;
  }

  for (const auto &cube: flyCubes) {
    std::vector<std::pair<nes_uchar, nes_uchar>> cubePositions = cube.getPos();
    for (std::vector<std::pair<nes_uchar, nes_uchar> >::size_type i = 0;
         i < cubePositions.size(); ++i) {

      std::size_t _xx = cubePositions[i].first;
      std::size_t _yy = cubePositions[i].second;
      if (PFMatrix::visible(_xx, _yy)) {
        tilecont->at(ntris::playfieldx + _xx,
                     ntris::playfieldy + _yy - 2) = tiletype(_level,
                                                             cube.get_color());
      }
    }
  }

  rendernextpiece(_level);
  if (gamestatus->lineclearframecounter > 0 || gamestatus->updatingmatrix > 0 ||
      gamestatus->ARE > 0)
    return;
  else if (true) {
    std::vector<std::pair<nes_uchar, nes_uchar> > piecepositions = currentpiece.getPos();
    for (std::vector<std::pair<nes_uchar, nes_uchar> >::size_type i = 0;
         i < piecepositions.size(); ++i) {
      std::size_t _xx = piecepositions[i].first;
      std::size_t _yy = piecepositions[i].second;

      if (PFMatrix::visible(_xx, _yy)) {
        tilecont->at(ntris::playfieldx + _xx,
                     ntris::playfieldy + _yy - 2) = tiletype(_level,
                                                             currentpiece.get_color());
      }
    }
  }
}

//TODO can keep unsigned char, unless we decide to add many pieces
void PieceContainer::spawnPiece(const LevelLines &levelLines) {
  //holddownpoints = 0; //in RenderPlayField for now
  spawned_event = true;
  nextpiece.print_info();
  currentpiece = nextpiece;

  if (current_drought <= 254) {
    if (currentpiece.piecetype != Piece::PieceType::Empty &&
        currentpiece.piecetype != Piece::PieceType::I) {
      current_drought++;
    } else if (currentpiece.piecetype == Piece::PieceType::I) {
      current_drought = 0;
    }
  }
  if (current_drought > max_drought)
    max_drought = current_drought;

  nes_uchar spawnID = spawn_table[static_cast<uint8_t>(nextpiece.piecetype)]; //creates a piece next to nextpiece
  ++spawncount;
  nes_uchar index = random::prng() >> 8;
  index += spawncount;
  index &= 7;
  nes_uchar newSpawnID;
  if (index != 7) {
    newSpawnID = spawn_table[index];
    if (newSpawnID == spawnID) {
      random::prng();
      index = random::prng() >> 8;
      index &= 7;
      index += spawnID;
      index %= 7;
      newSpawnID = spawn_table[index];
    }
  } else {
    random::prng();
    index = random::prng() >> 8;
    index &= 7;
    index += spawnID;
    index %= 7;
    newSpawnID = spawn_table[index];
  }
  spawnID = newSpawnID;
  nes_uchar realID = 0;
  for (std::size_t i = 0; i < 7; ++i) {
    if (spawn_table[i] == spawnID) {
      realID = i;
      break;
    }
  }
  nextpiece.piecetype = static_cast<Piece::PieceType>(realID);
  nextpiece.set_color();
  nextpiece.set_gravity_level(levelLines.get_real_level());
  currentpiece.counter = holddowncounter = 0;
}

bool PieceContainer::gameOver(const PFMatrix &pfmatrix) {
  if (collision(pfmatrix, currentpiece))return true;
  return false;
}

void PieceContainer::lockpiece(nes_uchar gravity_level) {
  //printf("Lockpiece\n");
  //spawnPiece();  //do it later
  currentpiece = Piece("lockpiece -> currentpiece", gravity_level);
  downinterrupted = true; //TODO where to put this

  printPieceTypes();
}

nes_uchar PieceContainer::getDrought() const {
  return max_drought;
}

//TODO change in bigger game modes
nes_uchar PieceContainer::spawn_table[7] = {0x02, 0x07, 0x08, 0x0A, 0x0B, 0x0E,
                                            0x12};

// ----------------------------- New Version ---------------------------------//

void PieceContainer::addPiece(const Piece &newPiece) {
  flyCubes.push_back(newPiece);
}

bool
PieceContainer::collision(const Piece &_piece1, const Piece &_piece2) const {
  bool collision = false;

  // Get the positions of the blocks in each piece
  std::vector<std::pair<nes_uchar, nes_uchar> > piece1Positions = _piece1.getPos();
  std::vector<std::pair<nes_uchar, nes_uchar> > piece2Positions = _piece2.getPos();

  // Check each block in the first piece against each block in the second piece
  for (const auto &pos1: piece1Positions) {
    for (const auto &pos2: piece2Positions) {
      // If the positions match, there is a collision
      if (pos1.first == pos2.first && pos1.second == pos2.second) {
        collision = true;
        break;
      }
    }

    // If a collision was found, no need to check further
    if (collision) {
      break;
    }
  }

  return collision;
}

std::pair<Piece, PieceContainer::CollisionAction>
PieceContainer::on_collide(const Piece &current_piece,
                           const std::vector<size_t> &collided_cubes) const {
  Piece transformed_piece(current_piece.gravity_level);
  CollisionAction action;

  if (current_piece.piecetype == Piece::PieceType::T) {
    // If tetromino is type 'T' - nothing happens and tetrominos fly their ways.
    transformed_piece = current_piece;
    action = CollisionAction::Continue;
  } else if (collided_cubes.size() == 1 &&
             current_piece.piecetype != Piece::PieceType::l) {
    // If only one cube smashes into tetromino - it turns into tetromino with new type 'l'
    transformed_piece = current_piece; // Assigning to prevent "may be uninitialized" warnings.
    transformed_piece.piecetype = Piece::PieceType::l;
    transformed_piece.rotation = current_piece.calculateOrientation();
    action = CollisionAction::Transform;
  } else {
    // If more cubes collide with the tetromino, the tetromino is demolished and new piece has to be generated
    action = CollisionAction::Demolish;
  }

  return std::make_pair(transformed_piece, action);
}

void PieceContainer::down_event(const PFMatrix &pfmatrix,
                                const LevelLines& levelLines) {

  currentpiece.counter = 0;
  Piece temppiece = currentpiece;
  temppiece.increment_y();

  if (collision(pfmatrix, temppiece)) {
    dropped_event = true;
    lastdroppedpiece = currentpiece;
  } else if (checkCubesCollision(temppiece)) {
    printPieceTypes();
    std::cout << "cubes_collision : down_event" << std::endl;

    std::cout << "Cube position: " << flyCubes[0].get_x() << ", "
              << flyCubes[0].get_y() << std::endl;
    std::cout << "Figure position: " << currentpiece.get_x() << ", "
              << currentpiece.get_y() << std::endl;

    handleCubesCollision(temppiece, levelLines);
    std::cout
            << "Number of fly cubes after handleCubesCollision in down_event: "
            << flyCubes.size() << std::endl;

    printPieceTypes();
  } else {
    currentpiece = temppiece;
  }

}

bool PieceContainer::checkCubesCollision(const Piece &current_piece) {
  for (size_t i = 0; i < flyCubes.size(); ++i) {
    if (collision(current_piece, flyCubes[i])) {
      collided_cubes.push_back(i);
    }
  }
  return !collided_cubes.empty();
}

std::pair<Piece, PieceContainer::CollisionAction>
PieceContainer::handleCubesCollision(Piece &piece, const LevelLines& levelLines) {
  // Default collision with cubes.
  if (currentpiece.piecetype != Piece::PieceType::l && currentpiece.piecetype != Piece::PieceType::T) {
    punch = true;
    punchData.piece = currentpiece;
    punchData.piece.set_gravity_level(currentpiece.gravity_level + 1);
    punchData.piece.set_x(5);
    punchData.piece.set_y(2);
    punchData.piece.rotation = 0;
  }

  // Handle the collision with each collided cube.
  auto [transformed_piece, action] = on_collide(piece, collided_cubes);

  if (action == CollisionAction::Transform ||
      action == CollisionAction::Demolish) {
    for (auto it = collided_cubes.rbegin();
         it != collided_cubes.rend(); ++it) {
      flyCubes.erase(flyCubes.begin() + *it);
    }
  }
  collided_cubes.clear();

  cube_collision_event = false;

  switch (action) {
    case CollisionAction::Continue:
      currentpiece = transformed_piece;
      // Do nothing
      break;
    case CollisionAction::Transform:
      currentpiece = transformed_piece;
      break;
    case CollisionAction::Demolish:
      // Assigning empty piece, so nothing sticks to the pfmatrix.
      currentpiece = transformed_piece;
      dropped_event = true;
      lastdroppedpiece = currentpiece;
      break;
  }
  return {transformed_piece, action};
}

bool PieceContainer::isOutOfBounds(const Piece &piece) const {
  // Get the positions of the blocks in the piece
  std::vector<std::pair<nes_uchar, nes_uchar> > piecePositions = piece.getPos();

  // Check each block in the piece
  for (const auto &pos: piecePositions) {
    // If the position is out of bounds, return true
    if (!PFMatrix::visible(pos.first, pos.second)) {
      return true;
    }
  }

  // If none of the positions are out of bounds, return false
  return false;
}

void PieceContainer::printPieceTypes() const {
  std::cout << "\n\nLast dropped piece: "
            << lastdroppedpiece.pieceTypeToString() << std::endl;
  std::cout << "Current piece: " << currentpiece.pieceTypeToString()
            << std::endl;
  std::cout << "Next piece: " << nextpiece.pieceTypeToString()
            << std::endl << std::endl;
}

void PieceContainer::addPunch(const PunchData &punchData) {
  nextpiece = punchData.piece;
}

PunchData PieceContainer::getPunch() {
  return punchData;
}
