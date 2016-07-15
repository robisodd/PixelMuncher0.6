#include "main.h"
Layer *root_layer;
AppTimer* looptimer = NULL;

uint8_t animate = 0;
uint32_t hi_score = 0;
uint16_t totalpellets;

bool game_paused = false;
GameModeType game_mode;
uint8_t mode_counter;         // Used for timing of starting, death and winning
bool animating;               // whether animations are happening or not (will get rid of this variable upon refinements later)

PlayerStruct player;
SpectreStruct spectre[4];
MuncherStruct muncher;
//LevelStruct level[21];
// LevelStruct currentlevel;
uint8_t levelplayerspeed;         // probably replace with level[min(player.level, 21)].playerspeed
// uint32_t bonuspoints[8] = {100, 300, 500, 700, 1000, 2000, 3000, 5000}; // 0:Cherries, 1:Strawberry, 2:Peach, 3:Apple, 4:Grapes, 5:Spaceship, 6:Bell, 7:Key

uint8_t map[MAP_W * MAP_H];

const int32_t face_to_dir[2][4] = {{1, 0, -1, 0}, {0, -1, 0, 1}}; // X then Y
#define face_to_dir_x 0
#define face_to_dir_y 1

int32_t abs32(int32_t x) {return (x ^ (x >> 31)) - (x >> 31);}

void set_mode(uint8_t mode) {
  game_mode = mode;
  mode_counter = 0;
}

void init_player() {
  player.score = 0;
  player.level = 1;
  player.lives = 3;
  for(uint8_t i=0; i<31; i++)
    player.dots[i] = 0; // all dots = uneaten
  //name="Player " & Number
}

void add_points(uint8_t points_to_add) {
  player.score += points_to_add;
  //if(player[current_player].score > hi_score) { }
  // Calculate if monsters come out of their pen
}

void init_muncher() {
  muncher.pos.x   = (14<<9);    // start halfway between 13&14;
  muncher.pos.y   = (23<<9)+256; // start in middle of 23;
  muncher.facing  = 2;          // Facing Left. 
                      // if using angles, then TRIG_MAX_ANGLE/4, or 1<<13; (up so counterclockwise button goes left, clockwise goes right)
  muncher.frame   =  0;        // Full Circle Sprite
  muncher.dir.x   = face_to_dir[face_to_dir_x][muncher.facing]; // Horizontal Component  (-1 = moving left)
  muncher.dir.y   = face_to_dir[face_to_dir_y][muncher.facing]; // Vertical component    (0 = not moving vertically)
//   muncher.mode    = ModeAlive; // Currently alive, but not for long... Muahahaha!
}

void move_muncher() {
  //TODO: tend toward the middle based on speed
  if(muncher.dir.x != 0) {     // if moving horizontally
    if(getmap(muncher.pos.x+(256*muncher.dir.x), muncher.pos.y)<128) { // if not running into a wall  
      muncher.pos.x += (muncher.speed*muncher.dir.x);
      //if(muncher.pos.x<(-32000)) muncher.pos.x -= (32<<9);  // tunnel right wraparound, appear left  // temp fix due to 1024x1024 overflow
      if(muncher.pos.x<-511)     muncher.pos.x += (32<<9);  // tunnel left wraparound, appear right
      if(muncher.pos.x>(32<<9)) muncher.pos.x -= (32<<9);  // tunnel right wraparound, appear left
//       muncher.frame=(muncher.frame+1)&3;
      muncher.frame=(muncher.frame+4)&15;
      muncher.pos.y = ((muncher.pos.y>>9)<<9)+256; // tend toward tile center (currently insta-moves to center)
    } else { // will hit a wall
      muncher.pos.x = ((muncher.pos.x>>9)<<9)+256; // finish moving toward wall, stop at tile center
      muncher.dir.x = 0;
    }
  } else if(muncher.dir.y !=0) {  // (not moving horizontally) if moving vertically
    if(getmap(muncher.pos.x, muncher.pos.y+(256*muncher.dir.y))<128) { // if not running into a wall  
      muncher.pos.y += (muncher.speed*muncher.dir.y);
//       muncher.frame=(muncher.frame+1)&3;
      muncher.frame=(muncher.frame+4)&15;
      muncher.pos.x = ((muncher.pos.x>>9)<<9)+256; // tend toward tile center
    } else { // hit a wall
      muncher.pos.y = ((muncher.pos.y>>9)<<9)+256; // finish moving toward wall, stop at tile center
      muncher.dir.y = 0;
    }
  } else { // (not moving horizontally, not moving vertically)
    
  }
}

void muncher_eat_dots() {
  //====================================//
  // Eat Dots, Update Score, Slow Speed
  //====================================//
  uint8_t tile = getmap(muncher.pos.x, muncher.pos.y);
  if(tile&8) {                                     // if on a dot
    if(tile&16) {                                    // if dot is a super dot
      muncher.speed -= 6;                              // slow down due to eating superdot
      add_points(50);                                  // add 50 points to score
//       player.totaldots--;
      // [TODO] start CHASE mode
      //for spectre = 1 to 4
      // enqueue mode: scared
      //for(uint8_t i=0; i<4; ++i)
      //  spectre[i].mode=ModeScared;
      // need to have a bluemode flag or a bunker blue mode.
    } else {                                         // else it's a regular dot
      muncher.speed -= 2;                              // slow down due to eating dot
      add_points(10);                                  // add 10 points to score
    }
    setmap(muncher.pos.x, muncher.pos.y, tile&(~24));// remove dot from board
    player.totaldots--;
  }
  
  if(player.totaldots==0)
    set_mode(ModeWinning);
}



void init_spectres() {
  // facing and direction are different as eyes point to where it WILL go
  for(uint8_t i=0; i<4; ++i) {
    spectre[i].pos.x  = (14<<9);    // start halfway between 13&14;
    spectre[i].pos.y  = (14<<9)+256; // start in middle of 14;
    spectre[i].dir.x  = 0;
    spectre[i].dir.y  = 0;
    spectre[i].speed  = 1;
    spectre[i].enqueued_mode = spectre[i].mode   = ModeBunker;
  }
  spectre[0].enqueued_mode = spectre[0].mode   = ModePatrol;
  
  spectre[0].pos.y  = (11<<9)+256; // start in middle of 11;
  spectre[1].pos.x  = (12<<9);    // start halfway between 11&12;
  spectre[3].pos.x  = (16<<9);    // start halfway between 15&16;

  spectre[0].facing = 2;          // Facing Left
  spectre[1].facing = 1;          // Facing Up
  spectre[2].facing = 3;          // Facing Down
  spectre[3].facing = 1;          // Facing Up
  
  spectre[0].dir.x  = -1; // left
  spectre[1].dir.y  = -1; // Up
  spectre[2].dir.y  =  1; // Down
  spectre[3].dir.y  = -1; // Up
  
  spectre[0].color = SPECTRE0_COLOR;
  spectre[1].color = SPECTRE1_COLOR;
  spectre[2].color = SPECTRE2_COLOR;
  spectre[3].color = SPECTRE3_COLOR;
  
  
  //spectre[].targettile.x = 0;
  spectre[0].targettile.x   = (23<<9);
  spectre[0].targettile.y   = (1<<9);
  spectre[1].targettile.x   = (23<<9);
  spectre[1].targettile.y   = (1<<9);
  spectre[2].targettile.x   = (23<<9);
  spectre[2].targettile.y   = (1<<9);
  spectre[3].targettile.x   = (23<<9);
  spectre[3].targettile.y   = (1<<9);

    
  //Enemy Init:
	//Start Positions
	//Target Coordinates for every mode (attack, regroup, scare)

  //Modes:
  // Bunker
  // Patrol
  // Attack
  // Cruise Elroy
  // Scared (Blue)
  // Dead/Eyes
  
}

void move_spectres() {
  for(uint8_t i=0; i<4; ++i) {
    spectre[i].speed=15*8;//15;
    



    
    // check mode
    // check dot count
    // set speed accordingly for position
    // set CRUISE accordingly

    //check collision
//0-31 = before, 32-63=after
// if previous center dot was before center square and will be after
// then check next whole square (after moving)

    
    /*
    
    Center Position = BottomLeft
    OOO|OOOO
    OOO|OOOO
    OOO|OOOO
    OOO|OOOO
    ---+----
    OOO|OOOO
    OOO|OOOO
    OOO|OOOO
    
    
Whenever a ghost enters a new tile, it looks ahead to the next tile that it will reach, and makes a decision about which direction it will turn when it gets there.
ghosts may never choose to reverse their direction of travel.
there is one exception to this rule, whenever ghosts change from Chase or Scatter to any other mode, they are forced to reverse direction as soon as they enter the next tile.
(when the ghosts leave Frightened mode they do not change direction)
This forced direction-reversal instruction is also applied to any ghosts still inside the ghost house,
so a ghost that hasn’t yet entered the maze by the time the first mode switch occurs will exit the ghost house with a “reverse direction as soon as you can” instruction already pending.
This causes them to move left as usual for a very short time, but they will almost immediately reverse direction and go to the right instead.
Ghosts typically move to the left once they get outside, but if the system changes modes one or more times when a ghost is inside, that ghost will move to the right instead of the left upon leaving the house

twice as fast (same speed as the disembodied eyes 

Turbo Mode
Each game has an alternate mode called Turbo (a.k.a. speedy mode). This is a popular hardware modification of the game found in many of the original arcade cabinets. In this mode, Pac-Man travels about twice as fast (same speed as the disembodied eyes of the ghosts) and is not slowed down when eating pellets.
source: http://pacman.shaunew.com/

  //Modes:
  // Bunker
  // Leaving Bunker
  // Entering Bunker
  // Patrol
  // Attack
  // Cruise Elroy
  // Scared (Blue)
  // Dead/Eyes
  
fill speedbucket with speed based on mode and position
while(speedbucket>0) {
  OldPosition=Position
  Position += XYSpeed                     // (1 pixel movement)
  speedbucket--;
  If(tile(OldPosition)<>tile(position)) { // if entered new tile
    Position += XYSpeed*speedbucket       // Finish Off speedbucket
    If(NewMode<>Mode) {                   // If Mode Changed
      If(Mode==Chase or Scatter){  //chase-to-scatter, chase-to-frightened, scatter-to-chase, and scatter-to-frightened
        Facing=Reversed
      }
      Mode=NewMode
      

    } else {
    
    }
    Based on mode, do the following:
    Facing = CalculateNewDecisionXY(TargetTile)
    Update Facing
  }
  
  if(position=middle) {
    XYspeed=Facling
  }
}



Move(Direction) {
    //If(Facing Left or Right and Y=Center) DirY=0
    //If(Facing Up   or Down  and X=Center) DirX=0
    //add DirX
    //add DirY

    If(Facing Left or Right) {
      If(Y<>Center) {
        PosY += DirY  // If goes beyond center, take remainder and add it to X*DirX
      } else {
        DirY=0  // maybe not necessary
        PosX += DirX
      }
    } else {
      If(X<>Center) {
        PosX += DirX  // If goes beyond center, take remainder and add it to Y*DirY
      } else {
        DirX=0  // maybe not necessary
        PosY += DirY
      }
    }
}

PositionXY = Move(DirectionXY)
If(InANewTile){
  If(ReverseQueue) {
    DirectionXY = Backwards(DirectionXY)
  } else {
    DirectionXY = DecisionXY
  }
  Mode = CalculateMode(board.spectremode)
  DecisionXY = CalculateNewDecisionXY(TargetTile)
  Pupils = DecisionXY
}

TargetTile XY: Not px but tile (/64) target
Position XY: 64px/tile position
DirectionXY: Direction currently traveling
             If X<>center, add DirX
             If Y<>center, add DirY
DecisionXY: Upon entering new tile, DirectionXY=DecisionXY, then calculate new DecisionXY based on TargetTile
            Pupils = DecisionXY
Reverse Queue:  Upon entering new tile, if ReverseQueue=1, Throw out DecisionXY, DirectionXY=-DirectionXY, calculate new DecisionXY based on TargetTile
                If system changes modes while inside bunker, move right instead of left after leaving
Mode Change Queue: (Changes modes only upon entering new tile)

    
    */
    
     //if ((spectre[i].pos.x>>5)&1)
    if(spectre[i].mode==ModeBunker) {
      if(getmap(spectre[i].pos.x+(512*spectre[i].dir.x), spectre[i].pos.y+(512*spectre[i].dir.y))<128) { // if not running into a wall  
        spectre[i].pos.x += (spectre[i].speed*spectre[i].dir.x);
        spectre[i].pos.y += (spectre[i].speed*spectre[i].dir.y);
      } else { // will hit a wall
          //spectre[i].pos.y -= ((spectre[i].dir.y*64*1)/5);
          //spectre[i].pos.y -= (spectre[i].dir.y*32);
          spectre[i].facing = (spectre[i].facing  + 2) & 3; // reverse face
          spectre[i].dir.y *= -1;  // reverse direction
          //spectre[i].dir.y = 0;
        } 
    } else {
      if(getmap(spectre[i].pos.x+(257*spectre[i].dir.x), spectre[i].pos.y+(257*spectre[i].dir.y))<128) { // if not running into a wall  
        spectre[i].pos.x += (spectre[i].speed*spectre[i].dir.x);
        spectre[i].pos.y += (spectre[i].speed*spectre[i].dir.y);
        if(spectre[i].pos.x<-511)     spectre[i].pos.x += (32<<9); // tunnel left wraparound
        if(spectre[i].pos.x>(32<<9)) spectre[i].pos.x -= (32<<9); // tunnel right wraparound
        //spectre[i].frame=(spectre[i].frame+1)&3;
        
        if(spectre[i].mode != ModeBunker) {
          if(spectre[i].dir.x == 0)    // (not moving horizontally) if moving vertically
            spectre[i].pos.x = (spectre[i].pos.x&(~511))+256; // tend toward tile center
          if(spectre[i].dir.y == 0)     // if moving horizontally
            spectre[i].pos.y = (spectre[i].pos.y&(~511))+256; // tend toward tile center
        }
      } else { // will hit a wall
//         if(spectre[i].mode==ModeBunker) {
//           //spectre[i].pos.x -= (spectre[i].dir.x*32);
//           //spectre[i].facing = (spectre[i].facing  + 2) & 3;
//           //spectre[i].dir.x = 0;// *= -1;
          
//           //spectre[i].pos.y -= ((spectre[i].dir.y*64*1)/5);
//           //spectre[i].pos.y -= (spectre[i].dir.y*32);
//           spectre[i].facing = (spectre[i].facing  + 2) & 3; // reverse face
//           spectre[i].dir.y *= -1;  // reverse direction
//           //spectre[i].dir.y = 0;        
//         }
        if(spectre[i].mode==ModePatrol) {
          spectre[i].facing = (spectre[i].facing + (rand()%3))&3;
          spectre[i].dir.x = face_to_dir[0][spectre[i].facing];
          spectre[i].dir.y = face_to_dir[1][spectre[i].facing];
          //spectre[i].dir.x = 0;// *= -1;
        }
      }
    }

    
//     if(spectre[i].dir.x != 0) {     // if moving horizontally
//       if(getmap(spectre[i].pos.x+(33*spectre[i].dir.x), spectre[i].pos.y)>=0) { // if not running into a wall  
//         spectre[i].pos.x += (spectre[i].speed*spectre[i].dir.x);
//         if(spectre[i].pos.x<-63)     spectre[i].pos.x += (32<<6); // tunnel left wraparound
//         if(spectre[i].pos.x>(32<<6)) spectre[i].pos.x -= (32<<6); // tunnel right wraparound
//         //spectre[i].frame=(spectre[i].frame+1)&3;
//         //spectre[i].pos.y = ((spectre[i].pos.y>>6)<<6)+32; // tend toward tile center
//         spectre[i].pos.y = (spectre[i].pos.y&(~63))+32; // tend toward tile center
//       } else { // will hit a wall
//         spectre[i].pos.x = (spectre[i].pos.x&(~63))+32; // finish moving toward wall, stop at tile center
//         if(spectre[i].mode==ModeBunker) {
//           //spectre[i].pos.x -= (spectre[i].dir.x*32);
//           //spectre[i].facing = (spectre[i].facing  + 2) & 3;
//           //spectre[i].dir.x = 0;//*= -1;
//         }
//         if(spectre[i].mode==ModePatrol) {
//           spectre[i].facing = (spectre[i].facing + (rand()%3))&3;
//           spectre[i].dir.x = face_to_dir[0][spectre[i].facing];
//           spectre[i].dir.y = face_to_dir[1][spectre[i].facing];
//           //spectre[i].dir.x = 0;//*= -1;
//         }
//       }
//     } else if(spectre[i].dir.y !=0) {  // (not moving horizontally) if moving vertically
//       if(getmap(spectre[i].pos.x, spectre[i].pos.y+(33*spectre[i].dir.y))>=0) { // if not running into a wall  
//         spectre[i].pos.y += (spectre[i].speed*spectre[i].dir.y);
//         //spectre[i].frame=(spectre[i].frame+1)&3;
//         if(spectre[i].mode != ModeBunker)
//           spectre[i].pos.x = ((spectre[i].pos.x>>6)<<6)+32; // tend toward tile center
//       } else { // hit a wall
//         spectre[i].pos.y = ((spectre[i].pos.y>>6)<<6)+32; // finish moving toward wall, stop at tile center
//         if(spectre[i].mode==ModeBunker) {
//           //spectre[i].pos.y -= ((spectre[i].dir.y*64*1)/5);
//           //spectre[i].pos.y -= (spectre[i].dir.y*32);
//           spectre[i].facing = (spectre[i].facing  + 2) & 3; // reverse face
//           spectre[i].dir.y *= -1;  // reverse direction
//           //spectre[i].dir.y = 0;
//         }
        
//         if(spectre[i].mode==ModePatrol) {
//           spectre[i].facing = (spectre[i].facing + (rand()%3))&3;
//           spectre[i].dir.x = face_to_dir[0][spectre[i].facing];
//           spectre[i].dir.y = face_to_dir[1][spectre[i].facing];
//           //spectre[i].dir.x = 0;//*= -1;
//         }

        
//       }
//     } else { } // (not moving horizontally, not moving vertically)
    
  }
}

// =========================================================================================================== //
//  Map and Level Variables
// ======================= //
//TODO:
// Init Map and Init Dots
// initial_dots will be just 36(?) 32bit ints, each bit will be 1 or 0 and the two 2s (superdots) will be done in function
// initial_map will also be just 1bit, converted to -1 or 0 in MAP[]
// player[].dots[] will be like initial_dots and use same function to copy to MAP[]

/*  Updated Map Data:
Map is now folded in half
bit 76543210
    xxxxxxxx = uint8_t
bit 0: 
bit 1: 
bit 2: 
bit 3: pellet
bit 4: power pellet
bit 5: spectre can't go up
bit 6: spectre goes slowly
bit 7: impassable (>127 means impassable)

Maybe bits 012 will be for 3D floor/ceiling squaretype

When bit 7 is on, the other bits change meaning:
bits 0-6: squaretype [0-127] (for 3D or 2D)
AND/OR bits show which tiles to build map out of (straight, corner, etc)

  1
  2631
  84268421
0b10000xxx = 128 = Impassible/Wall
0b11000xxx = 192 = Door(Impassible/Slow)
0b00000xxx =   0 = Normal Blank
0b00001xxx =   8 = Normal Dot
0b00011xxx =  24 = Normal BigDot
0b01000xxx =  64 = Slow Blank
0b01001xxx =  72 = Slow Dot
0b00100xxx =  32 = Can't go Up Blank
0b00101xxx =  40 = Can't go Up Dot
*/

static int8_t boardlayout[] = {
  128,128,128,128,128,128,128,128,128,128,128,128,128,128,
  128,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,128,
  128,  8,128,128,128,128,  8,128,128,128,128,128,  8,128,
  128, 24,128,  0,  0,128,  8,128,  0,  0,  0,128,  8,128,
  128,  8,128,128,128,128,  8,128,128,128,128,128,  8,128,
  128,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
  128,  8,128,128,128,128,  8,128,128,  8,128,128,128,128,
  128,  8,128,128,128,128,  8,128,128,  8,128,128,128,128,
  128,  8,  8,  8,  8,  8,  8,128,128,  8,  8,  8,  8,128,
  128,128,128,128,128,128,  8,128,128,128,128,128,  0,128,
    0,  0,  0,  0,  0,128,  8,128,128,128,128,128,  0,128,
    0,  0,  0,  0,  0,128,  8,128,128,  0,  0, 32, 32, 32,
    0,  0,  0,  0,  0,128,  8,128,128,  0,128,128,128,192,
  128,128,128,128,128,128,  8,128,128,  0,128, 64, 64, 64,
   64, 64, 64, 64, 64,  0,  8,  0,  0,  0,128, 64, 64, 64,
  128,128,128,128,128,128,  8,128,128,  0,128, 64, 64, 64,
    0,  0,  0,  0,  0,128,  8,128,128,  0,128,128,128,128,
    0,  0,  0,  0,  0,128,  8,128,128,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,128,  8,128,128,  0,128,128,128,128,
  128,128,128,128,128,128,  8,128,128,  0,128,128,128,128,
  128,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,128,
  128,  8,128,128,128,128,  8,128,128,128,128,128,  8,128,
  128,  8,128,128,128,128,  8,128,128,128,128,128,  8,128,
  128, 24,  8,  8,128,128,  8,  8,  8,  8,  8, 40, 40, 32,
  128,128,128,  8,128,128,  8,128,128,  8,128,128,128,128,
  128,128,128,  8,128,128,  8,128,128,  8,128,128,128,128,
  128,  8,  8,  8,  8,  8,  8,128,128,  8,  8,  8,  8,128,
  128,  8,128,128,128,128,128,128,128,128,128,128,  8,128,
  128,  8,128,128,128,128,128,128,128,128,128,128,  8,128,
  128,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
  128,128,128,128,128,128,128,128,128,128,128,128,128,128
};


void save_dots(uint8_t *dots) {
  // obsolete until fixed to use the new boardlayout[]
//   uint8_t bit = 0; // because 8 bits, can't have more than 256 total dots

//   // convert -1,0,1,2 to 0,1,2,3
//   for(uint16_t i=0; i<MAP_H*MAP_W; i++)
//     map[i]++;

//   for(uint16_t y=0, row=0; y<MAP_H*MAP_W; y+=MAP_W, row++)
//     for(uint16_t x=0; x<(MAP_W/2); x++) {  // map_w has to <= 32, else bitshift below breaks
//       if(((BoardLayout[row] >> (x*2))&3) > 1) { // if dot is supposed to exist
//         if(map[y+x]>1) {
//           //dots[(bit>>3)] &= 1(bit&7))&1
//         }
//         if(map[y + MAP_W - 1 - x]>1) {
          
//         }
//       }
//       map[y+x]               = (BoardLayout[row] >> (x*2))&3;
//       map[y + MAP_W - 1 - x] = (BoardLayout[row] >> (x*2))&3; // right 16 tiles = left side horizontally flipped
//       if(map[y+x] > 1) {  // 2(0b10) = dot, 3(0b11) = bigdot
//         if((dots[(bit>>3)]>>(bit&7))&1) // 1 = eaten
//           map[y+x] = 1;                 // 1 = blank
//         bit++;
//         if((dots[(bit>>3)]>>(bit&7))&1) // 1 = eaten
//           map[y + MAP_W - 1 - x] = 1;   // 1 = blank
//         bit++;
//       }
//     }

}

uint8_t load_dots(uint8_t *dots) {
 // note: in dots[], each bit is 0 if dot still exists or 1 if already eaten
 // init player[].dots[] all to 0 to signify full board (0 = dot there in dots[])
  uint8_t totaldots=0;
  uint8_t bit = 0; // because 8 bits, can't have more than 256 total dots
  for(uint16_t y=0, y2=0; y<MAP_H*MAP_W; y+=MAP_W, y2+=(MAP_W/2))
    for(uint16_t x=0; x<(MAP_W/2); x++) {  // map_w has to <= 32, else bitshift below breaks
      uint16_t pos = y + x;
      map[pos]               = boardlayout[y2 + x];
      map[y + MAP_W - 1 - x] = boardlayout[y2 + x]; // right 16 tiles = left side horizontally flipped
      if((map[pos] & 24)>0) {  // if map pos was a dot or bigdot
        if((dots[(bit>>3)]>>(bit&7))&1)  // if that dot was eaten (when dots[]=1, it means it was eaten)
          map[pos] &= ~24;               // remove dot
        else
          totaldots++;
        bit++;
        if((dots[(bit>>3)]>>(bit&7))&1)  // if that dot was eaten (when dots[]=1, it means it was eaten)
          map[y + MAP_W - 1 - x] &= ~24; // remove dot
        else
          totaldots++;
        bit++;
      }
    }
  return totaldots;
}

void init_board() {
//   for(uint16_t i=0; i<MAP_W*MAP_H; i++) map[i] = boardlayout[i];

  player.totaldots = load_dots((uint8_t*)&player.dots);
  
/*  
  for(uint16_t y=0; y<MAP_H*MAP_W; y+=MAP_W)
   for(uint16_t x=0; x<(MAP_W/2); x++) {
     map[y+x] = boardlayout[(y/2)+x];
     map[y + MAP_W - 1 - x] = boardlayout[(y/2)+x];
   }
*/
  levelplayerspeed = 8*19;//100;//19;  // Default 100% speed -- level speed will replace this
  
}

uint8_t getlevelspeed(uint8_t level) {
  return levelplayerspeed;
}

uint8_t getmap(int32_t x, int32_t y) {
  x>>=9; y>>=9;
  if(y==14 && (x<0 || x>=MAP_W)) return 0;
  return (x<0 || x>=MAP_W || y<0 || y>=MAP_H) ? -1 : map[(y * MAP_W) + x];
}

void setmap(int32_t x, int32_t y, int8_t data) {
  x>>=9; y>>=9;
  if(x>=0 && x<MAP_W && y>=0 && y<MAP_H)
    map[(y * MAP_W) + x]=data;
}

// =========================================================================================================== //
//  Control Input Options
// ======================= //
bool up_button_depressed = false; // Whether Pebble's   Up   button is being held
bool dn_button_depressed = false; // Whether Pebble's  Down  button is being held
bool sl_button_depressed = false; // Whether Pebble's Select button is being held
bool bk_button_depressed = false; // Whether Pebble's  Back  button is being held



// ------------------------------------------------------------------------ //
//  Button Pushing
// ------------------------------------------------------------------------ //
void up_push_in_handler(ClickRecognizerRef recognizer, void *context) {up_button_depressed = true;  }  //   UP   button was pushed in
void up_release_handler(ClickRecognizerRef recognizer, void *context) {up_button_depressed = false; }  //   UP   button was released
void dn_push_in_handler(ClickRecognizerRef recognizer, void *context) {dn_button_depressed = true;  }  //  DOWN  button was pushed in
void dn_release_handler(ClickRecognizerRef recognizer, void *context) {dn_button_depressed = false; }  //  DOWN  button was released
void sl_push_in_handler(ClickRecognizerRef recognizer, void *context) {sl_button_depressed = true;  }  // SELECT button was pushed in
void sl_release_handler(ClickRecognizerRef recognizer, void *context) {sl_button_depressed = false; }  // SELECT button was released
void bk_click_handler  (ClickRecognizerRef recognizer, void *context) {bk_button_depressed = true;  }  //  BACK  button was clicked (BACK doesn't support raw)

void game_click_config_provider(void *context) {
  window_raw_click_subscribe(BUTTON_ID_UP, up_push_in_handler, up_release_handler, context);
  window_raw_click_subscribe(BUTTON_ID_DOWN, dn_push_in_handler, dn_release_handler, context);
  window_raw_click_subscribe(BUTTON_ID_SELECT, sl_push_in_handler, sl_release_handler, context);
  window_single_click_subscribe(BUTTON_ID_BACK, bk_click_handler);
}


// ------------------------------------------------------------------------ //
//  Get Joystick, Update Eater
// ------------------------------------------------------------------------ //
// uint16_t   turning_at = 32*32;  // Stops turning every frame when button is held
void update_movement_via_joystick() {
  static int32_t   turning_at = 256*256;  // Stops turning every frame when button is held
  //if pushing SL
  //  reverse
  //else
  //  if can go ccw and pushing UP then
  //    if can go cw and pushing DN then
  //      don't move
  //    else
  //      go ccw(up)
  //  else
  //    if can go cw(down) and pushing down
  //      go cw(down)

  GPoint testfacing = GPoint(0, 0);
  if(sl_button_depressed) {            // Reverse
    //set_mode(ModeWinning);// test winning flashing animation
    sl_button_depressed = false;       // Stop reversing
    muncher.facing = (muncher.facing+2)&3;
    muncher.dir.x = face_to_dir[0][muncher.facing];
    muncher.dir.y = face_to_dir[1][muncher.facing];
  } else if(((muncher.pos.x>>9) * (muncher.pos.y>>9)) != turning_at) {
    turning_at = 256*256;
    testfacing.x = (muncher.facing+1)&3; // Left Turn
    testfacing.y = (muncher.facing+3)&3; // Right Turn

    if(getmap(muncher.pos.x+(512*face_to_dir[0][testfacing.x]), muncher.pos.y+(512*face_to_dir[1][testfacing.x]))<128 && up_button_depressed) {
      if(getmap(muncher.pos.x+(512*face_to_dir[0][testfacing.y]), muncher.pos.y+(512*face_to_dir[1][testfacing.y]))<128 && dn_button_depressed) {
      } else {
        //go ccw(up)
        muncher.facing = testfacing.x;
        muncher.dir.x = face_to_dir[0][muncher.facing];
        muncher.dir.y = face_to_dir[1][muncher.facing];
        turning_at = ((muncher.pos.x>>9) * (muncher.pos.y>>9));
      }
    } else {
      if(getmap(muncher.pos.x+(512*face_to_dir[0][testfacing.y]), muncher.pos.y+(512*face_to_dir[1][testfacing.y]))<128 && dn_button_depressed) {
        //go cw(down)
        muncher.facing = testfacing.y;
        muncher.dir.x = face_to_dir[0][muncher.facing];
        muncher.dir.y = face_to_dir[1][muncher.facing];
        turning_at = ((muncher.pos.x>>9) * (muncher.pos.y>>9));
      }
    }
  }
}


void check_collisions() {
  for(uint8_t i=0; i<4; ++i) {
    if(((spectre[i].pos.x>>9)==(muncher.pos.x>>9)) && ((spectre[i].pos.y>>9)==(muncher.pos.y>>9))) {
      set_mode(ModeDying);
//       muncher.mode = ModeDead;
    }
    
  }
  // Check Fruit Collision
  
}

void pause_game() {
  game_paused = true;
  bk_button_depressed = sl_button_depressed = up_button_depressed = dn_button_depressed = false;  // to stop auto-unpause
}


void iterate_one_frame() {
  muncher.speed = getlevelspeed(player.level);
  muncher_eat_dots();  // Reduce muncher speed if a dot is eaten
  update_movement_via_joystick();
  move_muncher();
  move_spectres();
  check_collisions();

 // muncher.speed = 100;//percent;getlevelspeed(player.level);

  for(int i = 0; i < 2; ++i) {  // 2 60Hz frames per 30FPS render
//     update_movement_via_joystick();
//     move_muncher2();
  }
  
//              5% =    10 offsets / frame  (tile = 1024x1024 offsets)
//       5% @60fps =   600 offsets / second (>1/2 tile/sec)
//     100% @60fps = 12000 offsets / second (11.71875 tiles/sec)

  
//   uint16_t speedbucket = 0;
//   speedbucket = muncher.speed = getlevelspeed(player.level);  // fill muncher speed bucket
//   for(uint8_t i=0; i<4; ++i) {                  // fill spectre speed bucket
//     spectre[i].speed = 15;
//     speedbucket += spectre[i].speed;
//   }
  
//   while(speedbucket>0) {
    
    
    
//   }

// fill speedbucket with speed based on mode and position
// while(speedbucket>0) {
//   OldPosition=Position
//   Position += XYSpeed                     // (1 pixel movement)
//   speedbucket--;
//   If(tile(OldPosition)<>tile(position)) { // if entered new tile
//     Position += XYSpeed*speedbucket       // Finish Off speedbucket
//     If(NewMode<>Mode) {                   // If Mode Changed
//       If(Mode==Chase or Scatter){  //chase-to-scatter, chase-to-frightened, scatter-to-chase, and scatter-to-frightened
//         Facing=Reversed
//       }
//       Mode=NewMode
      

//     } else {
    
//     }
//     Based on mode, do the following:
//     Facing = CalculateNewDecisionXY(TargetTile)
//     Update Facing
//   }
  
//   if(position=middle) {
//     XYspeed=Facling
//   }
// }

//   muncher_eat_dots();  // Reduce muncher speed if a dot is eaten
//   update_movement_via_joystick();
//   move_muncher();
//   move_spectres();
//   check_collisions();
  
  
  
}

// ------------------------------------------------------------------------ //
//  Main Loop Functions
// ------------------------------------------------------------------------ //
static void gameloop(void *data) {
  looptimer = NULL;
  if(game_paused) {
    // TODO: Make a menu and selecting resume unpauses. Other menu options like "new game" unpauses too
    if(bk_button_depressed) window_stack_pop_all(true); // back = quit (TODO: Save State)
    if(sl_button_depressed || up_button_depressed || dn_button_depressed) {  // any button press unpauses
      game_paused = false;
      //sl_button_depressed = up_button_depressed = dn_button_depressed = false;  // Make sure unpausing does't reverse or turn you
    }
  } else {
    // Handle Buttons and changing game mode
    switch(game_mode) {
      case ModeNewLifeBegin:// Just like ModeLevelBegin, but subtracts a life
        animating = false;
        if(bk_button_depressed) {
          pause_game();   // back = pause (TODO: Menu)
        } else {
          mode_counter++;
          if(mode_counter>16*4) {
            player.lives--;
            set_mode(ModeRoundBegin);
          }
        }
      break;


      case ModeLevelBegin:// Very beginning, show "player 1" "ready", no sprites, no animation, pause for 16 counts
        animating = false;
        if(bk_button_depressed) {
          pause_game();   // back = pause (TODO: Menu)
        } else {
          mode_counter++;
          if(mode_counter>16*4) {
            set_mode(ModeRoundBegin);
          }
        }
      break;

      case ModeRoundBegin: // Show board, ready, spectres, muncher, pause for 16 counts, change mode to Playing
        animating = false;
        if(bk_button_depressed) {
          pause_game();   // back = pause (TODO: Menu)
        } else {
          mode_counter++;
          if(mode_counter>16*4) {
            animating = true;
            set_mode(ModePlaying);
          }
        }
      break;

      case ModeGameOver: // Dead, no lives left.  Only back button to bring up menu to restart or exit
        if(bk_button_depressed) pause_game(); // back = pause (TODO: Menu)
      break;

      case ModeWinning:  // Pause, erase sprites, flash board, then run init (which will change mode to ModeStarting)
        if(bk_button_depressed) {
          pause_game();   // back = pause (TODO: Menu)
        } else {
          mode_counter++;
          if(mode_counter>(32+5*16))  // after 4 flashes, start new level
            init_level();
        }
      break;

      case ModeDying:  // Pause, go through death sequence, check GameOver, else run init (which will change mode to ModeStarting)
        if(bk_button_depressed) {
          pause_game();   // back = pause (TODO: Menu)
        } else {
          mode_counter++;
          if(muncher.frame<19) {  // if death animation has not yet started
            if(mode_counter==30) {  // 30 frame pause before death starts
              muncher.frame = 19;                      // start death animation at frame 0
              muncher.facing = 0;                      // set this to 0 so the math works in the rendering function
              for(uint8_t i=0; i<4; ++i)
                spectre[i].mode = ModeInvisible;       // Remove spectres
              mode_counter=0;
            }
          } else {  // else, death animation has already started -- step through death animation sequence
            if(muncher.frame<27) {   // if still going through animation
              if((mode_counter&3)==0 && mode_counter>30)  // increase animation every 4 frames (pause for 32 at first frame)
                muncher.frame++;
              if(muncher.frame==27)   // if reached the exploding frame
                mode_counter=0;        //   reset the counter
            } else {
              if(mode_counter==8)    // wait 8 frames at the explosion, then make muncher blank
                muncher.frame=253;  // -3 (= 32-3 = 29 = blank)
              if(mode_counter==50) { // wait 50 frames after muncher went blank, then start new round (if enough lives are available)
                if(player.lives==0) {
                  set_mode(ModeGameOver);                      // Game Over
                } else {                                       // Else start next round (keep dots and score the same) TODO: Player Two?
                  player.lives--;
                  init_round();
                }
              }
            }
          }
        }
      break;

      case ModePlaying:  // Most common mode
        if(bk_button_depressed) {
          pause_game();   // back = pause (TODO: Menu)
        } else {
          iterate_one_frame();
        }
      break;

      default:
        // error, bad game mode
      break;
    }
  }

  layer_mark_dirty(root_layer);  // Schedule redraw of screen
  looptimer = app_timer_register(UPDATE_MS, gameloop, NULL); // Finished. Wait UPDATE_MS then loop
}

static void draw_lives(uint8_t *framebuffer) {
  if(player.lives>0) draw_sprite(framebuffer, 4,     3, MUNCHER_COLOR, 38);    // 1 extra life
  if(player.lives>1) draw_sprite(framebuffer, 4+8,   3, MUNCHER_COLOR, 38);    // 2 extra lives
  if(player.lives>2) draw_sprite(framebuffer, 4+8+8, 3, MUNCHER_COLOR, 38);    // 3 extra lives
  if(player.lives>3) draw_sprite(framebuffer, 4+8+8+8, 3, MUNCHER_COLOR, 38);  // 4 extra lives
  if(player.lives>5)
    draw_font8(framebuffer, 4+8+8+8+8, 3, MUNCHER_COLOR, '+');  // More than 5 lives
  else if(player.lives>4)
    draw_sprite(framebuffer, 4+8+8+8+8, 3, MUNCHER_COLOR, 38);  // 5 lives
}

static void draw_score(uint8_t *framebuffer) {
  char text[10];
    snprintf(text, sizeof(text), "%06ld", (long int)player.score);
    draw_font8_text(framebuffer, 8*6, 2, SCORE_COLOR, text);
}

//====================================//
static void game_layer_update(Layer *me, GContext *ctx) {
  //uint8_t *framebuffer = (uint8_t*)*(uint32_t*)ctx;
  GBitmap* framebuffer_bmp = graphics_capture_frame_buffer(ctx);
  if(framebuffer_bmp) {
    uint8_t *framebuffer = gbitmap_get_data(framebuffer_bmp);
  
    draw_background(framebuffer);
    draw_dots(framebuffer, map, animate);
    draw_lives(framebuffer);
    draw_score(framebuffer);
    
    bool drawmuncher = false;
    bool drawspectres = false;
    switch(game_mode) {
      case ModeLevelBegin:
      case ModeNewLifeBegin:
        draw_font8_text(framebuffer, 8*4, 66, PLAYER_TEXT_COLOR, "PLAYER ONE");
        draw_font8_text(framebuffer, 8*6+1, 96, READY_COLOR, "READY!");  // yellow
      break;
      
      case ModeRoundBegin:
        drawmuncher=true;
        drawspectres = true;
        draw_font8_text(framebuffer, 8*6+1, 96, READY_COLOR, "READY!");  // yellow
      break;
      
      case ModeGameOver:
        draw_font8_text(framebuffer, 8*4, 96, GAME_OVER_COLOR, "GAME  OVER");
      break;
      
      case ModeWinning:
        if(mode_counter<32)      // pause for 30 frames, then flash background
          drawspectres = true;
        else
          draw_background_flashing(framebuffer, (mode_counter>>4)&1);
        drawmuncher = true;
      break;
      
      default:
        drawmuncher = true;
        drawspectres = true;
      break;
    }
    
    if(drawmuncher)
      draw_actor(framebuffer, muncher.pos.x, muncher.pos.y, MUNCHER_COLOR, 32 + muncher.frame + muncher.facing);
    

    // Draw Spectres
    if(drawspectres)
     for(uint8_t i=0; i<4; ++i) {
       if(spectre[i].mode!=ModeInvisible) {
         if(spectre[i].mode==ModeScared) {
           draw_actor(framebuffer, spectre[i].pos.x, spectre[i].pos.y, SCARED_BODY_COLOR, 17 + ((spectre[i].animate>>2)&1));              // spectre scared body (animate skirt)
           draw_actor(framebuffer, spectre[i].pos.x, spectre[i].pos.y, SCARED_FACE_COLOR, 19);              // spectre scared face
         } else {
           draw_actor(framebuffer, spectre[i].pos.x, spectre[i].pos.y, spectre[i].color, 17 + ((spectre[i].animate>>2)&1));              // spectre body (animate skirt)
           #ifdef PBL_COLOR
             draw_actor( framebuffer, spectre[i].pos.x, spectre[i].pos.y, SPECTRE_EYE_COLOR, 12 + ((spectre[i].facing)&3));      // spectre eyes
             draw_pupils(framebuffer, spectre[i].pos.x, spectre[i].pos.y, SPECTRE_PUPIL_COLOR,      ((spectre[i].facing)&3));      // spectre pupils
           #else
             // draw b&w pupils
           draw_actor( framebuffer, spectre[i].pos.x, spectre[i].pos.y, 0, 12 + ((spectre[i].facing)&3));      // spectre eyes
           draw_pupils(framebuffer, spectre[i].pos.x, spectre[i].pos.y, 1,      ((spectre[i].facing)&3));      // spectre pupils
           #endif
         }
         spectre[i].animate++;  // Only do this most of the time
         // Don't animate while mode is numbers (while being eaten) or while not moving.
         // in fact, move spectre.animate++ to the movespectre function
       }
     }
    
    if(game_paused) {
      GRect rect = GRect(8*6, 66+8, 8*6+2, 8*3+2);
      fill_rect(framebuffer, rect, 0b11000011);
      draw_font8_text(framebuffer, 8*6+1, 66+16, ((animate>>3)&1) ? 0b11110000 : 0b11111111, "PAUSED");
    }

    graphics_release_frame_buffer(ctx, framebuffer_bmp);
  }
  
  if(animating) animate++;  // to animate flashing bigdots
}
  
// ------------------------------------------------------------------------ //
//  Main Functions
// ------------------------------------------------------------------------ //

// animation off
//player 1
// ready
// 4 counts
// ghosts
//ready
//4 counts
// start, animation on
// gameover = no animations


// winning flash gets rid of ghost door
// ghost animation stops (but dots flash) during eating pause
// new round = ready (no anim)

// Init Round: Start of round at beginning or died and starting new life
void init_round() {
  init_muncher();    // put muncher at start
  init_spectres();   // put spectres at starting positions
  animating = false;
  set_mode(ModeRoundBegin);
}

// Init Level: Start first or new level
void init_level() {
  init_board();  // reset dots
  init_round();  // reset muncher and spectres
  set_mode(ModeLevelBegin);
  animate = 255;  // animation is frozen, but keep big dots lit
}

void init_game(Window *window) {
  root_layer = window_get_root_layer(window);
  layer_set_update_proc(root_layer, game_layer_update);
  
  load_graphics();
  
  window_set_click_config_provider(window, game_click_config_provider);
}

// Start game, also to restart new game
void start_game() {
  init_player();
  init_level();
  set_mode(ModeNewLifeBegin);
  looptimer = app_timer_register(UPDATE_MS, gameloop, NULL); // Finished. Wait UPDATE_MS then loop
}

void destroy_game() {
  if(looptimer) app_timer_cancel(looptimer);
  accel_data_service_unsubscribe();
  //TODO: destroy game layer
  //destroy_graphics();
//  for (uint8_t i=0; i<4; i++) for (uint8_t j=0; j<4; j++) gbitmap_destroy(playersprite[i][j]);
//  gbitmap_destroy(background);
}
