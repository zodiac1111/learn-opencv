/*
	简单从图片(RAW)编码成h264格式文件(流) 通过x264
	-lx264
	参考:
	1. http://stackoverflow.com/questions/2940671/how-does-one-encode-a-series-of-images-into-h264-using-the-x264-c-api
	2. http://zyg0227.blog.51cto.com/1043164/295479 (i420 vs yv12)
	3. RFC 3550		RFC 3984
	V0.01	多帧编码(误)
	by zodiac1111 @20120713
  */
#include <stdint.h>
#include <x264.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <x264_config.h>
#include "main.h"
#define FRAME_NUM (100) //编码帧数
#define MAX_RTP_PARK_SIZE (1442) //最大网络分包大小 bytes
//parament
#define W 640
#define  H 480
int width=640;
int  height=480;
uint32_t fps=10;
//数据原，yuyv yuv422 [4bytes per 2 pixels]

// get YUV422 data from datafile to a char*  array
int getsrcdata(const char *filename ,char *yuv422)
{
	//get yuv422data.
	FILE* fd;
	if((fd = fopen(filename,"r")) < (FILE*)0){
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
	printf("src data first 8 byte %X%X %X%X %X%X %X%X\n"
	       ,yuv422[0],yuv422[1],yuv422[2],yuv422[3]
	       , yuv422[4],yuv422[5],yuv422[6],yuv422[7]);
	return 0;
}

//处理nalu单元.回调函数;<del>
void *nalu_process( x264_t *h, x264_nal_t *nal )
{
	h;
	printf("Nalu_process\n");
}
int main()
{
	FILE* fd;	FILE* fdout;
	unsigned char yuv422[640*480*4/2];//一帧数据大小 4 byte per 2 pixels
	//	getsrcdata("in-i422-yuv422p.raw",yuv422);
	//IN FILE
	fd = fopen("in-i420-yuv420p.raw","r");
	if(fd< 0){
		perror("Fail to open in file");
		exit(1);
	}
	//OUT FILE 保存,useless
	if((fdout = fopen("out.264","w")) < 0){
		perror("Fail to open out file");
		exit(1);
	}
	//	if(fread(yuv422,640*480*2,1,fd)<=0){
	//		perror("Fail to read yuv422file");
	//		exit(2);
	//	}
	//	unsigned char yuv420p[648*480*6/4];
	//	yuv422Toyuv420p(yuv422,yuv420p,640,480);

	//配置参数
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
	//对于字节流格式.rtp不需要
	param.b_annexb = 0;//if set, place start codes (4 bytes) before NAL units
	//param.nalu_process=nalu_process;
	param.i_log_level=X264_LOG_DEBUG;//输出调试信息
	param.i_slice_max_size=MAX_RTP_PARK_SIZE;//
	param.b_cabac=0;//   关闭CABAC编码；
	param.i_bframe=2;
	//应用参数
	x264_param_apply_profile(&param, "baseline");//apply set
	//After this you can initialize the encoder as follows
	x264_t* encoder = x264_encoder_open(&param); //打开编码器
	if(encoder==NULL){
		perror("open encder");
		exit(101);
	}
	x264_picture_t pic_in, pic_out;
	//分配输入输出两(?)张图片的内存。
	if (x264_picture_alloc(&pic_in, X264_CSP_I422, width, height)!=0){
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
	//x264_picture_init(&pic_in); //初始化图片，必须的?包含在x264_picture_alloc里

	//	pic_in.img.i_csp=X264_CSP_I420;
	//	pic_in.img.i_plane=3;
	//	pic_in.img.plane[0]=(uint8_t *)yuv422;
	//memcpy(pic_in.img.plane[0],yuv422,pic_in.img.i_stride[0]);
	//	pic_in.img.plane[1]=yuv422+1;
	//	pic_in.img.plane[2]=yuv422+2;
	//	pic_in.img.plane[3]=NULL;
	//	pic_in.i_type=X264_TYPE_AUTO;
	//	pic_in.img.i_stride[0]=640*480;
	//	pic_in.img.i_stride[1]=640*480/4;
	//	pic_in.img.i_stride[2]=640*480/4;
	x264_nal_t a;
	x264_nal_t* nals;//=&a;

	int i_nals;
	int frame_size =0;
	int j;int i;uint8_t outCanUse[1500];
	int nal_encoder_size=0;
	x264_encoder_headers(encoder,&nals,&i_nals);
	int size = nals[0].i_payload + nals[1].i_payload + nals[2].i_payload;

	int ret_write_header=  fwrite( nals[0].p_payload, size, 1, (FILE*)fdout ) ;
	if(ret_write_header<=0){
		perror("write header");
		exit(107);
	}

	//一帧的操作
	int ret_write_frame=0;
	for(j=0;j<FRAME_NUM;j+=2){
		//x264_picture_init(&pic_in);
		//read ONE frame src data from file To Plane 输入
		fread(pic_in.img.plane[0],pic_in.img.i_stride[0],1,fd);//Y
		fread(pic_in.img.plane[1],pic_in.img.i_stride[1],1,fd);//U
		fread(pic_in.img.plane[2],pic_in.img.i_stride[2],1,fd);//V
		//encode 编码
		frame_size= x264_encoder_encode(encoder,&nals,&i_nals,&pic_in,&pic_out);

		if (!i_nals) {
			//*buffersize=0;
			return 1;
		}
		printf("***after x264_encoder_encode i_nals=%d\n",i_nals);
		fflush(stdout);
		//		for(i=0;i<i_nals;i++){
		//			x264_nal_encode(encoder,&outCanUse,nals);
		//		}
		printf("***    frame_size=%d\n",frame_size);
		uint8_t data[100000];
		int i_size;
		int i_data;
		x264_nal_t *nal;
		for(i=0;i<i_nals;i++){


			x264_nal_encode( encoder, data, nal );

			//把网络包写入到输出文件中去
			ret_write_frame=fwrite(	nals[0].p_payload,
						frame_size, 1, (FILE*)fdout );//这个是函数指针的形式，//p_write_nalu =

		}
		//		if( frame_size )
		//		{
		//			ret_write_frame=fwrite(
		//						nals[0].p_payload,
		//						frame_size, 1, (FILE*)fdout );
		//			if(ret_write_frame<=0){
		//				perror("write farme date");
		//				exit("108");
		//			}
		//		}
	}
	int64_t last_dts = 0;
	int64_t prev_dts = 0;
	//	int64_t prev_dts = 0;
	//int delayedframes=x264_encoder_delayed_frames( encoder );
	//printf("delayedframes=%d\n",delayedframes);

	while( x264_encoder_delayed_frames( encoder ) ){

		//	for(i=0;i<50;i++){
		printf("i=%d\n",i);
		//x264_picture_init(&pic_in);
		//read ONE frame src data from file To Plane 输入
		//		fread(pic_in.img.plane[0],pic_in.img.i_stride[0],1,fd);//Y
		//		fread(pic_in.img.plane[1],pic_in.img.i_stride[1],1,fd);//U
		//		fread(pic_in.img.plane[2],pic_in.img.i_stride[2],1,fd);//V
		//encode 编码
		//frame_size= x264_encoder_encode(encoder,&nals,&i_nals,NULL,&pic_out);
		frame_size= x264_encoder_encode(encoder,&nals,&i_nals,&pic_in,&pic_out);
		if (!i_nals) {
			//*buffersize=0;
			return 1;
		}
		printf("***i_nals=%d\n",i_nals);
		fflush(stdout);
		//		for(i=0;i<i_nals;i++){
		//			x264_nal_encode(encoder,&outCanUse,nals);
		//		}
		int ret_write_frame=0;
		if( frame_size )
		{
			ret_write_frame=fwrite(
						nals[0].p_payload,
						frame_size, 1, (FILE*)fdout );
			if(ret_write_frame<=0){
				perror("write farme date");
				exit("108");
			}
		}

	}
	//	printf("frame_size  is %d\n ",frame_size);
	//	if (frame_size < 0){// OK
	//		perror("Encoder Encode Failed\n");
	//		exit(103);
	//	}
	//debug print
	printf("after encode pic: %02X%02X %02X%02X %02X%02X %02X%02X\n",
	       pic_out.img.plane[0][0],
	       pic_out.img.plane[0][1],
	       pic_out.img.plane[0][2],
	       pic_out.img.plane[0][3],
	       pic_out.img.plane[0][4],
	       pic_out.img.plane[0][5],
	       pic_out.img.plane[0][6],
	       pic_out.img.plane[0][7]);
	fflush(stdout);//立即输出

	//读取buffer
	int iFrames = x264_encoder_delayed_frames(encoder);
	printf("当前编码器中缓存数据:%d帧\n",iFrames);
	fflush(stdout);//立即输出
	x264_picture_clean(&pic_in);
	//x264_picture_clean(&pic_out);
	//At last over encode close the encoder
	printf("********close the encoder:********\n");
	fflush(stdout);
	x264_encoder_close(encoder);

	//close In & Out File
	if(fclose(fdout)!=0){
		perror("Fail to close farme file");
		exit(3);
	}
	if(fclose(fd)!=0){
		perror("Fail to close yuv422file");
		exit(3);
	}
	return 0;
}
