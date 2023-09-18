#include "GameplayContainer.hpp"

GameplayContainer::GameplayContainer(TileContainer* _tilecont, unsigned long long _framecounter):
matrixhandler(_tilecont, _framecounter),
scorehandler(_tilecont, _framecounter),
levellineshandler(_tilecont, _framecounter, 0, &gamestatus),
statisticshandler(_tilecont, _framecounter, 0),
piecehandler(_tilecont, _framecounter,levellineshandler)
{
}
