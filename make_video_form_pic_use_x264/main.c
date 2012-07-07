/*
  -lx264
  */
#include <stdint.h>
#include <x264.h>
#include <fcntl.h>
#include <errno.h>
#include <x264_config.h>
#include "main.h"
//parament
int width=640;
int  height=480;
uint32_t fps=10;
//数据原，yuyv yuv422 [4bytes per 2 pixels]

// get YUV422 data from datafile to a char*  array
int getsrcdata(char *yuv422)
{
	//get yuv422data.
	int fd;
	if((fd = fopen("test-YUYV-YUV422.raw","r")) < 0){
		perror("Fail to open");
		exit(1);
	}
	if(fread(yuv422,640*480*2,1,fd)<=0){
		perror("Fail to read yuv422file");
		exit(2);
	}
	if(fclose(fd)!=0){
		perror("Fail to close yuv422file");
		exit(3);
	}
	return 0;
}
/*	transfrom yuv422 to yuv420p
	step 1: yuv422->yuv420
	step 2: yuv420->yuv420p
	because it seem that x264 need the frame pic format
	is yuv420p. and the webcam get data format is yuyv
	equ YUV422
*/
int yuv422Toyuv420p(const unsigned char* yuv422,unsigned char *yuv420p
			       , int width,int hight)
{
	//yuv420p has  SIX byte per FOUR pixels : 6/4
	// & At least 4 pixels per frame
	//unsigned char yuv420p[width*hight*6/4];
	int i=0,y=0,u=0,v=0;
	int srclen=width*hight*4/2;
	for (i=0;i<srclen;i++){
		if((i%4==0) || (i%4==2) ){
			yuv420p[y++]=yuv422[i];
			continue;
		}
		if(i%8==1){ //u0 u1减少到u
			yuv420p[u++]=yuv422[i];
			continue;
		}
		if(i%8==3){
			yuv420p[v++]=yuv422[i];
			continue;
		}
	}
	return 0;
}

int main()
{
	unsigned char yuv422[640*480*4/2];
	if(getsrcdata(yuv422)==0){
		printf("src data first 4 byte %X %X %X %X\n"
		       ,yuv422[0],yuv422[1],yuv422[2],yuv422[3]);
		fflush(stdout);
	}else{
		perror("read src date err");
		exit(4);
	}
	unsigned char yuv420p[648*480*6/4];
	yuv422Toyuv420p(yuv422,yuv420p,640,480);
	x264_param_t param;
	//set default param
	x264_param_default_preset(&param, "veryfast", "zerolatency");
	param.i_threads = 1;
	param.i_width = width;
	param.i_height = height;
	param.i_fps_num = fps;
	param.i_fps_den = (uint32_t)1;
	// Intra refres:
	param.i_keyint_max = fps;
	param.b_intra_refresh = 1;
	//Rate control:
	param.rc.i_rc_method = X264_RC_CRF;
	param.rc.f_rf_constant = 25;
	param.rc.f_rf_constant_max = 35;
	//For streaming:
	param.b_repeat_headers = 1;
	param.b_annexb = 1;
	x264_param_apply_profile(&param, "baseline");//apply set
	//After this you can initialize the encoder as follows
	x264_t* encoder = x264_encoder_open(&param);
	if(encoder==NULL){
		perror("open encder");
		exit(101);
	}
	x264_picture_t pic_in, pic_out;
	//分配输入输出两张图片的内存。
	if (x264_picture_alloc(&pic_in, X264_CSP_YV12, width, height)==-1){
		perror("x264_picture_alloc");
		exit(5);
	}
	if (x264_picture_alloc(&pic_out, X264_CSP_YV12, width, height)==-1){
		perror("x264_picture_alloc");
		exit(5);
	}
	//change pic format form RGB24 to YUV420p with libswscale (from ffmpeg)
	//because it seem that x246 need the YUV420p as src data format
	//i will convert from YUYV(422) to YUV420p myself .lib is too big,needless
	/* struct SwsContext*
			convertCtx =
			sws_getContext(in_w, in_h, PIX_FMT_RGB24,
				       out_w, out_h, PIX_FMT_YUV420P,
				       SWS_FAST_BILINEAR, NULL, NULL, NULL);
	*/
	x264_picture_init(&pic_out);
	x264_picture_init(&pic_in); //初始化图片，必须的！！
	pic_in.img.i_csp=X264_CSP_YV12;
	pic_in.img.i_plane=3;
	pic_in.img.i_stride[0]=640*480;
	pic_in.img.i_stride[1]=640*480/2;
	pic_in.img.i_stride[2]=640*480/2;
	pic_in.img.plane[0]=yuv420p+0;
	pic_in.img.plane[1]=yuv420p+680*480;
	pic_in.img.plane[2]=yuv420p+680*480+680*480/2;

	x264_nal_t* nals;
	int i_nals;
	int frame_size =0;
	frame_size= x264_encoder_encode(encoder,&nals,&i_nals,&pic_in,&pic_out);
	printf("frame_size  is %d\n ",frame_size);
	if (frame_size < 0){// OK
		perror("Encoder Encode Failed\n");

	}else{
		printf("after encode pic %X %X %X %X\n",
		       pic_out.img.plane[0][0],
		       pic_out.img.plane[0][1],
		       pic_out.img.plane[0][2],
		       pic_out.img.plane[0][3]);
	}


	//At last over encode close the encoder
	x264_encoder_close(encoder);
	return 0;
}
