/************************************************************************
 * This file is part of Wizznic.                                        *
 * Copyright 2009-2013 Jimmy Christensen <dusted@dusted.dk>             *
 * Wizznic is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * Wizznic is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Wizznic.  If not, see <http://www.gnu.org/licenses/>.     *
 ************************************************************************/

#include <stdlib.h>
#include <SDL/SDL.h>

typedef struct {
    SDL_Surface* dstSurface;
    int px,py;
    size_t minLen, maxLen;
    int curX;
    int curY;
    int curPos;
    char (*kb)[4][10];
    char* str;
    char* prompt;
    char* _buf;
} inpStrState;

//Return a state structure for use with inpGetStr and inpGetStrClear
inpStrState* inpStrCreate(SDL_Surface* dstSurface, char* promptTxt, size_t minLen, size_t maxLen, int px, int py );

void inpStrClear(inpStrState* state);

//Return FALSE until a string is present in state->str.
int inpStrGet(inpStrState* state, int menuPosX, int menuPosY, int blink);


#define INP_GET_STR_LIMITS menuMaxY=4; menuMaxX=10;