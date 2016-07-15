/*..X1UP···HIGH SCORE······X··    0
  ·····00·····················    1
  ····························    2
  ╔════════════╕╒════════════╗    3   1
  ║∙∙∙∙∙∙∙∙∙∙∙∙││∙∙∙∙∙∙∙∙∙∙∙∙║    4   2
  ║∙┌──┐∙┌───┐∙││∙┌───┐∙┌──┐∙║    5   3
  ║O│  │∙│   │∙││∙│   │∙│  │O║    6   4
  ║∙└──┘∙└───┘∙└┘∙└───┘∙└──┘∙║    7   5
  ║∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙║    8   6
  ║∙┌──┐∙┌┐∙┌──────┐∙┌┐∙┌──┐∙║    9   7
  ║∙└──┘∙││∙└──┐┌──┘∙││∙└──┘∙║   10   8
  ║∙∙∙∙∙∙││∙∙∙∙││∙∙∙∙││∙∙∙∙∙∙║   11   9
  ╚════╗∙│└──┐ ││ ┌──┘│∙╔════╝   12  10
       ║∙│┌──┘ └┘ └──┐│∙║        13  11
       ║∙││    X     ││∙║        14  12
       ║∙││ ╔══──══╗ ││∙║        15  13
═══════╝∙└┘ ║      ║ └┘∙╚═══════ 16  14
        ∙   ║      ║   ∙         17  15
═══════╗∙┌┐ ║      ║ ┌┐∙╔═══════ 18  16
       ║∙││ ╚══════╝ ││∙║        19  17
       ║∙││    $     ││∙║        20  18
       ║∙││ ┌──────┐ ││∙║        21  19
  ╔════╝∙└┘ └──┐┌──┘ └┘∙╚════╗   22  20
  ║∙∙∙∙∙∙∙∙∙∙∙∙││∙∙∙∙∙∙∙∙∙∙∙∙║   23  21
  ║∙┌──┐∙┌───┐∙││∙┌───┐∙┌──┐∙║   24  22
  ║∙└─┐│∙└───┘∙└┘∙└───┘∙│┌─┘∙║   25  23
  ║O∙∙││∙∙∙∙∙∙∙  ∙∙∙∙∙∙∙││∙∙O║   26  24
  ╙─┐∙││∙┌┐∙┌──────┐∙┌┐∙││∙┌─╜   27  25
  ╓─┘∙└┘∙││∙└──┐┌──┘∙││∙└┘∙└─╖   28  26
  ║∙∙∙∙∙∙││∙∙∙∙││∙∙∙∙││∙∙∙∙∙∙║   29  27
  ║∙┌────┘└──┐∙││∙┌──┘└────┐∙║   30  28
  ║∙└────────┘∙└┘∙└────────┘∙║   31  29
  ║∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙∙║   32  30
  ╚══════════════════════════╝   33  31
  ····························   34
  X··························X   35
          1111111111222222222233
01234567890123456789012345678901

           1111111111222222222   
  1234567890123456789012345678  

*/
/*
v0.3: Ditched fluff (menus, animated backgrounds, etc.) to actually release this
      Spectres move randomly
v0.5: Added Pause
      Too many extra lives ends in +
      Spectre:
      Current heading toward tile
      Next decision (shows where eyes go)
        Could be a tile
        Could be to become blue (Instantly turn color, but don't reverse until entering new tile)
        
        ---------------------------------
         8 x 8 per tile
        ---------------------------------
        Speed goes +1 offset (8x8 per tile)
               100% = 10 or 11 tiles/second = 80 to 88 offsets/second
                 5% = 4 to 4.4 offsets/second
          5% @60fps = 0.0666 to 0.07333 offsets / frame (10/150) to (11/150)
          
        
        ---------------------------------
         64 x 64 per tile
        ---------------------------------
        Speed goes +1 offset (64x64 per tile)
        100% = 10 or 11 tiles/second = 640 to 704 offsets/second
                 5% = 33.6 offsets/second
          5% @60fps = 0.56 offsets/second
          
          
          
        ---------------------------------
         256 x 256 per tile
        ---------------------------------
        Speed goes +1 offset (256x256 per tile)
               100% = 10 or 11 tiles/second = 2560 to 2816 offsets/second
                 5% = 128 to 140.8 offsets/second
          5% @60fps = 2.1333 to 2.34666 offsets / frame
          
                 5% =    2 offsets / frame  (tile = 256x256 offsets)
          5% @60fps =  120 offsets / second (~1/2 tile)
        100% @60fps = 2400 offsets / second (9.375 tiles)
        
                 5% =    3 offsets / frame  (tile = 256x256 offsets)
          5% @60fps =  180 offsets / second (0.703 tile)
        100% @60fps = 3600 offsets / second (14.0625 tiles)
        
        
        ---------------------------------
         512 x 512 per tile
        ---------------------------------
        Speed goes +1 offset (512x512 per tile)
               100% = 10 or 11 tiles/second = 5120 to 5632 offsets/second
                 5% = 256 to 281.6 offsets/second
          5% @60fps = 4.2666 to 4.69333 offsets / frame
        
                 1% =     1 offsets / frame  (tile = 512x512 offsets)
          1% @60fps =    60 offsets / second (0.12 tile/sec)
          Therefore: I can tweak the speed by increments of 0.12 tiles/sec
          
                 5% =     5 offsets / frame  (tile = 512x512 offsets)
          5% @60fps =   300 offsets / second (>1/2 tile/sec)
        100% @60fps =  6000 offsets / second (11.71875 tiles/sec)
        
          
        ---------------------------------
         1024 x 1024 per tile
        ---------------------------------
        Speed goes +1 offset (1024x1024 per tile)
               100% = 10 or 11 tiles/second = 10240 to 11264 offsets/second
                 5% = 512 to 563.2 offsets/second
          5% @60fps = 8.5333 to 9.38666 offsets / frame
          
                 5% =     9 offsets / frame  (tile = 1024x1024 offsets)
          5% @60fps =   540 offsets / second (>1/2 tile)
        100% @60fps = 10800 offsets / second (10.546875 tiles)
        
               0.5% =     1 offsets / frame  (tile = 1024x1024 offsets)
          x% @60fps =    60 offsets / second (0.59 tile/sec)
          Therefore: I can tweak the speed by increments of 0.59 tiles/sec
        
                 5% =    10 offsets / frame  (tile = 1024x1024 offsets)
          5% @60fps =   600 offsets / second (>1/2 tile/sec)
        100% @60fps = 12000 offsets / second (11.71875 tiles/sec)
        
        Position 16bit unsigned
        1024 >>  7 = 8
        1024 >>  4 = 64
        1024 >> 10 = 1
        1024 = 10 bits, leaving 6 bits, or leaving 5 bits plus sign
        5 bits = 32 tiles + sign
        6 bits = 64 tiles
        
        Wrap-around Board is 32 tiles wide.  Current code:
        if(muncher.pos.x<-63)     muncher.pos.x += (32<<6);  // tunnel left wraparound, appear right
        if(muncher.pos.x>(32<<6)) muncher.pos.x -= (32<<6);  // tunnel right wraparound, appear left
        
        Height is 31 (29 without top and bottom border)
        Height is actually 36, to include target tiles



Move one pixel
   If in new tile, check if should turn blue and reverse
   
   whenever ghosts change from Chase or Scatter to any other mode,
   they are forced to reverse direction as soon as they enter the next tile
   Even if they're in the house.  They'll just go left as usual, then right when entering a new tile


*/


#ifdef dontrunthis
/*
Definitions:

      Muncher: The circle character on the screen
       Player: The person controlling the Eater
      Spectre: Category term for bad guy
        Frame: 1/30th of 30fps
        Pixel: A point on the LCD
       Offset: Sub-tile positioning (a TILE is made up of 64x64 offsets)
         Tile: 64x64 offset when describing map, rendered as 5x5 pixels on screen
       Sprite: 7x7 pixel image of muncher or enemies or fruit
       Screen: 144x168 pixels
    Map/Board: 28x31 tiles
  Fruit/Bonus:
          Dot: single pixel Eater eats.  worth 10 points
     SuperDot: 4 Large dots, begins CHASEmode/BLUEmode

   Tile  sizes are 5px x 5px
  Sprite sizes are 7px x 7px
    Whole board is 28 tiles wide x 31 tiles tall  
  Meaning the tile a spectre or muncher is on = position / 64 = position >> 6
    
  
  Speedy: Red Spectre
  Name2
  Name3
  Name4
  
  CHASEmode
  NORMALmode
  SCATTERmode
  othermodes







Begin:

  BeginIntroScene(){
    Point root layer drawing to IntroDrawing()
    Point buttons to IntroButtons()
    IntroLoop()
  }
  IntroDrawing() {
    DrawBlack()
  }
  IntroButtons() {
    Back = Exit App
    Up/Dn/Sl = Stop IntroLoop Timer;  BeginMainMenuScene(SkipMenuPopup)
  }
  IntroLoop() {
    
    BeginMainMenuScene(StartMenuPopup)
    Set Timer to run IntroLoop()
  }


// at this point, the background is black
  BeginMainMenuScene(BeginMenuMode) {
    Point buttons to MainMenuButtons()
    Point root layer drawing to MainMenuDrawing()
    MainMenuLoop(MenuMode)
  }
  MainMenuButtons() {
    Up, Sel, Down = 
    If MenuMode = Establishing BG or PopIn, set MenuMode to InMenu
    Back = ExitApp()
  }
  MainMenuLoop(*data) {
  
  }
  MainMenuDrawing() {
    If MenuMode = Establishing
      Draw background however
      If background is done Set MenuMode to PopIn
    if PopIn
    
  }
Pop-In Menu()

Pop-Out Menu()

Start by pointing window draw routine to Intro

GameMode:
  0 BGpattern
  1 pop in menu
  2 menu up
  3 pop out menu
  4 

draw order:
bg pattern
then board
then closing pattern
then popup menu

Start muncher moving left
DRAWBOARD
switch(boardmode)
 ingame or normal
   draw board (normal)
   draw dots (flash dots)
   draw spectres
   draw
 playerstart(currentplayer)
   draw board
  playerready:
  
  won:
    draw board (flash color)
  dying:
    draw board
    draw dots (flashing)
    draw muncher (dying frame)
  gameover:
    draw board
    draw dots
    draw 
 

for spectre=0 to 3
If spectre state=normal
  Draw spectre as 1bit with ghostcolor
  DrawEyes(spectre.face)
else if spectre.state = dead
  DrawEyes(spectre.face)
else if spectre.state = Scare
  Draw blue spectre
end if

LCD LCDs:
 1  168  144
 2   84   72
 3   56   48
 4   42   36
 6   28   24
 7   24     
 8   21   18
 9        16
12   14   12

// =========================================================================================================== //
//TODO:
//      Eater is sprite on screen. Has temporary variables:
//        speed
//        pos
//        facing
//        frame
//        
//      Player should be human controlling Eater and has:
//        score
//        lives
//        current level
//        which dots remain
// =========================================================================================================== //

  ************************************************
  Notes
  ************************************************
  
  How positioning works:
  Each tile is 64x64
  Meaning the tile a spectre or muncher is on = position / 64 = position >> 6
   Tile  sizes are 5px x 5px
  Sprite sizes are 7px x 7px
  Whole board is 28 tiles wide x 31 tiles tall
  -- maybe change this to 28w x 36h (though 32wide, technically for wrap araound tunnels)
  
  
  
  
  Use JavaScript for Phone Storage for level layout?
  If no phone connection, use default level
  Persistent Storage for Hi-Score
  Future versions might have different game modes:
    Classic Arcade
    Item Collection (Use buttons to use items)
      - Bomb ("Eats" all ghosts)
      - Save Power Pellet for use anytime
      - Each dot = $1?
    Race the clock
    Dozens of ghosts
    multiple pebmen (Multiball)
      all controlled the same, eat each other
    Goal to eat specific tile, then board resets
    
    Begin:
    One Window
    Moving Textured Background
    Floating Window Menu:
    Title: "Pixel Muncher [Picture?]"
    Main Menu:
      Options (SL Flips to Options Menu)
      > Start / Continue (Depending on Persistant Storage)
      Players: 1/2/3/4 (SL to change)
      High Scores (SL Flips to HS Menu)
      
    While Playing:
    Accelerometer to control joystick
    Press Down to recenter joystick
    Press Back to pause/main menu
      Back again to exit
      Up/Down/Select to choose options
        Resume (default)
        Restart
        
    Back pauses/brings up floating menu
    
        
  Program Flow
  ------------
Version 1:
  1. Init
    A. Map initialized by static array
    B. Player/Ghost init positions hardcoded
    C. Init score, 1 player only
  2. Main Menu
    A. Begin Game
    B. Options
      - All Vibrations on/off
      - Death Vibration
      - "Jaws" vibration music
      - Siren vibration music
      - Sound Effects vibration
      - One/Two Player
      - Speed: 1/4x, 1/2x, 1x, 2x, 4x
    C. 
  3. Begin Game was selected
    A. Init
      - Create 2nd window
      - Init Map
      - Init Ghosts
      - Init Players
      


Version 2: (Level Editor)
  1. Init
    A. Load Map
      - Create Array
      - Read ROM, save into array
      - Draw into Bitmap (Without Dots)
    B. Init Player/Ghost positions
      - Read Map ROM for Player/Ghost positions (default facing left)
      
  


// static int8_t boardlayout[MAP_W * MAP_H] =
// {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
//  -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,-1,
//  -1, 1,-1,-1,-1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1,-1,-1, 1,-1,
//  -1, 2,-1, 0, 0,-1, 1,-1, 0, 0, 0,-1, 1,-1,-1, 1,-1, 0, 0, 0,-1, 1,-1, 0, 0,-1, 2,-1,
//  -1, 1,-1,-1,-1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1,-1,-1, 1,-1,
//  -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,-1,
//  -1, 1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1, 1,-1,
//  -1, 1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1, 1,-1,
//  -1, 1, 1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1, 1, 1,-1,
//  -1,-1,-1,-1,-1,-1, 1,-1,-1,-1,-1,-1, 0,-1,-1, 0,-1,-1,-1,-1,-1, 1,-1,-1,-1,-1,-1,-1,
//   0, 0, 0, 0, 0,-1, 1,-1,-1,-1,-1,-1, 0,-1,-1, 0,-1,-1,-1,-1,-1, 1,-1, 0, 0, 0, 0, 0,
//   0, 0, 0, 0, 0,-1, 1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1,-1, 1,-1, 0, 0, 0, 0, 0,
//   0, 0, 0, 0, 0,-1, 1,-1,-1, 0,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1, 1,-1, 0, 0, 0, 0, 0,
//  -1,-1,-1,-1,-1,-1, 1,-1,-1, 0,-1, 0, 0, 0, 0, 0, 0,-1, 0,-1,-1, 1,-1,-1,-1,-1,-1,-1,
//   0, 0, 0, 0, 0, 0, 1, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
//  -1,-1,-1,-1,-1,-1, 1,-1,-1, 0,-1, 0, 0, 0, 0, 0, 0,-1, 0,-1,-1, 1,-1,-1,-1,-1,-1,-1,
//   0, 0, 0, 0, 0,-1, 1,-1,-1, 0,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1, 1,-1, 0, 0, 0, 0, 0,
//   0, 0, 0, 0, 0,-1, 1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1,-1, 1,-1, 0, 0, 0, 0, 0,
//   0, 0, 0, 0, 0,-1, 1,-1,-1, 0,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1, 1,-1, 0, 0, 0, 0, 0,
//  -1,-1,-1,-1,-1,-1, 1,-1,-1, 0,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1,-1, 1,-1,-1,-1,-1,-1,-1,
//  -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,-1,
//  -1, 1,-1,-1,-1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1,-1,-1, 1,-1,
//  -1, 1,-1,-1,-1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1, 1,-1,-1,-1,-1, 1,-1,
//  -1, 2, 1, 1,-1,-1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,-1,-1, 1, 1, 2,-1,
//  -1,-1,-1, 1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1, 1,-1,-1,-1,
//  -1,-1,-1, 1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1, 1,-1,-1,-1,
//  -1, 1, 1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1,-1,-1, 1, 1, 1, 1, 1, 1,-1,
//  -1, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,
//  -1, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,-1, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1,-1,
//  -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,-1,
//  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
// };




// ------------------------------------------------------------------------ //
//  Notes
// ------------------------------------------------------------------------ //
#define FONT_KEY_FONT_FALLBACK "RESOURCE_ID_FONT_FALLBACK"
#define FONT_KEY_GOTHIC_18 "RESOURCE_ID_GOTHIC_18"
#define FONT_KEY_GOTHIC_18_BOLD "RESOURCE_ID_GOTHIC_18_BOLD"
#define FONT_KEY_GOTHIC_24 "RESOURCE_ID_GOTHIC_24"
#define FONT_KEY_GOTHIC_24_BOLD "RESOURCE_ID_GOTHIC_24_BOLD"
#define FONT_KEY_GOTHIC_28 "RESOURCE_ID_GOTHIC_28"
#define FONT_KEY_GOTHIC_28_BOLD "RESOURCE_ID_GOTHIC_28_BOLD"
#define FONT_KEY_BITHAM_30_BLACK "RESOURCE_ID_BITHAM_30_BLACK"
#define FONT_KEY_BITHAM_42_BOLD "RESOURCE_ID_BITHAM_42_BOLD"
#define FONT_KEY_BITHAM_42_LIGHT "RESOURCE_ID_BITHAM_42_LIGHT"
#define FONT_KEY_BITHAM_42_MEDIUM_NUMBERS "RESOURCE_ID_BITHAM_42_MEDIUM_NUMBERS"
#define FONT_KEY_BITHAM_34_MEDIUM_NUMBERS "RESOURCE_ID_BITHAM_34_MEDIUM_NUMBERS"
#define FONT_KEY_BITHAM_34_LIGHT_SUBSET "RESOURCE_ID_BITHAM_34_LIGHT_SUBSET"
#define FONT_KEY_BITHAM_18_LIGHT_SUBSET "RESOURCE_ID_BITHAM_18_LIGHT_SUBSET"
#define FONT_KEY_ROBOTO_CONDENSED_21 "RESOURCE_ID_ROBOTO_CONDENSED_21"
#define FONT_KEY_ROBOTO_BOLD_SUBSET_49 "RESOURCE_ID_ROBOTO_BOLD_SUBSET_49"
#define FONT_KEY_DROID_SERIF_28_BOLD "RESOURCE_ID_DROID_SERIF_28_BOLD"


// Used to encapsulate, but became too tedious.  May reinstate
// void     add_points(uint32_t points); // add points to current player
// uint8_t  get_current_player();
// uint8_t  get_lives();
// uint32_t get_score();
// uint8_t  get_level();

// void add_points(uint32_t points) {player[current_player].score+=points;}
// uint32_t get_score() {return player[current_player].score;}
//  uint8_t get_lives() {return player[current_player].lives;}
//  uint8_t get_level() {return player[current_player].level;}
//  uint8_t get_current_player() {return current_player;}
//bool player_is_dead(
//void set_current_player(uint8_t ID) {current_player = ID;}




Pos = Tile + offset
offset=6 bits [0-63]
tile = pos >> 6
offset = pos & 63
5 pixels per tile
64offset per tile


        
100% speed = "20 speed" (20 offset per frame at 30fps)

    Speed: how much + position per frame
----------------------------------------------- 
|      |=Muncher Speed=|=====Spectre Speed====|
|Level | Normal Fright | Normal Fright Tunnel |
|------+---------------+----------------------|
|1     |   16     18   |   15     10      8   |
|2-4   |   18     19   |   17     11      9   |
|5-20  |   20     20   |   19     12     10   |
|21+   |   18     --   |   19     --     10   |
-----------------------------------------------

if eating dot, slow 10% or "2 speed"
eating big dot, slow 30% or "6 speed"

===============================================================================================================================
|                          | Eater   Spectre  Spectre  Elroy 1  Elroy 1  Elroy 2  Elroy 2  | Fright  Fright   Fright  Fright  |
|        Bonus      Bonus  | Normal  Normal   Tunnel   Dots              Dots              | Eater   Spectre  Time     # of   |
| Level  Symbol     Points | Speed   Speed    Speed    Left     Speed    Left     Speed    | Speed   Speed    (sec)   Flashes |
===========================+===============================================================+==================================|
|   1    Cherries    100   |  16      15        8       20       16       10       17      |  18       10       6        5    |
|   2    Strawberry  300   |  18      17        9       30       18       15       19      |  19       11       5        5    |
|   3    Peach       500   |  18      17        9       40       18       20       19      |  19       11       4        5    |
|   4    Peach       500   |  18      17        9       40       18       20       19      |  19       11       3        5    |
|   5    Apple       700   |  20      19       10       40       20       20       21      |  20       12       2        5    |
|   6    Apple       700   |  20      19       10       50       20       25       21      |  20       12       5        5    |
|   7    Grapes     1000   |  20      19       10       50       20       25       21      |  20       12       2        5    |
|   8    Grapes     1000   |  20      19       10       50       20       25       21      |  20       12       2        5    |
|   9    Spaceship  2000   |  20      19       10       60       20       30       21      |  20       12       1        3    |
|  10    Spaceship  2000   |  20      19       10       60       20       30       21      |  20       12       5        5    |
|  11    Bell       3000   |  20      19       10       60       20       30       21      |  20       12       2        5    |
|  12    Bell       3000   |  20      19       10       80       20       40       21      |  20       12       1        3    |
|  13    Key        5000   |  20      19       10       80       20       40       21      |  20       12       1        3    |
|  14    Key        5000   |  20      19       10       80       20       40       21      |  20       12       3        5    |
|  15    Key        5000   |  20      19       10      100       20       50       21      |  20       12       1        3    |
|  16    Key        5000   |  20      19       10      100       20       50       21      |  20       12       1        3    |
|  17    Key        5000   |  20      19       10      100       20       50       21      |   –        –       –        –    |
|  18    Key        5000   |  20      19       10      100       20       50       21      |  20       12       1        3    |
|  19    Key        5000   |  20      19       10      120       20       60       21      |   –        –       –        –    |
|  20    Key        5000   |  20      19       10      120       20       60       21      |   –        –       –        –    |
|  21+   Key        5000   |  18      19       10      120       20       60       21      |   –        –       –        –    |
===============================================================================================================================


=========================================================
 Mode        Level 1      Levels 2–4       Levels 5+  (seconds)
=========================================================
Scatter           7               7               5
 Chase           20              20              20
Scatter           7               7               5
 Chase           20              20              20
Scatter           5               5               5
 Chase           20            1033            1037
Scatter           5            1/60            1/60
 Chase      indefinite      indefinite      indefinite
=========================================================

*/
#endif