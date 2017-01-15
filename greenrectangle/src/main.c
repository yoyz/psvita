#include <psp2/kernel/processmgr.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <psp2/ctrl.h>
#include <stdio.h>
#include <psp2/display.h>
#include <psp2/kernel/processmgr.h>
#include <stdio.h>
#include "psp2shell.h"

#define SCREEN_WIDTH   960
#define SCREEN_HEIGHT  544

#define BOX_SIZE_X    64
#define BOX_SIZE_Y    64
#define BOX_INCREMENT 5

int my_x1=( SCREEN_WIDTH  * 31) / 64;
int my_y1=( SCREEN_HEIGHT * 33) / 64;

int my_x2= BOX_SIZE_X;
int my_y2= BOX_SIZE_Y;

SDL_Surface   * screen    = NULL;
SDL_Window    * gWindow   = NULL;
SDL_Renderer  * gRenderer = NULL;
SDL_Event       event;
SDL_Rect        fillRect;
SceCtrlData     pad;
SDL_Surface *txt; 
int cont              = 1;
TTF_Font *font = NULL;
SDL_Color colorme = {255, 255, 255};
SDL_Rect     texture_rect; //create a rect
SDL_Texture* texture;

void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
  int i;
  int j;
  for (i=0;i<len;i++)
    {
      stream[i]=j;
      if (i%64==0)
	j++;
    }
}

#define printf psvDebugScreenPrintf


int main(int argc, char *argv[]) 
{
  int remain = 5;
  int loop=0;
  SDL_Rect r;
  int32_t c=0xC8A0A8;
  TTF_Font *font = NULL;
  static SDL_AudioSpec wav_spec; // the specs of our piece of music
  SDL_RWops * srw;
  wav_spec.callback = my_audio_callback;
  wav_spec.userdata = NULL;
  
  psp2shell_init(3333,0);
  psp2shell_print("Start\n");
  
  texture_rect.x = 0;   //controls the rect's x coordinate 
  texture_rect.y = 0;   // controls the rect's y coordinte
  texture_rect.w = 100; // controls the width of the rect
  texture_rect.h = 100; // controls the height of the rect
  
  //psvDebugScreenInit();
  
  //printf("Welcome to the psvDebugScreen showcase !\n");
  if( SDL_Init( SDL_INIT_VIDEO| SDL_INIT_AUDIO) < 0 )
    return -1;  
  
  if ((gWindow = SDL_CreateWindow( "GreenRectangle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN)) == NULL)
    if ((gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_PRESENTVSYNC)) == NULL)
      return -1;

  screen=SDL_GetWindowSurface(gWindow);

  if(TTF_Init() == -1)
    exit(-2);

  //srw=SDL_RWFromFile( "ux0:/app/VSDK00001/font.ttf","rb");
  font = TTF_OpenFont("ux0:/app/VSDK00001/font.ttf", 12);
  //if (!srw) exit(3);
  if (!font) exit(2);
    
  if ( SDL_OpenAudio(&wav_spec, NULL) < 0 )
    exit(-1);
  SDL_PauseAudio(0);

  

  
  txt = TTF_RenderText_Solid(font, "Salut !", colorme);
  
  while (cont)
    {
      psp2shell_print("loop\n");
      sceCtrlPeekBufferPositive(0, &pad, 1);
      
      if (pad.buttons & SCE_CTRL_RIGHT)
	my_x1=my_x1+BOX_INCREMENT;
      if (pad.buttons & SCE_CTRL_LEFT)
	my_x1=my_x1-BOX_INCREMENT;
      
      if (pad.buttons & SCE_CTRL_UP)
	my_y1=my_y1-BOX_INCREMENT;
      if (pad.buttons & SCE_CTRL_DOWN)
	my_y1=my_y1+BOX_INCREMENT;
      
      if (my_x1<10)                          my_x1=10;
      if (my_x1>SCREEN_WIDTH -BOX_SIZE_X) my_x1=SCREEN_WIDTH-BOX_SIZE_X;
      
      if (my_y1<10)                          my_y1=10;
      if (my_y1>SCREEN_HEIGHT-BOX_SIZE_Y) my_y1=SCREEN_HEIGHT-BOX_SIZE_Y;
      fillRect = { my_x1, my_y1, my_x2, my_y2   };
      
      r.x=my_x1;
      r.y=my_y1;
      r.w=my_x2;
      r.h=my_y2;
      
      SDL_BlitSurface( txt, NULL, screen, &texture_rect );
      SDL_FillRect(screen,&r,SDL_MapRGB(screen->format, (c&0xFF0000)>>16,(c&0x00FF00)>>8,(c&0x0000FF)>>0)); 
      SDL_UpdateWindowSurface( gWindow );
      SDL_Delay(10);
    }      
  
  sceKernelExitProcess(0);
  return 0;
}


