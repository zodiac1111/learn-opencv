---
layout: post
title: "hello world"
date: 2012-07-03 09:01
comments: true
categories: 
---

**rtp头浅析**
参考: 	
1. http://hi.baidu.com/studyarea/item/898d995845b610cfd3e10c32  
2. http://topic.csdn.net/u/20071219/12/9ee213e7-aff6-4c81-b3d9-21d6438904b1.html
下面是一个例子
```
RFC文档为RFC3550（RFC1889为其过期版本）
base+0(暂略)此处省略0x33字节
UDP数据区(base+0x33):
RTP header:(头数据)位于udp包的base+0x33地址(内容)
80 e0 69 37 3c 2e 6b 4e 6c 77 83 bf (大尾端[大端])网络字节序
详解(地址都+0x33)
地址	值	意义(不明...
0Byte 	80	[1 Byte] 
	`10b		2bit 版本号:RFC1889 Version 2[过期版本!我去~]
	`0b		1bit 填充(Padding)? 否
	`0b		1bit 扩展? 否
	`0000b		4bit 特约来源识别码数：0
1	e0	[1 Byte]
	`1b		1bit 1重要事件,通常表示一帧结束.MARK
	`110-0000b	7bit 负载类型(Payload type):DynamicRTP-Type-96 (96)
2	69	[2 Byte]序列号(Sequence):
3	37		0x6937=26935(十进制) 
4	3c	[4 Byte]时间戳
5	2e		0x3c2e6b4e=1009675086
6	6b		^^
7	4e		^^
8	6c	[4 Byte]同步源标识符(SSRC)
9	77		Synchronization Source identifier: 
10	83		0x6c7783bf (1819771839)
11	bf		^^
12~??	??	[>=0 Byte]可选的数据
时间戳 差值=3750 = 900000(视频)/24(帧间隔)
一帧最大MUT? 1500Byte =14(EthernetII)+20(ip头) +8UDP+12(RTP头)+1442(RTP数据体)??
参见RFC 3984 p.13:NAL Unit Octet Usage 1字节
例如:01011100 =0x5a 
0禁止位-不禁止
10优先级:2	Coded slice data partition A 	00最低 11最高
         Type   Packet    Single NAL    Non-Interleaved    Interleaved
                               Unit Mode           Mode             Mode  
11100  28     FU-A          no              yes              yes
FU-a格式:
       0                   1                   2                   3
       0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | FU indicator  |   FU header   |                               |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
      |                                                               |
      |                         FU payload                            |
      |                                                               |
      |                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      |                               :...OPTIONAL RTP padding        |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

      Figure 14.  RTP payload format for FU-A
FU indicator : 1Byte
      +---------------+
      |0|1|2|3|4|5|6|7|
      +-+-+-+-+-+-+-+-+
      |F|NRI|  Type   |
      +---------------+
FU则与RTP定义头一样
FU header :1Byte
      +---------------+
      |0|1|2|3|4|5|6|7|
      +-+-+-+-+-+-+-+-+
      |S|E|R|  Type   |
      +---------------+
S=start E=end  of a fragmented NAL unit 
   R: 1 bit
      The Reserved bit MUST be equal to 0 and MUST be ignored by the
      receiver.
81=1`0`0`0-0001
这是一个帧的开始的NALU 单元

Type: 5 bits
   NAL单元荷载类型定义见下表


表1.  单元类型以及荷载结构总结
      Type   Packet      Type name                       
      ---------------------------------------------------------
      0      undefined                                    -
      1-23   NAL unit    Single NAL unit packet per H.264  
      24     STAP-A     Single-time aggregation packet    
      25     STAP-B     Single-time aggregation packet    
      26     MTAP16    Multi-time aggregation packet     
      27     MTAP24    Multi-time aggregation packet     
      28     FU-A      Fragmentation unit                
      29     FU-B      Fragmentation unit                 
      30-31  undefined    
0-0001 即单个

```
