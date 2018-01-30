#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#import <unistd.h>
#import <sys/socket.h>
#import <netinet/in.h>

#include "CMV_define.h"

GSS_TLV_T * AddTLVWithLen( GSS_TLV_T * p, GSS_VARIABLE_TYPE_E type, char * val_str, unsigned short val_str_len, unsigned short * total_len )
{
	memcpy( (char *)&(p->value), val_str, val_str_len ) ;
	p->len = TLV_VAR_LEN_ASSIGNED_THRU_ANOTHER_VAR ;
	p->type = type ;
	//*total_len += 2 + p->len ;
	//p = ( GSS_TLV_T * )( ( char * )p + 2 + p->len ) ;
	*total_len += 2 + val_str_len ;
	p = ( GSS_TLV_T * )( ( char * )p + 2 + val_str_len ) ;
	return p ;
}

GSS_TLV_T * AddTLV( GSS_TLV_T * p, GSS_VARIABLE_TYPE_E type, char len, int val_int, char * val_str, unsigned short * total_len )
{
	unsigned short tmp_16 = 0 ;

	switch( len )
	{
		case TLV_VAR_LEN_UINT8:
			p->len = len ;
			p->value = val_int ;
			break ;
		case TLV_VAR_LEN_UINT16:
			memcpy( (char *)&(p->value), (char *)&val_int, 2 ) ;
			p->len = len ;
			break ;
		case TLV_VAR_LEN_UINT32:	// Howard. 2011.3.7
			memcpy( (char *)&(p->value), (char *)&val_int, 4 ) ;
			p->len = len ;
			break ;
		case TLV_VAR_LEN_VARIABLE:
			if( !val_str )
			{
				return p ;
			}
			p->len = strlen( val_str ) ;
			if( 0 == p->len )
			{
				return p ;
			}
#if 1	// Howard. 2012.2.17
			strcpy( (char *)&(p->value), val_str ) ;
			*( (char *)( &(p->value) ) + p->len ) = 0 ;
			p->len += 1 ;	// +1 for zero (zero-terminated string)
#else
			strcpy( (char *)&(p->value), val_str ) ;
#endif
			break ;
		default:
			return p ;
	}
	p->type = type ;
	*total_len += 2 + p->len ;
	p = ( GSS_TLV_T * )( ( char * )p + 2 + p->len ) ;
	return p ;
}

int SendAudioCommand(int fd, char audioOn ,int channel_number)
{
	GSS_TLV_T * p __attribute__((unused)) ;
	GSS_MSG_HEAD_T *h=NULL;
	char sendbuf[64];
	int msg_total_len=0 ,sendbuf_size=0;
	unsigned short TLV_len=0;
	unsigned char windows_number = 0 ;

    struct sockaddr_in SvGSS_addr ;
    SvGSS_addr.sin_family = AF_INET ;
    SvGSS_addr.sin_addr.s_addr = 0x0100007f;
    SvGSS_addr.sin_port = htons(P2PC_INTERNAL_LISTEN_PORT) ;
	
	printf("\n SendAudioCommand on = %d\n", audioOn ) ;
	h = (GSS_MSG_HEAD_T *)sendbuf ;
	h->magicid = htons(INTERNAL_SOCKET_MAGIC_ID) ;
	h->version = GSS_MSG_VERSION ;
	h->msg_type = GSS_MSG_AUDIO_CONTROL_REQ ;
	
	p = (GSS_TLV_T *)&(sendbuf[6]) ;
	p = AddTLV( p, GSS_VAR_CHANNEL, TLV_VAR_LEN_UINT8, channel_number, "", &TLV_len ) ;
	
	p = AddTLV( p, GSS_VAR_AUDIO_COMMAND, TLV_VAR_LEN_UINT8, audioOn, "", &TLV_len ) ;
	p = AddTLV( p, GSS_VAR_END, TLV_VAR_LEN_UINT8, 0, "", &TLV_len ) ;
	
	msg_total_len = version_msgtype_len + TLV_len;//version and message type (2unsigned char)
	h->total_length = htons(msg_total_len);
	
	sendbuf_size = msg_head_len + TLV_len ;//magid 2 unsigned char
	return sendto(fd, sendbuf, sendbuf_size , 0, (struct sockaddr *)&SvGSS_addr, sizeof(SvGSS_addr));
}

int SendAudioData(int fd, char * audioData, int channel_number)
{
	GSS_TLV_T * p __attribute__((unused)) ;
	GSS_MSG_HEAD_T *h=NULL;
	char sendbuf[900];
	int msg_total_len=0 ,sendbuf_size=0;
	unsigned short TLV_len=0;
	unsigned char windows_number = 0 ;

    struct sockaddr_in SvGSS_addr ;
    SvGSS_addr.sin_family = AF_INET ;
    SvGSS_addr.sin_addr.s_addr = 0x0100007f;
    SvGSS_addr.sin_port = htons(P2PC_INTERNAL_LISTEN_PORT) ;
	
	h = (GSS_MSG_HEAD_T *)sendbuf ;
	h->magicid = htons(INTERNAL_SOCKET_MAGIC_ID) ;
	h->version = GSS_MSG_VERSION ;
	h->msg_type = GSS_MSG_AUDIO_DATA ;
	
	p = (GSS_TLV_T *)&(sendbuf[6]) ;
	p = AddTLV( p, GSS_VAR_CHANNEL, TLV_VAR_LEN_UINT8, channel_number, "", &TLV_len ) ;
	
	p = AddTLVWithLen( p, GSS_VAR_AUDIO_DATA, audioData, 800, &TLV_len) ;
	p = AddTLV( p, GSS_VAR_END, TLV_VAR_LEN_UINT8, 0, "", &TLV_len ) ;
	
	msg_total_len = version_msgtype_len + TLV_len;//version and message type (2unsigned char)
	h->total_length = htons(msg_total_len);
	
	sendbuf_size = msg_head_len + TLV_len ;//magid 2 unsigned char

	return sendto(fd, sendbuf, sendbuf_size , 0, (struct sockaddr *)&SvGSS_addr, sizeof(SvGSS_addr));
}