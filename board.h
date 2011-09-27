#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include "defs.h"
#include "cursor.h"
#include "list.h"
#include "sound.h"
#include "levels.h"
#include "teleport.h"

#define NOBRICKSLEFT -1
#define UNSOLVABLE -2

struct brick_t
{
  int type;
  int dx, dy; //destionation positions, for when moving
  int sx, sy; //source destionation, for clearing block when they moved
  int dmx; //"Delayed movement" for if moving a brick when it's between bricks.
  int moveXspeed, moveYspeed;
  int pxx, pxy; //Position in pixels
  int curLock; //Cursor locked to brick

  int dir; //Used to switch direction on movers, otherwise used to indicate that brick is just added to removelist
  int tl; //dirchange time left, used for kill timeout too

  int checked; //Have this brick been checked in this loop ? (for bricks that moved)
  int wall; //Only used by walls, index in walls-sprite array.
};
typedef struct brick_t brickType;

struct playField_t
{
  levelInfo_t* levelInfo;

  brickType* board[FIELDSIZE][FIELDSIZE]; //Matrix of pointers, initialized to 0 by loadmap

  //lol
  brickType* blocker; //Universial, invisible, quite magic blocker, for reserving space when bricks are travelling.
  listItem* movingList; //Start of the linked list of moving bricks

  listItem* removeList; //Start of the linked list of bricks that's going to die, tl counts down their lifespan
};

typedef struct playField_t playField;

void boardSetWalls(playField* pf);
int loadField(playField* pf, const char* file); //Henter et spillefelt med filnavnet, retunerer 0 ved fejl.
void freeField(playField* pf); //Frees allocated memory
void simField(playField* pf, cursorType* cur); //Does logic on the field (gravity/moving bricks)
int doRules(playField* pf); //Does gameRules, returns number of bricks destroyed, returns -1 when no more bricks left.
int moveBrick(playField* pf, int x, int y, int dirx,int diry, int block, int speed); //Move a brick within the field (if possible) in either 0:Left, 1: Right, returns 1 on success

void telePortBrick(playField* pf,telePort_t* t, cursorType* cur);



int isBrickFalling(playField* pf, brickType* b);
inline int isBrick(brickType* b);
int isMover(brickType* b);
int isOneWay(brickType* b);
int curMoveBrick(playField *pf, brickType *b, int dir);

brickType* findMoving(playField* pf, int x, int y);
brickType* brickUnderCursor(playField* pf, int x, int y);

#endif // BOARD_H_INCLUDED