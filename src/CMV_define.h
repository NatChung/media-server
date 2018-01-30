/*
 *  CMV_define.h
 *  mCamViewBeta
 *
 *  Created by Nat on 2009/9/29.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif
    extern int recordCrashLog(const char* format, ...);
#ifdef __cplusplus
}
#endif

#ifndef CMV_DEFINE_H
#define	CMV_DEFINE_H

#define		P2PC_INTERNAL_LISTEN_PORT           6037	// if failed to bind, we try +100 port, 6137 -> 6237 -> 6337 -> 6437
#define		CAMVIEW_DEFAULT_LISTEN_PORT         6038	// if failed to bind, we try +100 port, 6138 -> 6238 -> 6338 -> 6438
#define		SOCKET_DEFAULT_LISTEN_PORT          6039	// 2013.3.13 Junteng if failed to bind
#define     JITTER_SOCKET_DEFAULT_LISTEN_PORT   6051    // 2014.7.30 Junteng for Jitter Buffer
#define		INTERNAL_SOCKET_MAGIC_ID		0xABCD
#define		GSS_MSG_VERSION					1	// msg version: GSSc <-> CamView
#define		GSS_MSG_VERSION_V2				2	// msg version: GSSc <-> CamView

#define     RECV_BUF_LEN                    32768//8192        // 2015.2.25 Junteng

#define PLAY_OK				1
#define PLAY_FAIL			-1
#define GET_SETTINGS_OK		2
#define GET_SETTINGS_FAIL	-2
#define SET_SETTINGS_OK		3
#define SET_SETTINGS_FAIL	-3
#define DISCONN				-4
#define USER_CANCEL_CONN	5 
#define ERR_TIMEOUT			-9

#define	msg_head_len  		6
#define	msg_head_len_v2  		7
#define version_msgtype_len	2
#define version_msgtype_len_v2	3

typedef struct
	{
		unsigned short	magicid ; 
		unsigned short 	total_length ;
		unsigned char	version ;
		unsigned char 	msg_type;
	} GSS_MSG_HEAD_T ;

typedef struct
	{
    unsigned short	magicid ;
    unsigned short 	total_length ;
    unsigned char	version ;
    unsigned short 	msg_type;
} GSS_MSG_HEAD_T_V2 ;

typedef struct
	{
		unsigned char	type ;
		unsigned char	len ;
		unsigned char	value ;
	} GSS_TLV_T ;

typedef struct
{
    unsigned short	type ;
    unsigned short	len ;
    unsigned char	value ;
} GSS_TLV_T_v2 ;

typedef struct
{
    unsigned int	node;
    unsigned char	name[20];
} GSS_TLV_NAME ;

typedef enum
{
    TLV_VAR_LEN_UINT8 = 1,
    TLV_VAR_LEN_UINT16 = 2,
    TLV_VAR_LEN_UINT32 = 4,
    TLV_VAR_LEN_VARIABLE = 8,
    TLV_VAR_LEN_ASSIGNED_THRU_ANOTHER_VAR = 100		// for now, it's used only for GSS_VAR_RECORDED_FILE_LIST
} TLV_VAR_LEN_E ;

typedef enum
	{
		GSS_MSG_NONE = 0,									// 0x0
		GSS_MSG_SYN_REQ,									// 0x1
		GSS_MSG_ACK_REP,									// 0x2
		GSS_MSG_PLAY_REQ,									// 0x3
		GSS_MSG_PLAY_REP,									// 0x4
		GSS_MSG_PAUSE_REQ,									// 0x5
		GSS_MSG_PAUSE_REP,									// 0x6
		GSS_MSG_RESUME_REQ,									// 0x7
		GSS_MSG_RESUME_REP,									// 0x8
		GSS_MSG_STOP_REQ,									// 0x9 CamView asks IP Cam to stop sending video
		GSS_MSG_STOP_REP,									// 0xA
		GSS_MSG_GET_SETTINGS_REQ,							// 0xB
		GSS_MSG_GET_SETTINGS_REP,							// 0xC
		GSS_MSG_SET_SETTINGS_REQ,							// 0xD
		GSS_MSG_SET_SETTINGS_REP,							// 0xE
		GSS_MSG_AUTO_SEARCH_REQ,							// 0xF
		GSS_MSG_AUTO_SEARCH_REP,							// 0x10
		//GSS_MSG_CHANGE_PASSWD_REQ,						// 0x11
		//GSS_MSG_CHANGE_PASSWD_REP,						// 0x12
		GSS_MSG_GET_CAMID_ONLINE_STATUS_REQ = 0x13,			// 0x13 CamView asks IP Cam for the status of each Cam ID it has
		GSS_MSG_GET_CAMID_ONLINE_STATUS_REP,				// 0x14
		GSS_MSG_GET_IPC_TIME_REQ,							// 0x15 CamView asks P2Pc to get the updated time from IP Cam for synchronization
		GSS_MSG_GET_IPC_TIME_REP,							// 0x16
		GSS_MSG_ADD_ONE_LOCAL_IPCAM_IND,					// 0x17 When P2Pc gets the startup notification from IP Cam, it sends this to CamView
		//GSS_MSG_FRAME_RATE_REPORT,						// 0x18 Updates the count of recvd frames to IP Cam for auto adjust bandwidth
		GSS_MSG_UPGRADE_IPCAM_REQ = 0x19,					// 0x19 CamView asks IP Cam to upgrade its firmware
		GSS_MSG_UPGRADE_IPCAM_REP,							// 0x1A 
		GSS_MSG_UPGRADE_STATUS_IND,							// 0x1B CamView -> p2pc -> IP Cam
		GSS_MSG_MD_RECORDING_SETTING_REQ,					// 0x1C	IP Cam -> p2pc -> CamView
		GSS_MSG_MD_RECORDING_SETTING_REP,					// 0x1D IP Cam -> p2pc -> CamView
		GSS_MSG_MOTION_DETECTED_IND,						// 0x1E p2pc --> CamView
		GSS_MSG_REMOTE_MOTOR_CONTROL_IND,					// 0x1F p2pc --> CamView
		GSS_MSG_MOTOR_MOVING_CONTROL_REQ,					// 0x20 CamView --> p2pc
		GSS_MSG_MOTOR_PATROL_CONTROL_REQ,					// 0x21 p2pc --> CamView
		GSS_MSG_MOTOR_PATROL_CONTROL_REP,					// 0x22
		GSS_MSG_MOTOR_GET_PATROL_SETTINGS_REQ,				// 0x23 for IC502w only
		GSS_MSG_MOTOR_GET_PATROL_SETTINGS_REP,				// 0x24 
		GSS_MSG_AUTO_SEARCH_PLUS_REFRESH_NIC,				// 0x25 
		GSS_MSG_GET_CAMID_ONLINE_STATUS_PLUS_REFRESH_NIC,	// 0x26 
		GSS_MSG_GET_IR_SETTINGS_REQ = 0x27,					// 0x27 
		GSS_MSG_GET_IR_SETTINGS_REP,						// 0x28 
		GSS_MSG_SET_IR_SETTINGS_REQ,						// 0x29 
		GSS_MSG_SET_IR_SETTINGS_REP,						// 0x2A
		GSS_MSG_GET_3GPP_SETTINGS_REQ,						// 0x2B
		GSS_MSG_GET_3GPP_SETTINGS_REP,						// 0x2C
		GSS_MSG_SET_3GPP_SETTINGS_REQ,						// 0x2D
		GSS_MSG_SET_3GPP_SETTINGS_REP,						// 0x2E
		GSS_MSG_GET_SDCARD_INFO_REQ,                        // 0x2F
		GSS_MSG_GET_SDCARD_INFO_REP,                        // 0x30
		GSS_MSG_GET_SDCARD_FILE_LIST_REQ,                   // 0x31
		GSS_MSG_GET_SDCARD_FILE_LIST_REP,                   // 0x32
		GSS_MSG_DELETE_FILE_REQ,                            // 0x33
		GSS_MSG_DELETE_FILE_REP,                            // 0x34
		GSS_MSG_DOWNLOAD_FILE_REQ,                          // 0x35
		GSS_MSG_DOWNLOAD_FILE_REP,                          // 0x36
		GSS_MSG_STOP_DOWNLOAD_FILE_REQ,                     // 0x37
		GSS_MSG_KEEP_ALIVE,									// 0x38
		GSS_MSG_RECORDING_REQ,								// 0x39
		GSS_MSG_RECORDING_REP,								// 0x3A
		GSS_MSG_CYCLIC_RECORDING_SETTINGS,					// 0x3B
		GSS_MSG_RECORDING_STATUS_IND,						// 0x3C P2Pc -> CamView
		GSS_MSG_MD_RECORDING_STATUS_IND,					// 0x3D P2Pc -> CamView
		GSS_MSG_RS485_SETTINGS_IND,							// 0x3E P2Pc -> CamView
		GSS_MSG_RS485_GET_SETTINGS_REQ,						// 0x3F
		GSS_MSG_RS485_GET_SETTINGS_REP,						// 0x40
		GSS_MSG_RS485_SET_SETTINGS_REQ,						// 0x41 not in use for now
		GSS_MSG_RS485_SET_SETTINGS_REP,						// 0x42 not in use for now
		GSS_MSG_RS485_SET_PATROL_REQ,						// 0x43
		GSS_MSG_RS485_SET_PATROL_REP,						// 0x44
		GSS_MSG_DI_DETECTION_SETTINGS_REQ,					// 0x45
		GSS_MSG_DI_DETECTION_SETTINGS_REP,					// 0x46
		GSS_MSG_DI_DETECTION_IND,							// 0x47
		
		GSS_MSG_WIFI_GET_SETTINGS_REQ,						// 0x48
		GSS_MSG_WIFI_GET_SETTINGS_REP,						// 0x49
		GSS_MSG_WIFI_SET_SETTINGS_REQ,						// 0x4A
		GSS_MSG_WIFI_SET_SETTINGS_REP,						// 0x4B
		GSS_MSG_WIFI_SCAN_REQ,								// 0x4C
		GSS_MSG_WIFI_SCAN_REP,								// 0x4D
		GSS_MSG_RECORDED_FILE_LIST_REQ,						// 0x4E
		GSS_MSG_RECORDED_FILE_LIST_REP,						// 0x4F
		GSS_MSG_PLAYBACK_START_REQ,							// 0x50
		GSS_MSG_PLAYBACK_START_REP,							// 0x51
		GSS_MSG_PLAYBACK_STOP_REQ,							// 0x52
		GSS_MSG_PLAYBACK_STOP_REP,							// 0x53
		GSS_MSG_PLAYBACK_FILE_FINISHED_IND,					// 0x54 when GSSc rx file_end, it sends this indication to CamView
		GSS_MSG_PLAYBACK_FILE_STILL_PLAYING,				// 0x55 if CamView rx file finish indication from GSSc but it's still playback the file (buffered in Queue), it sends this msg to GSSc to ask it not to close connection with IP Cam yet
		GSS_MSG_CHECK_MOTOR_CONTROLLABLE_REQ,				// 0x56 currently, used in Android only
		GSS_MSG_CHECK_MOTOR_CONTROLLABLE_REP,				// 0x57 reply variable GSS_VAR_REMOTE_MOTOR_CONTROL, currently, used in Android only
		
		GSS_MSG_GET_ALL_SUPPORTED_TYPES_REQ,				// 0x58
		GSS_MSG_GET_ALL_SUPPORTED_TYPES_REP,				// 0x59
		GSS_MSG_GET_MOTION_DETECTION_TRIGGER_TYPE_REQ,		// 0x5A
		GSS_MSG_GET_MOTION_DETECTION_TRIGGER_TYPE_REP,		// 0x5B
		GSS_MSG_SET_MOTION_DETECTION_TRIGGER_TYPE_REQ,		// 0x5C
		GSS_MSG_SET_MOTION_DETECTION_TRIGGER_TYPE_REP,		// 0x5D
		
		GSS_MSG_GET_CAM_INFO_REQ,							// 0x5E
		GSS_MSG_GET_CAM_INFO_REP,							// 0x5F
		
		GSS_MSG_GET_WIRED_NETWORK_INFO_REQ,					// 0x60
		GSS_MSG_GET_WIRED_NETWORK_INFO_REP,					// 0x61
		GSS_MSG_SET_WIRED_NETWORK_INFO_REQ,					// 0x62
		GSS_MSG_SET_WIRED_NETWORK_INFO_REP,					// 0x63
		
		GSS_MSG_GET_WIFI_NETWORK_INFO_REQ,					// 0x64
		GSS_MSG_GET_WIFI_NETWORK_INFO_REP,					// 0x65
		GSS_MSG_SET_WIFI_NETWORK_INFO_REQ,					// 0x66
		GSS_MSG_SET_WIFI_NETWORK_INFO_REP,					// 0x67
		
		GSS_MSG_GET_ADVANCED_NETWORK_INFO_REQ,				// 0x68
		GSS_MSG_GET_ADVANCED_NETWORK_INFO_REP,				// 0x69
		GSS_MSG_SET_ADVANCED_NETWORK_INFO_REQ,				// 0x6A
		GSS_MSG_SET_ADVANCED_NETWORK_INFO_REP,				// 0x6B
		
		GSS_MSG_GET_EMAIL_ALARM_INFO_REQ,					// 0x6C
		GSS_MSG_GET_EMAIL_ALARM_INFO_REP,					// 0x6D
		GSS_MSG_SET_EMAIL_ALARM_INFO_REQ,					// 0x6E
		GSS_MSG_SET_EMAIL_ALARM_INFO_REP,					// 0x6F
		
		GSS_MSG_GET_DI_DO_INFO_REQ,							// 0x70
		GSS_MSG_GET_DI_DO_INFO_REP,							// 0x71
		GSS_MSG_SET_DI_DO_INFO_REQ,							// 0x72
		GSS_MSG_SET_DI_DO_INFO_REP,							// 0x73
		
		GSS_MSG_GET_SDCARD_ALL_INFO_REQ,					// 0x74
		GSS_MSG_GET_SDCARD_ALL_INFO_REP,					// 0x75
		GSS_MSG_SET_SDCARD_ALL_INFO_REQ,					// 0x76
		GSS_MSG_SET_SDCARD_ALL_INFO_REP,					// 0x77
		
		GSS_MSG_GET_SCHEDULE_EVENTS_INFO_REQ,				// 0x78
		GSS_MSG_GET_SCHEDULE_EVENTS_INFO_REP,				// 0x79
		GSS_MSG_SET_SCHEDULE_EVENTS_INFO_REQ,				// 0x7A
		GSS_MSG_SET_SCHEDULE_EVENTS_INFO_REP,				// 0x7B
		
		GSS_MSG_GET_ADMIN_LOGIN_INFO_REQ,					// 0x7C
		GSS_MSG_GET_ADMIN_LOGIN_INFO_REP,					// 0x7D
		GSS_MSG_SET_ADMIN_LOGIN_INFO_REQ,					// 0x7E
		GSS_MSG_SET_ADMIN_LOGIN_INFO_REP,					// 0x7F	
		
		GSS_MSG_GET_DATE_TIME_INFO_REQ,						// 0x80
		GSS_MSG_GET_DATE_TIME_INFO_REP,						// 0x81
		GSS_MSG_SET_DATE_TIME_INFO_REQ,						// 0x82
		GSS_MSG_SET_DATE_TIME_INFO_REP,						// 0x83
		
		GSS_MSG_REBOOT_DEVICE_REQ,							// 0x84
		GSS_MSG_REBOOT_DEVICE_REP,							// 0x85
		
		GSS_MSG_NEED_UPDATE_CYCLIC_RECORDING_SETTINGS,		// 0x86 Howard. 2011.6.20
		GSS_MSG_PLAYBACK_FILE_STILL_PARSING,				// 0x87 if CamView rx file finish indication from GSSc but it's still parsing the packet (buffered in Queue), it sends this msg to GSSc to ask it not to close connection with IP Cam yet
		
		// Howard. 2011.10.14 for Push Notification
		GSS_MSG_REGISTER_TOKEN_WITH_CAMID,					// 0x88 mCamView sends this to GSSc and GSSc sends out the request to all the PNS. (currently PNS = P2Pd)
		GSS_MSG_UNREGISTER_TOKEN_WITH_CAMID,				// 0x89
		
		GSS_MSG_GET_PUSH_ALARM_SETTINGS_REQ,				// 0x8A
		GSS_MSG_GET_PUSH_ALARM_SETTINGS_REP,				// 0x8B
		GSS_MSG_SET_PUSH_ALARM_SETTINGS_REQ,				// 0x8C
		GSS_MSG_SET_PUSH_ALARM_SETTINGS_REP,				// 0x8D
		
		GSS_MSG_AUDIO_CONTROL_REQ,							// 0x8E
		GSS_MSG_AUDIO_CONTROL_REP,							// 0x8F
		GSS_MSG_AUDIO_DATA,									// 0x90, CamView -> GSSc data
		
		GSS_MSG_CAMID_STATUS_CHECK_DONE_IND,				// 0x91, GSSc -> CamView. For GSS_MSG_GET_CAMID_ONLINE_STATUS_REQ request, after all GSS_MSG_GET_CAMID_ONLINE_STATUS_REP are sent back, send this msg to notify CamView
		
		GSS_MSG_GET_SPEAKER_SETTINGS_REQ,					// 0x92
		GSS_MSG_GET_SPEAKER_SETTINGS_REP,					// 0x93
		GSS_MSG_SET_SPEAKER_SETTINGS_REQ,					// 0x94
		GSS_MSG_SET_SPEAKER_SETTINGS_REP,					// 0x95
		
		GSS_MSG_GET_3G_DONGLE_SETTINGS_REQ,                 // 0x96
		GSS_MSG_GET_3G_DONGLE_SETTINGS_REP,                 // 0x97
		GSS_MSG_SET_3G_DONGLE_SETTINGS_REQ,                 // 0x98
		GSS_MSG_SET_3G_DONGLE_SETTINGS_REP,                 // 0x99
		
		GSS_MSG_TEMPERATURE_INFO_IND,		                // 0x9A
		GSS_MSG_WIFI_QRCODE_SUCCESS_IND,	                // 0x9B
		
		GSS_MSG_SDCARD_DELETE_REQ,							// 0x9C
		GSS_MSG_SDCARD_DELETE_REP,							// 0x9D
		
		GSS_MSG_GET_TEMPERATURE_RECORDS_REQ,				// 0x9E
		GSS_MSG_GET_TEMPERATURE_RECORDS_REP,				// 0x9F
		
		GSS_MSG_GET_DROPBOX_REQ,							// 0xA0
		GSS_MSG_GET_DROPBOX_REP,							// 0xA1
		GSS_MSG_SET_DROPBOX_REQ,							// 0xA2
		GSS_MSG_SET_DROPBOX_REP,							// 0xA3
		
        GSS_MSG_ZWAVE_REGISTER_TOKEN_WITH_CAMID,			// 0xA4 mCamView sends this to GSSc and GSSc sends out the request to all the PNS. (currently PNS = P2Pd)
        GSS_MSG_ZWAVE_UNREGISTER_TOKEN_WITH_CAMID,			// 0xA5
        
        GSS_MSG_CHANGE_CAMERA_NAME_FOR_PNS,                 // 0xA6 Larry.2015.06.26 for change camera name in PNS
        GSS_MSG_OPEN_CLOSE_ALL_PUSH,                        // 0xA7 2015.8.25 Junteng
        GSS_MSG_GET_PRIVACY_MODE_REQ ,                      // 0xA8 2015.11.18 Allen
        GSS_MSG_GET_PRIVACY_MODE_REP ,                      // 0xA9
        GSS_MSG_SET_PRIVACY_MODE_REQ ,                      // 0xAA
        GSS_MSG_SET_PRIVACY_MODE_REP ,                      // 0xAB
		
        GSS_MSG_DEL_NAS_FILE_REQ,                    // 0xAC, 20170618 added,
        GSS_MSG_DEL_NAS_FILE_REP,                   // 0xAD,
        
		GSS_MSG_LOCATE_REP = 0xE0,							// 0xE0
		GSS_MSG_HANDSHAKE1_FAIL_REP,						// 0xE1
		GSS_MSG_HANDSHAKE2_FAIL_REP,						// 0xE2
		GSS_MSG_PLAY_MOBILE_REQ,							// 0xE3
		// Howard. 2011.2.18 for debugging
		GSS_MSG_UPDATE_CONNECTION_STATUS,					// 0xE4 works with GSS_VAR_CONNECTION_STATUS
		GSS_MSG_CHECK_INTERNET,								// 0xE5
        
        GSS_MSG_PRE_SETUP_REQ,								// 0xE6	Howard. 2014.6.26
        GSS_MSG_CLOSE_PRE_SETUP_REQ,						// 0xE7	Howard. 2014.7.11

		GSS_MSG_CLOSE_REQ = 0xF0,							// 0xF0 CamView asks P2Pc to kill itself
		GSS_MSG_CLOSE_REP,									// 0xF1
		GSS_MSG_CLOSE_CHANNEL_IND,							// 0xF2 P2Pc asks CamView to close certain window which is playing video (see reason GSS_VAR_STOP_REASON)
		GSS_MSG_CLOSE_CHANNEL_CONF,							// 0xF3
		GSS_MSG_PAUSE_ROBUST_BUFFERING,						// 0xF4 currently, only for iPhone
        GSS_MSG_GET_DISABLE_ALL_EVENTS_REQ,                 // 0xF5
        GSS_MSG_GET_DISABLE_ALL_EVENTS_REP,                 // 0xF6
        GSS_MSG_SET_DISABLE_ALL_EVENTS_REQ,                 // 0xF7
        GSS_MSG_SET_DISABLE_ALL_EVENTS_REP,                 // 0xF8
        //GSS_MSG_LOCATE_P2PD_REPLY,                          // 0xF9
    #ifdef ANDROID
        GSS_MSG_PLAYBACK_LOCAL_FILE_REQ,					// 0xF9
        GSS_MSG_PLAYBACK_LOCAL_FILE_REP,					// 0xFA
        GSS_MSG_PLAYBACK_LOCAL_FILE_FINISH_IND,				// 0xFB
    #endif
        GSS_MSG_LOCATE_P2PD_REPLY = 0xFC,                   // 0xFC
        GSS_MSG_TRANSPARENT_REQ,						    // 0xFD
        GSS_MSG_TRANSPARENT_REP,							// 0xFE
        
		GSS_MSG_END
	} GSS_MSG_TYPE_E ;

typedef enum
	{
        GSS_VAR_USELESS = 0,
		GSS_VAR_CHANNEL = 1,			// 0x1 the window number of the CamView
		GSS_VAR_IPC_ID,					// 0x2
		GSS_VAR_IPC_PASSWD,				// 0x3 this password is for accessing the video
		GSS_VAR_RESOLUTION_X,			// 0x4
		GSS_VAR_RESOLUTION_Y,			// 0x5
		GSS_VAR_BIT_RATE,				// 0x6 
		GSS_VAR_FRAME_RATE,				// 0x7 frame rate
		GSS_VAR_STREAM_PORT,			// 0x8 
		GSS_VAR_RESPONSE_CODE,			// 0x9 value 0 means the CamView's request is success, value 1 means the CamView's request is failed.
		GSS_VAR_FAILED_REASON,			// 0xA if response code is 1 (failed), then this is the reason why the request failed
		GSS_VAR_IPC_IP,					// 0xB the IP address of the Cam ID which is used when users auto-search the CamID
		GSS_VAR_IPCAM_FOUND,			// 0xC 
		GSS_VAR_AP_LISTEN_PORT,			// 0xD CamView keeps listening on this port for P2Pc's actively request.
		GSS_VAR_ADMIN_ACCOUNT,			// 0xE the admin account for modifying the settings of the IP Cam
		GSS_VAR_ADMIN_PASSWD,			// 0xF the admin password for modifying the settings of the IP Cam
		GSS_VAR_CAMID_ONLINE_STATUS,	// 0x10
		GSS_VAR_IPC_CURRNET_TIME,		// 0x11 format: year-month-day-hour-minute-second
		GSS_VAR_MIC_ENABLE,				// 0x12		// 0: ignore, 1: off, 2:on
		GSS_VAR_AUTO_SELECT,			// 0x13		// 0: manual, 1: auto select
		GSS_VAR_RECORDING_ON,			// 0x14		// 0: recording off, 1: recording on
		//GSS_VAR_FRAME_COUNTS,			// 0x15
		GSS_VAR_INDOOR_MODE = 0x16,		// 0x16
		GSS_VAR_LIGHT_FREQ,				// 0x17
		GSS_VAR_LED_MODE,				// 0x18
		GSS_VAR_BRIGHTNESS,				// 0x19
		GSS_VAR_FTPSERVER,				// 0x1A
		GSS_VAR_FTPUSER,				// 0x1B
		GSS_VAR_FTPPASSWD,				// 0x1C
		GSS_VAR_FTPFWNAME,				// 0x1D
		GSS_VAR_UPGRADE_STATUS,			// 0x1E
		GSS_VAR_UPGRADE_PERCENT,		// 0x1F
	#if 1	// Howard. 2013.2.19
		GSS_VAR_UPGRADE_DO_NOT_RESTART,	// 0x20		If we have config file to upgrade after firmware upgrade, set this variable
	#else
		//GSS_VAR_HTTP_PROXY_ADDRESS,		// 0x20
		//GSS_VAR_HTTP_PROXY_PORT,			// 0x21
		//GSS_VAR_HTTP_PROXY_CREDENTIAL,	// 0x22		username:password    ex. "admin:1234"
	#endif
		GSS_VAR_MD_RECORDING_ENABLE = 0x23,	// 0x23
		GSS_VAR_MD_SENSITIVITY,			// 0x24
		GSS_VAR_TIME_ON_VIDEO,			// 0x25
		GSS_VAR_VIDEO_SYSTEM,			// 0x26		0: digital, 1:NTSC, 2:PAL
		GSS_VAR_IPCAM_FW_VERSION,		// 0X27
		GSS_VAR_IPCAM_MODEL_ID,			// 0x28		refer to IPCAM_MODEL_ID_E
		GSS_VAR_MOVING_DIRECTION,		// 0x29		CamView --> p2pc --> IP Cam
		GSS_VAR_MOVING_SPEED,			// 0x2A		for IC502w only. not for VS306/316! CamView --> p2pc --> IP Cam
		GSS_VAR_REMOTE_MOTOR_CONTROL,	// 0x2B		
		GSS_VAR_PATROL_STAY_TIME,		// 0x2C		CamView --> p2pc --> IP Cam, unit: seconds
		GSS_VAR_PATROL_POS1_COORDINATE,	// 0x2D     IP Cam --> p2pc --> CamView, string "x,y". for example "120,88"
		GSS_VAR_PATROL_POS2_COORDINATE,	// 0x2E		IP Cam --> p2pc --> CamView
		GSS_VAR_PATROL_POS3_COORDINATE,	// 0x2F		IP Cam --> p2pc --> CamView
		GSS_VAR_PATROL_POS4_COORDINATE,	// 0x30		IP Cam --> p2pc --> CamView
		GSS_VAR_PATROL_POS5_COORDINATE,	// 0x31		IP Cam --> p2pc --> CamView
		GSS_VAR_PATROL_COMMAND,			// 0x32		CamView --> p2pc --> IP Cam, refer to PATROL_COMMAND_E
		GSS_VAR_PATROL_PONITS,			// 0x33		
		GSS_VAR_PREFERENCE_SUPPORTED,	// 0x34		1: supported
		GSS_VAR_PREFERENCE,				// 0x35		0: Motion, 1: Quality
		GSS_VAR_IR_MODE,				// 0x36		0: auto, 1: schedule, 2: manual
		GSS_VAR_IR_SCHEDULE_START_HOUR,	// 0x37
		GSS_VAR_IR_SCHEDULE_START_MIN,	// 0x38
		GSS_VAR_IR_SCHEDULE_STOP_HOUR,	// 0x39
		GSS_VAR_IR_SCHEDULE_STOP_MIN,	// 0x3A
		GSS_VAR_IR_MANUAL_IR_ON,		// 0x3B		0: IR Off, 1: IR On
		GSS_VAR_IR_CURRENT_STATE,		// 0x3C		0: IR Off, 1: IR On
		GSS_VAR_IMAGE_REVERSAL_SUPPORTED,// 0x3D
		GSS_VAR_IMAGE_REVERSAL_VALUE,	// 0x3E
		GSS_VAR_LOW_LIGHT_SUPPORTED,	// 0x3F
		GSS_VAR_LOW_LIGHT_SENSITIVITY,	// 0x40		// 0: high, 1: normal, 2: very high
		GSS_VAR_VIDEO_COLOR,			// 0x41		// 0: color, 1: black & white
		GSS_VAR_TGPP_DISABLE,			// 0x42
		GSS_VAR_VIDEO_OVER_TCPTS,		// 0x43
		GSS_VAR_RECORDING_PATH_UTF8,	// 0x44
		GSS_VAR_RESERVED_DISK_SPACE,	// 0x45		// in MB
		GSS_VAR_DISK_FULL_ACTION,		// 0x46		// refers to CYCLIC_RECORDING_DISK_FULL_ACTION_E in cyclic_recording.h
		GSS_VAR_RECORDING_STATUS,		// 0X47		// 0 : recording stopped, 1: recording in progress
		
		GSS_VAR_RS485_ENABLE,					// 0x48
		GSS_VAR_RS485_PROTOCOL,					// 0x49
		GSS_VAR_RS485_CAMERA_ID,				// 0x4A
		GSS_VAR_RS485_BAUD_RATE,				// 0x4B
		//GSS_VAR_RS485_COMMAND,						// use GSS_VAR_PATROL_COMMAND instead
		//GSS_VAR_RS485_MOVING_SPEED,					// use GSS_VAR_MOVING_SPEED instead
		GSS_VAR_RS485_PRESET_COUNTS,			// 0x4C
		GSS_VAR_RS485_PRESET_NAME,				// 0x4D
		GSS_VAR_RS485_PATROL_IS_RUNNING,		// 0x4E
		GSS_VAR_RS485_PATROL_SELECTED_TOUR,		// 0x4F
		GSS_VAR_RS485_PATROL_COUNTS,			// 0x50
		GSS_VAR_RS485_PATROL_SEQUENCE,			// 0x51
		GSS_VAR_RS485_ZOOM,						// 0x52 0: zoom out, 1: zoom in
		GSS_VAR_RS485_FOCUS,					// 0x53 0: - focus (near), 1: + focus (far)
		
		GSS_VAR_IR_NUMBERS_SUPPORTED,			// 0x54
		GSS_VAR_IR_NUMBERS,						// 0x55
		GSS_VAR_SHARPNESS_SUPPORTED,			// 0x56
		GSS_VAR_SHARPNESS,						// 0x57
		
		GSS_VAR_DI_DETECTION_ENABLE,			// 0x58 1: enabled, 0: disabled
		
		GSS_VAR_WIFI_STATUS,					// 0x59
		GSS_VAR_WIFI_ENABLED,					// 0x5A
		GSS_VAR_WIFI_SSID,						// 0x5B
		GSS_VAR_WIFI_SECURITY_MODE,				// 0x5C
		GSS_VAR_WIFI_WEP_ENCRYPT,				// 0x5D
		GSS_VAR_WIFI_WEP_KEY,					// 0x5E
		GSS_VAR_WIFI_WPA_ENCRYPT,				// 0x5F
		GSS_VAR_WIFI_WPA_KEY,					// 0x60
		GSS_VAR_USER_INFO_PROVIDED,				// 0x61 0: No, 1: Provided
		GSS_VAR_CONNECTED_USERS,				// 0x62
		GSS_VAR_NAS_RECORDING_IN_PROGRESS,		// 0x63 0: No, 1: Yes recording to NAS in progress
		GSS_VAR_SD_RECORDING_IN_PROGRESS,		// 0x64 0: No, 1: Yes recording to SD card in progress, 2: SD card not supported
		
		GSS_VAR_TUNNEL_SERVER_IP,				// 0x65	// Howard. 2010.4.9 in big endian, network order
		
		GSS_VAR_NEXT_TLV_LEN,                   // 0x66		// in big-endian. IMPORTANT: this variable must be sent before GSS_VAR_RECORDED_FILE_LIST
		GSS_VAR_RECORDED_FILE_LIST,				// 0x67		// the len of the data is indicated through GSS_VAR_RECORDED_FILE_LIST_LEN
		GSS_VAR_RECORDED_FILE_LIST_END,			// 0x68
		GSS_VAR_PLAYBACK_FILE_NAME,				// 0x69
		GSS_VAR_PLAYBACK_TYPE,					// 0x6A		// refers to PLAYBACK_TYPE_E
		GSS_VAR_SPECIAL_FEATURE,				// 0x6B
		
		GSS_VAR_MOTION_TRIGGER_TYPE,			// 0x6C
		
		GSS_VAR_LOGIN_USERNAME,					// 0x6D, to set new username, max string size is 19
		GSS_VAR_LOGIN_PASSWORD,                 // 0x6E, to set new password, max string size is 19
		GSS_VAR_REBOOT,							// 0x6F
		GSS_VAR_TIME_ZONE,						// 0x70, valid range from 1 to 51
		GSS_VAR_DAYLIGHT_SAVING_TIME,			// 0x71
		GSS_VAR_DST_RANGE,						// 0x72, 6 bytes for the daylight saving time range
		GSS_VAR_SYSTEM_UP_TIME,					// 0x73, in seconds
		GSS_VAR_CURRENT_TIME,					// 0x74, 6 bytes, same as P2P_IPC_TIME_T
		GSS_VAR_WIRED_DHCP,						// 0x75
		GSS_VAR_WIRED_IP_ADDR,					// 0x76
		GSS_VAR_WIRED_NETMASK,					// 0x77
		GSS_VAR_WIRED_GATEWAY,					// 0x78
		GSS_VAR_WIRED_DNS1,						// 0x79
		GSS_VAR_WIRED_DNS2,						// 0x7A
		GSS_VAR_WIFI_SCAN,						// 0x7B, use only when CamView asks remote cam to do wifi scan. the var for reply is GSS_VAR_WIFI_AP_INFO, not GSS_VAR_WIFI_SCAN
		GSS_VAR_WIFI_TEST,						// 0x7C
		GSS_VAR_WIFI_DHCP,						// 0x7D
		GSS_VAR_WIFI_IP_ADDR,					// 0x7E
		GSS_VAR_WIFI_NETMASK,					// 0x7F
		GSS_VAR_WIFI_GATEWAY,					// 0x80
		GSS_VAR_WIFI_DNS1,						// 0x81
		GSS_VAR_WIFI_DNS2,						// 0x82
		GSS_VAR_PPPOE_ENABLE,					// 0x83
		GSS_VAR_PPPOE_USERNAME,					// 0x84, max string size is 31
		GSS_VAR_PPPOE_PASSWORD,					// 0x85, max string size is 19
		GSS_VAR_MOTION_SENSITIVITY,				// 0x86, motion sensitivity 1~10, 1:high, 10:low
		GSS_VAR_EMAIL_ALARM,					// 0x87, 0:disable	1:enable email alarm
		GSS_VAR_EMAIL_RECIPIENT1,				// 0x88, max string size is 51
		GSS_VAR_EMAIL_RECIPIENT2,				// 0x89, max string size is 51
		GSS_VAR_EMAIL_RECIPIENT3,				// 0x8A, max string size is 51
		GSS_VAR_SMTP_SERVER,					// 0x8B, max string size is 31
		GSS_VAR_SMTP_PORT,						// 0x8C
		GSS_VAR_SMTP_USERNAME,					// 0x8D, max string size is 51
		GSS_VAR_SMTP_PASSWORD,					// 0x8E,  max string size is 32
		GSS_VAR_DI_NORMAL_STATUS,				// 0x8F, 0:open circuit, 1:closed circuit
		GSS_VAR_DI_CURRENT_STATUS,				// 0x90, 0:normal status, 1:active status
		GSS_VAR_DO_NORMAL_STATUS,				// 0x91, 0:open circuit, 1:closed circuit
		GSS_VAR_DO_CURRENT_STATUS,				// 0x92, 0:normal status, 1:active status
		GSS_VAR_DO_ALARM_TRIGGER,				// 0x93, 0:force to normal, 8:force to active, 4:schedule trigger
		// 		 1:motion drigger, 2:DI trigger, 1 and 2 could be bit wised
		GSS_VAR_DO_ALARM_DURATION,				// 0x94
		GSS_VAR_SD_RECORDING,					// 0x95, 0:disable, 1:schedule, 2:always recording
		GSS_VAR_SD_FULL_OPTION,					// 0x96, 0: stop, 1: circular
		GSS_VAR_SD_STATUS,						// 0x97
		GSS_VAR_SD_SIZE_CAPACITY,				// 0x98, in MB
		GSS_VAR_SD_SIZE_AVAIALBLE,				// 0x99, in MB
		GSS_VAR_SCHEDULE_EVENTS,				// 0x9A, refers to SCHEDULES_T. format: eventAction-scheduleMethod-weekDayFlag-startTime-endTime
		GSS_VAR_REGISTRATION_STATUS,			// 0x9B
		GSS_VAR_CURRENT_IP_ADDR,				// 0x9C
		GSS_VAR_CURRENT_NETWORK_TYPE,			// 0x9D, string. ex. wired(DHCP)
		GSS_VAR_MAC_ADDR,						// 0x9E
		GSS_VAR_WIFI_MAC_ADDR,					// 0x9F
		GSS_VAR_NAT_TYPE,						// 0xA0
		GSS_VAR_WIFI_AP_INFO,					// 0xA1, used for returning the wifi AP scan results. format: SSID-signal-speed-mode. ex. Starvedia-7-54-1
		GSS_VAR_SMTP_USE_USER_ACCOUNT,			// 0xA2
		GSS_VAR_GET_ALL_SUPPORTED_TYPES,		// 0xA3
		GSS_VAR_CAMERA_SETTING_SUPPORTED_TYPE,	// 0xA4

		// Howard. 2011.10.14 for Push Notification
		GSS_VAR_PUSH_NOTIFICATION_TOKEN,		// 0xA5
		GSS_VAR_PNS_TRIGGER_SOURCES,			// 0xA6 refers to PNS_TRIGGERED_BY_E, bitwise
		
		// Howard. 2011.10.21
		GSS_VAR_SOUND_ALARM_STATUS,				// 0xA7
		GSS_VAR_SOUND_ALARM_SENSITIVITY,		// 0xA8
		GSS_VAR_PIR_STATUS,						// 0xA9
		GSS_VAR_CURRENT_TEMPERATURE,			// 0xAA
		GSS_VAR_HIGH_TEMPERATURE,				// 0xAB
		GSS_VAR_LOW_TEMPERATURE,				// 0xAC
		GSS_VAR_TEMPERATURE_TYPE,				// 0xAD, 0: Celsius, 1: Fahrenheit
		GSS_VAR_TEMPERATURE_ALARM,				// 0xAE, 0: trigger when outside of [low:high], 1: inside of [low:high]
		GSS_VAR_TEMPERATURE_ALARM_STATUS,		// 0xAF
		GSS_VAR_CAMERA_NAME,					// 0xB0
		
		GSS_VAR_AUDIO_COMMAND,					// 0xB1, Request(CMV->GSSc) 0: stop playing, 1: start playing; Reply(GSSc->CMV) 0: forced stopped by Camera. 1: request received (not sure if it's play or stop)
		GSS_VAR_AUDIO_DATA,						// 0xB2 CamView -> GSSc for 2-way audio. this variable must be added lastly. no more variable will be parsed after this variable
		
		GSS_VAR_SPEAKER_SUPPORT,				// 0xB3
		GSS_VAR_NIGHT_MODE_SUPPORT,				// 0xB4
		GSS_VAR_SPK_ALARM_TRIGGER,				// 0xB5
		GSS_VAR_SPK_ALARM_LOOP_TIMES,			// 0xB6
		GSS_VAR_SPK_ALARM_PLAY,					// 0xB7

		GSS_VAR_3G_MANUAL_CONFIGURE,			// 0xB8, 0: auto configure, 1: manual configure
		GSS_VAR_3G_ISP_SELECT,					// 0xB9
		GSS_VAR_3G_APN,							// 0xBA, string, max size is 32
		GSS_VAR_3G_ATD,							// 0xBB, string, max size is 20
		GSS_VAR_3G_PIN,							// 0xBC, string, max size is 12
		GSS_VAR_3G_USERNAME,					// 0xBD, string, max size is 32
		GSS_VAR_3G_PASSWORD,					// 0xBE, string, max size is 20
		GSS_VAR_3G_ISP_NAME,					// 0xBF, string
		GSS_VAR_3G_DONGLE_INFO,					// 0xC0
		
		GSS_VAR_PLAY_MOBILE_SUPPORTED,			// 0xC1
		GSS_VAR_320x480_SUPPORTED,				// 0xC2
		GSS_VAR_CONNECTION_STATUS,				// 0xC3 Howard. 2011.2.18 for debugging
		
	#if 1	// Howard. 2012.7.31
		GSS_VAR_FW_VERSION,						// 0xC4 int. ex. v030304 -> 30304
		GSS_VAR_CONFIG_VERSION,					// 0xC5 int
		GSS_VAR_CLIENT_MODEL_ID,				// 0xC6 string
	#endif
		GSS_VAR_WIFI_QRCODE_ID,					// 0xC7		//2013.5.22 Junteng
		GSS_VAR_IPC_WAN_IP,						// 0xC8	Camera's WAN IP, big endian
        GSS_VAR_FUNCTION_BITS,                  // 0xC9
        GSS_VAR_NAS_DELETE,                     // 0xCA NAS delete
		GSS_VAR_MORE_PACKETS = 0xD0,			// 0xD0
	#if ANDROID || IPHONE
		GSS_VAR_MOBILE_OP = 0xD1,				// 0xD1
		GSS_VAR_PAUSE_ROBUST_BUFFERING,			// 0xD2
	#endif
        GSS_VAR_ZWAVE_NUMBER_EVENT = 0XD6,		// 0xD6
        GSS_VAR_ZWAVE_EVENT,					// 0xD7	value: referts to structure NODE_TRIGGER_EVENT_T
        
        GSS_VAR_ZWAVE_SCENE_EVENT,				// 0xD8
        GSS_VAR_ZWAVE_PUSH_TYPE,                // 0xD9
        GSS_VAR_PRIVACY_MODE,                   // 0xDA 2015.11.18 Allen 0x99: in privacy mode, else:not
        GSS_VAR_SET_PRIVACY_PUSH_TYPE,          // 0xDB 2015.11.27 for privacy mode push
        GSS_VAR_SDC_TIME_LAPSE_STATUS,          // 0xDC Tianxi, 2017.1.5, 1:recording now, 0 : not
        
		GSS_VAR_STOP_REASON = 0xE0,				// 0xE0
		GSS_VAR_SELF_WAN_IP,					// 0xE1 tell CamView our WAN IP, big endian
		GSS_VAR_PNS_Reply = 0xE2,				// 0xE2 2014.1.4 Junteng Push_Resend_Mechanism
		GSS_VAR_SDCARD_DELETE,					// 0xE3 2014.3.5 Junteng sdcard delete
		GSS_VAR_TEMPERATURE_RECORDS = 0xE4,		// 0xE4 
		
		//2014.4.3 Junteng Dropbox
		GSS_VAR_CLOUD_SELECT,					// 0xE5
		GSS_VAR_CLOUD_RECORDING_OPTION,			// 0xE6
		GSS_VAR_CLOUD_KEEP_DAYS,				// 0xE7
		GSS_VAR_CLOUD_LEFT_FREE_SPACE,			// 0xE8
		GSS_VAR_CLOUD_FULL_OPTION,				// 0xE9
		GSS_VAR_CLOUD_STATUS,					// 0xEA
		GSS_VAR_NAS_IP_ADDRESS,					// 0xEB
		GSS_VAR_NAS_SERVER_NAME,				// 0xEC
		GSS_VAR_NAS_SHARED_FOLDER,				// 0xED
		GSS_VAR_NAS_ACCESS_ACCOUNT,				// 0xEE
		GSS_VAR_NAS_ACCESS_PASSWORD,			// 0xEF
		GSS_VAR_DROPBOX_TOKEN,					// 0xF0
		GSS_VAR_DROPBOX_TOKEN_SECRET,			// 0xF1
        GSS_VAR_CLOUD_LOGOUT,                   // 0XF2
		GSS_VAR_DISABLE_ALL_EVENTS,             // 0xF3
        GSS_VAR_LOCATE_REPLY,                   // 0xF4
        GSS_VAR_SCHEDULE_EVENTS_V2,             // 0xF5
        GSS_VAR_HTTPSERVER = 0xF6,              // 0xF6 2015.4.20 Junteng
        GSS_VAR_USEHTTP,                        // 0xF7
        GSS_VAR_MSG_REQ_TYPE ,                  // 0xF8, 0: Get, 1: Set. Mainly for GSS_MSG_TRANSPARENT_REQ
        GSS_VAR_TRANSPARENT_MSG,                // 0xF9
        GSS_VAR_HOME_CAMERA_CNT,                // 0xFA
        GSS_VAR_HOME_CAMERA_LIST,               // 0xFB
        GSS_VAR_PUSH_OPEN,                      // 0xFC
#ifdef WIN32
        GSS_VAR_IGNORE_TRY_OP,					// 0xFD for p2p server service down detection use only. ignore try_op, let it go with tunnel connection
#endif
        GSS_VAR_END = 0xFE,
        GSS_VAR_SET_SCHEDULE = 0x0100,
        GSS_VAR_INIT_PUSH,
        GSS_VAR_HOUSE_MODE_PUSH,                // 2017.1.11 Allen set house mode push
        GSS_VAR_ADD_DEVICE_INCREASETIME,        // 2017.4.5 Junteng set add device time out
} GSS_VARIABLE_TYPE_E ;

typedef enum
	{
		PCMD_SET_ONE_COORDINATE = 1,		// set coordinate of one of the five locations
		PCMD_START_PATROL,					// 2
		PCMD_STOP_PATROL,					// 3
		PCMD_SET_PATROL_POINTS,				// 4
		PCMD_PAN_SCAN,						// 5
		PCMD_TILT_SCAN,						// 6
		PCMD_GOTO_PRESET_INDEX,				// 7
		PCMD_CHANGE_MOVING_SPEED,			// 8 for RS485 only.
		PCMD_ZOOM,							// 9 for RS485 only
		PCMD_FOCUS,							// 10 for RS485 only
		PCMD_CLEAR_PRESET,					// for RS485 only
		PCMD_ADD_PRESET,					// for RS485 only
        
        PCMD_GET_RS485_SETTINGS = 0xA0, // for RS485 only
        PCMD_SET_RS485_SETTINGS,   // for RS485 only
        PCMD_GET_ALL_COORDINATE       // only for IC502w to get the patrol settings
	} PATROL_COMMAND_E ;

#define BSDSOCKET_FIX 10


#endif
