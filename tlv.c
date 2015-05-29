#include <string.h>
#include <malloc.h>
#include "tlv.h"

int tlv_get_integer (PTLV pTlv)
{
	int i = 0;
	int result = 0;
	int len = be16toh(pTlv->wLength);

	switch (pTlv->byType)
	{
	case TLV_TYPE_INTEGER1:
	case TLV_TYPE_INTEGER2:
	case TLV_TYPE_INTEGER4:
		break;
	default:
		return -1;
	}

	for (i = 0; i < len; i++)
	{
		result <<= 8;
		result |= (pTlv->byValue[i] & 0xff);
	}

	return result;
}

PTLV tlv_get_next(PTLV pNow)
{
	return (PTLV)(((byte*)(pNow)) + 3 + be16toh(pNow->wLength));
}

//tlv_analyze_data
//从原始的TLV字节流分析整理出TLV_ARRAY结构，方便使用。
//与pData共享数据，不另外复制数据，nWant为需要分析出的TLV个数，传0表示全部分析。
//返回指针需要手动释放。
PTLV_ARRAY tlv_analyze_data (byte * pData, int nDataLen, int nWant)
{
	PTLV_ARRAY pArray = NULL;
	word wLength;
	int nCount = 0;

	if (pData == NULL || nDataLen < 3 || nWant < 0)
		return NULL;

	pArray = (PTLV_ARRAY) malloc(sizeof(TLV_ARRAY) + (nDataLen / 3) * sizeof(PTLV));
	pArray->wArraySize = 0;

	while (1)
	{
		pArray->arTlv[pArray->wArraySize] = (PTLV) (pData + nCount);
		wLength = be16toh(pArray->arTlv[pArray->wArraySize]->wLength);

		nCount += 3 + wLength;
		pArray->wArraySize++;
		if (pArray->wArraySize == nWant || nCount == nDataLen)
			break;
		else if (nCount > nDataLen)
		{
			free(pArray);
			return NULL;
		}
	}
	//释放多余空间
	pArray = (PTLV_ARRAY) realloc(pArray, sizeof(TLV_ARRAY) + (pArray->wArraySize - 1) * sizeof(PTLV));
	return pArray;
}

int tlv_put_integer (word nLength, int nValue, void * pOut)
{
	PTLV p = (PTLV) pOut;
	int i = 0;

	switch (nLength)
	{
	case 1:
		p->byType = TLV_TYPE_INTEGER1;
		break;
	case 2:
		p->byType = TLV_TYPE_INTEGER2;
		break;
	case 4:
		p->byType = TLV_TYPE_INTEGER4;
		break;
	default:
		return 0;
	}

	p->wLength = htobe16(nLength);

	for (i = 0; i < nLength; i++)
	{
		p->byValue[i] = (nValue >> (8 * (nLength - i - 1))) & 0xff;
	}
	return 3 + nLength;
}

int tlv_put_byte_array (word nLength, const void * pValue, void * pOut)
{
	PTLV p = (PTLV) pOut;
	p->byType = TLV_TYPE_STRING;
	p->wLength = htobe16(nLength);
	memcpy(p->byValue, pValue, nLength);
	return 3 + nLength;
}

int tlv_put_string (const char * szValue, word wMaxLen, void * pOut)
{
	word len = (word) strlen(szValue);
	len = (len > wMaxLen ? wMaxLen : len);
	return tlv_put_byte_array(len, (byte*) szValue, pOut);
}

