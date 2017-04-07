#pragma once
#include <stdio.h>
#include "FixedSizeQueue.h"
#include "MemoryMap.h"

#define  MAX_BUFFER_SIZE 250
#define  MAX_SEND_BUFFER_SIZE 15
#define  MAX_SEND_QUEUE_DEPTH 15
#define  PACKET_TIMEOUT_DEFINED 2000
#define  MAX_ZONE_COUNT 31
#define  SLOW_PANEL_INIT_WAIT_TIME 10 //10s is enough for even old panels to complete INIT command

//This Pin is a temporary pin for powerlink (this app). Does not have to match any of user or installer codes.
//If your pin is 1234, you need to return 0x1234 (this is strange, as 0x makes it hex, but the only way it works).
#define POWERLINK_PIN 0x3622;

class PowerMaxAlarm;

enum PmaxCommand
{
    Pmax_ACK,
    Pmax_PING,
    Pmax_GETEVENTLOG,
    Pmax_DISARM,
    Pmax_ARMHOME,
    Pmax_ARMAWAY,
    Pmax_ARMAWAY_INSTANT,
    Pmax_REQSTATUS,
    Pmax_ENROLLREPLY,
    Pmax_INIT,
    Pmax_RESTORE,
    Pmax_DL_START,
    Pmax_DL_GET,
    Pmax_DL_EXIT, //stop download mode
    Pmax_DL_PANELFW,
    Pmax_DL_SERIAL,
    Pmax_DL_ZONESTR,
    Pmax_DL_ZONESIGNAL //signal strength for zones
};

enum ZoneEvent
{
    ZE_None,
    ZE_TamperAlarm,
    ZE_TamperRestore,
    ZE_Open,
    ZE_Closed,
    ZE_Violated,
    ZE_PanicAlarm,
    ZE_RFJamming,
    ZE_TamperOpen,
    ZE_CommunicationFailure,
    ZE_LineFailure,
    ZE_Fuse,
    ZE_NotActive,
    ZE_LowBattery,
    ZE_ACFailure,
    ZE_FireAlarm,
    ZE_Emergency,
    ZE_SirenTamper,
    ZE_SirenTamperRestore,
    ZE_SirenLowBattery,
    ZE_SirenACFail
};

enum SystemStatus
{
    SS_Disarm = 0x00,
    SS_Exit_Delay = 0x01,
    SS_Exit_Delay2 = 0x02,
    SS_Entry_Delay = 0x03,
    SS_Armed_Home = 0x04,
    SS_Armed_Away = 0x05,
    SS_User_Test = 0x06,
    SS_Downloading = 0x07,
    SS_Programming = 0x08,
    SS_Installer = 0x09,
    SS_Home_Bypass = 0x0A,
    SS_Away_Bypass = 0x0B,
    SS_Ready = 0x0C,
    SS_Not_Ready = 0x0D
};

enum PmAckType
{
    ACK_1,
    ACK_2
};

//this abstract class is used by DumpToJson API
//it allows to redirect JSON to file, console, www output
class IOutput
{
public:
    virtual void write(const char* str) = 0;
    
    void writeQuotedStr(const char* str);
    void writeJsonTag(const char* name, bool value, bool addComma = true);
    void writeJsonTag(const char* name, int value, bool addComma = true);
    void writeJsonTag(const char* name, const char* value, bool addComma = true, bool quoteValue = true);
};

class ConsoleOutput : public IOutput
{
public:
    void write(const char* str);
};

struct PlinkCommand {
    unsigned char buffer[MAX_SEND_BUFFER_SIZE];
    int size;
    const char* description;
    void (PowerMaxAlarm::*action)(const struct PlinkBuffer *);
};

struct PlinkBuffer {
    unsigned char buffer[MAX_BUFFER_SIZE];
    int size;
};

struct ZoneState {
    bool lowBattery; //battery needs replacing
    bool tamper;     //someone tampered with the device
    bool doorOpen;   //door is open (either intrusion or not redy to arm)
    bool bypased;    //user temporarly disabled this zone
    bool active;     //commication with one is OK
};

struct Zone {
    bool enrolled;       //PowerMax knows about this zone (it's configured)
    char name[0x11];     //Will be dowloaded from PowerMax eprom
    unsigned char zoneType;
    const char* zoneTypeStr;
    unsigned char sensorId;
    const char* sensorType;
    const char* sensorMake;
    unsigned char signalStrength; //0 - bad, 1 - poor, 2 - good, 3 - strong, 0xFF unknown

    ZoneState stat;      //basic state of the zone

    ZoneEvent lastEvent; //last event recodred for this zone
    unsigned long lastEventTime;

    void DumpToJson(IOutput* outputStream);
};

struct PmQueueItem
{
    unsigned char buffer[MAX_SEND_BUFFER_SIZE];
    int bufferLen;

    const char* description;
    unsigned char expectedRepply;
    const char* options;
};

struct PmConfig
{
    bool parsedOK;

    char installerPin[5];
    char masterInstallerPin[5];
    char powerLinkPin[5];
    char userPins[48][5];

    //telephone numbers to call:
    char phone[4][15]; //15: max 14 digits + NULL

    char serialNumber[15];
    char eprom[17];
    char software[17];

    unsigned char partitionCnt;

    //panel max capabilities (not actual count used):
    unsigned char maxZoneCnt;
    unsigned char maxCustomCnt;
    unsigned char maxUserCnt;
    unsigned char maxPartitionCnt;
    unsigned char maxSirenCnt;
    unsigned char maxKeypad1Cnt;
    unsigned char maxKeypad2Cnt;
    unsigned char maxKeyfobCnt;

    PmConfig()
    {
        Init();
    }

    void Init()
    {
        memset(this, 0, sizeof(PmConfig));
    }

    void DumpToJson(IOutput* outputStream);
    int GetMasterPinAsHex() const;
};

//NOTE: if you want to add some new functionality (like MQTT support), don't modify this class.
//Rather than: derive from this class, and override one of the virtual functions.
//This class should contain only code to comminicate with PowerMax alarms.
class PowerMaxAlarm
{
protected:
    //Flags with[*] are one-shot notifications of last event
    //For example when user arms away - bit 6 will be set
    //When system will deliver 'last 10 sec' notification - bit 6 will be cleared

    //bit 0: Ready if set
    //bit 1: [?] Alert in Memory if set
    //bit 2: [?] Trouble if set
    //bit 3: [?] Bypass On if set
    //bit 4: [*] Last 10 seconds of entry or exit delay if set
    //bit 5: [*] Zone event if set 
    //bit 6: [*] Arm, disarm event 
    //bit 7: [?] Alarm event if set
    unsigned char flags;

    //overall system status
    SystemStatus stat;

    //specifies if system is in alarm mode (this is value from 0 to 9 that match first values from PmaxLogEvents)
    unsigned char alarmState;

    //this is helpful to detect if alarm is real or false, will contain list of zones that were tripped during last alarm
    unsigned char alarmTrippedZones[MAX_ZONE_COUNT]; 

    //status of all zones (0 is not used, system)
    Zone zone[MAX_ZONE_COUNT];

    //config downloaded from PM, parsed using ProcessSettings
    PmConfig m_cfg;

    //used to detect when PowerMax stops talking to us, that will trigger re-establish comms message
    time_t lastIoTime;

    //older alarms were slow to respond to INIT command, and require a sleep of around 8 seconds.
    //new panels (like PowerMax Complete) don't require this at all
    int m_iInitWaitTime;

    FixedSizeQueue<PmQueueItem, MAX_SEND_QUEUE_DEPTH> m_sendQueue;

    bool m_bEnrolCompleted;
    bool m_bDownloadMode;
    int m_iPanelType;
    int m_iModelType;
    bool m_bPowerMaster;
    PmAckType m_ackTypeForLastMsg;

    //used to store data downloaded from PM
    MemoryMap m_mapMain;
    MemoryMap m_mapExtended;

    PlinkCommand m_lastSentCommand;
    unsigned long m_ulLastPing; //last ping fom Pmax->Alarm
    unsigned long m_ulNextPingDeadline; //when to expect next ping from Alarm->Pmax, if it will not arrive, we will send restore command. Pings are not expected in donwload mode
public:

    void init(int initWaitTime = SLOW_PANEL_INIT_WAIT_TIME);
    void sendNextCommand();
    bool restoreCommsIfLost();
    void clearQueue(){ m_sendQueue.clear(); }
    
    bool sendCommand(PmaxCommand cmd);
    void handlePacket(PlinkBuffer  * commandBuffer);
    
    //Sets the date and time of the panel (works only in programming (download) mode)
    //year  : must be specified as the years since 2000
    //month : must be between 1-12
    //day   : must be the day of the month (1-31)
    //hours : must be in 24 hour format
    bool setDateTime(unsigned char year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minutes, unsigned char seconds);

    static bool isBufferOK(const PlinkBuffer* commandBuffer);
    const char* getZoneName(unsigned char zoneId);

    bool isConfigParsed() const { return m_cfg.parsedOK; }
    unsigned int getEnrolledZoneCnt() const;
    unsigned long getSecondsFromLastComm() const;
    void dumpToJson(IOutput* outputStream);

#ifdef _MSC_VER
    void IZIZTODO_testMap();
#endif

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //those are 'RAW' handlers for alarm messages
    //override thise function in derived class to handle notifications
    //some of those will generate further events that you can also override
    virtual void OnStatusUpdateZoneBat(const PlinkBuffer  * Buff);
    virtual void OnStatusUpdatePanel(const PlinkBuffer  * Buff);
    virtual void OnStatusUpdateZoneBypassed(const PlinkBuffer  * Buff);
    virtual void OnStatusUpdateZoneTamper(const PlinkBuffer  * Buff);
    virtual void OnStatusChange(const PlinkBuffer  * Buff);
    virtual void OnStatusUpdate(const PlinkBuffer  * Buff);
    virtual void OnEventLog(const PlinkBuffer  * Buff);
    virtual void OnAccessDenied(const PlinkBuffer  * Bufff);
    virtual void OnAck(const PlinkBuffer  * Buff);
    virtual void OnTimeOut(const PlinkBuffer  * Buff);
    virtual void OnStop(const PlinkBuffer  * Buff);
    virtual void OnEnroll(const PlinkBuffer  * Buff);
    virtual void OnPing(const PlinkBuffer  * Buff);
    virtual void OnPanelInfo(const PlinkBuffer  * Buff);
    virtual void OnDownloadInfo(const PlinkBuffer  * Buff);
    virtual void OnDownloadSettings(const PlinkBuffer  * Buff);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //those events are generated in 'RAW' On... handlers defined above, the only reason for those is to simplify common tasks

    //Fired when system enters armed state
    //armType     : 0x51 = "Arm Home", 0x53 = "Quick Arm Home", 0x52 = "Arm Away", 0x54 = "Quick Arm Away"
    //armTypeStr  : text representation of arming method
    //whoArmed    : specifies who armed system (for example a keyfob 1), values from PmaxEventSource
    //whoArmedStr : text representation of who armed
    virtual void OnSytemArmed(unsigned char armType, const char* armTypeStr, unsigned char whoArmed, const char* whoArmedStr){};

    //Fired when system enters disarmed state
    //whoDisarmed    : specifies who disarmed system (for example a keyfob 1), values from PmaxEventSource
    //whoDisarmedStr : text representation of who disarmed
    virtual void OnSytemDisarmed(unsigned char whoDisarmed, const char* whoDisarmedStr){};

    //Fired when system enters alarm state
    //alarmType      : type of alarm, first 9 values from PmaxLogEvents
    //alarmTypeStr   : text representation of alarmType
    //zoneTripped    : specifies zone that initiated the alarm, values from PmaxEventSource
    //zoneTrippedStr : zone name
    virtual void OnAlarmStarted(unsigned char alarmType, const char* alarmTypeStr, unsigned char zoneTripped, const char* zoneTrippedStr){};

    //Fired when alarm is cancelled
    //whoDisarmed    : specifies who cancelled the alarm (for example a keyfob 1), values from PmaxEventSource
    //whoDisarmedStr : text representation of who disarmed
    virtual void OnAlarmCancelled(unsigned char whoDisarmed, const char* whoDisarmedStr){};

    //Fired when date and time is downloaded from the panel (can be used for example to compare date against external source, and reset it using setDateTime if it's not accurate)
    //parameters     : see setDateTime function
    virtual void OnPanelDateTime(unsigned char year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minutes, unsigned char seconds){};

    //Allows to add extra fields to json output
    virtual void OnDumpToJsonStarted(IOutput* outputStream){};

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //those functions provide string representation of various types in alarm, you can override to provide your own text
    virtual const char* GetStrPmaxSystemStatus(int index);
    virtual const char* GetStrSystemStateFlags(int index);
    virtual const char* GetStrPmaxZoneEventTypes(int index);
    virtual const char* GetStrPmaxLogEvents(int index);
    virtual const char* GetStrPmaxPanelType(int index);
    virtual const char* GetStrPmaxZoneTypes(int index);
    virtual const char* GetStrPmaxEventSource(int index);

    SystemStatus GetSystemStatus() const { return stat;} 
protected:
    void addPin(unsigned char* bufferToSend, int pos = 4, bool useMasterCode = false);

    bool isFlagSet(unsigned char id) const { return (flags & 1<<id) != 0; }
    bool isAlarmEvent() const{  return isFlagSet(7); }
    bool isZoneEvent()  const{  return isFlagSet(5); }

    void format_SystemStatus(char* tpbuff, int buffSize);

    //buffer is coppied, description and options need to be in pernament addressess (not something from stack)
    bool queueCommand(const unsigned char* buffer, int bufferLen, const char* description, unsigned char expectedRepply = 0x00, const char* options = NULL);
    void powerLinkEnrolled();
    void processSettings();

    int readMemoryMap(const unsigned char* msg, unsigned char* buffOut, int buffOutSize);
    void writeMemoryMap(int iPage, int iIndex, const unsigned char* sData, int sDataLen);

    bool sendBuffer(const unsigned char * data, int bufferSize);
    void sendBuffer(struct PlinkBuffer * Buff);
    static PmAckType calculateAckType(const unsigned char* deformattedBuffer, int bufferLen);

    void startKeepAliveTimer();
    void stopKeepAliveTimer();
};

/* This section specifies OS specific functions. */
/* Implementation for Windows (MSVS) is provided */
/* If you compile for other OS provide your own. */
#ifndef LOG_INFO
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */
#endif
#define	LOG_NO_FILTER 0 /* message always outputed */
#define DEBUG(x,...) os_debugLog(x, false,__FUNCTION__,__LINE__,__VA_ARGS__);
#define DEBUG_RAW(x,...) os_debugLog(x, true,__FUNCTION__,__LINE__,__VA_ARGS__);
int log_console_setlogmask(int mask);

bool os_pmComPortInit(const char* portName);
int  os_pmComPortRead(void* writePos, int bytesToRead);
int  os_pmComPortWrite(const void* dataToWrite, int bytesToWrite);
bool os_pmComPortClose();
void os_usleep(int microseconds);

int os_cfg_getPacketTimeout();

void os_debugLog(int priority, bool raw, const char *function, int line,const char *format, ...);
void os_strncat_s(char* dst, int dst_size, const char* src);

//see PowerMaxAlarm::setDateTime for details of the parameters, if your OS does not have a RTC clock, simply return false
bool os_getLocalTime(unsigned char& year, unsigned char& month, unsigned char& day, unsigned char& hour, unsigned char& minutes, unsigned char& seconds);

unsigned long os_getCurrentTimeSec();
