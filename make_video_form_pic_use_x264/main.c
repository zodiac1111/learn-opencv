/*
  -lx264
  */
//#include <signal.h>
#include <stdint.h>
#include <x264.h>
//#include <x264_config.h>
//parament
int width=640;
int  height=480;
uint32_t fps=10;
int main()
{
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
	x264_picture_t pic_in, pic_out;
	x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);
	//change pic format form RGB24 to YUV420p with libswscale (from ffmpeg)
	//because it seem that x246 need the YUV420p as src data format
	//i will convert from YUYV(422) to YUV420p myself .lib is too big,needless
	/* struct SwsContext*
			convertCtx =
			sws_getContext(in_w, in_h, PIX_FMT_RGB24,
				       out_w, out_h, PIX_FMT_YUV420P,
				       SWS_FAST_BILINEAR, NULL, NULL, NULL);
				       */

	return 0;
}
