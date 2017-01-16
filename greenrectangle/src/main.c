#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h> 
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "psp2shell.h"
#include <math.h>

#define SCREEN_WIDTH   960
#define SCREEN_HEIGHT  544

#define BOX_SIZE_X    64
#define BOX_SIZE_Y    64
#define BOX_INCREMENT 5

#define MAXCOLOR 9

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
SceUID thid;

int    start=0;
float phase=0.0;
Sint16 buf[8192];

int thread=1;
int sce_freqs[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
int sce_size = 2048;
int sce_freq = 8;
int sce_mode = SCE_AUDIO_OUT_MODE_MONO;
int sce_vol = SCE_AUDIO_VOLUME_0DB;
int16_t wave_buf[SCE_AUDIO_MAX_LEN]={0};  
int sce_port;
int sce_vol_tmp[2] = {sce_vol,sce_vol};

int32_t pal0[MAXCOLOR]={0x0,      0xC8A0A8, 0x59AADA, 0xFFA11C, 0x39CE90, 0x36B82B, 0x3E789A, 0xFFFFFF, 0x0 };
int32_t * pal = pal0;

void stupid_thread()
{
  while (1)
    {
      for (int i=0;i<sce_size;i++)
	{
	  buf[i]=(sin(2*M_PI*phase)*1000.0);
	  phase=phase+(((float)my_x1)/40000.0);
	  if (phase>1.0)
	    phase=0.0;
	}
      sceAudioOutOutput(sce_port, buf);
    }  
}

void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
  int i;
  int j;
  if (start==0)
    for (int i=0;i<len*2;i++)
      stream[i]=0;
  start=1;

  typedef Sint16 MY_TYPE;
  MY_TYPE *buffer = (MY_TYPE *) stream;

  for (int i=0;i<len;i++)
    {
      buffer[i]=(sin(2*M_PI*phase)*1000.0);
      phase=phase+(((float)my_x1)/10000.0);
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
  int sdl_audio=0;
  
  wav_spec.callback = my_audio_callback;
  wav_spec.userdata = NULL;
  //wav_spec.format=AUDIO_S16;
  //wav_spec.freq=48000;
  //wav_spec.samples=4096;
  psp2shell_init(3333,0);
  psp2shell_print("Start\n");
  
  texture_rect.x = 0;   //controls the rect's x coordinate 
  texture_rect.y = 0;   // controls the rect's y coordinte
  texture_rect.w = 100; // controls the width of the rect
  texture_rect.h = 100; // controls the height of the rect
  
  if (sdl_audio)
    if( SDL_Init( SDL_INIT_VIDEO| SDL_INIT_AUDIO) < 0 )
      return -1;
  else
    if( SDL_Init( SDL_INIT_VIDEO)<0)
      return -1;
  
  if ((gWindow = SDL_CreateWindow( "GreenRectangle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN)) == NULL)
    //if ((gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_PRESENTVSYNC)) == NULL)
    if ((gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED)) == NULL)
      return -1;

  screen=SDL_GetWindowSurface(gWindow);

  if(TTF_Init() == -1)
    exit(-2);

  font = TTF_OpenFont("ux0:/app/VSDK00001/font.ttf", 12);
  if (!font) exit(2);

  if (sdl_audio)
    {
      if ( SDL_OpenAudio(&wav_spec, NULL) < 0 )
	exit(-1);
      SDL_PauseAudio(0);
    }
  else
    {
      sce_port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, sce_size, sce_freqs[sce_freq], sce_mode);
      sceAudioOutSetVolume(sce_port, SCE_AUDIO_VOLUME_FLAG_L_CH |SCE_AUDIO_VOLUME_FLAG_R_CH, sce_vol_tmp);
      sceAudioOutSetConfig(sce_port, sce_size, sce_freqs[sce_freq], sce_mode);

    }
  
  
  thid = sceKernelCreateThread("my_thread", stupid_thread, 0x10000100, 0x10000, 0, 0, NULL);
  sceKernelStartThread(thid,0,NULL);
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
      SDL_FillRect(screen,NULL, SDL_MapRGB(screen->format, (pal[8]&0xFF0000)>>16,(pal[8]&0x00FF00)>>8,(pal[8]&0x0000FF)>>0));
      SDL_BlitSurface( txt, NULL, screen, &texture_rect );
      SDL_FillRect(screen,&r,SDL_MapRGB(screen->format, (c&0xFF0000)>>16,(c&0x00FF00)>>8,(c&0x0000FF)>>0)); 
      SDL_UpdateWindowSurface( gWindow );

      if(sdl_audio==0 && thread==0)
	{
	  for (int i=0;i<sce_size;i++)
	    {
	      buf[i]=(sin(2*M_PI*phase)*1000.0);
	      phase=phase+(((float)my_x1)/40000.0);
	      if (phase>1.0)
		phase=0.0;
	    }
	  sceAudioOutOutput(sce_port, buf);
	}      
      SDL_Delay(10);      
    }      
  
  sceKernelExitProcess(0);
  return 0;
}


