#include "credits.h"
#include "waveimg.h"
#include "pixel.h"
#include "text.h"
#include "list.h"
#include "particles.h"
#include "defs.h"

#define MSGSTATE_TITLE_SLIDING_IN 0
#define MSGSTATE_NAME_SLIDING_IN 1
#define MSGSTATE_NAME_DECREASE_WAVE 2
#define MSGSTATE_NAME_SHAKING 3
#define MSGSTATE_NEXT_MSG 4

struct msg_s {
  SDL_Surface *surfTitle;
  wavingImage_t nameWaving;
  int state;
  int stateTicks;
  SDL_Rect rTitle;
  SDL_Rect nWander;
};
typedef struct msg_s msg_t;

static msg_t* cm; //Current msg.
static int currentMsgIndex; //in list
static listItem* msgList;
static int ticksToNextPs=0;
static psysSet_t ps;
SDL_Rect r; //Used to shake the name



msg_t* initMsg(const char* strTitle, const char* strName,SDL_Surface* screen)
{
  msg_t* t = malloc(sizeof(msg_t));

  //Create surface
  t->surfTitle = SDL_CreateRGBSurface(SDL_SWSURFACE, (getCharSize(FONTSMALL)[0]*strlen(strTitle)),(getCharSize(FONTSMALL)[1]),16, screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,0xff000000);
  t->nameWaving.img = SDL_CreateRGBSurface(SDL_SWSURFACE, (getCharSize(FONTMEDIUM)[0]*strlen(strName)),(getCharSize(FONTMEDIUM)[1]),16, screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,0xff000000);
  t->nameWaving.screen=screen;
  SDL_FillRect(t->surfTitle, 0, SDL_MapRGB(t->surfTitle->format, 0,255,255));
  SDL_FillRect(t->nameWaving.img, 0, SDL_MapRGB(t->nameWaving.img->format, 0,255,255));

  //Render text to surface
  txtWrite(t->surfTitle, FONTSMALL, strTitle, 0,0);
  txtWrite(t->nameWaving.img, FONTMEDIUM, strName, 0,0);

  SDL_Surface* tempSurf;

  tempSurf=SDL_DisplayFormat(t->surfTitle);
  SDL_FreeSurface(t->surfTitle);
  SDL_SetColorKey( tempSurf, SDL_SRCCOLORKEY, SDL_MapRGB( tempSurf->format, 0, 0xFF, 0xFF ) );
  t->surfTitle=tempSurf;

  tempSurf=SDL_DisplayFormat(t->nameWaving.img);
  SDL_FreeSurface(t->nameWaving.img);
  SDL_SetColorKey( tempSurf, SDL_SRCCOLORKEY, SDL_MapRGB( tempSurf->format, 0, 0xFF, 0xFF ) );
  t->nameWaving.img=tempSurf;

  t->stateTicks=0;
  return(t);
}

void setCurrent()
{
  cm=(msg_t*)listGetItemData(msgList,currentMsgIndex);


  cm->state=MSGSTATE_TITLE_SLIDING_IN;
  cm->rTitle.w=0;
  cm->rTitle.h=0;
  cm->rTitle.x=321;
  cm->rTitle.y=HSCREENH+15;
  cm->nameWaving.x= -(cm->nameWaving.img->w);
  cm->nameWaving.y=HSCREENH+35;
  cm->nameWaving.rotations=2+(cm->nameWaving.img->w/100);
  cm->nameWaving.amount=35;
  cm->nameWaving.speed=50;

  //Setup particle system
  ps.vel=100;
  ps.life=200;
  ps.lifeVar=100;
  ps.numParticles=0;
  ps.fade=0;
  ps.gravity=0;
  ps.bounce=0;
  ps.srcRect.x=0;
  ps.srcRect.y=0;

}
void freeMsg(msg_t* msg)
{
  SDL_FreeSurface( msg->surfTitle );
  SDL_FreeSurface( msg->nameWaving.img );

  free(msg);
}

void initCredits(SDL_Surface* screen)
{
  printf("initCredits();\n");
  msgList=initList();
  listAddData(msgList, (void*)initMsg("Website","wizznic.sf.net", screen));
  listAddData(msgList, (void*)initMsg("Code/Gfx/Sfx","Jimmy Christensen", screen));
  listAddData(msgList, (void*)initMsg("Music","Sean Hawk", screen));
  listAddData(msgList, (void*)initMsg("Thx","bMan", screen));
  listAddData(msgList, (void*)initMsg("Thx","KML", screen));
  listAddData(msgList, (void*)initMsg("Thx","Neil L", screen));

  listAddData(msgList, (void*)initMsg("Greetings","GP32X.com", screen));
  listAddData(msgList, (void*)initMsg("Greetings","freegamedev.net", screen));
  listAddData(msgList, (void*)initMsg("Greetings","Qubodup", screen));

  //Set current
  currentMsgIndex=0;
  setCurrent();
  printf("credits initialized.\n");
}

void clearCredits()
{
  printf("clearCredits();\n");
  listItem* it=msgList;
  while( (it=it->next) )
  {
    cm = (msg_t*)it->data;
    freeMsg(cm);
  }
  freeList(msgList);
  clearParticles();
}


void drawTitle(SDL_Surface* screen, msg_t* m)
{
  ticksToNextPs += getTicks();

  if(ticksToNextPs > 60)
  {
    ticksToNextPs=0;
    ps.bounce=0;

    ps.x=m->rTitle.x;
    ps.y=m->rTitle.y;
    ps.srcImg=m->surfTitle;
    ps.srcRect.w=m->surfTitle->w;
    ps.srcRect.h=m->surfTitle->h;

    spawnParticleSystem( &ps );

  }

  SDL_BlitSurface( m->surfTitle, 0, screen, &m->rTitle );
}

void runCredits(SDL_Surface* screen)
{
  switch(cm->state)
  {
    case MSGSTATE_TITLE_SLIDING_IN:
      cm->rTitle.x -= 10;
      if( cm->rTitle.x <= (HSCREENW-(cm->surfTitle->w/2)) )
      {
        cm->rTitle.x=(HSCREENW-(cm->surfTitle->w/2));

        cm->stateTicks += getTicks();
        //wait 250 ms
        if(cm->stateTicks >= 250)
        {
          cm->state=MSGSTATE_NAME_SLIDING_IN;
          cm->stateTicks=0;
        }
      }
      drawTitle(screen, cm);

    break;

    case MSGSTATE_NAME_SLIDING_IN:
      //Draw title
      drawTitle(screen, cm);
      //Slide in name
      cm->nameWaving.x += 7;
      if( cm->nameWaving.x >= ( HSCREENW-cm->nameWaving.img->w/2 ) )
      {
        cm->nameWaving.x = ( HSCREENW-cm->nameWaving.img->w/2 );
        cm->state=MSGSTATE_NAME_DECREASE_WAVE;
      }
      waveImg( &cm->nameWaving );
    break;

    case MSGSTATE_NAME_DECREASE_WAVE:
      //Draw title
      SDL_BlitSurface( cm->surfTitle, 0, screen, &cm->rTitle );

      cm->stateTicks += getTicks();
      if(cm->stateTicks > 0)
      {
        cm->stateTicks=0;

        cm->nameWaving.amount--;
        if(cm->nameWaving.amount==0)
        {
          cm->stateTicks=0;
          cm->state=MSGSTATE_NAME_SHAKING;
          //Set
          r.x=cm->nameWaving.x;
          r.y=cm->nameWaving.y;
        }
      }
      waveImg( &cm->nameWaving );
    break;

    case MSGSTATE_NAME_SHAKING:
    if(cm->stateTicks<500)
        SDL_BlitSurface( cm->surfTitle, 0, screen, &cm->rTitle );

      //shake
      cm->nWander.x=((rand()%6000)-3000)/1000;
      cm->nWander.y=((rand()%4000)-2000)/1000;

      if( abs(cm->nameWaving.x - (r.x+cm->nWander.x))  > 5) cm->nWander.x *=-1;
      if( abs(cm->nameWaving.y - (r.y+cm->nWander.y))  > 3) cm->nWander.y *=-1;

      r.x += cm->nWander.x;
      r.y += cm->nWander.y;

      if(cm->stateTicks <1500)
        SDL_BlitSurface( cm->nameWaving.img, 0, screen, &r );


      cm->stateTicks += getTicks();
      if(cm->stateTicks >= 500 && cm->stateTicks-getTicks() <= 500)
      {
        ps.vel=200;
        ps.life=1000;
        ps.lifeVar=500;

        spawnParticleSystem( &ps );
        spawnParticleSystem( &ps );
        spawnParticleSystem( &ps );
        spawnParticleSystem( &ps );
      } else if(cm->stateTicks >= 1500 && cm->stateTicks-getTicks() <= 1500)
      {
        ps.vel=400;
        ps.life=1500;
        ps.lifeVar=500;
        ps.x=cm->nameWaving.x;
        ps.y=cm->nameWaving.y;
        ps.srcImg=cm->nameWaving.img;
        ps.srcRect.w=cm->nameWaving.img->w;
        ps.srcRect.h=cm->nameWaving.img->h;

        spawnParticleSystem( &ps );
        spawnParticleSystem( &ps );

      } else if(cm->stateTicks > 3000)
      {
        //Next
        cm->state=MSGSTATE_NEXT_MSG;
      }
    break;

    case MSGSTATE_NEXT_MSG:
      //Update current msg
    currentMsgIndex++;
    if(currentMsgIndex == listSize(msgList)) currentMsgIndex=0;
    setCurrent();

    break;
  };
}