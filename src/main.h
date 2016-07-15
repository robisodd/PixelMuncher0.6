#pragma once
#include <pebble.h>
  

#define UPDATE_MS 30 // Refresh rate in milliseconds (about 32fps)
#define ZOOM 5       // Number of pixels per map square
// Map SHOULD be:
// Currently: 28w x 31h
// Original:  28w x 36h
// Should be: 32w x 36h
#define MAP_W 28
#define MAP_H 31

// Pixel Offset to display board on screen
#define BOARD_X 2
#define BOARD_Y 13

// =========================================================================================================== //
typedef enum {
  Cherries   = 0,
  Strawberry = 1,
  Peach      = 2,
  Apple      = 3,
  Grapes     = 4,
  Spaceship  = 5,
  Bell       = 6,
  Key        = 7
} BonusType;

//uint32_t bonuspoints[8] = {100, 300, 500, 700, 1000, 2000, 3000, 5000};
  
// =========================================================================================================== //
typedef enum {
 ModeNewLifeBegin = 0,  // Just like ModeLevelBegin, but subtracts a life after showing "PLAYER ONE" "READY!"
 ModeLevelBegin   = 1,  // Very beginning, show "player 1" "ready", no sprites, no animation, pause for 16 counts
 ModeRoundBegin   = 2,  // Show board, ready, spectres, muncher (and reduce 1 life), pause for 16 counts, change mode to Playing
 ModePlaying      = 3,  // Most common mode
 ModeDying        = 4,  // Pause, go through death sequence, check GameOver, else run init (which will change mode to ModeStarting)
 ModeWinning      = 5,  // Pause, erase sprites, flash board, then run init (which will change mode to ModeStarting)
 ModeGameOver     = 6  // Dead, no lives left.  Only back button to exit
} GameModeType;
// =========================================================================================================== //
typedef enum {
  ModeEyes      = 0,
  ModeBunker    = 1,
  ModePatrol    = 2,
  ModeAttack    = 3,
  ModeCruise    = 4,
  ModeScared    = 5,
  ModeInvisible = 6
} SpectreModeType;

typedef struct SpectreStruct {
  GPoint pos;        // in pixels, not tile -- center pixel
  GPoint dir;        // direction of movement (always -1, 0 or 1)
  GPoint targettile; // tile, not pixel
  //GPoint 
  uint8_t  speed;      // speed bucket for direction of movement
  //uint8_t face;      // 0=Left, 1=Up, 2=Right, 3=Down
  //int16_t  facing;     // Eyes Direction Facing (from 0 - TRIG_MAX_ANGLE)
  uint8_t facing;      // Direction Eyes are facing, 
  uint8_t moving;      //
  //uint32_t frame;    // Animation frame. 0=Skirt, 1=Skirt
  uint8_t  color;      // Spectre color, hard coded and overridden if scared
  SpectreModeType  mode;       //
  uint8_t  animate;    // Animation frame. LSB: 0=Skirt, 1=Skirt
  SpectreModeType  enqueued_mode;
  
  //Modes:
  // Bunker
  // Leaving Bunker
  // Entering Bunker
  // Patrol
  // Attack
  // Cruise Elroy
  // Scared (Blue)
  // Dead/Eyes
} SpectreStruct;


/*
   Move one pixel
   If in new tile, check if should turn blue and reverse
   
   whenever ghosts change from Chase or Scatter to any other mode,
   they are forced to reverse direction as soon as they enter the next tile
   Even if they're in the house.  They'll just go left as usual, then right when entering a new tile


*/

// PlayerStruct are things about the person playing
//   which are the things that stay with "Player 1" and "Player 2" etc
//   including: initials, current dots remaining, score, lives, current level
typedef struct PlayerStruct {
  uint32_t score;
   uint8_t lives;
   uint8_t level;
   bool dots[244];   // dots left
   uint8_t totaldots;  // current number of dots on the board
  // name/initials?
} PlayerStruct;

// MuncherStruct is about the character.
//   Properties reset between each player and/or single player's death
//   including: x,y pos, speed, mouth frame
typedef struct MuncherStruct {
  GPoint pos;    // in pixels, not tile.  ~center pixel of sprite
  GPoint dir;    // direction of movement (always -1, 0 or 1)
   uint8_t speed;  // speed multlipier for direction of movement
  //uint8_t face;  // 0=Left, 1=Up, 2=Right, 3=Down
   int16_t facing; // Eater Direction Facing (from 0 - TRIG_MAX_ANGLE)
  uint32_t frame;  // Animation frame. Mouth: 0=Closed, 1=Open, 2=Wide, 3=Open
//   uint8_t  mode;   // Alive, Dead
} MuncherStruct;


#define SPECTRE0_COLOR 0b11110000
#define SPECTRE1_COLOR 0b11001111
#define SPECTRE2_COLOR 0b11110111
#define SPECTRE3_COLOR 0b11111000
#define SCARED_BODY_COLOR 0b11010111
#define SCARED_FACE_COLOR 0b11111111
#define SPECTRE_EYE_COLOR 0b11111111
#define SPECTRE_PUPIL_COLOR 0b11000011

#define MUNCHER_COLOR 0b11111100

#define PLAYER_TEXT_COLOR 0b11011111
#define READY_COLOR 0b11111100
#define GAME_OVER_COLOR 0b11110000
#define SCORE_COLOR 0b11111111

#define BACKGROUND_COLOR 0b11000000
#define BOARD_COLOR 0b11010111
#define BOARD_FLASHING_COLOR 0b11111111
#define DOOR_COLOR 0b11111010
#define DOT_COLOR 0b11111111
#define BIGDOT_COLOR 0b11111111
  
// ------------------------------------------------------------------------ //
//  Make all functions globally accessable
// ------------------------------------------------------------------------ //
int32_t abs32(int32_t x);

uint8_t getmap(int32_t x, int32_t y);
void    setmap(int32_t x, int32_t y, int8_t data);

void load_graphics();
void unload_graphics();

void draw_background(uint8_t *fb);
void draw_background_flashing(uint8_t *fb, bool flashing);
void draw_dots(uint8_t *fb, uint8_t *map, uint8_t animate);
void draw_font8(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t chr);
void draw_font8_text(uint8_t *fb, int16_t x, int16_t y, uint8_t color, char *str); // str points to zero-terminated string
void draw_actor(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t spr);
void draw_pupils(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t facing);
void draw_sprite(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t spr);
void fill_rect(uint8_t *screen, GRect rect, uint8_t color);

void init_game(Window *window);
void start_game();
void destroy_game();
void init_round();
void init_level();


//void update_movement_via_joystick();
//void game_click_config_provider(void *context);
// void   init_board();
// uint8_t getlevelspeed(uint8_t level);
// void draw_muncher(uint8_t *fb);
// void draw_spectres(uint8_t *fb);
//void draw_sprite8(uint8_t *fb, uint8_t *font, int16_t start_x, int16_t start_y, uint8_t color, uint8_t spr);
// void init_player();
// void init_muncher();
// void move_muncher();
// void init_spectres();
// void move_spectres();
// void check_collisions();
// void muncher_eat_dots();


// =========================================================================================================== //
//  Logging
// =========================================================================================================== //
#define logging true  // Enable/Disable logging for debugging
//Note: printf uses APP_LOG_LEVEL_DEBUG
#if logging
  #define LOG(...) (printf(__VA_ARGS__))
#else
  #define LOG(...)
#endif

// =========================================================================================================== //
//  Inline Multi-Pebble Support
// =========================================================================================================== //
#if defined(PBL_PLATFORM_APLITE)
  #define IF_APLITE(aplite) (aplite)
  #define IF_BASALT(basalt)
  #define IF_CHALK(chalk)
  #define IF_NOT_APLITE(other)
  #define IF_NOT_BASALT(other) (other)
  #define IF_NOT_CHALK(other) (other)
  #define IF_APLITE_ELSE(aplite, other) (aplite)
  #define IF_BASALT_ELSE(basalt, other) (other)
  #define IF_CHALK_ELSE(chalk, other) (other)
  #define IF_APLITE_BASALT(aplite, basalt) (aplite)
  #define IF_APLITE_BASALT_ELSE(aplite, basalt, other) (aplite)
  #define IF_APLITE_CHALK(aplite, chalk) (aplite)
  #define IF_APLITE_CHALK_ELSE(aplite, chalk, other) (aplite)
  #define IF_BASALT_CHALK(basalt, chalk)
  #define IF_BASALT_CHALK_ELSE(basalt, chalk, other) (other)
  #define IF_APLITE_BASALT_CHALK(aplite, basalt, chalk) (aplite)
  #define IF_APLITE_BASALT_CHALK_ELSE(aplite, basalt, chalk, other) (aplite)
#elif defined(PBL_PLATFORM_BASALT)
  #define IF_APLITE(aplite)
  #define IF_BASALT(basalt) (basalt)
  #define IF_CHALK(chalk)
  #define IF_NOT_APLITE(other) (other)
  #define IF_NOT_BASALT(other)
  #define IF_NOT_CHALK(other) (other)
  #define IF_APLITE_ELSE(aplite, other) (other)
  #define IF_BASALT_ELSE(basalt, other) (basalt)
  #define IF_CHALK_ELSE(chalk, other) (other)
  #define IF_APLITE_BASALT(aplite, basalt) (basalt)
  #define IF_APLITE_BASALT_ELSE(aplite, basalt, other) (basalt)
  #define IF_APLITE_CHALK(aplite, chalk) (other)
  #define IF_APLITE_CHALK_ELSE(aplite, chalk, other) (other)
  #define IF_BASALT_CHALK(basalt, chalk) (basalt)
  #define IF_BASALT_CHALK_ELSE(basalt, chalk, other) (basalt)
  #define IF_APLITE_BASALT_CHALK(aplite, basalt, chalk) (basalt)
  #define IF_APLITE_BASALT_CHALK_ELSE(aplite, basalt, chalk, other) (basalt)
#elif defined(PBL_PLATFORM_CHALK)
  #define IF_APLITE(aplite)
  #define IF_BASALT(basalt)
  #define IF_CHALK(chalk) (chalk)
  #define IF_NOT_APLITE(other) (other)
  #define IF_NOT_BASALT(other) (other)
  #define IF_NOT_CHALK(other)
  #define IF_APLITE_ELSE(aplite, other) (other)
  #define IF_BASALT_ELSE(basalt, other) (other)
  #define IF_CHALK_ELSE(chalk, other) (chalk)
  #define IF_APLITE_BASALT(aplite, basalt)
  #define IF_APLITE_BASALT_ELSE(aplite, basalt, other) (other)
  #define IF_APLITE_CHALK(aplite, chalk) (chalk)
  #define IF_APLITE_CHALK_ELSE(aplite, chalk, other) (chalk)
  #define IF_BASALT_CHALK(basalt, chalk) (chalk)
  #define IF_BASALT_CHALK_ELSE(basalt, chalk, other) (chalk)
  #define IF_APLITE_BASALT_CHALK(aplite, basalt, chalk) (chalk)
  #define IF_APLITE_BASALT_CHALK_ELSE(aplite, basalt, chalk, other) (chalk)
#else
  #define IF_APLITE(aplite)
  #define IF_BASALT(basalt)
  #define IF_CHALK(chalk)
  #define IF_NOT_APLITE(aplite) (other)
  #define IF_NOT_BASALT(basalt) (other)
  #define IF_NOT_CHALK(chalk) (other)
  #define IF_APLITE_ELSE(aplite, other) (other)
  #define IF_BASALT_ELSE(basalt, other) (other)
  #define IF_CHALK_ELSE(chalk, other) (other)
  #define IF_APLITE_BASALT(aplite, basalt)
  #define IF_APLITE_BASALT_ELSE(aplite, basalt, other) (other)
  #define IF_APLITE_CHALK(aplite, chalk)
  #define IF_APLITE_CHALK_ELSE(aplite, chalk, other) (other)
  #define IF_BASALT_CHALK(basalt, chalk)
  #define IF_BASALT_CHALK_ELSE(basalt, chalk, other) (other)
  #define IF_APLITE_BASALT_CHALK(aplite, basalt, chalk)
  #define IF_APLITE_BASALT_CHALK_ELSE(aplite, basalt, chalk, other) (other)
#endif


#if defined(PBL_COLOR)
  #define IF_COLOR(color) (color)
  #define IF_BW(bw)
  #define IF_NOT_COLOR(other)
  #define IF_NOT_BW(other) (other)
  #define IF_COLOR_ELSE(color, other) (color)
  #define IF_BW_ELSE(bw, other) (other)
  #define IF_COLOR_BW(color, bw) (color)
  #define IF_BW_COLOR(bw, color) (color)
  #define IF_COLOR_BW_ELSE(color, bw, other) (color)
  #define IF_BW_COLOR_ELSE(bw, color, other) (color)
#elif defined(PBL_BW)
  #define IF_COLOR(color)
  #define IF_BW(bw) (bw)
  #define IF_NOT_COLOR(other) (other)
  #define IF_NOT_BW(other)
  #define IF_COLOR_ELSE(color, other) (other)
  #define IF_BW_ELSE(bw, other) (bw)
  #define IF_COLOR_BW(color, bw) (bw)
  #define IF_BW_COLOR(bw, color) (bw)
  #define IF_COLOR_BW_ELSE(color, bw, other) (bw)
  #define IF_BW_COLOR_ELSE(bw, color, other) (bw)
#else
  #define IF_COLOR(color)
  #define IF_BW(bw)
  #define IF_NOT_COLOR(other) (other)
  #define IF_NOT_BW(other) (other)
  #define IF_COLOR_ELSE(color, other) (other)
  #define IF_BW_ELSE(bw, other) (other)
  #define IF_COLOR_BW(color, bw)
  #define IF_BW_COLOR(bw, color)
  #define IF_COLOR_BW_ELSE(color, bw, other) (other)
  #define IF_BW_COLOR_ELSE(bw, color, other) (other)
#endif

  
#if defined(PBL_SDK_2)
  #define IF_SDK2(SDK2) (SDK2)
  #define IF_SDK3(SDK3)
  #define IF_NOT_SDK2(other)
  #define IF_NOT_SDK3(other) (other)
  #define IF_SDK2_ELSE(SDK2, other) (SDK2)
  #define IF_SDK3_ELSE(SDK3, other) (other)
  #define IF_SDK2_SDK3(SDK2, SDK3) (SDK2)
  #define IF_SDK2_SDK3_ELSE(SDK2, SDK3, other) (SDK2)
#elif defined(PBL_SDK_3)
  #define IF_SDK2(SDK2)
  #define IF_SDK3(SDK3) (SDK3)
  #define IF_NOT_SDK2(other) (other)
  #define IF_NOT_SDK3(other)
  #define IF_SDK2_ELSE(SDK2, other) (other)
  #define IF_SDK3_ELSE(SDK3, other) (SDK3)
  #define IF_SDK2_SDK3(SDK2, SDK3) (SDK3)
  #define IF_SDK2_SDK3_ELSE(SDK2, SDK3, other) (SDK3)
#else
  #define IF_SDK2(SDK2)
  #define IF_SDK3(SDK3)
  #define IF_NOT_SDK2(other) (other)
  #define IF_NOT_SDK3(other) (other)
  #define IF_SDK2_ELSE(SDK2, other) (other)
  #define IF_SDK3_ELSE(SDK3, other) (other)
  #define IF_SDK2_SDK3(SDK2, SDK3)
  #define IF_SDK2_SDK3_ELSE(SDK2, SDK3, other) (other)
#endif

#if defined(PBL_RECT)
  #define IF_RECT(rect) (rect)
  #define IF_ROUND(round)
  #define IF_NOT_RECT(other)
  #define IF_NOT_ROUND(other) (other)
  #define IF_RECT_ELSE(rect, other) (rect)
  #define IF_ROUND_ELSE(round, other) (other)
  #define IF_RECT_ROUND(rect, round) (rect)
  #define IF_ROUND_RECT(round, rect) (rect)
  #define IF_RECT_ROUND_ELSE(rect, round, other) (rect)
  #define IF_ROUND_RECT_ELSE(round, rect, other) (rect)
#elif defined(PBL_ROUND)
  #define IF_RECT(rect)
  #define IF_ROUND(round) (round)
  #define IF_NOT_RECT(other) (other)
  #define IF_NOT_ROUND(other)
  #define IF_RECT_ELSE(rect, other) (other)
  #define IF_ROUND_ELSE(round, other) (round)
  #define IF_RECT_ROUND(rect, round) (round)
  #define IF_ROUND_RECT(round, rect) (round)
  #define IF_RECT_ROUND_ELSE(rect, round, other) (round)
  #define IF_ROUND_RECT_ELSE(round, rect, other) (round)
#else
  #define IF_RECT(rect)
  #define IF_ROUND(round)
  #define IF_NOT_RECT(other) (other)
  #define IF_NOT_ROUND(other) (other)
  #define IF_RECT_ELSE(rect, other) (other)
  #define IF_ROUND_ELSE(round, other) (other)
  #define IF_RECT_ROUND(rect, round)
  #define IF_ROUND_RECT(round, rect)
  #define IF_RECT_ROUND_ELSE(rect, round, other) (other)
  #define IF_ROUND_RECT_ELSE(round, rect, other) (other)
#endif