/*
	简单从图片编码成h264格式文件 通过x264
	-lx264
	参考:
	1. http://stackoverflow.com/questions/2940671/how-does-one-encode-a-series-of-images-into-h264-using-the-x264-c-api
	2. http://zyg0227.blog.51cto.com/1043164/295479 (i420 vs yv12)
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
	NOTE:	i420 = yyyy+vv+uu
		yv12/yuv420 = yyyy+uu+vv
*/
int yuv422Toyuv420p(const unsigned char* yuv422,unsigned char *yuv420p
		    , int width,int hight)
{
	//yuv420p has  SIX byte per FOUR pixels : 6/4
	// & At least 4 pixels per frame
	//unsigned char yuv420p[width*hight*6/4];

	int srclen=width*hight*4/2;
	int i=0,y=0,v=width*hight,u=width*hight+width*hight/4;
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
		printf("src data first 8 byte %X%X %X%X %X%X %X%X\n"
		       ,yuv422[0],yuv422[1],yuv422[2],yuv422[3]
		       , yuv422[4],yuv422[5],yuv422[6],yuv422[7]);
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
	//分配输入输出两张图片的内存。 ?
	if (x264_picture_alloc(&pic_in, X264_CSP_I420, width, height)!=0){
		perror("x264_picture_alloc");
		exit(5);
	}
	//	if (x264_picture_alloc(&pic_out, X264_CSP_NV16, width, height)!=0){
	//		perror("x264_picture_alloc");
	//		exit(5);
	//	}
	//change pic format form RGB24 to YUV420p with libswscale (from ffmpeg)
	//because it seem that x246 need the YUV420p as src data format
	//i will convert from YUYV(422) to YUV420p myself .lib is too big,needless
	/* struct SwsContext*
			convertCtx =
			sws_getContext(in_w, in_h, PIX_FMT_RGB24,
				       out_w, out_h, PIX_FMT_YUV420P,
				       SWS_FAST_BILINEAR, NULL, NULL, NULL);
	*/
	//x264_picture_init(&pic_out);
	//x264_picture_init(&pic_in); //初始化图片，必须的！！
	pic_in.img.i_csp=X264_CSP_I420;
	pic_in.img.i_plane=3;
	pic_in.img.plane[0]=yuv420p;
	pic_in.img.plane[1]=yuv420p+640*480;
	pic_in.img.plane[2]=yuv420p+640*480+640*480/4;
	pic_in.img.i_stride[0]=640;
	pic_in.img.i_stride[1]=320;
//	pic_in.img.i_stride[2]=0;


	x264_nal_t a;
	x264_nal_t* nals=&a;

	int i_nals;
	int frame_size =0;
	frame_size= x264_encoder_encode(encoder,&nals,&i_nals,&pic_in,&pic_out);

	printf("frame_size  is %d\n ",frame_size);
	if (frame_size < 0){// OK
		perror("Encoder Encode Failed\n");
		exit(103);
	}else{
		printf("after encode pic %X%X %X%X %X%X %X%X\n",
		       pic_out.img.plane[0][0],
		       pic_out.img.plane[0][1],
		       pic_out.img.plane[0][2],
		       pic_out.img.plane[0][3],
		       pic_out.img.plane[0][4],
		       pic_out.img.plane[0][5],
		       pic_out.img.plane[0][6],
		       pic_out.img.plane[0][7]);
		fflush(stdout);
		int fd;
		if((fd = fopen("test-YUYV-YUV422-changed.raw","w")) < 0){
			perror("Fail to open changed file");
			exit(1);
		}
		if(fwrite(pic_out.img.plane[0],640*480*2,1,fd)<=0){
			perror("Fail to write farme file");
			exit(2);
		}
		if(fclose(fd)!=0){
			perror("Fail to close farme file");
			exit(3);
		}

	}

	{//读取buffer
	int iFrames = x264_encoder_delayed_frames(encoder);
	printf("当前编码器中缓存数据:%d帧\n",iFrames);
	}
	//At last over encode close the encoder
	x264_encoder_close(encoder);
	return 0;
}
