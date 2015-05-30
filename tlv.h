#ifndef TLV_H_
#define TLV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <endian.h>
#include "../types.h"

#define TLV_TYPE_INTEGER2		1  //以大端模式保存，低字节存数字高位。
#define TLV_TYPE_INTEGER4		2
#define TLV_TYPE_STRING		3
#define TLV_TYPE_INTEGER1		4

//本机字节序与big_endian字节序相互转换请使用：
//htobe16,htobe32,be32toh,be16toh



#pragma pack(1)
//基本的TLV类型数据结构
typedef struct st_TLV
{
	byte byType;
	word wLength;  		//以big_endian模式保存，注意需要适当转换
	byte byValue[1];  	//长度可变
} TLV, *PTLV;

//由多个TLV类型数据的指针组成的数组，方便按下标取得相应数据
typedef struct st_TLV_ARRAY
{
	word wArraySize;
	PTLV arTlv[1];  //长度可变
} TLV_ARRAY, *PTLV_ARRAY;

#pragma pack()

//tlv_analyze_data
//从原始的TLV字节流分析整理出TLV_ARRAY结构，方便使用。
//与pData共享数据，不另外复制数据，nWant为需要分析出的TLV个数，传0表示全部分析。
//注意返回指针需要手动释放free。
PTLV_ARRAY tlv_analyze_data(byte * pData, int nDataLen, int nWant);

int tlv_get_integer(PTLV pTlv);

PTLV tlv_get_next(PTLV pNow);

//构成tlv字节流函数tlv_put_*，其中tlv_put_string需要以'\0'结尾的字符串作为输入，或者当输入字符串长度超过最大值wMaxLen时使用wMaxLen作为长度截断字符串，
//返回输出的字节数。
int tlv_put_integer(word nLength , int nValue, void * pOut);
int tlv_put_byte_array(word nLength , const void * pValue, void * pOut);
int tlv_put_string(const char * szValue, word wMaxLen, void * pOut);

#ifdef __cplusplus
}
#endif

#endif /* TLV_H_ */
