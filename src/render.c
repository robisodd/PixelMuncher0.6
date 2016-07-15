#include "main.h"

// ================================================================================================================================================ //
//  Graphical Rendering Functions
// ================================================================================================================================================ //
GBitmap *font8_bmp; uint8_t *font8;
GBitmap *background_bmp; uint8_t *background;
GBitmap *font_sprites_bmp; uint8_t *font_sprites;

// ------------------------------------------------------------------------------------------------------------------------------------------------ //
void load_graphics() {
          font8_bmp = gbitmap_create_with_resource(RESOURCE_ID_FONT8);
     background_bmp = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
   font_sprites_bmp = gbitmap_create_with_resource(RESOURCE_ID_FONT_SPRITES);

              font8 = gbitmap_get_data(font8_bmp);
         background = gbitmap_get_data(background_bmp);
       font_sprites = gbitmap_get_data(font_sprites_bmp);
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
void unload_graphics() {
  gbitmap_destroy(font8_bmp);
  gbitmap_destroy(background_bmp);
  gbitmap_destroy(font_sprites_bmp);
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //

// Fill screen with color.  Note: For Aplite, color should be either 0 or 255. Vertical stripes will appear otherwise.
void fill_screen(GContext *ctx, uint8_t color) {
  memset((uint8_t*)*(uintptr_t*)ctx, color, IF_APLITE_BASALT_CHALK_ELSE(144*168/8, 144*168, 76 + 25792, 0)); // First pixel on PTR doesn't start til framebuffer + 76
}

void draw_sprite8(uint8_t *fb, uint8_t *font, int16_t start_x, int16_t start_y, uint8_t color, uint8_t spr) {  // in B&W, color=0 is black, else white
  uint16_t left   = (start_x <     0) ? (start_x >  -8) ?   0 - start_x : 8 : 0;
  uint16_t right  = (start_x > 144-8) ? (start_x < 144) ? 144 - start_x : 0 : 8;
  uint16_t top    = (start_y <     0) ? (start_y >  -8) ?   0 - start_y : 8 : 0;
  uint16_t bottom = (start_y > 168-8) ? (start_y < 168) ? 168 - start_y : 0 : 8;
  uint8_t    *row = font + (spr&3) + ((spr&252)*8) + (top*4);
  uint16_t y_addr = (start_y + top) * IF_COLOR_BW(144, 20);
  
  for(uint16_t y=top; y<bottom; ++y) {
    for(uint16_t x=left; x<right; ++x) {
      #ifdef PBL_BW
        //fb[y_addr + ((start_x+x) >> 3)] &= ~(1 << ((start_x+x)&7)); // Black Background (comment both out for clear background)
        //fb[y_addr + ((start_x+x) >> 3)] |=  (1 << ((start_x+x)&7)); // White Background (comment both out for clear background)
        if(color)
          fb[y_addr + ((start_x+x) >> 3)] |=  ((((*row>>x)&1)) << ((start_x+x)&7)); // White Pixel
        else
          fb[y_addr + ((start_x+x) >> 3)] &= ~((((*row>>x)&1)) << ((start_x+x)&7)); // Black Pixel
      #else
        if((*row>>x)&1)   // horizontally flipped
        //if((*row&(128>>x))) // normal
          fb[y_addr + start_x + x] = color;
          // else fb[y_addr + x] = background_color;
      #endif
    }
    y_addr += IF_COLOR_BW(144, 20);
    row += 4;
  }
}

void draw_font8_text(uint8_t *fb, int16_t x, int16_t y, uint8_t color, char *str) { // str points to zero-terminated string
  uint8_t strpos=0;
  while(str[strpos]>0) {
    if(x>136) {x=0; y+=8;}  // wrap horizontally
    if(y>160) y=0;          // wrap vertically
    draw_sprite8(fb, font8, x, y, color, str[strpos]);
    x+=8; strpos++;
  }
}

void draw_font8(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t spr) {
  draw_sprite8(fb, font8, x, y, color, spr);
}

void draw_sprite(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t spr) {
  draw_sprite8(fb, font_sprites, x, y, color, spr);
}

// draw_actor is much like draw_sprite, but converts 64px-per-tile to 5px-per-tile
// also, actor x,y is the center of the sprite, draw_sprite puts x,y on the upper-left corner
void draw_actor(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t spr) {
  draw_sprite8(fb, font_sprites, (((x>>9)*5) + (((x&511)*5)/512) + BOARD_X - 3), (((y>>9)*5) + (((y&511)*5)/512) + BOARD_Y - 3), color, spr);
  //IF_COLOR(fb[((((y>>6)*5) + (((y&63)*5)/64) + BOARD_Y)*144)+(((x>>6)*5) + (((x&63)*5)/64) + BOARD_X)]=0b11001100); // put green dot at center of actor (actor's x,y pos) (for testing, color only)
}




#ifdef PBL_BW
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
//  Black and White Drawing Functions
// ------------------------------------------------------------------------ //
void draw_background(uint8_t *fb) {
  for (uint16_t i=0; i<BOARD_Y*5; i++) ((uint32_t*)fb)[i] = 0; // Top rows black
  //for (uint16_t i=0, j=BOARD_Y*5; i<155*5; i++,j++) ((uint32_t*)fb)[j] = ((uint32_t*)background)[i]; // Draw Background Image -- equivilant to: graphics_draw_bitmap_in_rect(ctx, background, GRect(0,BOARD_Y,144,155));
  memcpy(fb + BOARD_Y*20, background, 155*20);  // Draw Background Image -- equivilant to: graphics_draw_bitmap_in_rect(ctx, background, GRect(0,BOARD_Y,144,155));
}
// ------------------------------------------------------------------------ //
void draw_background_flashing(uint8_t *fb, bool flashing) {
  if(flashing)
    for (uint16_t i=BOARD_Y*5; i<=167*5; i++)
      ((uint32_t*)fb)[i] = ~((uint32_t*)fb)[i]; // Invert Board
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //

#define   pset(x, y) fb[((y)*20) + ((x) >> 3)] |=  (1 << ((x)&7))
#define pclear(x, y) fb[((y)*20) + ((x) >> 3)] &= ~(1 << ((x)&7))
//uint8_t xaddr = (x*5+BOARD_X+2) >> 3;
//uint8_t xbit  = (x*5+BOARD_X+2) & 7;
//fb[((y*5+BOARD_Y+2)*20) + xaddr] |= 1 << xbit;

void draw_dots(uint8_t *fb, uint8_t *map, uint8_t animate) {
  for(uint16_t y=0, i=0; y<MAP_H; y++)
    for(uint16_t x=0; x<MAP_W; x++, i++)
      if(map[i]&16) {      // big dot
        if(((animate>>2)&1) == 1) {
          uint16_t px=x*5+BOARD_X, py=y*5+BOARD_Y;
          pset(px+2, py+0);
          pset(px+1, py+1);
          pset(px+2, py+1);
          pset(px+3, py+1);
          pset(px+0, py+2);
          pset(px+1, py+2);
          pset(px+2, py+2);
          pset(px+3, py+2);
          pset(px+4, py+2);
          pset(px+1, py+3);
          pset(px+2, py+3);
          pset(px+3, py+3);
          pset(px+2, py+4);
        }
      } else if(map[i]&8) { // regular dot
        pset(x*5+BOARD_X+2, y*5+BOARD_Y+2);
      }
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
void draw_pupils(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t facing) {
  x = ((x>>9)*ZOOM) + (((x&511)*ZOOM)/512) + BOARD_X;
  y = ((y>>9)*ZOOM) + (((y&511)*ZOOM)/512) + BOARD_Y;
  if(color) {
  switch(facing) {
    case 0:  //00
      pset(x, y);
      pset(x+3, y);
    break;
    case 1:  //01
      pset(x-1, y-2);
      pset(x+1, y-2);
    break;
    case 2:  //10
      pset(x, y);
      pset(x-3, y);
    break;
    case 3:  //11
      pset(x-1, y+1);
      pset(x+1, y+1);
    break;
  }
}else {
  switch(facing) {
    case 0:  //00
      pclear(x, y);
      pclear(x+3, y);
    break;
    case 1:  //01
      pclear(x-1, y-2);
      pclear(x+1, y-2);
    break;
    case 2:  //10
      pclear(x, y);
      pclear(x-3, y);
    break;
    case 3:  //11
      pclear(x-1, y+1);
      pclear(x+1, y+1);
    break;
  }
}
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //

void fill_rect(uint8_t *screen, GRect rect, uint8_t color) {
  uint8_t data[] = {170, 85};
  if((color&192)!=0b00000000) {    // if not clear
    if(color==0b11000000)      {data[0]=  0; data[1]=  0;} // opaque black
    else if(color==0b11111111) {data[0]=255; data[1]=255;} // opaque white
    else                       {data[0]=170; data[1]= 85;} // opaque grey

    rect.size.w  += rect.origin.x; rect.size.h  += rect.origin.y;                      // convert rect.size.w and rect.size.h to rect.x2 and rect.y2
    rect.size.w   = rect.size.w   < 0 ? 0 : rect.size.w   > 144 ? 144 : rect.size.w;   // make sure rect.x2 is within screen bounds
    rect.origin.x = rect.origin.x < 0 ? 0 : rect.origin.x > 144 ? 144 : rect.origin.x; // make sure rect.x1 is within screen bounds
    rect.size.h   = rect.size.h   < 0 ? 0 : rect.size.h   > 168 ? 168 : rect.size.h;   // make sure rect.y2 is within screen bounds
    rect.origin.y = rect.origin.y < 0 ? 0 : rect.origin.y > 168 ? 168 : rect.origin.y; // make sure rect.y1 is within screen bounds

    GPoint addr;
    addr.y = rect.origin.y*20;
    uint8_t l_mask = 255 << (rect.origin.x%8); // mask for the left side
    uint8_t r_mask = 255 << (rect.size.w%8);   // mask for the right side

    for(int16_t y=0; y<(rect.size.h-rect.origin.y); y++, addr.y+=20) {
      addr.x = rect.origin.x>>3;       // init X memory address
      if  (addr.x >= 0 && addr.x < 19) screen[addr.y + addr.x] = (data[y&1] & l_mask) + (screen[addr.y + addr.x] & ~l_mask); // fill left-side of row
      for(addr.x++; addr.x<(rect.size.w>>3); addr.x++)        if(addr.x >= 0 && addr.x < 19) screen[addr.y + addr.x] = data[y&1]; // fill middle of row
      if  (addr.x >= 0 && addr.x < 19) screen[addr.y + addr.x] = (screen[addr.y + addr.x] & r_mask) + (data[y&1] & ~r_mask); // fill right-side of row
    }
  }
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#endif  // END IF PBL_BW

  
  
  
  
  
  
  
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
//  Color Drawing Functions
// ------------------------------------------------------------------------ //  
#ifdef PBL_COLOR
// ================================================================ //
//   How to support transparencies and the alpha channel
// ================================================================ //
#define FULL_SHADOW 0b00111111 // 100% black
#define MORE_SHADOW 0b01111111 // 66%  dark
#define SOME_SHADOW 0b10111111 // 33%  shade
#define NONE_SHADOW 0b11111111 // full color

uint8_t shadowtable[] = {192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,  /* ------------------ */ \
                         192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,  /*      0% alpha      */ \
                         192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,  /*        Clear       */ \
                         192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,  /* ------------------ */ \

                         192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197,  /* ------------------ */ \
                         192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197,  /*     33% alpha      */ \
                         192,192,192,193,192,192,192,193,192,192,192,193,196,196,196,197,  /*    Transparent     */ \
                         208,208,208,209,208,208,208,209,208,208,208,209,212,212,212,213,  /* ------------------ */ \

                         192,192,193,194,192,192,193,194,196,196,197,198,200,200,201,202,  /* ------------------ */ \
                         192,192,193,194,192,192,193,194,196,196,197,198,200,200,201,202,  /*     66% alpha      */ \
                         208,208,209,210,208,208,209,210,212,212,213,214,216,216,217,218,  /*    Translucent     */ \
                         224,224,225,226,224,224,225,226,228,228,229,230,232,232,233,234,  /* ------------------ */ \

                         192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,  /* ------------------ */ \
                         208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,  /*    100% alpha      */ \
                         224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,  /*      Opaque        */ \
                         240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255}; /* ------------------ */

// ------------------------------------------------------------------------------------------------------------------------------------------------ //
void draw_background(uint8_t *fb) {
  for(uint16_t i=0; i<BOARD_Y*144; ++i) fb[i] = 0b11000000; // Top rows black

  uint32_t fbpos = BOARD_Y * 144;
  uint32_t j=0;
  for(uint32_t y=0; y<155; ++y, j+=16)
    for(uint32_t x=0; x<144; ++x, ++j)
      fb[fbpos++] = ((background[j>>3] >> (j&7)) & 1) ? BOARD_COLOR : BACKGROUND_COLOR;  // draw board blue with black background

  //Spectre Door
  for(fbpos = (BOARD_Y+63)*144 + 67; fbpos<(BOARD_Y+63)*144 + 67 + 10; ++fbpos)
    fb[fbpos] = DOOR_COLOR;
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
void draw_background_flashing(uint8_t *fb, bool flashing) {
  uint32_t fbpos = BOARD_Y * 144;
  uint32_t j=0;
  for(uint32_t y=0; y<155; ++y, j+=16)
    for(uint32_t x=0; x<144; ++x, ++j)
      fb[fbpos++] = ((background[j>>3] >> (j&7)) & 1) ? flashing ? BOARD_FLASHING_COLOR : BOARD_COLOR : BACKGROUND_COLOR;  // draw board blue with black background

  //Spectre Door is supposed to be missing during flashing
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
void draw_dots(uint8_t *fb, uint8_t *map, uint8_t animate) {
  uint32_t addr = ((BOARD_Y+2)*144)+BOARD_X+2;
  for(uint16_t y=0, i=0; y<MAP_H; ++y, addr+=(144*4) + (144-(5*MAP_W)))
    for(uint16_t x=0; x<MAP_W; ++x, ++i, addr+=5)  // +5 is because each square is 5 pixels apart
      if(map[i]&16) {        // big dot
        if((animate>>2)&1) {  // Big Dot Flashes (meaning draw or don't draw).  change >>2 to change flash rate
          fb[addr - 144 - 144 - 1] = BIGDOT_COLOR;
          fb[addr - 144 - 144    ] = BIGDOT_COLOR;
          fb[addr - 144 - 144 + 1] = BIGDOT_COLOR;
          
          fb[addr - 144       - 2] = BIGDOT_COLOR;
          fb[addr - 144       - 1] = BIGDOT_COLOR;
          fb[addr - 144          ] = BIGDOT_COLOR;
          fb[addr - 144       + 1] = BIGDOT_COLOR;
          fb[addr - 144       + 2] = BIGDOT_COLOR;
          
          fb[addr             - 2] = BIGDOT_COLOR;
          fb[addr             - 1] = BIGDOT_COLOR;
          fb[addr                ] = BIGDOT_COLOR;
          fb[addr             + 1] = BIGDOT_COLOR;
          fb[addr             + 2] = BIGDOT_COLOR;
          
          fb[addr + 144       - 2] = BIGDOT_COLOR;
          fb[addr + 144       - 1] = BIGDOT_COLOR;
          fb[addr + 144          ] = BIGDOT_COLOR;
          fb[addr + 144       + 1] = BIGDOT_COLOR;
          fb[addr + 144       + 2] = BIGDOT_COLOR;
          
          fb[addr + 144 + 144 - 1] = BIGDOT_COLOR;
          fb[addr + 144 + 144    ] = BIGDOT_COLOR;
          fb[addr + 144 + 144 + 1] = BIGDOT_COLOR;
        }
      } else if(map[i]&8) {  // regular dot
        fb[addr] = DOT_COLOR;
      }
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
void draw_pupils(uint8_t *fb, int32_t x, int32_t y, uint8_t color, uint8_t facing) {
  //NOTE: Will probably break when going out of screen bounds
  x=(((x>>9)*ZOOM) + (((x&511)*ZOOM)/512) + BOARD_X);
  y=(((y>>9)*ZOOM) + (((y&511)*ZOOM)/512) + BOARD_Y);
  uint32_t addr = (y*144)+x;
  if(facing&1) {
    addr += ((facing&2)?+144:-288);
    fb[addr - 1] = color;
    fb[addr + 1] = color;
  } else {
      fb[addr] = color;
      fb[addr + ((facing&2)?-3:3)] = color;
  }
//   switch(facing) {
//     case 0:  //00
//       fb[addr]=0;
//       fb[addr+3]=0;
//     break;
//     case 1:  //01
//       fb[addr-144-144-1]=0;
//       fb[addr-144-144+1]=0;
//     break;
//     case 2:  //10
//       fb[addr]=0;
//       fb[addr-3]=0;
//     break;
//     case 3:  //11
//       fb[addr+144-1]=0;
//       fb[addr+144+1]=0;
//     break;
//   }
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
void fill_rect(uint8_t *screen, GRect rect, uint8_t color) {
  uint8_t bg_opacity = (~color)&0b11000000;
  rect.size.w  += rect.origin.x; rect.size.h  += rect.origin.y;                      // convert rect.size.w and rect.size.h to rect.x2 and rect.y2
  rect.size.w   = rect.size.w   < 0 ? 0 : rect.size.w   > 144 ? 144 : rect.size.w;   // make sure rect.x2 is within screen bounds
  rect.origin.x = rect.origin.x < 0 ? 0 : rect.origin.x > 144 ? 144 : rect.origin.x; // make sure rect.x1 is within screen bounds
  rect.size.h   = rect.size.h   < 0 ? 0 : rect.size.h   > 168 ? 168 : rect.size.h;   // make sure rect.y2 is within screen bounds
  rect.origin.y = rect.origin.y < 0 ? 0 : rect.origin.y > 168 ? 168 : rect.origin.y; // make sure rect.y1 is within screen bounds

  rect.origin.y*=144; rect.size.h*=144;
  for (uint16_t y_addr=rect.origin.y; y_addr<rect.size.h; y_addr+=144)
    for(uint16_t x_addr=rect.origin.x; x_addr<rect.size.w; x_addr++)
      screen[y_addr+x_addr] = shadowtable[bg_opacity + (screen[y_addr+x_addr]&63)] + color;
}
// ------------------------------------------------------------------------------------------------------------------------------------------------ //

#endif  // END IF PBL_COLOR
