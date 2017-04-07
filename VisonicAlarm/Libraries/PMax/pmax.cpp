#include "pmax.h"
#include <string.h>
#include <stdlib.h>
#include <cctype>
#include <limits.h>

#define IMPEMENT_GET_FUNCTION(tblName)\
const char* PowerMaxAlarm::GetStr##tblName(int index)\
{\
    int nameCnt = sizeof(tblName)/sizeof(tblName[0]);\
    if(index < nameCnt)\
    {\
        return tblName[index];\
    }\
\
    return "??";\
}

/*
'########################################################
' PowerMax/Master send messages
'########################################################

' ### ACK messages, depending on the type, we need to ACK differently ###
Private VMSG_ACK1 {&H02] 'NONE
Private VMSG_ACK2 {&H02, &H43] 'NONE



Private VMSG_ARMDISARM {&HA1, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H43] 'NONE - MasterCode: 4 & 5
Private VMSG_STATUS {&HA2, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H43] 'A5
Private VMSG_EVENTLOG {&HA0, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H00, &H43] 'A0 - MasterCode: 4 & 5

' #### PowerMaster message ###
Private VMSG_PMASTER_STAT1 {&HB0, &H01, &H04, &H06, &H02, &HFF, &H08, &H03, &H00, &H00, &H43] 'B0 STAT1



' ### PowerMax download/config items (some apply to PowerMaster too) ###
' ---
' Pos.  0  1  2  3  4  5  6  7  8  9  A
' E.g. 3E 00 04 20 00 B0 00 00 00 00 00
' 1=Index, 2=Page, 3=Low Length, 4=High Length, 5=Always B0?
' ---
' PowerMaster30:
' Pos.  0  1  2  3  4  5  6  7  8  9  A
' E.g. 3E FF FF 42 1F B0 05 48 01 00 00
' 1=FF 2=FF 3=Low Length, 4=High Length, 5=Always B0, 6=Index, 7=Page
' ---*/
#define VMSG_DL_PANELFW {0x3E, 0x00, 0x04, 0x20, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_ZONESTR {0x3E, 0x00, 0x19, 0x00, 0x02, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_SERIAL {0x3E, 0x30, 0x04, 0x08, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
//'Private VMSG_DL_EVENTLOG {0x3E, 0xDF, 0x04, 0x28, 0x03, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
#define VMSG_DL_TIME {0x3E, 0xF8, 0x00, 0x20, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_COMMDEF {0x3E, 0x01, 0x01, 0x1E, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_USERPINCODES {0x3E, 0xFA, 0x01, 0x10, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_OTHERPINCODES {0x3E, 0x0A, 0x02, 0x0A, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_PHONENRS {0x3E, 0x36, 0x01, 0x20, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
//Private VMSG_DL_PGMX10 {0x3E, 0x14, 0x02, 0xD5, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
#define VMSG_DL_PARTITIONS {0x3E, 0x00, 0x03, 0xF0, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_ZONES {0x3E, 0x00, 0x09, 0x78, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
//Private VMSG_DL_KEYFOBS {0x3E, 0x78, 0x09, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
//Private VMSG_DL_2WKEYPADS {0x3E, 0x00, 0x0A, 0x08, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
//Private VMSG_DL_1WKEYPADS {0x3E, 0x20, 0x0A, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
//Private VMSG_DL_SIRENS {0x3E, 0x60, 0x0A, 0x08, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
//Private VMSG_DL_X10NAMES {0x3E, 0x30, 0x0B, 0x10, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
#define VMSG_DL_ZONENAMES {0x3E, 0x40, 0x0B, 0x1E, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_ZONESIGNAL {0x3E, 0xDA ,0x09, 0x1C, 0x00, 0xB0, 0x03, 0x00, 0x03, 0x00, 0x03}
//'Private VMSG_DL_ZONECUSTOM {0x3E, 0xA0, 0x1A, 0x50, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
//
//' ### PowerMaster download/config items ###
//Private VMSG_DL_MASTER_SIRENKEYPADSZONE {0x3E, 0xE2, 0xB6, 0x10, 0x04, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
#define VMSG_DL_MASTER_USERPINCODES {0x3E, 0x98, 0x0A, 0x60, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
//Private VMSG_DL_MASTER_SIRENS {0x3E, 0xE2, 0xB6, 0x50, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
//Private VMSG_DL_MASTER_KEYPADS {0x3E, 0x32, 0xB7, 0x40, 0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00] '0x3F
#define VMSG_DL_MASTER_ZONENAMES {0x3E, 0x60, 0x09, 0x40, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
#define VMSG_DL_MASTER_ZONES {0x3E, 0x72, 0xB8, 0x80, 0x02, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00}
//Private VMSG_DL_MASTER10_EVENTLOG {0x3E, 0xFF, 0xFF, 0xD2, 0x07, 0xB0, 0x05, 0x48, 0x01, 0x00, 0x00] '0x3F
//Private VMSG_DL_MASTER30_EVENTLOG {0x3E, 0xFF, 0xFF, 0x42, 0x1F, 0xB0, 0x05, 0x48, 0x01, 0x00, 0x00] '0x3F

//########################################################
// PowerMax/Master definitions for partitions, events, keyfobs, etc
//########################################################
// 0=PowerMax, 1=PowerMax+, 2=PowerMax Pro, 3=PowerMax Complete, 4=PowerMax Pro Part
// 5=PowerMax Complete Part, 6=PowerMax Express, 7=PowerMaster10, 8=PowerMaster30
// Those are defines, not globals to save memory (important on embeded devices)
#define VCFG_PARTITIONS {1, 1, 1, 1, 3, 3, 1, 3, 3}
#define VCFG_KEYFOBS {8, 8, 8, 8, 8, 8, 8, 8, 32}
#define VCFG_1WKEYPADS {8, 8, 8, 8, 8, 8, 8, 0, 0}
#define VCFG_2WKEYPADS {2, 2, 2, 2, 2, 2, 2, 8, 32}
#define VCFG_SIRENS {2, 2, 2, 2, 2, 2, 2, 4, 8}
#define VCFG_USERCODES {8, 8, 8, 8, 8, 8, 8, 8, 48}
#define VCFG_WIRELESS {28, 28, 28, 28, 28, 28, 28, 29, 62}
#define VCFG_WIRED {2, 2, 2, 2, 2, 2, 1, 1, 2}
#define VCFG_ZONECUSTOM {0, 5, 5, 5, 5, 5, 5, 5, 5}

bool PowerMaxAlarm::setDateTime(unsigned char year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minutes, unsigned char seconds)
{
    unsigned char buff[] = {0x46,0xF8,0x00,seconds,minutes,hour,day,month,year,0xFF,0xFF};
    return queueCommand(buff, sizeof(buff), "SET_DATE_TIME", 0xA0);
}

bool PowerMaxAlarm::sendCommand(PmaxCommand cmd)
{
    switch(cmd)
    {
    case Pmax_ACK:
        {
            if(m_ackTypeForLastMsg == ACK_1)
            {
                unsigned char buff[] = {0x02};
                return sendBuffer(buff, sizeof(buff));
            }
            else
            {
                unsigned char buff[] = {0x02,0x43};
                return sendBuffer(buff, sizeof(buff));
            }
        }

    case Pmax_PING:
        {
            unsigned char buff[] = {0xAB,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x43};
            return sendBuffer(buff, sizeof(buff));

            if(m_bDownloadMode == true)
            {
                DEBUG(LOG_WARNING,"Sending Ping in Download Mode?");
            }
        }

    case Pmax_GETEVENTLOG:
        {
            unsigned char buff[] = {0xA0,0x00,0x00,0x00,0x12,0x34,0x00,0x00,0x00,0x00,0x00,0x43}; addPin(buff, 4, true);
            return queueCommand(buff, sizeof(buff), "Pmax_GETEVENTLOG", 0xA0, "PIN:MasterCode:4");
        }

    case Pmax_DISARM:
        {
            unsigned char buff[] = {0xA1,0x00,0x00,0x00,0x12,0x34,0x00,0x00,0x00,0x00,0x00,0x43}; addPin(buff, 4, true);
            return sendBuffer(buff, sizeof(buff));
        }

    case Pmax_ARMHOME:
        {
            unsigned char buff[] = {0xA1,0x00,0x00,0x04,0x12,0x34,0x00,0x00,0x00,0x00,0x00,0x43}; addPin(buff, 4, true);
            return sendBuffer(buff, sizeof(buff));
        }

    case Pmax_ARMAWAY:
        {
            unsigned char buff[] = {0xA1,0x00,0x00,0x05,0x12,0x34,0x00,0x00,0x00,0x00,0x00,0x43}; addPin(buff, 4, true);
            return sendBuffer(buff, sizeof(buff));
        }

    case Pmax_ARMAWAY_INSTANT:
        {
            unsigned char buff[] = {0xA1,0x00,0x00,0x14,0x12,0x34,0x00,0x00,0x00,0x00,0x00,0x43}; addPin(buff, 4, true);
            return sendBuffer(buff, sizeof(buff));
        }

    case Pmax_REQSTATUS:
        {
            unsigned char buff[] = {0xA2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x43};
            return sendBuffer(buff, sizeof(buff));
        }

    /*
        The enrollment process
        The (emulated) Powerlink needs a pin code in order to:
        - Get some information from the Powermax
        - Use the bypass command
        - Use the disarm and arm commands
        During the enrollment process the Powerlink will create a pin (see POWERLINK_PIN) and register this pin at the Powermax. The advantage is that none of the already pins is required.
        On the Powermax+ and Powermax Pro the (emulated) Powerlink can be enrolled via the installer menu, on the Powermax Complete the Powerlink will be registered automatically.

        When the (emulated) Powerlink is connected and the 'Install Powerlink' option is selected from the installer menu the Powermax sends the following message:
        CODE: SELECT ALL
        0xAB 0x0A 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x43

        The (emulated) Powerlink should respond with the following message:
        CODE: SELECT ALL
        0xAB 0x0A 0x00 0x00 <pin1> <pin2> 0x00 0x00 0x00 0x00 0x00 0x43

        where <pin1> and <pin2> are the digits of the pin code that needs to be registered in order to be used by the Powerlink. When the enrollment process is completed successfully, a beep will be sounded.
        Note: When a new Powerlink module needs to be registered while there is already a Powerlink registered, the previous registered one needs to be uninstalled. You can do so by selecting 'Install Powerlink' from the installer menu en then press the disarm button.

        On PowerMax Complete: trigger enrolment by asking to start download, this will fail on access denied, and PM will ask for enrolment.
        NOTE: POWERLINK_PIN is a download pin only (it will allow to enroll/download settings, but will not allow to arm/disarm. 
              You can do this however: download the EEPROM settings, etract real pin, use it to arm/disarm.
    */
    case Pmax_ENROLLREPLY:
        {
            m_bDownloadMode = false;

            unsigned char buff[] = {0xAB,0x0A,0x00,0x00,0x12,0x34,0x00,0x00,0x00,0x00,0x00,0x43}; addPin(buff);
            return queueCommand(buff, sizeof(buff), "Pmax_ENROLLREPLY");
        }

    case Pmax_RESTORE:
        {
            unsigned char buff[] = {0xAB,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x43};
            return queueCommand(buff, sizeof(buff), "Pmax_RESTORE");
        }

    case Pmax_INIT:
        {
            unsigned char buff[] = {0xAB,0x0A,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x43};
            return queueCommand(buff, sizeof(buff), "Pmax_INIT");
        }

    // ### PowerMax download/config items (some apply to PowerMaster too) ###
    // ---
    // Pos.  0  1  2  3  4  5  6  7  8  9  A
    // E.g. 3E 00 04 20 00 B0 00 00 00 00 00
    // 1=Index, 2=Page, 3=Low Length, 4=High Length, 5=Always B0?
    // ---
    // PowerMaster30:
    // Pos.  0  1  2  3  4  5  6  7  8  9  A
    // E.g. 3E FF FF 42 1F B0 05 48 01 00 00
    // 1=FF 2=FF 3=Low Length, 4=High Length, 5=Always B0, 6=Index, 7=Page
    // ---
    case Pmax_DL_PANELFW:
        {
            unsigned char buff[] = {0x3E, 0x00, 0x04, 0x20, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00};
            return queueCommand(buff, sizeof(buff), "Pmax_DL_PANELFW", 0x3F);
        }

    case Pmax_DL_SERIAL:
        {
            unsigned char buff[] = {0x3E, 0x30, 0x04, 0x08, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00};
            return queueCommand(buff, sizeof(buff), "Pmax_DL_SERIAL", 0x3F);
        }

   case Pmax_DL_ZONESTR:
        {
            unsigned char buff[] = {0x3E, 0x00, 0x19, 0x00, 0x02, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00};
            return queueCommand(buff, sizeof(buff), "Pmax_DL_ZONESTR", 0x3F);
        }

   case Pmax_DL_ZONESIGNAL:
       {
            unsigned char buff[] = {0x3E, 0xDA, 0x09, 0x1C /*this might be the zone count*/, 0x00, 0xB0, 0x03, 0x00, 0x03, 0x00, 0x03};
            return queueCommand(buff, sizeof(buff), "Pmax_DL_ZONESIGNAL", 0x3F);
       }

   case Pmax_DL_GET:
        {
            unsigned char buff[] = {0x0A};
            return queueCommand(buff, sizeof(buff), "Pmax_DL_GET", 0x33);
        }

    case Pmax_DL_START: //start download (0x3C - DownloadCode: 3 & 4)
        {
            if(m_bDownloadMode == false)
            {
                m_bDownloadMode = true;
            }
            else
            {
                DEBUG(LOG_WARNING,"Already in Download Mode?");
            }

            //in download mode we don't expect ping messages
            stopKeepAliveTimer();

            unsigned char buff[] = {0x24,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; addPin(buff, 3);
            return queueCommand(buff, sizeof(buff), "Pmax_DL_START", 0x3C, "PIN:DownloadCode:3");
        }

    case Pmax_DL_EXIT: //stop download
        {
            unsigned char buff[] = {0x0F};
            return queueCommand(buff, sizeof(buff), "Pmax_DL_EXIT");

            if(m_bDownloadMode)
            {
                m_bDownloadMode = false;
            }
            else
            {
                DEBUG(LOG_WARNING,"Not in Download Mode?");
            }
        }
        
    default:
        return false;
    }
}


//FF means: match anything
struct PlinkCommand PmaxCommand[] =
{
    {{0x08                                                       },1  ,"Access denied"              ,&PowerMaxAlarm::OnAccessDenied},
    {{0x08,0x43                                                  },2  ,"Access denied 2"            ,&PowerMaxAlarm::OnAccessDenied},
    {{0xA0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x43},12 ,"Event Log"                  ,&PowerMaxAlarm::OnEventLog},
    {{0xA5,0xFF,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x43},12 ,"Status Update Zone Battery" ,&PowerMaxAlarm::OnStatusUpdateZoneBat},
    {{0xA5,0xFF,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x43},12 ,"Status Update Zone tamper"  ,&PowerMaxAlarm::OnStatusUpdateZoneTamper},
    {{0xA5,0xFF,0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x43},12 ,"Status Update Panel"        ,&PowerMaxAlarm::OnStatusUpdatePanel},
    {{0xA5,0xFF,0x06,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x43},12 ,"Status Update Zone Bypassed",&PowerMaxAlarm::OnStatusUpdateZoneBypassed},
    {{0xA7,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x43},12 ,"Panel status change"        ,&PowerMaxAlarm::OnStatusChange},
    {{0xAB,0x0A,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x43},12 ,"Enroll request"             ,&PowerMaxAlarm::OnEnroll},
    {{0xAB,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x43},12 ,"Ping"                       ,&PowerMaxAlarm::OnPing},
    {{0x3C,0xFD,0x0A,0x00,0x00,0xFF,0xFF,0xFF,0xFF               },-9, "Panel Info"                 ,&PowerMaxAlarm::OnPanelInfo},    // -9 means: size>=9, len can differ
    {{0x3F,0xFF,0xFF,0xFF                                        },-4, "Download Info"              ,&PowerMaxAlarm::OnDownloadInfo}, // -4 means: size>=4, len can differ
    {{0x33,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF     },11, "Download Settings"          ,&PowerMaxAlarm::OnDownloadSettings},
    {{0x02,0x43                                                  },2  ,"Acknowledgement"            ,&PowerMaxAlarm::OnAck},
    {{0x02,                                                      },1  ,"Acknowledgement 2"          ,&PowerMaxAlarm::OnAck},
	{{0x06                                                       },1  ,"Time Out"                   ,&PowerMaxAlarm::OnTimeOut},
    {{0x0B                                                       },1  ,"Stop (Dload Complete)"      ,&PowerMaxAlarm::OnStop} 
}; 

const char* PmaxSystemStatus[] = {
    "Disarmed"     ,
    "Exit Delay" ,
    "Exit Delay" ,
    "Entry Delay",
    "Armed Home" ,
    "Armed Away" ,
    "User Test"  ,
    "Downloading",
    "Programming",
    "Installer"  ,
    "Home Bypass",
    "Away Bypass",
    "Ready"      ,
    "Not Ready"
};

const char* SystemStateFlags[] = {
    "Ready",
    "Alert-in-Memory",
    "Trouble",
    "Bypass-On",
    "Last-10-sec-delay",
    "Zone-event",
    "Arm/disarm-event",
    "Alarm-event"
};

const char*  PmaxLogEvents[] = {            
    "None"                                      , //0
    "Interior Alarm"                            , //1
    "Perimeter Alarm"                           , //2
    "Delay Alarm"                               , //3
    "24h Silent Alarm"                          , //4
    "24h Audible Alarm"                         , //5
    "Tamper"                                    , //6
    "Control Panel Tamper"                      , //7
    "Tamper Alarm"                              , //8
    "Tamper Alarm"                              , //9
    "Communication Loss"                        , //10
    "Panic From Keyfob"                         , //11
    "Panic From Control Panel"                  , //12
    "Duress"                                    , //13
    "Confirm Alarm"                             , //14
    "General Trouble"                           , //15
    "General Trouble Restore"                   , //16
    "Interior Restore"                          , //17
    "Perimeter Restore"                         , //18
    "Delay Restore"                             , //19
    "24h Silent Restore"                        , //20
    "24h Audible Restore"                       , //21
    "Tamper Restore"                            , //22
    "Control Panel Tamper Restore"              , //23
    "Tamper Restore"                            , //24
    "Tamper Restore"                            , //25
    "Communication Restore"                     , //26
    "Cancel Alarm"                              , //27
    "General Restore"                           , //28
    "Trouble Restore"                           , //29
    "Not used"                                  ,
    "Recent Close"                              ,
    "Fire"                                      ,
    "Fire Restore"                              ,
    "No Active"                                 ,
    "Emergency"                                 ,
    "No used"                                   ,
    "Disarm Latchkey"                           ,
    "Panic Restore"                             ,
    "Supervision (Inactive)"                    ,
    "Supervision Restore (Active)"              ,
    "Low Battery"                               ,
    "Low Battery Restore"                       ,
    "AC Fail"                                   ,
    "AC Restore"                                ,
    "Control Panel Low Battery"                 ,
    "Control Panel Low Battery Restore"         ,
    "RF Jamming"                                ,
    "RF Jamming Restore"                        ,
    "Communications Failure"                    ,
    "Communications Restore"                    ,
    "Telephone Line Failure"                    ,
    "Telephone Line Restore"                    ,
    "Auto Test"                                 ,
    "Fuse Failure"                              ,
    "Fuse Restore"                              ,
    "Keyfob Low Battery"                        ,
    "Keyfob Low Battery Restore"                ,
    "Engineer Reset"                            ,
    "Battery Disconnect"                        ,
    "1-Way Keypad Low Battery"                  ,
    "1-Way Keypad Low Battery Restore"          ,
    "1-Way Keypad Inactive"                     ,
    "1-Way Keypad Restore Active"               ,
    "Low Battery"                               ,
    "Clean Me"                                  ,
    "Fire Trouble"                              ,
    "Low Battery"                               ,
    "Battery Restore"                           ,
    "AC Fail"                                   ,
    "AC Restore"                                ,
    "Supervision (Inactive)"                    ,
    "Supervision Restore (Active)"              ,
    "Gas Alert"                                 ,
    "Gas Alert Restore"                         ,
    "Gas Trouble"                               ,
    "Gas Trouble Restore"                       ,
    "Flood Alert"                               ,
    "Flood Alert Restore"                       ,
    "X-10 Trouble"                              ,
    "X-10 Trouble Restore"                      ,
    "Arm Home"                                  ,
    "Arm Away"                                  ,
    "Quick Arm Home"                            ,
    "Quick Arm Away"                            ,
    "Disarm"                                    ,
    "Fail To Auto-Arm"                          ,
    "Enter To Test Mode"                        ,
    "Exit From Test Mode"                       ,
    "Force Arm"                                 ,
    "Auto Arm"                                  ,
    "Instant Arm"                               ,
    "Bypass"                                    ,
    "Fail To Arm"                               ,
    "Door Open"                                 ,
    "Communication Established By Control Panel",
    "System Reset"                              ,
    "Installer Programming"                     ,
    "Wrong Password"                            ,
    "Not Sys Event"                             ,
    "Not Sys Event"                             ,
    "Extreme Hot Alert"                         ,
    "Extreme Hot Alert Restore"                 ,
    "Freeze Alert"                              ,
    "Freeze Alert Restore"                      ,
    "Human Cold Alert"                          ,
    "Human Cold Alert Restore"                  ,
    "Human Hot Alert"                           ,
    "Human Hot Alert Restore"                   ,
    "Temperature Sensor Trouble"                ,
    "Temperature Sensor Trouble Restore"        
};



const char*  PmaxZoneEventTypes[] = {
    "None"                 ,
    "Tamper Alarm"         ,
    "Tamper Restore"       ,
    "Open"                 ,
    "Closed"               ,
    "Violated (Motion)"    ,
    "Panic Alarm"          ,
    "RF Jamming"           ,
    "Tamper Open"          ,
    "Communication Failure",
    "Line Failure"         ,
    "Fuse"                 ,
    "Not Active"           ,
    "Low Battery"          ,
    "AC Failure"           ,
    "Fire Alarm"           ,
    "Emergency"            ,
    "Siren Tamper"         ,
    "Siren Tamper Restore" ,
    "Siren Low Battery"    ,
    "Siren AC Fail"
};

const char*  PmaxPanelType[] = {
    "PowerMax"               ,
    "PowerMax+"              ,
    "PowerMax Pro"           ,
    "PowerMax Complete"      ,
    "PowerMax Pro Part"      , //"part" stands for "partition support", this unit supports multiple partitions
    "PowerMax Complete Part" ,
    "PowerMax Express"       ,
    "PowerMaster10"          ,
    "PowerMaster30"              
};

const char*  PmaxZoneTypes[] = {
	   "Non-Alarm", "Emergency", "Flood", "Gas", "Delay 1", "Delay 2", "Interior-Follow", "Perimeter", "Perimeter-Follow", 
	   "24 Hours Silent", "24 Hours Audible", "Fire", "Interior", "Home Delay", "Temperature", "Outdoor", "16"          
};

const char* PmaxEventSource[] = {
    "System", "Zone 1", "Zone 2", "Zone 3", "Zone 4", "Zone 5", "Zone 6", "Zone 7", "Zone 8",
    "Zone 09", "Zone 10", "Zone 11", "Zone 12", "Zone 13", "Zone 14", "Zone 15", "Zone 16", "Zone 17", "Zone 18",
    "Zone 19", "Zone 20", "Zone 21", "Zone 22", "Zone 23", "Zone 24", "Zone 25", "Zone 26", "Zone 27", "Zone 28", 
    "Zone 29", "Zone 30", "Fob 1" /*31, 0x1F*/, "Fob 2", "Fob 3", "Fob 4", "Fob 5", "Fob 6", "Fob 7", "Fob 8" /*38, 0x26*/, 
    "User 1", "User 2", "User 3", "User 4", "User 5", "User 6", "User 7", "User 8", "Pad 1", "Pad 2",
    "Pad 3", "Pad 4", "Pad 5", "Pad 6", "Pad 7", "Pad 8", "Sir 1", "Sir 2", "2Pad 1", "2Pad 2",
    "2Pad 3", "2Pad 4", "X10 1", "X10 2", "X10 3", "X10 4", "X10 5", "X10 6", "X10 7", "X10 8",
    "X10 9", "X10 10", "X10 11", "X10 12", "X10 13", "X10 14", "X10 15", "PGM", "GSM", "P-LINK",
    "PTag 1", "PTag 2", "PTag 3", "PTag 4", "PTag 5", "PTag 6", "PTag 7", "PTag 8" };

IMPEMENT_GET_FUNCTION(PmaxSystemStatus);
IMPEMENT_GET_FUNCTION(SystemStateFlags);
IMPEMENT_GET_FUNCTION(PmaxZoneEventTypes);
IMPEMENT_GET_FUNCTION(PmaxLogEvents);
IMPEMENT_GET_FUNCTION(PmaxPanelType);
IMPEMENT_GET_FUNCTION(PmaxZoneTypes);
IMPEMENT_GET_FUNCTION(PmaxEventSource);

void PowerMaxAlarm::init(int initWaitTime) {

    memset(&m_lastSentCommand, 0, sizeof(m_lastSentCommand));
    m_bEnrolCompleted = false;
    m_bDownloadMode = false;
    m_iPanelType = -1;
    m_iModelType = 0;
    m_bPowerMaster = false;
    m_ackTypeForLastMsg = ACK_1;
    m_ulLastPing = os_getCurrentTimeSec();
    m_ulNextPingDeadline = ULONG_MAX;
    m_iInitWaitTime = initWaitTime;

    flags = 0;
    stat  = SS_Not_Ready;
    alarmState = 0; //no alarm
    memset(alarmTrippedZones, 0, sizeof(alarmTrippedZones));
    lastIoTime = 0;

    memset(zone, 0, sizeof(zone));

    //if PM is in dowload mode, then it would interfere with out init sequence
    //so let's start with DL exit command
    PowerMaxAlarm::sendCommand(Pmax_DL_EXIT); 

    PowerMaxAlarm::sendCommand(Pmax_INIT);

    //Send the download command, this should initiate the communication
    //First DL_START request is likely to fail on access denied, and PM should request the enrolment
    PowerMaxAlarm::sendCommand(Pmax_DL_START);
}

unsigned int PowerMaxAlarm::getEnrolledZoneCnt() const
{
    unsigned int cnt = 0;
    for (unsigned char i=1;i<MAX_ZONE_COUNT;i++) //zone 0 is not used (system)
    {
        if(zone[i].enrolled)
        {
            cnt++;
        }
    }

    return cnt;
}

unsigned long PowerMaxAlarm::getSecondsFromLastComm() const
{ 
    return (unsigned long)(os_getCurrentTimeSec()-lastIoTime); 
}

void PowerMaxAlarm::addPin(unsigned char* bufferToSend, int pos, bool useMasterCode)
{
    const int pin = useMasterCode ? m_cfg.GetMasterPinAsHex() : POWERLINK_PIN;
    bufferToSend[pos]=pin>>8;
    bufferToSend[pos+1]=pin & 0x00FF ;
}

void PowerMaxAlarm::OnEnroll(const PlinkBuffer  * Buff)
{
    //Remove anything else from the queue, we need to restart
    this->clearQueue();
    this->sendCommand(Pmax_ENROLLREPLY);
    DEBUG(LOG_INFO,"Enrolling.....");

    //if we're doing enroll, most likely first call to download failed, re-try now, after enrolment is done:
    this->sendCommand(Pmax_DL_START);
}

//Should be called when panel is enrolled and entered download mode successfully
void PowerMaxAlarm::powerLinkEnrolled()
{
    m_bEnrolCompleted = true;

   //Request the panel FW
   sendCommand(Pmax_DL_PANELFW);

   //Request serial & type (not always sent by default)
   sendCommand(Pmax_DL_SERIAL);

   //Read the names of the zones
   sendCommand(Pmax_DL_ZONESTR);

   //' Retrieve extra info if this is a PowerMaster
   //If $bPowerMaster Then 
   //  SendMsg_DL_MASTER_SIRENKEYPADSZONE()

   //  ' Only request eventlog in debug mode
   //  If $bDebug Then
   //    Select $iPanelType
   //        Case &H07 ' PowerMaster10
   //          SendMsg_DL_MASTER10_EVENTLOG()
   //        Case &H08 ' PowerMaster30
   //          SendMsg_DL_MASTER30_EVENTLOG()
   //      End Select
   //    Endif
   //Endif

   //Request all other relevant information (this includes zone signal strength)
   sendCommand(Pmax_DL_GET);

   //' auto-sync date/time
   unsigned char year, month, day, hour, minutes, seconds;
   if(os_getLocalTime(year, month, day, hour, minutes, seconds))
   {
       setDateTime(year, month, day, hour, minutes, seconds);
   }

   //' We are done, exit download mode
   sendCommand(Pmax_DL_EXIT);

    //Lets request the eventlogs
    if(m_bPowerMaster == false)
    {
        //IZIZTOOD: uncomment:
        //this->sendCommand(Pmax_GETEVENTLOG);
    }
}

//Direct message after we do a download start. Contains the paneltype information
void PowerMaxAlarm::OnPanelInfo(const PlinkBuffer  * Buff)
{
    // The panel information is in 5 & 6
    // 6=PanelType e.g. PowerMax, PowerMaster
    // 5=Sub model type of the panel - just informational
    this->m_iPanelType = Buff->buffer[6];
    this->m_iModelType = Buff->buffer[5];
    this->m_bPowerMaster =  (this->m_iPanelType >= 7);
    this->sendCommand(Pmax_ACK);

    DEBUG(LOG_INFO,"Received Panel Info. PanelType: %s, Model=%d (0x%X)", GetStrPmaxPanelType(this->m_iPanelType),  this->m_iModelType, (this->m_iPanelType * 0x100 + this->m_iModelType));

    //We got a first response, now we can continue enrollment the PowerMax/Master PowerLink
    this->powerLinkEnrolled();
}

int PowerMaxAlarm::readMemoryMap(const unsigned char* sData, unsigned char* buffOut, int buffOutSize)
{
    //The aMsg is in the regular download format, ignore the SubType and only use page, index and length
    //NOTE: Length can be more then &HFF bytes, in such we got multiple 3F responses with a "real" download
    int iPage = sData[2];
    int iIndex = sData[1];
    int iLength = (sData[4] * 0x100) + sData[3];

    if(iLength > buffOutSize)
    {
        DEBUG(LOG_ERR, "readMemoryMap, buffer too small, needed: %d, got: %d", iLength, buffOutSize);
        return 0;
    }

    MemoryMap* pMap = &m_mapMain;
    if(iPage == 0xFF && iIndex == 0xFF)
    {
        pMap = &m_mapExtended;
        
        //Overrule page/index/data if we have an extended message
        iPage = sData[7];
        iIndex = sData[6];
    }

    return pMap->Read(iPage, iIndex, iLength, buffOut);
}


//Write the 3C and 3F information into our own memory map structure. This contains all the
//information of the PowerMax/Master and will later be processed in ReadSettings
void PowerMaxAlarm::writeMemoryMap(int iPage, int iIndex, const unsigned char* sData, int sDataLen)
{
    MemoryMap* pMap = &m_mapMain;
    if(iPage == 0xFF && iIndex == 0xFF)
    {
        pMap = &m_mapExtended;
        
        //Overrule page/index/data if we have an extended message
        iPage = sData[1];
        iIndex = sData[0];

        //Remove page/index and other 2 bytes, unknown usage
        sData += 4;
        sDataLen -= 4;
    }

    int bytesWritten = pMap->Write(iPage, iIndex, sDataLen, sData);
    if(bytesWritten != sDataLen)
    {
        DEBUG(LOG_ERR, "Failed to write to memory, page: %d, index: %d, len: %d, got: %d", iPage, iIndex, sDataLen, bytesWritten);
    }
}

//MsgType=3F - Download information
//Multiple 3F can follow eachother, if we request more then 0xFF bytes
void PowerMaxAlarm::OnDownloadInfo(const PlinkBuffer  * Buff)
{
    this->sendCommand(Pmax_ACK);

    //Format is normally: <MsgType> <index> <page> <length> <data ...>
    //If the <index> <page> = FF, then it is an additional PowerMaster MemoryMap
    int iIndex  = Buff->buffer[1];
    int iPage   = Buff->buffer[2];
    int iLength = Buff->buffer[3];


    if(iLength != Buff->size-4)
    {
        DEBUG(LOG_WARNING,"Received Download Data with invalid len indication: %d, got: %d", iLength, Buff->size-4);
    }

    //Write to memory map structure, but remove the first 4 bytes (3F/index/page/length) from the data
    this->writeMemoryMap(iPage, iIndex, Buff->buffer+4, Buff->size-4);
}

//MsgType=33 - Settings
// Message send after a DL_GET. We will store the information in an internal array/collection
void PowerMaxAlarm::OnDownloadSettings(const PlinkBuffer  * Buff)
{
    this->sendCommand(Pmax_ACK);

    //Format is: <MsgType> <index> <page> <data 8x bytes>
    int iIndex  = Buff->buffer[1];
    int iPage   = Buff->buffer[2];

    //Write to memory map structure, but remove the first 3 bytes from the data
    this->writeMemoryMap(iPage, iIndex, Buff->buffer+3, Buff->size-3);
}

//This one happens after all items requested by DLOAD_Get has been sent by PM
void PowerMaxAlarm::OnStop(const PlinkBuffer  * Buff)
{
    this->sendCommand(Pmax_ACK);   
    DEBUG(LOG_INFO,"Stop (Dload complete)"); 
}

void PowerMaxAlarm::startKeepAliveTimer()
{
    m_ulNextPingDeadline = os_getCurrentTimeSec() + 180; //if we don't receive a ping in next 3 minutes we will assume comms are lost
}

void PowerMaxAlarm::stopKeepAliveTimer()
{
    m_ulNextPingDeadline = ULONG_MAX;
}

bool PowerMaxAlarm::restoreCommsIfLost()
{
    if(m_bDownloadMode == false &&
       os_getCurrentTimeSec() > m_ulNextPingDeadline)
    {
        sendCommand(Pmax_RESTORE);
        startKeepAliveTimer();

        return true;
    }

    return false;
}

void PowerMaxAlarm::OnPing(const PlinkBuffer  * Buff)
{
    this->m_bDownloadMode = false; //ping never happens in download mode
    this->sendCommand(Pmax_ACK);
    DEBUG(LOG_INFO,"Ping.....");

    //re-starting keep alive timer
    this->startKeepAliveTimer();
}

#ifdef _MSC_VER
//IZIZTODO: delete this
void loadMapToFile(const char* name, MemoryMap* map);
void saveMapToFile(const char* name, MemoryMap* map);
void PowerMaxAlarm::IZIZTODO_testMap()
{
    m_iPanelType = 5;
    m_bPowerMaster = false;
    loadMapToFile("main.map", &m_mapMain);
    processSettings();

    ConsoleOutput c;
    m_cfg.DumpToJson(&c);
}
#endif

void PmConfig::DumpToJson(IOutput* outputStream)
{
    outputStream->write("{");
    {
        outputStream->writeJsonTag("installer_pin", installerPin);
        outputStream->writeJsonTag("masterinstaller_pin", masterInstallerPin);
        outputStream->writeJsonTag("powerlink_pin", powerLinkPin);
        
        outputStream->write("\"user_pins\":[");
        bool first = true;
        for(int ix=0; ix<maxUserCnt; ix++)
        {
            if(strcmp(userPins[ix], "0000") != 0)
            {
                if(first == false)
                {
                    outputStream->write(",");
                }
                outputStream->write("\"");
                outputStream->write(userPins[ix]);
                outputStream->write("\"");
                first = false;
           }
        }
        outputStream->write("],\r\n");


        outputStream->write("\"telephone_numbers\":[");
        first = true;
        for(int ix=0; ix<4; ix++)
        {
            if(phone[ix][0] != '\0')
            {
                if(first == false)
                {
                    outputStream->write(",");
                }
                outputStream->write("\"");
                outputStream->write(phone[ix]);
                outputStream->write("\"");
                first = false;
           }
        }
        outputStream->write("],\r\n");

        outputStream->writeJsonTag("serial_number", serialNumber);
        outputStream->writeJsonTag("eprom", eprom);
        outputStream->writeJsonTag("software", software);
        outputStream->writeJsonTag("partitionCnt", (int)partitionCnt, false);
    }

    outputStream->write("}");
}

int PmConfig::GetMasterPinAsHex() const
{
    //master pin is the first pin of the user:
    return strtol(userPins[0], NULL, 16);
}

void trimRight(char* str)
{
    int len = strlen(str);
    while(len > 0)
    {
        if(isspace(str[len-1]))
        {
            str[len-1] = '\0';
        }
        else
        {
            break;
        }

        len--;
    }
}

void PowerMaxAlarm::processSettings()
{
    m_cfg.Init();
    
    unsigned char readBuff[512] = {0}; //512 is needed to read zone names in PowerMax Complete

    if( m_iPanelType == -1 )
    {
        DEBUG(LOG_WARNING, "ERROR: Can't process settings, the PanelType is unknown");
        return;
    }   

    if( m_iPanelType > 8 )
    {
        DEBUG(LOG_WARNING,"ERROR: Can't process settings, the PanelType= %d is a too new type", m_iPanelType);
        return;
    }  
    
    //Read serialnumber and paneltype info, and some sanity check
    {
        const unsigned char msg[] = VMSG_DL_SERIAL;
        const int readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        if(readCnt < 8)
        {
            DEBUG(LOG_WARNING,"ERROR: Can't read the PowerMax/Master MemoryMap, communication with the unit went wrong?");
            return;
        }

        if(readBuff[7] != m_iPanelType)
        {
            DEBUG(LOG_WARNING,"ERROR: Initial received PanelType is different then read from MemoryMap. PanelType=%d, Read=%d", m_iPanelType, readBuff[7]);
            return;
        }

        sprintf(m_cfg.serialNumber, "%02X%02X%02X%02X%02X%02X", readBuff[0], readBuff[1], readBuff[2], readBuff[3], readBuff[4], readBuff[5]);
        char* end = strchr(m_cfg.serialNumber, 'F');
        if(end)
        {
            *end = '\0';
        }

        //basic sanity check passed:
        m_cfg.parsedOK = true;
    }

    { //Now it looks to be safe to determine panel options/dimensions
        unsigned char tmpVCFG_PARTITIONS[] = VCFG_PARTITIONS;
        unsigned char tmpVCFG_KEYFOBS[]    = VCFG_KEYFOBS;
        unsigned char tmpVCFG_1WKEYPADS[]  = VCFG_1WKEYPADS;
        unsigned char tmpVCFG_2WKEYPADS[]  = VCFG_2WKEYPADS;
        unsigned char tmpVCFG_SIRENS[]     = VCFG_SIRENS;
        unsigned char tmpVCFG_USERCODES[]  = VCFG_USERCODES;
        unsigned char tmpVCFG_WIRELESS[]   = VCFG_WIRELESS;
        unsigned char tmpVCFG_WIRED[]      = VCFG_WIRED;
        unsigned char tmpVCFG_ZONECUSTOM[] = VCFG_ZONECUSTOM;

        m_cfg.maxZoneCnt =      tmpVCFG_WIRELESS[m_iPanelType] + tmpVCFG_WIRED[m_iPanelType];
        m_cfg.maxCustomCnt =    tmpVCFG_ZONECUSTOM[m_iPanelType];
        m_cfg.maxUserCnt =      tmpVCFG_USERCODES[m_iPanelType];
        m_cfg.maxPartitionCnt = tmpVCFG_PARTITIONS[m_iPanelType];
        m_cfg.partitionCnt    = tmpVCFG_PARTITIONS[m_iPanelType];
        m_cfg.maxSirenCnt =     tmpVCFG_SIRENS[m_iPanelType];
        m_cfg.maxKeypad1Cnt =   tmpVCFG_1WKEYPADS[m_iPanelType];
        m_cfg.maxKeypad2Cnt =   tmpVCFG_2WKEYPADS[m_iPanelType];
        m_cfg.maxKeyfobCnt =    tmpVCFG_KEYFOBS[m_iPanelType];
    }

    { //Read panel eprom and software info
        const unsigned char msg[] = VMSG_DL_PANELFW;
        const int readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        if(readCnt >=  16) strncpy(m_cfg.eprom, (const char*)readBuff, sizeof(m_cfg.eprom)-1);
        if(readCnt >=  32) sprintf(m_cfg.software, (const char*)readBuff+16, sizeof(m_cfg.software)-1); 
    }

    char zoneNames[MAX_ZONE_COUNT][0x11];
    memset(zoneNames, 0, sizeof(zoneNames));

    { //Determine the zone names, including the custom ones.
        const unsigned char msg[] = VMSG_DL_ZONESTR;
        const int readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        if(readCnt == 0)
        {
            DEBUG(LOG_WARNING,"ERROR: Failed to read zone names, possibly data not downloaded, or supplied buffer too small");
        }
        else
        {
            for(int iCnt=0; iCnt < MAX_ZONE_COUNT; iCnt++)
            {
                if((iCnt*0x10)+0x10 <= readCnt)
                {
                    memcpy(zoneNames[iCnt], readBuff + (iCnt*0x10), 0x10);
                    if((unsigned char)zoneNames[iCnt][0] == 0xFF)
                    {
                        zoneNames[iCnt][0] = '\0';
                    }
                }
            }
        }
    }

    { //Get PHONE numbers:
        const unsigned char msg[] = VMSG_DL_PHONENRS;
        const int readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        for(int iCnt=0; iCnt<4; iCnt++)
        {
            memset(m_cfg.phone[iCnt], 0, sizeof(m_cfg.phone[iCnt]));
            for(int jCnt=0; jCnt<=7; jCnt++)
            {
                if(readBuff[8 * iCnt + jCnt] != 0xFF)
                {
                    char szTwoDigits[10] = "";
                    sprintf(szTwoDigits, "%02X", readBuff[8 * iCnt + jCnt]);
                    os_strncat_s(m_cfg.phone[iCnt], sizeof(m_cfg.phone[iCnt]), szTwoDigits);
                }

                char* pEnd = strchr(m_cfg.phone[iCnt], 'F');
                if(pEnd != NULL)
                {
                    *pEnd = '\0';
                }
            }
        }
    }

    { //Alarm settings
        const unsigned char msg[] = VMSG_DL_COMMDEF;
        const int readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        if(readCnt < 30)
        {
            DEBUG(LOG_WARNING,"ERROR: Failed to read alarm settings");
        }
        else
        {
            //IZIZTODO: store, use?
            unsigned char alarm_entrydelay1 = readBuff[0];
            unsigned char alarm_entrydelay2 = readBuff[1];
            unsigned char alarm_exitdelay = readBuff[2];
            unsigned char alarm_belltime = readBuff[3];
            unsigned char alarm_silentpanic = readBuff[25];

            /*
            $cConfig["alarm"]["silentpanic"] = (sData[25] And &H10 = &H10)
            $cConfig["alarm"]["quickarm"] = (sData[26] And &H08 = &H08)
            $cConfig["alarm"]["bypassoff"] = (sData[27] And &HC0 = &H00)
            $cConfig["alarm"]["forceddisablecode"] = ByteToHex(sData.Copy(16, 2))
            $iDisarmArmCode = CInt(sData[16]) * 256 + sData[17]
            */
        }
    }

    //If our panel supports multiple partitions, try to figure out if it is enabled or not
    //PowerMax without partitions will not give the 0x0300 back when requested
    if(m_cfg.maxPartitionCnt > 0)
    { //Retrieve if partitioning is enabled
            const unsigned char msg[] = VMSG_DL_PARTITIONS;
            const int readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
            if(readCnt > 0)
            {
                if(readBuff[0] == 0)
                {
                    m_cfg.partitionCnt = 1;
                }
            }
    }

    { //Get panel date and time
        unsigned char dateAndTime[32] = {0};
        const unsigned char msg[] = VMSG_DL_TIME;
        int bytesRead = readMemoryMap(msg, dateAndTime, sizeof(dateAndTime));
        if(bytesRead >= 6)
        {
            OnPanelDateTime(dateAndTime[5],dateAndTime[4],dateAndTime[3],dateAndTime[2],dateAndTime[1],dateAndTime[0]);
        }
    }

    { //Retrieve detailed zone information
        unsigned char masterReadBuff[128] = {0};

        const unsigned char msg[] = VMSG_DL_ZONES;
        const int readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        if(readCnt < 120)
        {
            DEBUG(LOG_WARNING,"ERROR: Failed to read zone settings");
        }
        else
        {
            int zoneNameIdxCnt = 0;
            unsigned char zoneNamesIndexes[30] = {0};

            unsigned char zoneSignal[30];
            memset(zoneSignal, 0xFF, sizeof(zoneSignal));
            
            if(m_bPowerMaster)
            {
                const unsigned char msg[] = VMSG_DL_MASTER_ZONENAMES;
                zoneNameIdxCnt = readMemoryMap(msg, zoneNamesIndexes, sizeof(zoneNamesIndexes));

                const unsigned char msg2[] = VMSG_DL_MASTER_ZONES;
                readMemoryMap(msg2, masterReadBuff, sizeof(masterReadBuff));
            }
            else
            {
                const unsigned char msg[] = VMSG_DL_ZONENAMES;
                zoneNameIdxCnt = readMemoryMap(msg, zoneNamesIndexes, sizeof(zoneNamesIndexes));
                if(zoneNameIdxCnt != sizeof(zoneNamesIndexes))
                {
                    DEBUG(LOG_WARNING,"ERROR: Failed to read zone name indexes");
                }
            }
            
            {
                const unsigned char msgSig[] = VMSG_DL_ZONESIGNAL;
                int zoneCnt = readMemoryMap(msgSig, zoneSignal, sizeof(zoneSignal));
                if(zoneCnt == 0)
                {
                    DEBUG(LOG_WARNING,"ERROR: Failed to read zone signal strength");
                }
            }

            strcpy(zone[0].name, "System"); //we enumerate from 1, zone 0 is not used (system)
            zone[0].signalStrength = 0xFF;
            for(int iCnt=1; iCnt<=m_cfg.maxZoneCnt;  iCnt++)
            {
                if(iCnt > MAX_ZONE_COUNT)
                {
                    DEBUG(LOG_WARNING,"ERROR: Failed to read all zones, as MAX_ZONE_COUNT is too small. Increase it to: %d", m_cfg.maxZoneCnt);
                    break;
                }

                Zone* pZone = &zone[iCnt];

                if(m_bPowerMaster)
                {
                    pZone->enrolled = masterReadBuff[iCnt * 10 - 6] != 0 || masterReadBuff[iCnt * 10 - 5] != 0 || masterReadBuff[iCnt * 10 - 4] != 0 || masterReadBuff[iCnt * 10 - 3] != 0 || masterReadBuff[iCnt * 10 - 2] != 0;
                }
                else
                {
                    pZone->enrolled = readBuff[iCnt * 4 - 4] != 0 || readBuff[iCnt * 4 - 3] != 0 || readBuff[iCnt * 4 - 2] != 0;
                }

                if(pZone->enrolled == false)
                {
                    continue;
                }

                int zoneIndex = zoneNamesIndexes[iCnt-1];
                if(zoneIndex < MAX_ZONE_COUNT)
                {
                    strcpy(pZone->name, zoneNames[zoneIndex]);
                    trimRight(pZone->name);
                }
                else
                {
                    strcpy(pZone->name, "Unknown");
                }
                
                if(iCnt-1 < sizeof(zoneSignal))
                {
                    pZone->signalStrength = zoneSignal[iCnt-1];
                }
                else
                {
                    pZone->signalStrength = 0xFF;
                }

                if(m_bPowerMaster)
                {
                    //IZIZTODO
                }
                else
                {
                    pZone->zoneType = readBuff[iCnt * 4 - 1];
                    pZone->zoneTypeStr = GetStrPmaxZoneTypes(pZone->zoneType);
                    pZone->sensorId = readBuff[iCnt * 4 - 2];

                    switch(pZone->sensorId & 0xF)
                    {
                    case 0x0:
                        pZone->sensorType = "Vibration";
                        pZone->sensorMake = "Visonic Vibration Sensor";
                        break;

                    case 0x3:
                    case 0x4:
                    case 0xC:
                        pZone->sensorType = "Motion";
                        pZone->sensorMake = "Visonic PIR";
                        break;

                    case 0x5:
                    case 0x6:
                    case 0x7:
                        pZone->sensorType = "Magnet";
                        pZone->sensorMake = "Visonic Door/Window Contact";
                        break;

                    case 0xA:
                        pZone->sensorType = "Smoke";
                        pZone->sensorMake = "Visonic Smoke Detector";
                        break;

                    case 0xB:
                        pZone->sensorType = "Gas";
                        pZone->sensorMake = "Visonic Gas Detector";
                        break;

                    case 0xF:
                        pZone->sensorType = "Wired";
                        pZone->sensorMake = "Visonic PIRWired";
                        break;

                    default:
                        pZone->sensorType = "Unknown";
                        pZone->sensorMake = "Visonic Unknown";
                        break;
                    }
                }
            }

        }
    }

    { //Get user pin codes
        int readCnt;
        if(m_bPowerMaster)
        {
            const unsigned char msg[] = VMSG_DL_MASTER_USERPINCODES;
            readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        }
        else
        {
            const unsigned char msg[] = VMSG_DL_USERPINCODES;
            readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        }

        if(readCnt != m_cfg.maxUserCnt*2)
        {
            DEBUG(LOG_WARNING,"ERROR: Failed to read user codes. Expected len: %d, got: %d", m_cfg.maxUserCnt*2, readCnt);
        }
        else
        {
            for(int ix=0; ix<m_cfg.maxUserCnt; ix++)
            {
                sprintf(m_cfg.userPins[ix], "%02X%02X", readBuff[ix*2+0], readBuff[ix*2+1]);
            }
        }
    }

    { //Retrieve the installer and powerlink pincodes - they are known/visible
        const unsigned char msg[] = VMSG_DL_OTHERPINCODES;
        const int readCnt = readMemoryMap(msg, readBuff, sizeof(readBuff));
        if(readCnt >=  2) sprintf(m_cfg.installerPin,       "%02X%02X", readBuff[0], readBuff[1]);
        if(readCnt >=  4) sprintf(m_cfg.masterInstallerPin, "%02X%02X", readBuff[2], readBuff[3]); 
        if(readCnt >= 10) sprintf(m_cfg.powerLinkPin,       "%02X%02X", readBuff[8], readBuff[9]); 
    }
}

void PowerMaxAlarm::OnAck(const PlinkBuffer  * Buff)
{
    if(this->m_lastSentCommand.size == 12 && 
       this->m_lastSentCommand.buffer[0] == 0xAB &&
       this->m_lastSentCommand.buffer[1] == 0x0A &&
       this->m_lastSentCommand.buffer[3] == 0x01)
    {
        //we got an ack for Pmax_INIT command, this on PowerMax+ can take some time, we need to pause execution before issuing any new commands
        os_usleep(this->m_iInitWaitTime * 1000000); //sleep for m_iInitWaitTime seconds
    }

    if(this->m_lastSentCommand.size == 1 &&
       this->m_lastSentCommand.buffer[0] == 0x0F) //Pmax_DL_EXIT
    {
        //we got an ack for exit from dload mode:
        this->m_bDownloadMode = false;

        //this will be false for the first Pmax_DL_EXIT that is called from Init()
        if(this->m_bEnrolCompleted)
        {
#ifdef _MSC_VER
            saveMapToFile("main.map", &this->m_mapMain);
            saveMapToFile("ext.map", &this->m_mapExtended);
#endif

            this->processSettings();

            //after download is complete, we call restore - this will get other important settings, and make sure panel is happy with comms
            this->sendCommand(Pmax_RESTORE);
        }

        //re-starting keep alive timer
        this->startKeepAliveTimer();
    }
}

//Timeout message from the PM, most likely we are/were in download mode
void PowerMaxAlarm::OnTimeOut(const PlinkBuffer  * Buff)
{
    if(this->m_bDownloadMode)
    {
        this->sendCommand(Pmax_DL_EXIT); 
    }
    else
    {
        this->sendCommand(Pmax_ACK);
    }

    DEBUG(LOG_INFO,"Time Out"); 
}

void PowerMaxAlarm::OnAccessDenied(const PlinkBuffer  * Buff)
{
    DEBUG(LOG_INFO,"Access denied");
}

void PowerMaxAlarm::OnEventLog(const PlinkBuffer  * Buff)
{
    this->sendCommand(Pmax_ACK);   

    const unsigned char zoneId = Buff->buffer[9];
    const char* tpzone = this->getZoneName(zoneId);

    char logline[MAX_BUFFER_SIZE] = "";
    sprintf(logline,"event number:%d/%d at %d:%d:%d %d/%d/%d %s:%s",
        Buff->buffer[2],Buff->buffer[1],  // event number amoung number
        Buff->buffer[5],Buff->buffer[4],Buff->buffer[3],  //hh:mm:ss
        Buff->buffer[6],Buff->buffer[7],2000+Buff->buffer[8], //day/month/year
        tpzone,       //zone
        GetStrPmaxLogEvents(Buff->buffer[10])
    );  
    DEBUG(LOG_NOTICE,"event log:%s",logline);
}

void PowerMaxAlarm::OnStatusUpdate(const PlinkBuffer  * Buff)
{
    this->sendCommand(Pmax_ACK);   
    DEBUG(LOG_INFO,"pmax status update")   ;
}

//0xA7 message, called when system is armed/disarmed or alarm is started/cancelled
void PowerMaxAlarm::OnStatusChange(const PlinkBuffer  * Buff)
{
    this->sendCommand(Pmax_ACK);
    DEBUG(LOG_INFO,"PmaxStatusChange: '%s' by '%s'(0x%X)", GetStrPmaxLogEvents(Buff->buffer[4]), GetStrPmaxEventSource(Buff->buffer[3]), Buff->buffer[3]); 

    switch(Buff->buffer[4])
    {
    case 0x51: //"Arm Home" 
    case 0x53: //"Quick Arm Home"
        this->stat = SS_Armed_Home;
        memset(alarmTrippedZones, 0, sizeof(alarmTrippedZones));
        OnSytemArmed(Buff->buffer[4], GetStrPmaxLogEvents(Buff->buffer[4]), Buff->buffer[3], GetStrPmaxEventSource(Buff->buffer[3]));
        break;

    case 0x52: //"Arm Away"
    case 0x54: //"Quick Arm Away"
        this->stat = SS_Armed_Away;
        memset(alarmTrippedZones, 0, sizeof(alarmTrippedZones));
        OnSytemArmed(Buff->buffer[4], GetStrPmaxLogEvents(Buff->buffer[4]), Buff->buffer[3], GetStrPmaxEventSource(Buff->buffer[3]));
        break;

    case 0x55: //"Disarm"
        this->stat = SS_Disarm;
        OnSytemDisarmed(Buff->buffer[3], GetStrPmaxEventSource(Buff->buffer[3]));
        break;

    case 0x58: //"Exit From Test Mode"
        //TODO: this should trigger refresh of signal strength
        break;

    case 0x1B: //"Cancel Alarm"
        this->alarmState = 0;
        OnAlarmCancelled(Buff->buffer[3], GetStrPmaxEventSource(Buff->buffer[3]));
        break;

    case 0x1C: //"General Restore"
        if(this->alarmState != 0)
        {
            //this can happen when, alarm is opended (triggering tamper alarm), after it's closed - "General Restore" is called
            this->alarmState = 0;
            OnAlarmCancelled(Buff->buffer[3], GetStrPmaxEventSource(Buff->buffer[3]));
        }
        break;
    }

    if(Buff->buffer[4] > 0 && 
       Buff->buffer[4] <= 9) //see first 9 values from PmaxLogEvents
    {
        //alarm activated:
        //note: delay-alarm will not be delivered as OnStatusChange

        this->alarmState = Buff->buffer[4];
        OnAlarmStarted(Buff->buffer[4], GetStrPmaxLogEvents(Buff->buffer[4]), Buff->buffer[3], GetStrPmaxEventSource(Buff->buffer[3]));
    }
}

void PowerMaxAlarm::OnStatusUpdateZoneTamper(const PlinkBuffer  * Buff)
{
    this->sendCommand(Pmax_ACK);
    DEBUG(LOG_INFO,"Status Update : Zone active/tampered");

    const unsigned char * ZoneBuffer = Buff->buffer+3;
    for (int i=1;i<MAX_ZONE_COUNT;i++)
    {
        int byte=(i-1)/8;
        int offset=(i-1)%8;
        if (ZoneBuffer[byte] & 1<<offset)
        { 
            DEBUG(LOG_INFO,"Zone %d is NOT active",i ); 
            this->zone[i].stat.active=false;
        }
        else
        {
            this->zone[i].stat.active=true;
        }
    }

    ZoneBuffer=Buff->buffer+7;  
    for (int i=1;i<MAX_ZONE_COUNT;i++)
    {
        int byte=(i-1)/8;
        int offset=(i-1)%8;
        if (ZoneBuffer[byte] & 1<<offset)
        {
            DEBUG(LOG_INFO,"Zone %d is tampered",i );
            this->zone[i].stat.tamper=true;
        }
        else
        {
            this->zone[i].stat.tamper=false;    
        }      
    } 
}

void PowerMaxAlarm::OnStatusUpdateZoneBypassed(const PlinkBuffer  * Buff)
{  
    this->sendCommand(Pmax_ACK);
    DEBUG(LOG_INFO,"Status Update : Zone Enrolled/Bypassed");

    const unsigned char * ZoneBuffer = Buff->buffer+3;
    for (int i=1;i<MAX_ZONE_COUNT;i++)
    {
        int byte=(i-1)/8;
        int offset=(i-1)%8;
        if (ZoneBuffer[byte] & 1<<offset)
        {
            DEBUG(LOG_INFO,"Zone %d is enrolled",i );
            this->zone[i].enrolled = true;
        }
        else
        {
            this->zone[i].enrolled = false;
        }
    }

    ZoneBuffer=Buff->buffer+7;  
    for (int i=1;i<MAX_ZONE_COUNT;i++)
    {
        int byte=(i-1)/8;
        int offset=(i-1)%8;
        if (ZoneBuffer[byte] & 1<<offset) {
            DEBUG(LOG_INFO,"Zone %d is bypassed",i );
            this->zone[i].stat.bypased=true;
        }
        else {
            this->zone[i].stat.bypased=false;
        }       
    }
}

//0xA5,XX,0x04
void PowerMaxAlarm::OnStatusUpdatePanel(const PlinkBuffer  * Buff)    
{
    this->sendCommand(Pmax_ACK);

    this->stat  = (SystemStatus)Buff->buffer[3];
    this->flags = Buff->buffer[4];

    {
        char tpbuff[MAX_BUFFER_SIZE] = "";  //IZIZTODO: remove this, output directly to console
        sprintf(tpbuff,"System status: %s (%d), Flags :", GetStrPmaxSystemStatus(this->stat), this->stat);    
        for (int i=0;i<8;i++) //loop trough all 8 bits
        {
            if (this->flags & 1<<i) {
                os_strncat_s(tpbuff, MAX_BUFFER_SIZE, " ");
                os_strncat_s(tpbuff, MAX_BUFFER_SIZE, GetStrSystemStateFlags(i));
            }      
        }
        DEBUG(LOG_INFO,"%s", tpbuff);
    }

    // if system state flag says it is a zone event (bit 5 of system flag)  
    if (this->isZoneEvent()) {
        const unsigned char zoneId = Buff->buffer[5];
        ZoneEvent eventType = (ZoneEvent)Buff->buffer[6];

        //this information is delivered also by periodic updates, but here we get instant update:
        if  ( zoneId<MAX_ZONE_COUNT )
        {
            DEBUG(LOG_INFO,"Zone-%d-event, (%s) %s", zoneId, this->getZoneName(zoneId), GetStrPmaxZoneEventTypes(Buff->buffer[6]));
            this->zone[zoneId].lastEvent = eventType;
            this->zone[zoneId].lastEventTime = os_getCurrentTimeSec();

            switch(eventType)
            {
            case ZE_NotActive:
                this->zone[zoneId].stat.active = false;
                break;

            case ZE_Open:
                this->zone[zoneId].stat.doorOpen = true;
                break;

            case ZE_Closed:
                this->zone[zoneId].stat.doorOpen = false;
                break;

            case ZE_LowBattery:
            case ZE_SirenLowBattery:
                this->zone[zoneId].stat.lowBattery = true;
                break;

            case ZE_TamperAlarm:
            case ZE_TamperOpen:
            case ZE_SirenTamper:
                this->zone[zoneId].stat.tamper = true;
                break;

            case ZE_TamperRestore:
            case ZE_SirenTamperRestore:
                this->zone[zoneId].stat.tamper = false;
                break;

            case ZE_Violated:
                if(this->isAlarmEvent())
                {
                    if(this->stat == SS_Armed_Home && this->zone[zoneId].zoneType == 12 /* interior */)
                    {
                        //do nothing
                    }
                    else if(this->stat == SS_Armed_Away || this->stat == SS_Armed_Home)
                    {
                        //we have a zone violation in armed state
                        if(this->zone[zoneId].enrolled &&
                           this->zone[zoneId].stat.bypased == false)
                        {
                            //let's find an empty slot, and write current zone id, we don't skip duplicates as this will allow to see how atacker was moving around property
                            for(int ix=0; ix<MAX_ZONE_COUNT; ix++)
                            {
                                if(this->alarmTrippedZones[ix] == 0)
                                {
                                    this->alarmTrippedZones[ix] = zoneId;
                                    break;
                                }
                            }
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
    }
}

void PowerMaxAlarm::OnStatusUpdateZoneBat(const PlinkBuffer  * Buff)
{
    this->sendCommand(Pmax_ACK);
    DEBUG(LOG_INFO,"Status Update : Zone state/Battery");

    const unsigned char * ZoneBuffer = Buff->buffer+3;
    for (int i=1;i<MAX_ZONE_COUNT;i++)
    {
        int byte=(i-1)/8;
        int offset=(i-1)%8;
        if (ZoneBuffer[byte] & 1<<offset) {
            DEBUG(LOG_INFO,"Zone %d is open",i );
            this->zone[i].stat.doorOpen=true;      
        }
        else {
            this->zone[i].stat.doorOpen=false;
        }     
    }

    ZoneBuffer=Buff->buffer+7;  
    for (int i=1;i<MAX_ZONE_COUNT;i++)
    {
        int byte=(i-1)/8;
        int offset=(i-1)%8;
        if (ZoneBuffer[byte] & 1<<offset) {
            DEBUG(LOG_INFO,"Zone %d battery is low",i );
            this->zone[i].stat.lowBattery=true;
        }
        else  {
            this->zone[i].stat.lowBattery=false;
        }             
    }
} 


void logBuffer(int priority,struct PlinkBuffer * Buff)  {
    unsigned short i;
    char printBuffer[MAX_BUFFER_SIZE*3+3];
    char *bufptr;      /* Current char in buffer */
    bufptr=printBuffer;
    for (i=0;i<(Buff->size);i++) {
        sprintf(bufptr,"%02X ",Buff->buffer[i]);
        bufptr=bufptr+3;
    }  
    DEBUG(LOG_DEBUG,"BufferSize: %d" ,Buff->size);
    DEBUG(priority,"Buffer: %s", printBuffer);  
}

unsigned char calculChecksum(const unsigned char* data, int dataSize) {
    unsigned short checksum = 0xFFFF;
    for (int i=0;i<dataSize;i++)
        checksum=checksum - data[i];
    checksum=checksum%0xFF;
    DEBUG(LOG_DEBUG,"checksum: %04X",checksum);
    return (unsigned char) checksum;
} 

void  PowerMaxAlarm::sendNextCommand()
{
    if(m_bDownloadMode == false)
    {
        if(os_getCurrentTimeSec() - m_ulLastPing > 30)
        {
            m_ulLastPing = os_getCurrentTimeSec();
            sendCommand(Pmax_PING);
            return;
        }
    }

    if(m_sendQueue.isEmpty())
    {
        return;
    }

    os_usleep(50*1000); //to prevent messages going too quickly to PM

    PmQueueItem item = m_sendQueue.pop();

    sendBuffer(item.buffer, item.bufferLen);
}

//buffer is coppied, description and options need to be in pernament addressess (not something from stack)
bool PowerMaxAlarm::queueCommand(const unsigned char* buffer, int bufferLen, const char* description, unsigned char expectedRepply, const char* options)
{
    if(m_sendQueue.isFull())
    {
        DEBUG(LOG_CRIT,"Send queue is full, dropping packet: %s", description);  
        return false;
    }

    if(bufferLen > MAX_SEND_BUFFER_SIZE)
    {
        DEBUG(LOG_CRIT,"Buffer to send too long: %d", bufferLen);  
        return false;
    }

    PmQueueItem item;
    memcpy(item.buffer, buffer, bufferLen);
    item.bufferLen = bufferLen;
    item.description = description;
    item.expectedRepply = expectedRepply;
    item.options = options;

    m_sendQueue.push(item);
    return true;
}

bool PowerMaxAlarm::sendBuffer(const unsigned char * data, int bufferSize)
{
    DEBUG(LOG_DEBUG,"Sending the following buffer to serial TTY");  
    //logBuffer(LOG_DEBUG,Buff); //IZIZTODO

    if(bufferSize >= MAX_BUFFER_SIZE-2 || 
       bufferSize > MAX_SEND_BUFFER_SIZE) //should never happen, but this will prevent any buffer overflows to writeBuffer.buffer
    {
        DEBUG(LOG_ERR,"Too long buffer: %d", bufferSize);  
        return false;
    }

    memcpy(m_lastSentCommand.buffer, data, bufferSize);
    m_lastSentCommand.size = bufferSize;

    PlinkBuffer writeBuffer;
    writeBuffer.buffer[0]=0x0D;
    for (int i=0;i<(bufferSize);i++)
    {
        writeBuffer.buffer[i+1]=data[i];
    }
    writeBuffer.buffer[bufferSize+1]=calculChecksum(data, bufferSize);
    writeBuffer.buffer[2+bufferSize]=0x0A;
    writeBuffer.size=bufferSize+3;

    const int bytesWritten = os_pmComPortWrite(writeBuffer.buffer,bufferSize+3);
    if(bytesWritten == bufferSize+3)
    {
        DEBUG(LOG_DEBUG,"serial write OK"); 
        return true;
    }
    else
    {
        DEBUG(LOG_ERR,"serial write failed, bytes transfered: %d, expected: %d",bytesWritten, bufferSize+3); 	
        return false;
    }
}

void PowerMaxAlarm::sendBuffer(struct PlinkBuffer * Buff)  {
    sendBuffer(Buff->buffer, Buff->size);
}

bool deFormatBuffer(struct PlinkBuffer  * Buff, bool ignoreErrors) {
    const unsigned char checksum=Buff->buffer[Buff->size-2];

    for (int i=0;i<Buff->size;i++)
        Buff->buffer[i]=Buff->buffer[i+1]; 

    Buff->size=Buff->size-3;
    const unsigned char checkedChecksum=calculChecksum(Buff->buffer, Buff->size);
    if (checksum==checkedChecksum)  {
        DEBUG(LOG_DEBUG,"checksum OK");
        return true;
    } 
    else  {
        if(ignoreErrors != 0){
            DEBUG(LOG_ERR,"checksum NOK calculated:%02X in packet:%02X",checkedChecksum,checksum);
        }
        return false;
    }  
}  

// compare two buffer, 0xff are used as jocker char
bool findCommand(const PlinkBuffer  * Buff, const PlinkCommand  * BuffCommand)  {
    
    if(BuffCommand->size < 0)
    {
        //ignore len, match begining only
        if(Buff->size < (-BuffCommand->size))
        {
            return false;
        }

        for (int i=0;i<(-BuffCommand->size);i++)  
        {
            if ((Buff->buffer[i] != BuffCommand->buffer[i]) && (BuffCommand->buffer[i] != 0xFF )) return false;
        }
    }
    else
    {
        if (Buff->size!=BuffCommand->size)
        {
            return false;
        }

        for (int i=0;i<Buff->size;i++)  
        {
            if ((Buff->buffer[i] != BuffCommand->buffer[i]) && (BuffCommand->buffer[i] != 0xFF )) return false;
        }
    }

    return true;   
}


bool PowerMaxAlarm::isBufferOK(const PlinkBuffer* commandBuffer)
{
    const int old = log_console_setlogmask(LOG_ALERT); //highest level only
	PlinkBuffer commandBufferTmp ;
	memcpy(&commandBufferTmp, commandBuffer, sizeof(PlinkBuffer));
	bool ok = deFormatBuffer(&commandBufferTmp, true);

	log_console_setlogmask(old);
	return ok;
}

PmAckType PowerMaxAlarm::calculateAckType(const unsigned char* deformattedBuffer, int bufferLen)
{
    //Depending on the msgtype and/or last byte, we will be sending type-1 or 2 ACK
    if(bufferLen > 1)
    { 
        if(deformattedBuffer[0] >= 0x80 || (deformattedBuffer[0] < 0x10 && deformattedBuffer[bufferLen-1] == 0x43))
        {
            return ACK_2;
        }
    }

    return ACK_1;
}

void PowerMaxAlarm::handlePacket(PlinkBuffer* commandBuffer) {

    m_ackTypeForLastMsg = ACK_1;

    if (deFormatBuffer(commandBuffer, false)) {
        DEBUG(LOG_DEBUG,"Packet received");
        lastIoTime = os_getCurrentTimeSec();

        m_ackTypeForLastMsg = calculateAckType(commandBuffer->buffer, commandBuffer->size);

        logBuffer(LOG_DEBUG,commandBuffer);         
        int cmd_not_recognized=1;
        
        const int cmdCnt = (sizeof(PmaxCommand)/sizeof(PmaxCommand[0]));
        for (int i=0;i<cmdCnt;i++)  {
            if (findCommand(commandBuffer,&PmaxCommand[i]))  {
				DEBUG(LOG_INFO, "Command found: '%s'", PmaxCommand[i].description);
                (this->*PmaxCommand[i].action)(commandBuffer);
                cmd_not_recognized=0;
                break;
            }   
        }

        if ( cmd_not_recognized==1 )  {
            DEBUG(LOG_INFO,"Packet not recognized");
            logBuffer(LOG_INFO,commandBuffer);
            sendCommand(Pmax_ACK);   
        }                  
    }                                                         
    else  {
        DEBUG(LOG_ERR,"Packet not correctly formated");
        logBuffer(LOG_ERR,commandBuffer);

        //IZIZTODO:
        //should we send ack?
    }              
    //command has been treated, reset the commandbuffer
    commandBuffer->size=0;                    
    //reset End Of Packet to listen for a new packet       

    DEBUG(LOG_DEBUG,"End of packet treatment");
}

const char* PowerMaxAlarm::getZoneName(unsigned char zoneId)
{
    if(zoneId < MAX_ZONE_COUNT &&
       zone[zoneId].enrolled)
    {
        return zone[zoneId].name;
    }

    return "Unknown";
}

void ConsoleOutput::write(const char* str)
{
    DEBUG_RAW(LOG_NO_FILTER, "%s", str);
}

void PowerMaxAlarm::dumpToJson(IOutput* outputStream)
{
    outputStream->write("{");
    {
        OnDumpToJsonStarted(outputStream);

        outputStream->writeJsonTag("stat", stat);
        outputStream->writeJsonTag("stat_str", GetStrPmaxSystemStatus(stat));
        outputStream->writeJsonTag("lastCom", (int)getSecondsFromLastComm());
        outputStream->writeJsonTag("panelType", m_iPanelType);
        outputStream->writeJsonTag("panelTypeStr", GetStrPmaxPanelType(m_iPanelType));
        outputStream->writeJsonTag("panelModelType", m_iModelType);
        outputStream->writeJsonTag("alarmState", alarmState);
        outputStream->writeJsonTag("alarmStateStr", GetStrPmaxLogEvents(alarmState));        

        outputStream->write("\"alarmTrippedZones\":[");
        {
            for(int ix=0; ix<MAX_ZONE_COUNT; ix++)
            {
                if(alarmTrippedZones[ix] == 0)
                {
                    break;
                }

                if(ix >0)
                {
                    outputStream->write(",\r\n");
                }
                
                char zoneId[10] = "";
                sprintf(zoneId, "%d", ix);
                outputStream->write(zoneId);
            }
        }
        outputStream->write("],\r\n");

        if(m_cfg.parsedOK)
        {
            outputStream->write("\"config\":");
            m_cfg.DumpToJson(outputStream);
            outputStream->write(",\r\n");
        }

        outputStream->writeJsonTag("flags", flags);
        outputStream->writeJsonTag("flags_ready", isFlagSet(0));
        outputStream->writeJsonTag("flags_alertInMemory", isFlagSet(1));
        outputStream->writeJsonTag("flags_trouble", isFlagSet(2));
        outputStream->writeJsonTag("flags_bypasOn", isFlagSet(3));
        outputStream->writeJsonTag("flags_last10sec", isFlagSet(4));
        outputStream->writeJsonTag("flags_zoneEvent", isFlagSet(5));
        outputStream->writeJsonTag("flags_armDisarmEvent", isFlagSet(6));
        outputStream->writeJsonTag("flags_alarmEvent", isFlagSet(7));

        outputStream->write("\"enroled_zones\":[");
        {
            int addedCnt=0;
            for(int ix=1; ix<MAX_ZONE_COUNT; ix++)
            {
                if(zone[ix].enrolled)
                {
                    if(addedCnt >0)
                    {
                        outputStream->write(",\r\n");
                    }

                    addedCnt++;
                    zone[ix].DumpToJson(outputStream);
                }
            }
        }
        outputStream->write("]");

    }
    outputStream->write("}");
}

void Zone::DumpToJson(IOutput* outputStream)
{
    outputStream->write("{");
    {
        outputStream->writeJsonTag("zoneName", name);
        outputStream->writeJsonTag("zoneType", zoneType);
        outputStream->writeJsonTag("zoneTypeStr", zoneTypeStr);
        outputStream->writeJsonTag("sensorId", sensorId);
        outputStream->writeJsonTag("sensorType", sensorType);
        outputStream->writeJsonTag("sensorMake", sensorMake);
        outputStream->writeJsonTag("signalStrength", (int)signalStrength);
        
        if(lastEventTime > 0)
        {
            outputStream->writeJsonTag("lastEvent", lastEvent);
            outputStream->writeJsonTag("lastEventAge", (int)(os_getCurrentTimeSec()-lastEventTime));
        }

        outputStream->writeJsonTag("stat_doorOpen", stat.doorOpen);
        outputStream->writeJsonTag("stat_bypased", stat.bypased);
        outputStream->writeJsonTag("stat_lowBattery", stat.lowBattery);
        outputStream->writeJsonTag("stat_active", stat.active);
        outputStream->writeJsonTag("stat_tamper", stat.tamper, false);
    }
    outputStream->write("}");
}

void IOutput::writeQuotedStr(const char* str)
{
    write("\"");
    write(str); //todo: escape
    write("\"");
}

void IOutput::writeJsonTag(const char* name, bool value, bool addComma)
{
    writeJsonTag(name, value ? "true" : "false", addComma, false);
}

void IOutput::writeJsonTag(const char* name, int value, bool addComma)
{
    char szTmp[20];
    sprintf(szTmp, "%d", value);
    writeJsonTag(name, szTmp, addComma, false);
}

void IOutput::writeJsonTag(const char* name, const char* value, bool addComma, bool quoteValue)
{
    writeQuotedStr(name);
    write(":");
    if(quoteValue)
    {
        writeQuotedStr(value);
    }
    else
    {
        write(value);
    }

    if(addComma)
    {
        write(",\r\n");
    }
}
