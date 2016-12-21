/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>
#include <string>
#include <sstream>
#include <iostream>
#include <psp2/rtc.h>

//#define ARRAYSIZE 1024*1024*1
#define ARRAYSIZE 1024*1024
/*
 * Symbol of the image.png file
 */
extern unsigned char _binary_image_png_start;

int main()
{
	SceCtrlData pad;
	vita2d_pgf *pgf;
	vita2d_texture *image;
	float rad = 0.0f;

	int64_t i;
	int64_t j;
	int64_t iter=0;
	int32_t * A = (int32_t*)malloc(sizeof(int32_t)*ARRAYSIZE);
	int32_t * B = (int32_t*)malloc(sizeof(int32_t)*ARRAYSIZE);
	int32_t * C = (int32_t*)malloc(sizeof(int32_t)*ARRAYSIZE);
	std::string str_output="";
	std::ostringstream oss;
	SceDateTime t0;
	SceDateTime t1;

	float dt0;
	float dt1;
	float dt2;
	vita2d_init();
	vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	pgf = vita2d_load_default_pgf();

	/*
	 * Load the statically compiled image.png file.
	 */
	image = vita2d_load_PNG_buffer(&_binary_image_png_start);

	memset(&pad, 0, sizeof(pad));

	while (1)
	  {
		sceCtrlPeekBufferPositive(0, &pad, 1);

		if (pad.buttons & SCE_CTRL_START)
			break;

		vita2d_start_drawing();
		vita2d_clear_screen();

		sceRtcGetCurrentClock(&t0, 13);
		for (j=0;j<64;j++)
		  for (i=0;i<ARRAYSIZE;i++)
		    A[i]=B[i];
		    //A[i]=B[i]+C[i];
		sceRtcGetCurrentClock(&t1, 13);
		iter++;
		dt0=sceRtcGetMinute(&t0)*60*1000*1000+sceRtcGetSecond(&t0)*1000*1000+sceRtcGetMicrosecond(&t0);
		dt1=sceRtcGetMinute(&t1)*60*1000*1000+sceRtcGetSecond(&t1)*1000*1000+sceRtcGetMicrosecond(&t1);
		dt2=dt1-dt0;
		
		oss.str("");
		oss << "iter :" << iter << "   t: " << dt2/1000/1000 << "   bwMB/s: " << ARRAYSIZE*2*64*4/dt2; 
		str_output=oss.str();
		vita2d_pgf_draw_text(pgf, 200, 30, RGBA8(0,255,0,255), 1.0f, str_output.c_str());

		vita2d_end_drawing();
		vita2d_swap_buffers();
	}

	/*
	 * vita2d_fini() waits until the GPU has finished rendering,
	 * then we can free the assets freely.
	 */
	vita2d_fini();
	vita2d_free_texture(image);
	vita2d_free_pgf(pgf);

	sceKernelExitProcess(0);
	return 0;
}
