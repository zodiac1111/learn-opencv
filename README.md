opencv/v4l/x264/rtsp **学习**  
这个项目已经很有没有更新了.
这方面的学习先到此为止
IDE为qt-creater qt4.8.0 fedora-17-i86  
一些opencv练习例子来自https://www.opencvchina.com  
pdf是官方文档.  
一些例子:
* webcam-(v4L2)->YUYV **i422**
* V4L2->YUYV->RGB->BMP文件 (中间步骤)
* RGB->jpg文件 (中间步骤)
* YUYV-(change)->YUV420p (for encode ?)
* YUV file(many pic in one file) -> .264/ .mp4 FILE(not data stream) with **(x264 command line)**
* TODO:YUV->H264(use c lang)
* TODO:H264/<del>frame</del> slice -><del>rtsp包?</del> NAL(U)
* TODO:rtsp/rtp/rtcp+ UDP/TCP 服务器.
* rtsp->opencv->简单图像处理(半成)
* TODO:图像处理.

