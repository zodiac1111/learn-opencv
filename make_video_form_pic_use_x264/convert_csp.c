
/*	transfrom yuv422 to yuv420p Color Space -csps
	step 1: yuv422->yuv420
	step 2: yuv420->yuv420p
	because it seem that x264 need the frame pic format
	is yuv420p. and the webcam get data format is yuyv
	equ YUV422
	NOTE:	i420 = yyyy+vv+uu ????误???
		yv12/yuv420 = yyyy+uu+vv
*/
int yuv422Toyuv420p(const unsigned char* yuv422
		    ,unsigned char *yuv420p
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
