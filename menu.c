#include "menu.h"

#include "userfiles.h"

#include "sound.h"
#include "ticks.h"
#include "input.h"

#include "states.h"
#include "game.h"
#include "levelselector.h"

#include "pack.h"
#include "strings.h"
#include "settings.h"

#include "stats.h"
#include "mbrowse.h"

#include "credits.h"

#include "waveimg.h"
#include "stats.h"

static float rot=0;

#define MENUGFXINTRO 0
#define MENUGFXHELP 1
#define MENUGFXBYE 2
#define MENUGFXPACKBOX 3
#define MENUGFXNUM 4

#ifndef DATADIR
  #define DATADIR ""
#endif

static SDL_Surface* menuBg[MENUGFXNUM];
static spriteType* helpSpr;

static int menuState=menuStateIntro;

static int menuPosX=0, menuPosY=0;
static int menuMaxX, menuMaxY;

static int dir=0;
static int countdown=0;

static int menuReturnHack=0; //Well, for returning to the right place after entering highscore.

static int x,y;

static wavingImage_t waving;

static int kbRows = 4;
static int kbCols = 10;

static char kbl[4][10] = {
  {'1','2','3','4','5','6','7','8','9','0'},
  {'q','w','e','r','t','y','u','i','o','p'},
  {'a','s','d','f','g','h','j','k','l',';'},
  {'z','x','c','v','b','n','m',' ',',','.'}
};

static char kbh[4][10] = {

  {'!','"','#','$','%','&','/','(',')','='},
  {'Q','W','E','R','T','Y','U','I','O','P'},
  {'A','S','D','F','G','H','J','K','L',':'},
  {'Z','X','C','V','B','N','M','_',',','.'}
};
static char (*kb)[4][10];
static char kbCase=0;
int getMenuState() { return(menuState); }

int initMenu(SDL_Surface* screen)
{
  kb=&kbl;
  menuBg[MENUGFXINTRO] = loadImg( DATADIR"data/menu/intro.png" );

  #ifdef GP2X
    menuBg[MENUGFXHELP] = loadImg( DATADIR"data/menu/helpwiz.png" );
  #elif defined (MAME_CTRL)
    menuBg[MENUGFXHELP] = loadImg( DATADIR"data/menu/helppc-mame.png" );
  #elif defined (PSP)
     menuBg[MENUGFXHELP] = loadImg( DATADIR"data/menu/helppsp.png" );
  #else
    menuBg[MENUGFXHELP] = loadImg( DATADIR"data/menu/helppc.png" );
  #endif

  helpSpr = cutSprite(menuBg[MENUGFXHELP],0,0,320,240);
  menuBg[MENUGFXBYE]=0;

  menuBg[MENUGFXPACKBOX] = SDL_CreateRGBSurface(SDL_SWSURFACE, 260,42,16, screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,0xff000000);

  setWaving(&waving, screen, menuBg[MENUGFXINTRO], HSCREENW-149,HSCREENH-80,1,15,300);

  return(1);
}


void setMenu(int mstate)
{
  menuPosY=0;
  menuPosX=0;
  menuState=mstate;
  dir=0;
  countdown=0;

}

void setMenuPosX(int x)
{
  menuPosX=x;
}
void setMenuPosY(int Y)
{
  menuPosX=Y;
}

int runMenu(SDL_Surface* screen)
{
  char buf[64];
  int scroll; //Generic scoll int (for scrolling lists)
  int ul=0;   //Userlevel (and used for scrolling)
  int menuChangeX=0,menuChangeY=0; //Is one if there's a change
  psysSet_t ps; //Particle system for particle effects in menu
  listItem* it;
  fileListItem_t* fItem;


  if(!dir)
  {
    countdown+=getTicks();
    if(countdown > 250)
      dir=1;
  } else {
    countdown-=getTicks();
    if(countdown < 1)
      dir=0;
  }

  if(getButton(C_BTNB))
    sndPlayOnce(SND_MENUCLICK, 160);

  if(getButton(C_UP) )
  {
    resetBtn(C_UP);
    menuChangeY=1;
    sndPlay(SND_MENUMOVE, 160);
    countdown=0;
    dir=0;
    menuPosY--;
    if(menuPosY < 0)
      menuPosY=menuMaxY;
  }
  if(getButton(C_DOWN))
  {
    resetBtn(C_DOWN);
    menuChangeY=1;
    sndPlay(SND_MENUMOVE, 160);
    countdown=0;
    dir=0;
    menuPosY++;
    if(menuPosY > menuMaxY)
      menuPosY=0;
  }

  if(getButton(C_LEFT) )
  {
    resetBtn(C_LEFT);
    menuChangeX=1;
    sndPlay(SND_MENUMOVE, 160);

    menuPosX--;
    if(menuPosX < 0)
      menuPosX=menuMaxX;
  }
  if(getButton(C_RIGHT))
  {
    resetBtn(C_RIGHT);
    menuChangeX=1;
    sndPlay(SND_MENUMOVE, 160);

    menuPosX++;
    if(menuPosX > menuMaxX)
      menuPosX=0;
  }

  switch(menuState)
  {
    case menuStateIntro:

      //Intr not working on PSP.. Skip it untill its fixed.
      #if defined (PSP) && !defined (PSP_DEBUG_INTRO)
        menuState=menuStatePaused;
        SDL_FreeSurface( menuBg[MENUGFXINTRO] );
        menuBg[MENUGFXINTRO]=0;
        menuPosY=0;
        clearParticles();
        clearCredits();
        break;
      #endif

      //Show background image
      starField(screen,1);

      runParticles(screen);

      runCredits(screen);

      waveImg(&waving);

      //Blink "Press B"
      if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+70);

      //Show version
      txtWrite(screen, FONTSMALL, VERSION_STRING, HSCREENW-150,HSCREENH+100);

      //Wait for keypress
      if( getButton( C_BTNB ) )
      {
        resetBtn( C_BTNB );
        menuState=menuStatePaused;
        SDL_FreeSurface( menuBg[MENUGFXINTRO] );
        menuBg[MENUGFXINTRO]=0;
        menuPosY=0;
        clearParticles();
        clearCredits();
        #if !defined (GP2X) && !defined(PSP)
        if(setting()->firstRun)
        {
          setMenu(menuStateUploadDiag);
          menuPosX=1; //Default to "choose"
        }
        #endif
      } else
      if( getButton( C_BTNY ) )
      {
        resetBtn( C_BTNY );
        ps.x=HSCREENW-149;
        ps.y=HSCREENH;
        ps.vel=100; // +/- in each dir
        ps.life=2000;
        ps.lifeVar=1000;
        ps.fade=0;
        ps.gravity=0;
        ps.bounce=1;      //If 1, particles will bounce off screen borders ( (vel * -1)/2 )
        ps.fadeColor=0x00;
        ps.color=PARTICLECOLORRANDOM;
        ps.srcImg=menuBg[MENUGFXINTRO]; //Getcolors for each particle from this image.
        ps.srcRect.x=0;
        ps.srcRect.y=0;
        ps.srcRect.w=menuBg[MENUGFXINTRO]->w;
        ps.srcRect.h=menuBg[MENUGFXINTRO]->h;
        spawnParticleSystem(&ps);
      }

    break; //Intro screen

    case menuStateNewGame: //Start new game (select starting level)
      starField(screen,1);

      menuMaxX = stats()->progress+2;

      //To make sure that we don't try to show a level that's not there
      if(menuMaxX > getNumLevels())
      {
        menuMaxX=getNumLevels();
      }
      if(menuPosX > getNumLevels())
      {
        menuPosX = getNumLevels();
      }

      //The "Finished" image..
      if(menuPosX == menuMaxX)
      {
        levelSelector(screen,menuPosX, 0);
      } else {
        levelSelector(screen,menuPosX, 1);
        txtWriteCenter(screen, FONTMEDIUM, STR_MENU_NEW_GAME, HSCREENW, HSCREENH-100);
        txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B_PLAY, HSCREENW, HSCREENH+108);

        if(getButton(C_BTNB) && menuPosX!=getNumLevels())
        {
          resetBtn(C_BTNB);

          initPlayer(); //New game, reset player.
          player()->levelFile = levelInfo( menuPosX )->file;
          player()->level = menuPosX;

          // Show help if it's the first level
          menuState=menuStateNextLevel;
        }
      }

      if(getButton(C_BTNMENU))
      {
        resetBtn(C_BTNMENU);
        menuPosY=0;
        setMenu(menuStatePaused);
      }

      if(menuPosX-1 < stats()->progress &&  menuPosX!=getNumLevels())
        txtWrite(screen, FONTMEDIUM, ">>", HSCREENW+120,HSCREENH-12);

      if(menuPosX > 0)
        txtWrite(screen, FONTMEDIUM, "<<", HSCREENW-155, HSCREENH-12);

    break; // New game

    case menuStateNextLevel:
      starField(screen,0);

      levelSelector(screen, player()->level,1);


      if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B_PLAY, HSCREENW, HSCREENH+108);

      if(getButton(C_BTNB))
      {
        resetBtn(C_BTNB);

        if(initGame())
        {
          if(player()->level == 0)
          {
            setMenu(menuStateHowto);
          } else {
            return(STATEPLAY);
          }
        } else {
          printf("Fatal: Couldn't init game. \n");
          return(STATEQUIT);
        }
      }

      if(getButton(C_BTNMENU))
      {
        resetBtn(C_BTNMENU);
        menuState=menuStateNewGame;
        if( statsIsHighScore() )
        {
          setMenu(menuStateEnterHighScore);
          menuReturnHack=menuStateNewGame;
        }

      }
    break;

    case menuStateFinishedLevel:
      starField(screen,0);
      fireWorks(screen);
      sprintf(buf, "Level %i Done!", player()->level);
      txtWave(screen, FONTMEDIUM,buf, HSCREENW, HSCREENH-95, &rot);

      sprintf(buf, "Progress: %.1f%%",  ((100.0f/(float)getNumLevels())*((float)player()->level+1.0f)) );
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-60);

      sprintf(buf, "%i pts total",  player()->campStats.score);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-40);

      sprintf(buf, "Stats for level %i", player()->level);
      txtWriteCenter(screen, FONTSMALL, buf,HSCREENW,HSCREENH);
      txtWrite(screen, FONTSMALL, " Stat      Yours         Best     ", (HSCREENW-153),(HSCREENH+20) );
      txtWrite(screen, FONTSMALL, "+-----+-------------+------------+", (HSCREENW-153),(HSCREENH+30) );
      txtWrite(screen, FONTSMALL, "Score :", (HSCREENW-153),(HSCREENH+42) );
      txtWrite(screen, FONTSMALL, "Time  :", (HSCREENW-153),(HSCREENH+54) );
      txtWrite(screen, FONTSMALL, "Moves :", (HSCREENW-153),(HSCREENH+68) );
      txtWrite(screen, FONTSMALL, "Combos:", (HSCREENW-153),(HSCREENH+80) );

      //levelscore
      sprintf(buf, "%i", player()->hsEntry.score);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW-32, HSCREENH+42);

      //best levelscore
      sprintf(buf, "%i", stats()->cl->score);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW+90, HSCREENH+42);

      //time
      sprintf(buf, "%i:%i", player()->hsEntry.time/60,player()->hsEntry.time%60 );
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW-32, HSCREENH+54);

      //best time
      sprintf(buf, "%i:%i", stats()->cl->time/60,stats()->cl->time%60 );
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW+90, HSCREENH+54);

      //Moves
      sprintf(buf, "%i", player()->hsEntry.moves);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW-32, HSCREENH+68);

      //best moves
      sprintf(buf, "%i", stats()->cl->moves);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW+90, HSCREENH+68);

      //Combos
      sprintf(buf, "%i", player()->hsEntry.combos);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW-32, HSCREENH+80);
      //best moves
      sprintf(buf, "%i", stats()->cl->combos);
      txtWriteCenter(screen, FONTSMALL, buf, HSCREENW+90, HSCREENH+80);

      txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+108);

      if(getButton(C_BTNB))
      {
        resetBtn(C_BTNB);

        //Reset board score
        player()->hsEntry.score=0;

        //Check if the player compleated all levels
        if(player()->level+1 == getNumLevels())
        {
          //Go to "Completed pack" image.
          setMenu(menuStateBeatPack);
          break;
        }

        //Inc to next lvl
        player()->level++;

        //Set new level
        player()->levelFile = levelInfo( player()->level )->file;

        //Entering screen
        menuState=menuStateNextLevel;
        return(STATEMENU);
      }

    break;

    case menuStatePaused:
      menuMaxY = 8;
      starField(screen,1);
      if(player()->gameStarted)
      {
        txtWave(screen, FONTMEDIUM, STR_MENU_PAUSE_HEADLINE, HSCREENW, HSCREENH-105, &rot);
      } else {
        txtWave(screen, FONTMEDIUM, STR_MENU_WIZZNIC_HEADLINE, HSCREENW, HSCREENH-105, &rot);
      }

      if(dir || menuPosY!= 0) txtWriteCenter(screen, FONTSMALL, STR_MENU_NEW_GAME_CHOICE, HSCREENW, HSCREENH-70);
      if( player()->gameStarted && (dir || menuPosY!= 1)) txtWriteCenter(screen, FONTSMALL, STR_MENU_RESUME_CHOICE, HSCREENW, HSCREENH-50);
      if(dir || menuPosY!= 2) txtWriteCenter(screen, FONTSMALL, STR_MENU_HIGHSCORES_CHOICE, HSCREENW, HSCREENH-30);
      if(dir || menuPosY!= 3) txtWriteCenter(screen, FONTSMALL, STR_MENU_OPTIONS_CHOICE, HSCREENW, HSCREENH-10);
      if(dir || menuPosY!= 4) txtWriteCenter(screen, FONTSMALL, STR_MENU_EDITOR_CHOICE, HSCREENW, HSCREENH+10);
      if(dir || menuPosY!= 5) txtWriteCenter(screen, FONTSMALL, STR_MENU_ABOUT_CHOICE, HSCREENW, HSCREENH+30);
      if(dir || menuPosY!= 6) txtWriteCenter(screen, FONTSMALL, STR_MENU_HELP_CHOICE, HSCREENW, HSCREENH+50);
      if(dir || menuPosY!= 7) txtWriteCenter(screen, FONTSMALL, STR_MENU_EXIT_CHOICE, HSCREENW, HSCREENH+70);

      sprintf(buf, STR_MENU_PACK_CHOICE, packState()->cp->name);
      if(dir || menuPosY!= 8) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+100);


      if( getButton( C_BTNB ) )
      {
        resetBtn( C_BTNB );
        switch(menuPosY)
        {
          case 0: //Newgame
            //If in arcade-mode, clear progress before entering level-selection.
            if(setting()->arcadeMode)
            {
              stats()->progress=-1;
            }

            setMenu(menuStateNewGame);

            //Clean up.
            if(player()->gameStarted)
            {
              cleanUpGame();
              //Check if highscore, if a game was started
              if( statsIsHighScore() )
              {
                setMenu(menuStateEnterHighScore);
                menuReturnHack=menuStateNewGame;
              }
            }

            //Current selected level.
            menuPosX=stats()->progress+1;
          break;
          case 1: //Resume, unset paused, return to game
            if(player()->gameStarted)
              return(STATEPLAY);
          break;
          case 2: //Highscores
            setMenu(menuStateHighScores);
          break;
          case 3: //Options
            setMenu(menuStateOptions);
          break;
          case 4: //UserLevels
            setMenu(menuStateUserLevels);
            if(player()->gameStarted) cleanUpGame();
            return(STATEMENU);
          break;
          case 5: //Switch to about screen
            menuState=menuStateAbout;
          break;
          case 6: //Switch to help screen
            setMenu(menuStateHowto);
          break;
          case 7: //Exit program
            if( statsIsHighScore() )
            {
              setMenu(menuStateEnterHighScore);
              menuReturnHack=menuStateOutro;
            } else {
              setMenu(menuStateOutro);
            }
          break;
          case 8: //Pack selection
            setMenu(menuStatePackList);
            menuPosY = packState()->selected;
          break;
        }
      }
      break;

      case menuStateHowto:
      starField(screen,1);

      #if defined (GP2X) || defined (PSP)
      //Sprinkle particles
      if( !dir && countdown < 1 )
      {
        ps.x=(rand()%109)+107;
        ps.y=(rand()%40)+78;
        ps.vel=50; // +/- in each dir
        ps.life=2000;
        ps.lifeVar=500;
        ps.fade=0;
        ps.gravity=1;
        ps.bounce=0;      //If 1, particles will bounce off screen borders ( (vel * -1)/2 )
        ps.fadeColor=0x00;
        ps.color=PARTICLECOLORRANDOM;
        ps.srcImg=0; //Don't get colors from an image
        ps.numParticles=20;
        spawnParticleSystem(&ps);
      }
      runParticles(screen);
      #else
      fireWorks(screen);
      #endif

      drawSprite(screen, helpSpr,setting()->bgPos.x,setting()->bgPos.y);

        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B_PLAY, HSCREENW, HSCREENH+108);

        if( getButton( C_BTNB ) )
        {
          resetBtn( C_BTNB );
          clearParticles();
          if(player()->gameStarted) //Return to game if allready started
          {
            return(STATEPLAY);
          } else {
            setMenu(menuStatePaused);
          }

        }
      break;

      case menuStateAbout:
        starField(screen, 1);
        fireWorks(screen);

        txtWriteCenter(screen, FONTMEDIUM,STR_MENU_ABOUT_HEADLINE, HSCREENW, HSCREENH-105);

        txtWrite(screen, FONTSMALL, STR_MENU_ABOUT_TEXT, HSCREENW-155, HSCREENH-80);
        txtWave(screen, FONTSMALL, STR_MENU_ABOUT_WEBSITE, HSCREENW, HSCREENH+108,&rot);

        static int cheat=-1;
        if( getButton( C_BTNA ) )
        {
          cheat=0;
          resetBtn( C_BTNA );
        }

        if( (cheat==0 || cheat==2)  && getButton(C_BTNY) )
        {
          resetBtn(C_BTNY);
          cheat++;
        }
        if( (cheat==1 || cheat==3) && getButton(C_BTNX) )
        {
          resetBtn(C_BTNX);
          cheat++;
        }
        if(cheat==4)
        {
          sndPlay( SND_MENUCLICK, 160 );
          cheat=-1;
          stats()->progress = getNumLevels();
        }

        if( getButton(C_BTNB) )
        {
          resetBtn( C_BTNB );
          menuState=menuStatePaused;
        }

      break;

      case menuStateGameOver:

        starField(screen,0);
        txtWriteCenter(screen, FONTMEDIUM,STR_MENU_GAMEOVER, HSCREENW, HSCREENH-85);
        sprintf(buf, STR_MENU_SCORE, player()->campStats.score);
        txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+108);

        //If in arcade mode, lose progress
        if(setting()->arcadeMode)
        {
          stats()->progress=-1;
          statsSave();
        }

        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+60);
        if( getButton( C_BTNB ) )
        {
          setMenu(menuStatePaused);
          resetBtn( C_BTNB );

          if( statsIsHighScore() )
          {
            setMenu(menuStateEnterHighScore);
            menuReturnHack=menuStatePaused;
          }
        }
      break;

      case menuStateBeatPack:

        starField(screen, 1);
        if( packState()->cp->hasFinishedImg )
        {
          if( !packState()->finishedImg )
          {
            packState()->finishedImg = loadImg( packGetFile("./","finished.png" ) );
            if( !packState()->finishedImg )
            {
              printf("Error: Can't load pack-finished file '%s'\n", packGetFile("./","finished.png" ) );
              return(STATEQUIT);
            }
          }
        } else {
          fireWorks(screen);

          txtWave(screen, FONTMEDIUM, STR_MENU_BEAT_PACK_HEADLINE, HSCREENW, HSCREENH-108, &rot);
          txtWrite(screen, FONTSMALL, STR_MENU_BEAT_PACK_RANT, HSCREENW-105, HSCREENH-50);
        }

        //Blit image to screen
        SDL_BlitSurface(packState()->finishedImg , NULL, screen, &(setting()->bgPos) );

        //bink "press b"
        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+100);

        if(getButton(C_BTNB))
        {
          resetBtn(C_BTNB);

          //Free image
          SDL_FreeSurface( packState()->finishedImg );

          //Set img ptr so it knows to load a new next time.
          packState()->finishedImg=0;

          //Check if he got a highscore.
          if( statsIsHighScore() )
          {
            setMenu(menuStateEnterHighScore);
            menuReturnHack=menuStatePackList;
            menuPosY = packState()->selected;
          } else {
            setMenu(menuStatePackList);
          }

          return(STATEMENU);
        }
      break;

      case menuStateOutro:
        starField(screen, 0);
        //Show background image
        if(!menuBg[MENUGFXBYE])
        {
          menuBg[MENUGFXBYE]=loadImg( DATADIR"data/menu/bye.png");
        }
        SDL_BlitSurface(menuBg[MENUGFXBYE] , NULL, screen, &(setting()->bgPos) );
        //Fireworks?
        fireWorks(screen);

        //Blink "Press B"
        if(dir) txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B, HSCREENW, HSCREENH+60);

        if(getButton(C_BTNB))
        {
          resetBtn(C_BTNB);
          cleanUpGame();
          return(STATEQUIT);
        }
      break;

      case menuStateUserLevels:
        starField(screen, 1);
        fireWorks(screen);
        txtWave(screen, FONTMEDIUM, STR_MENU_LVLEDIT_HEADLINE, HSCREENW, HSCREENH-105, &rot);
        menuMaxY=getNumUserLevels()+1;

        //Just keep it clean.
        if(player()->gameStarted) cleanUpGame();

        //Show Create and Exit menu-points.
        if(dir || menuPosY!= 0) txtWriteCenter(screen, FONTSMALL, STR_LVLEDIT_CREATE_CHOICE,HSCREENW,HSCREENH-70);
        if(dir || menuPosY!= 1) txtWriteCenter(screen, FONTSMALL, STR_LVLEDIT_EXIT_CHOICE,HSCREENW,HSCREENH-60);

        //Show usage
        txtWriteCenter(screen, FONTSMALL, STR_MENU_LVLEDIT_USAGE, HSCREENW, HSCREENH+108);

        //List levels
        ul=0;   //Userlevel
        scroll=0; //Generic scoll int
        if(menuPosY > 10)
        {
          ul = menuPosY-10;
          scroll=ul;
        }

        int i=0;
        while(ul < getNumUserLevels())
        {
          i++;
          sprintf(buf, STR_MENU_LVLEDIT_USRLVL, ul);
          if(dir || menuPosY!= ul+2) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-50+(10*(ul-scroll)));
          ul++;
          if( i > 12 )
          {
            break;
          }
        }

        if( getNumUserLevels() > ul )
        {
          txtWriteCenter(screen, FONTSMALL, STR_LVLEDIT_MORE, HSCREENW, HSCREENH-50+(10*(ul-scroll+1)));
        }


        //Edit levels/select
        if(getButton(C_BTNB))
        {
          resetBtn(C_BTNB);

          if(menuPosY==0) //Load empty, and create new levelname
          {
            //Load empty, set name to something diffrent
            editorLoad( DATADIR"data/empty.wzp");

            sprintf(buf, "%s/level%i.wzp", getUserLevelDir(), getNumUserLevels());
            editorFileName(buf);

            return(STATEEDIT);
          } else if(menuPosY==1)  //Exit from editor
          {
            player()->inEditor=0;
            menuState=menuStatePaused;
            menuPosY=0;
          } else {
            //Edit existing level
            editorLoad(userLevelFile(menuPosY-2));
            return(STATEEDIT);
          }
        }

        //Clone a level
        if(getButton(C_BTNY))
        {
          resetBtn(C_BTNY);

          if(menuPosY > 1)
          {
            //Load the selected level
            editorLoad(userLevelFile(menuPosY-2));

            //Make new filename
            sprintf(buf, "%s/level%i.wzp", getUserLevelDir(), getNumUserLevels());
            editorFileName(buf);

            //Start editing state
            return(STATEEDIT);
          }
        }

        //Play levelfile
        if(getButton(C_BTNA))
        {
          resetBtn(C_BTNA);
          if(menuPosY > 1)
          {
            initPlayer(); //Reset player for editor.
            player()->levelFile = userLevelFile(menuPosY-2);
            player()->level = 0;
            player()->inEditor=1;

            if(initGame())
            {
              return(STATEPLAY);
            } else {
              printf("Editor couldn't init game for the editor.\n");
              return(STATEQUIT);
            }
          }
        }
      break; //< userlevels

      case menuStatePackList:
        starField(screen, 1);
        fireWorks(screen);
        txtWriteCenter(screen, FONTMEDIUM, STR_MENU_SELECT_PACK, HSCREENW, HSCREENH-106);
        menuMaxY= packState()->numPacks-1;
        ul=0;
        scroll=0;
        if(menuPosY > 2)
        {
          ul= menuPosY - 2;
          scroll=ul;
        }

        //Clear box
        SDL_FillRect(menuBg[MENUGFXPACKBOX],0, SDL_MapRGB(screen->format, 0,255,255));
        while(ul < packState()->numPacks && ul-scroll <  4)
        {
          if(menuPosY== ul)
          {
            showPackBox(menuBg[MENUGFXPACKBOX], 0,0, ul );
            setWaving(&waving, screen, menuBg[MENUGFXPACKBOX], HSCREENW-130,HSCREENH-70+(48*(ul-scroll)),2,4,150);
            waveImg(&waving);
          }
          else
            showPackBox(screen, HSCREENW-130,HSCREENH-70+(48*(ul-scroll)), ul );
          ul++;
        }
        if( ul < packState()->numPacks )
        {
          if(dir) txtWrite(screen, FONTSMALL, STR_MENU_PACKS_MORE, HSCREENW+100, HSCREENH+108);
        }

        if( getButton(C_BTNB) )
        {
          resetBtn(C_BTNB);
          packFreeGfx();
          //If it's a diffrent pack
          if(menuPosY != packState()->selected)
          {
            //If there's a game started
            if(player()->gameStarted)
            {
              cleanUpGame();
            }

            //Clear the levelselector image (it caches images)
            resetLevelSelector();

            //Set selected pack.
            packSet(menuPosY);
            printf("Selected pack number: %i\n", menuPosY);

            //Save setting
            free( setting()->packDir);
            setting()->packDir = malloc( sizeof(char)*(strlen(packState()->cp->path)+1) );
            strcpy( setting()->packDir, packState()->cp->path );
            saveSettings(); //Save
            //--
          }
          //Return to main menu
          setMenu(menuStatePaused);
        }
      break;

      case menuStateOptions:
        starField(screen, 1);
        fireWorks(screen);
        txtWave(screen, FONTMEDIUM, STR_MENU_OPTIONS, HSCREENW, HSCREENH-108, &rot);

        //Number of options in list
        menuMaxY = 8;

        if(dir || menuPosY!= 0) txtWriteCenter(screen, FONTSMALL, STR_MENU_OPTIONS_EXIT, HSCREENW, HSCREENH-70);

        sprintf(buf, STR_MENU_OPTIONS_SOUNDVOL, setting()->soundVol);
        if(dir || menuPosY!= 1) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-50);
        //Set posY
        if(menuPosY==1)
        {
          if(menuChangeY)
          {
            menuMaxX=16;
            menuPosX = setting()->soundVol/8;
          }
          setting()->soundVol = menuPosX*8;

          //Change sound-volume if it changed
          if(menuChangeX)
          {
            applySettings();
          }
        }

        sprintf(buf, STR_MENU_OPTIONS_MUSICVOL, setting()->musicVol);
        if(dir || menuPosY!= 2) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-40);
        //Set posY
        if(menuPosY==2)
        {
          if(menuChangeY)
          {
            menuMaxX=16;
            menuPosX = setting()->musicVol/8;
          }
          setting()->musicVol = menuPosX*8;
          //Change music-volume if it changed
          if(menuChangeX)
          {
            applySettings();
          }
        }

        sprintf(buf, (setting()->userMusic)?"Music: <User selected>":"Music: <Game Music>");
        if(dir || menuPosY!=3) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW,HSCREENH-30);
        if(menuPosY==3)
        {
          if(menuChangeX||getButton(C_BTNB))
          {
              resetBtn(C_BTNB);
              menuPosY=0;
              fileListMake(setting()->musicDir);
              setMenu(menuStateSelectMusicDir);
          }
        }

        sprintf(buf, (setting()->arcadeMode)?"Game mode: < Arcade >":"Game mode: < Career >");
        if(dir || menuPosY!= 4) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH-10);
        //Set posy
        if(menuPosY==4)
        {
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX = setting()->arcadeMode;
          }
          setting()->arcadeMode = menuPosX;
        }

        sprintf(buf, (setting()->showFps)?"Show FPS: < On >":"Show FPS: < Off >");
        if(dir || menuPosY!= 5) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+10);
        //Set posy
        if(menuPosY==5)
        {
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX = setting()->showFps;
          }
          setting()->showFps = menuPosX;
        }

        sprintf(buf, (setting()->particles)?"Particles: < On >":"Particles: < Off >");
        if(dir || menuPosY!= 6) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+20);
        //Set posy
        if(menuPosY==6)
        {
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX = setting()->particles;
          }
          setting()->particles = menuPosX;
        }
        //Disable music (requires restart)
        sprintf(buf, (setting()->disableMusic)?"Load Music: < Disabled >":"Load Music: < Enabled >");
        if(dir || menuPosY!= 7) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+30);
        //Set posy
        if(menuPosY==7)
        {
          txtWriteCenter(screen, FONTSMALL, "(Restart required)", HSCREENW, HSCREENH+100);
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX = setting()->disableMusic;
          }
          setting()->disableMusic = menuPosX;
        }

        #ifdef GP2X
        sprintf(buf, "Wiz Clock: < %i >", setting()->wizClock);
        if(dir || menuPosY!= 8) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+50);
        //Set posY
        if(menuPosY==8)
        {
          if(menuChangeY)
          {
            menuMaxX=70;
            menuPosX = setting()->wizClock/10;
          }

          if( menuPosX < 10 )
            menuPosX = 10;

          setting()->wizClock = menuPosX*10;

          //Only clock the wiz when value changes.
          if(menuChangeX)
          {
            WIZ_SetClock( setting()->wizClock );
          }
        }
        #else
        sprintf(buf, (setting()->uploadStats)?"Upload Stats: <Enabled>":"Upload Stats: <Disabled>");
        if(dir || menuPosY!= 8) txtWriteCenter(screen, FONTSMALL, buf, HSCREENW, HSCREENH+50);
        //Set posY
        if(menuPosY==8)
        {
          if(menuChangeY)
          {
            menuMaxX=1;
            menuPosX=setting()->uploadStats;
          }
          setting()->uploadStats = menuPosX;
          if(menuChangeX)
          {
            if(setting()->uploadStats && !setting()->online)
            {
              statsUpload(0,0,0,0,0,"check",1);
            } else if(!setting()->uploadStats)
            {
              setting()->online=0;
            }
          }
        }
        #endif

        //Save and exit Options menu
        if( getButton( C_BTNMENU ) || (getButton( C_BTNB ) && menuPosY==0) )
        {
          resetBtn( C_BTNMENU );
          resetBtn( C_BTNB );
          saveSettings();
          setMenu(menuStatePaused);
        }
      break;

      //Highscore list
      case menuStateHighScores:
      starField(screen, 0);
      fireWorks(screen);
      menuMaxY=1;

      statsDrawHs(screen);


      txtWriteCenter(screen, FONTSMALL, STR_MENU_PRESS_B,HSCREENW,HSCREENH+100);

      if( getButton( C_BTNB ) )
      {
        resetBtn( C_BTNB );

          setMenu(menuStatePaused);
      }

      break;

      case menuStateEnterHighScore:
        starField(screen, 0);
        menuMaxY=4;
        menuMaxX=10;
        txtWriteCenter(screen, FONTMEDIUM, "High score!", HSCREENW,HSCREENH-95);
        txtWriteCenter(screen, FONTSMALL, "Enter your name for highscore.", HSCREENW, HSCREENH-70);

        sprintf(buf, "Name: %s", setting()->playerName);

        txtWrite(screen, FONTSMALL, buf, HSCREENW-110, HSCREENH-45);
        txtWriteCenter(screen, FONTSMALL,STR_MENU_HIGHSCORE_NAME_CONTROLS, HSCREENW,HSCREENH+108);
        int cy,cx;
        char hack[2]={ ' ',0x00 };
        for(cy=0; cy < kbRows; cy++)
        {
          for(cx=0; cx < kbCols; cx++)
          {

            if( (menuPosX==cx && menuPosY==cy) && !dir )
            {
              txtWrite(screen, FONTSMALL, "[_]", (HSCREENW-70)+(cx)*16-8, (HSCREENH-9)+cy*15);
            }

            hack[0]=(*kb)[cy][cx];
            txtWrite(screen, FONTSMALL, hack, (HSCREENW-70)+cx*16, (HSCREENH-10)+cy*15);

          }
        }
        //Blink "Save" button if selected
        if( menuPosY==4 && dir)
        {
          if(menuPosX > 9) menuPosX=0;
          txtWriteCenter(screen, FONTSMALL, " ____ ", HSCREENW, HSCREENH+50);
        }
        txtWriteCenter(screen, FONTSMALL, "[Save]", HSCREENW, HSCREENH+50);

        if( menuPosX==10 && dir)
        {
          if(menuPosY >3) menuPosY =1;
          txtWrite( screen, FONTSMALL, " ____", HSCREENW-130, HSCREENH+10 );
        }
        txtWrite( screen, FONTSMALL, "[Caps]", HSCREENW-130, HSCREENH+10 );

        //Save if we're at row 4
        if( menuPosX == 10 )
        {
          if( getButton( C_BTNB ) )
          {
            resetBtn( C_BTNB );
            kbCase=!kbCase;
            if(kbCase)
            {
              kb=&kbh;
            } else {
              kb=&kbl;
            }
          }
        } else if( menuPosY==4 && getButton( C_BTNB ) )
        {
          resetBtn( C_BTNB );
          saveSettings();
          //Copy player name.
          strcpy(player()->campStats.name, setting()->playerName);
          statsSaveHighScore();
         // printf("AddingHighscore: %i\n", player()->campStats.score);
          menuState=menuReturnHack;
        } else if( getButton( C_BTNB ) )
        {
          resetBtn( C_BTNB );
          if(strlen( setting()->playerName ) < 11)
          {
            setting()->playerName[ strlen(setting()->playerName) ] = (*kb)[menuPosY][menuPosX];
            setting()->playerName[ strlen(setting()->playerName)+1 ] = 0x0;
          }
        } else if( getButton( C_BTNA ) )
        {
          resetBtn( C_BTNA);
          if(strlen(setting()->playerName) > 0)
          {
            setting()->playerName[ strlen(setting()->playerName)-1 ]=0;
          }
        }
      break;

      case menuStateSelectMusicDir:

        starField(screen,0);
        txtWave(screen, FONTMEDIUM, "Select Music", 160,15, &rot);
        if(dir || menuPosY!=0) txtWriteCenter(screen, FONTSMALL, "Play Game Music", HSCREENW, HSCREENH-70);
        if(dir || menuPosY!=1) txtWriteCenter(screen, FONTSMALL, "Play From Directory", HSCREENW, HSCREENH-60);


        x=0;
        y=0;
        scroll=0;
        if(menuPosY > 10)
        {
          x = menuPosY-10;
          scroll=x;
        }
        //Run through list
        it=fileList();

        while( (it=it->next) )
        {
          fItem=(fileListItem_t*)it->data;
          if(fItem->dir)
          {
            y++;
            if(y>x)
            {
              if(dir || menuPosY!=x+2) txtWriteCenter(screen, FONTSMALL, fItem->name, HSCREENW, (HSCREENH-40)+((x-scroll)*10));
              if(menuPosY-2==x)
              {
                if(getButton(C_BTNB))
                {
                  resetBtn(C_BTNB);
                  free(setting()->musicDir);
                  setting()->musicDir=malloc(sizeof(char)*(strlen(fItem->fullName)+1));
                  strcpy(setting()->musicDir, fItem->fullName);
                  fileListMake(setting()->musicDir);
                  menuPosY=2;
                  return(STATEMENU);
                }
              }
              x++;
            }
          }
        }

        txtWriteCenter(screen, FONTSMALL, "----- Files -----", HSCREENW, (HSCREENH-40)+(x-scroll)*10);
        x++;
        menuMaxY=x;

        it=fileList();
        while( (it=it->next) )
        {
          fItem=(fileListItem_t*)it->data;
          if(!fItem->dir)
          {
            if(dir || menuPosY!=x+2) txtWriteCenter(screen, FONTSMALL, fItem->name, HSCREENW, (HSCREENH-40)+(x-scroll)*10);
            x++;
          }
        }

        if(getButton(C_BTNB))
        {
          resetBtn(C_BTNB);

          if(menuPosY < 2)
          {
            if(menuPosY==0)
            {
              free(setting()->musicDir);
              setting()->musicDir=malloc(sizeof(char)*strlen("./"));
              strcpy(setting()->musicDir, ".");
            }
            setting()->userMusic=menuPosY;
            saveSettings();
            fileListFree();
            soundSetMusic(); //Important to set after freeing file list, since this is using that list
            setMenu(menuStateOptions);
            menuPosX=3;
          }
        }

      break;

      #ifndef GP2X
      case menuStateUploadDiag:
        starField(screen,1);
        menuMaxY=0;
        menuMaxX=2;
        txtWrite(screen, FONTSMALL, STR_MENU_UPLOADNAG, HSCREENW-152, HSCREENH-100 );

        txtWriteCenter(screen, FONTSMALL, "< Help out? >", HSCREENW, HSCREENH+100);

        txtWriteCenter(screen, FONTSMALL, "Yes", HSCREENW-(13*8), HSCREENH+100 );
        if( menuPosX == 0 )
          if(dir) txtWriteCenter(screen, FONTSMALL, "___", HSCREENW-(13*8), HSCREENH+100 );

        txtWriteCenter(screen, FONTSMALL, "No", HSCREENW+(13*8), HSCREENH+100 );
        if( menuPosX == 2 )
          if(dir) txtWriteCenter(screen, FONTSMALL, "__", HSCREENW+(13*8), HSCREENH+100 );

        if( getButton( C_BTNB) )
        {
          resetBtn( C_BTNB );
          //Enabled
          if( menuPosX == 0 )
          {
            setting()->firstRun=0;
            saveSettings();
            statsUpload(0,0,0,0,0,"check",1);
            setMenu( menuStatePaused );
          }

          //Disabled
          if( menuPosX == 2 )
          {
            setting()->firstRun=0;
            setting()->uploadStats=0;
            saveSettings();
            setMenu( menuStatePaused );
          }
        }
      break;
      #endif
  }
  return(STATEMENU);
}