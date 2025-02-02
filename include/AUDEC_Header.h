﻿//AUDEC_Header.h
#ifndef __AUDEC_HEADER_H__
#define __AUDEC_HEADER_H__


#ifdef _WIN32
#include <winsock2.h>
#define __INLINE__	_inline
#else
#define __INLINE__	inline

#ifndef _WIN32_WCE
#include <netinet/in.h>
#endif

#endif





#define AUDEC_HEADER_VER_01					0x01
#define AUDEC_HEADER_VER_01_LEN				8


typedef struct tagAUDEC_HEADER
{
	unsigned short ver:2;
	unsigned short tcp:1;
	unsigned short dual:1;	
	unsigned short fec:1;
	unsigned short syn_point:1;
	unsigned short do_not_syn:1;
	unsigned short used_fec:1;
	unsigned short codecid:8;

	unsigned short seq;
	unsigned int   ts;
}AUDEC_HEADER,*PAUDEC_HEADER;

#if defined(_WIN32_WCE) || defined(NETEC_ARM)
typedef struct tagAUDEC_SUB_HEADER
{
	unsigned short ver:2;
	unsigned short tcp:1;
	unsigned short dual:1;	
	unsigned short fec:1;
	unsigned short syn_point:1;
	unsigned short do_not_syn:1;
	unsigned short used_fec:1;
	unsigned short codecid:8;
}AUDEC_SUB_HEADER,*PAUDEC_SUB_HEADER;
#endif

__INLINE__ void AUDEC_HEADER_RESET(void*p)
{
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->ver=AUDEC_HEADER_VER_01;
	pHeader->tcp=0;
	pHeader->dual=0;
	pHeader->fec=0;
	pHeader->syn_point=0;
	pHeader->codecid=0;
	pHeader->used_fec=0;
	pHeader->do_not_syn=0;
	pHeader->seq=0;
	pHeader->ts=0;
}

__INLINE__ unsigned short AUDEC_HEADER_IS_VALID(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	if (tSubHeader.ver==AUDEC_HEADER_VER_01)
	{
		return 1;
	}
	return 0;
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	if (pHeader->ver==AUDEC_HEADER_VER_01)
	{
		return 1;
	}
	return 0;
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_TCP(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	return tSubHeader.tcp;
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return pHeader->tcp;
#endif
}

__INLINE__ void AUDEC_HEADER_SET_TCP(void*p,unsigned short v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	tSubHeader.tcp=v;
	memcpy(p,&tSubHeader,2);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->tcp=v;
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_DUAL(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	return tSubHeader.dual;
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return pHeader->dual;
#endif
}

__INLINE__ void AUDEC_HEADER_SET_DUAL(void*p,unsigned short v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	tSubHeader.dual=v;
	memcpy(p,&tSubHeader,2);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->dual=v;
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_FEC(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	return tSubHeader.fec;
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return pHeader->fec;
#endif
}

__INLINE__ void AUDEC_HEADER_SET_FEC(void*p,unsigned short v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	tSubHeader.fec=v;
	memcpy(p,&tSubHeader,2);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->fec=v;
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_SYN_POINT(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	return tSubHeader.syn_point;
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return pHeader->syn_point;
#endif
}

__INLINE__ void AUDEC_HEADER_SET_SYN_POINT(void*p,unsigned short v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	tSubHeader.syn_point=v;
	memcpy(p,&tSubHeader,2);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->syn_point=v;
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_DO_NOT_SYN(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	return tSubHeader.do_not_syn;
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return pHeader->do_not_syn;
#endif
}

__INLINE__ void AUDEC_HEADER_SET_DO_NOT_SYN(void*p,unsigned short v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	tSubHeader.do_not_syn=v;
	memcpy(p,&tSubHeader,2);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->do_not_syn=v;
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_USED_FEC(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	return tSubHeader.used_fec;
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return pHeader->used_fec;
#endif
}

__INLINE__ void AUDEC_HEADER_SET_USED_FEC(void*p,unsigned short v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	tSubHeader.used_fec=v;
	memcpy(p,&tSubHeader,2);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->used_fec=v;
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_CODEC_ID(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	return tSubHeader.codecid;
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return pHeader->codecid;
#endif
}

__INLINE__ void AUDEC_HEADER_SET_CODEC_ID(void*p,unsigned short v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	AUDEC_SUB_HEADER tSubHeader;
	memcpy(&tSubHeader,p,2);
	tSubHeader.codecid=v;
	memcpy(p,&tSubHeader,2);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->codecid=v;
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_SEQUENCE(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	unsigned short nTemp;
	unsigned char*pData=(unsigned char*)p;
	memcpy(&nTemp,pData+2,2);
	return ntohs(nTemp);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return ntohs(pHeader->seq);
#endif
}

__INLINE__ void AUDEC_HEADER_SET_SEQUENCE(void*p,unsigned short v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	unsigned short nTemp=htons(v);
	unsigned char*pData=(unsigned char*)p;
	memcpy(pData+2,&nTemp,2);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->seq=htons(v);
#endif
}

__INLINE__ unsigned int AUDEC_HEADER_GET_TIMESTAMP(const void*p)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	unsigned int nTemp;
	unsigned char*pData=(unsigned char*)p;
	memcpy(&nTemp,pData+4,4);
	return ntohl(nTemp);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	return ntohl(pHeader->ts);
#endif
}

__INLINE__ void AUDEC_HEADER_SET_TIMESTAMP(void*p,unsigned int v)
{
#if defined(_WIN32_WCE) || defined(NETEC_ARM)
	unsigned int nTemp=htonl(v);
	unsigned char*pData=(unsigned char*)p;
	memcpy(pData+4,&nTemp,4);
#else
	PAUDEC_HEADER pHeader=(PAUDEC_HEADER)p;
	pHeader->ts=htonl(v);
#endif
}

__INLINE__ unsigned short AUDEC_HEADER_GET_LEN(const void*p)
{
	return AUDEC_HEADER_VER_01_LEN;
}


#endif