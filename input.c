#include "input.h"
#include "ticks.h"
#include <SDL/SDL.h>

#ifdef PSP
  #include "platform/pspspec.h"
#endif

#ifdef GP2X
  #include "platform/wiz.h"
#endif

#include "settings.h"

static struct {
  int button;
  int state;
  int time;
} button[C_NUM];

inline int getButton(int btn)
{
  return(button[btn].state);
}

inline void resetBtnTimer(int btn)
{
  button[btn].time=0;
}

inline void resetBtn(int btn)
{
  button[btn].time=0;
  button[btn].state=0;
}

void resetBtnAll()
{
  int i;
  for(i=0; i < C_NUM; i++)
  {
    resetBtn(i);
  }
}

inline int getBtnTime(int btn)
{
  return(button[btn].time);
}

int runControls()
{
  SDL_Event event;
  int i;

  //Loop through buttons to update hold-down time
  for(i=0; i < C_NUM; i++)
  {
    if( button[i].state )
    {
      button[i].time += getTicks();
    }
  }

  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {

        #if defined (GP2X) || defined (PSP)
        case SDL_JOYBUTTONDOWN:
          for(i=0; i < C_NUM; i++)
          {
            if( event.jbutton.button == button[i].button )
            {
              button[i].state=1;
              button[i].time=0;
            }
          }
          #ifdef GP2X
          switch( event.jbutton.button )
          {
           case GP2X_BUTTON_UPLEFT:
              button[C_UP].state=1;
              button[C_UP].time=0;
              button[C_LEFT].state=1;
              button[C_LEFT].time=0;
            break;
            case GP2X_BUTTON_UPRIGHT:
              button[C_UP].state=1;
              button[C_UP].time=0;
              button[C_RIGHT].state=1;
              button[C_RIGHT].time=0;
            break;
            case GP2X_BUTTON_DOWNLEFT:
              button[C_DOWN].state=1;
              button[C_DOWN].time=0;
              button[C_LEFT].state=1;
              button[C_LEFT].time=0;
            break;
            case GP2X_BUTTON_DOWNRIGHT:
              button[C_DOWN].state=1;
              button[C_DOWN].time=0;
              button[C_RIGHT].state=1;
              button[C_RIGHT].time=0;
            break;
            case GP2X_BUTTON_VOLDOWN:
              //WIZ_AdjustVolume( VOLUME_DOWN );
              applySettings();
            break;
            case GP2X_BUTTON_VOLUP:
              //WIZ_AdjustVolume( VOLUME_UP );
            applySettings();
            break;
          }
        break;
        #endif
        case SDL_JOYBUTTONUP:
          for(i=0; i < C_NUM; i++)
          {
            if( event.jbutton.button == button[i].button )
            {
              button[i].state=0;
              button[i].time=0;
            }
          }
          #ifdef GP2X
          switch( event.jbutton.button )
          {
            case GP2X_BUTTON_UPLEFT:
              button[C_UP].state=0;
              button[C_UP].time=0;
              button[C_LEFT].state=0;
              button[C_LEFT].time=0;
            break;
            case GP2X_BUTTON_UPRIGHT:
              button[C_UP].state=0;
              button[C_UP].time=0;
              button[C_RIGHT].state=0;
              button[C_RIGHT].time=0;
            break;
            case GP2X_BUTTON_DOWNLEFT:
              button[C_DOWN].state=0;
              button[C_DOWN].time=0;
              button[C_LEFT].state=0;
              button[C_LEFT].time=0;
            break;
            case GP2X_BUTTON_DOWNRIGHT:
              button[C_DOWN].state=0;
              button[C_DOWN].time=0;
              button[C_RIGHT].state=0;
              button[C_RIGHT].time=0;
            break;
          }
        break;
        #endif

        //Keyboard
        #else
        case SDL_KEYDOWN:
          for(i=0; i < C_NUM; i++)
          {
            if( event.key.keysym.sym == button[i].button )
            {
              button[i].state=1;
              button[i].time=0;
            }
          }
        break;
        case SDL_KEYUP:
          for(i=0; i < C_NUM; i++)
          {
            if( event.key.keysym.sym == button[i].button )
            {
              button[i].state=0;
              button[i].time=0;
            }
          }
        break;
        #endif

        case SDL_QUIT:
          return(1);
        break;
    }
  }

  return(0);
}

void initControls()
{

  #ifdef GP2X
    button[C_UP].button = GP2X_BUTTON_UP;
    button[C_DOWN].button = GP2X_BUTTON_DOWN;
    button[C_LEFT].button =  GP2X_BUTTON_LEFT;
    button[C_RIGHT].button = GP2X_BUTTON_RIGHT;
    button[C_BTNY].button = GP2X_BUTTON_Y;
    button[C_BTNX].button = GP2X_BUTTON_X;
    button[C_BTNA].button = GP2X_BUTTON_A;
    button[C_BTNB].button = GP2X_BUTTON_B;
    button[C_SHOULDERA].button = GP2X_BUTTON_L;
    button[C_SHOULDERB].button = GP2X_BUTTON_R;
    button[C_BTNMENU].button = GP2X_BUTTON_START;
    button[C_BTNSELECT].button = GP2X_BUTTON_SELECT;
    button[C_BTNVOLUP].button = GP2X_BUTTON_VOLUP;
    button[C_BTNVOLDOWN].button = GP2X_BUTTON_VOLDOWN;
  #elif defined (PSP)
    button[C_UP].button = PSP_BUTTON_UP;
    button[C_DOWN].button = PSP_BUTTON_DOWN;
    button[C_LEFT].button =  PSP_BUTTON_LEFT;
    button[C_RIGHT].button = PSP_BUTTON_RIGHT;
    button[C_BTNY].button = PSP_BUTTON_TRIANGLE;
    button[C_BTNX].button = PSP_BUTTON_SQUARE;
    button[C_BTNA].button = PSP_BUTTON_CIRCLE;
    button[C_BTNB].button = PSP_BUTTON_CROSS;
    button[C_SHOULDERA].button = PSP_BUTTON_LTRIGGER;
    button[C_SHOULDERB].button = PSP_BUTTON_RTRIGGER;
    button[C_BTNMENU].button = PSP_BUTTON_START;
    button[C_BTNSELECT].button = PSP_BUTTON_SELECT;
    button[C_BTNVOLUP].button = GP2X_BUTTON_VOLUP;
    button[C_BTNVOLDOWN].button = GP2X_BUTTON_VOLDOWN;
  #elif defined (MAME_CTRL)
    #warning "Compiling with MAME keys"
    button[C_UP].button = SDLK_UP;
    button[C_DOWN].button = SDLK_DOWN;
    button[C_LEFT].button = SDLK_LEFT;
    button[C_RIGHT].button = SDLK_RIGHT;
    button[C_BTNX].button = SDLK_SPACE; //b4
    button[C_BTNY].button = SDLK_LSHIFT;//b3
    button[C_BTNA].button = SDLK_LALT;  //b2
    button[C_BTNB].button = SDLK_LCTRL; //b1
    button[C_SHOULDERA].button = SDLK_8;
    button[C_SHOULDERB].button = SDLK_9;
    button[C_BTNMENU].button = SDLK_1;  //p1start
    button[C_BTNSELECT].button = SDLK_2;//p2start
    button[C_BTNVOLUP].button = SDLK_a;
    button[C_BTNVOLDOWN].button = SDLK_s;
  #else
    button[C_UP].button = SDLK_UP;
    button[C_DOWN].button = SDLK_DOWN;
    button[C_LEFT].button = SDLK_LEFT;
    button[C_RIGHT].button = SDLK_RIGHT;
    button[C_BTNY].button = SDLK_z;
    button[C_BTNX].button = SDLK_x;
    button[C_BTNA].button = SDLK_LALT;
    button[C_BTNB].button = SDLK_LCTRL;
    button[C_SHOULDERA].button = SDLK_1;
    button[C_SHOULDERB].button = SDLK_2;
    button[C_BTNMENU].button = SDLK_ESCAPE;
    button[C_BTNSELECT].button = SDLK_SPACE;
    button[C_BTNVOLUP].button = SDLK_F1;
    button[C_BTNVOLDOWN].button = SDLK_F2;
  #endif
}