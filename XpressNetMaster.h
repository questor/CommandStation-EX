/*
  XpressNetMaster.h - library for XpressNet Master protocoll
  Copyright (c) 2023 Philipp Gahtow  All right reserved.

  Notice:
  Support for XPressNet v4.1

  Change History:
	- timing issue when change to the next slot
	- add busy request for loco
	- narrow conversation compiler warning
	- speed up transmission window (Callbyte) and adjust timing
	- add MultiMaus full Support for F13 to F20
	- add unknown message return
	- add UART RX interrupt
	- add UART TX Buffer with interrupt control
	- fix MultiMaus CV read and write in Master Mode
	- fix speed step setting
	- add ack request answer in slave mode
	- add feedback return to all clients
	- add POM write byte and bit
	- fix slave ack return
	- add support for RAW Data in-/output
	- add inside 'AddBusySlot' a skip if already busy by this slot
	- adjust init client mode power setting
	- add support ESP8266 with RS485SoftwareSerial
	- fix sending long Adresses missing the two highest bits of the High bytes are set to "1" (0xC000)
	- fix hanging in CV-Prog
	- fix Accessory Decoder operation request Byte 2
	- fix Locomotive speed and direction operation (0xE4) Speed Steps
	- fix range CV# Adr to uint16_t
	- add software serial support for ESP8266 and ESP32
	- add internal power function
	- add support for XpressNet v4.0 with switch up to 2048
	- fix SWSERIAL_PARITY_MARK to PARITY_MARK because they change the name!
	- remove blocking Interrups while tx on ESP8266 and ESP32
*/

// ensure this library description is only included once
#ifndef XpressNetMaster_h
#define XpressNetMaster_h

//CONFIG:
#define XNetVersion 0x40	//System Bus Version
#define XNetID 0x10			//Zentrale: 0x00 = LZ100; 0x01 = LH200; 0x10 = ROCO MultiMaus; 0x02 = other; 

// include types & constants of Wiring core API
#if defined(WIRING)
 #include <Wiring.h>
#elif ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

/* From the ATMega datasheet: */
//--------------------------------------------------------------------------------------------
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) //Arduino MEGA
#define SERIAL_PORT_1
#undef SERIAL_PORT_0

#elif defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__)  //Sanguino (other pins!)
#define SERIAL_PORT_1
#undef SERIAL_PORT_0

#else //others Arduino UNO
#define SERIAL_PORT_0
#undef SERIAL_PORT_1
#endif

//--------------------------------------------------------------------------------------------
//only for Debug:
//#define XNetSerial Serial	//Debugging Serial
//#define XNetDEBUG		//Put out the messages
//#define XNetDEBUGTime	//Put out the microseconds


//--------------------------------------------------------------------------------------------
/*An XpressNet device designed to work with XpressNet V3 and later systems must be designed so that it 
begins its transmission within 110 microseconds of receiving its transmission window.  (older X-Bus 
based systems required this transmission to begin with in 40 microseconds.)  Command stations must be 
designed to accept transmissions received up to 120 microseconds after transmitting the window.  The 
difference is to provide a design tolerance between the different types of devices. 
Under normal conditions an XpressNet device must be designed to be able to handle the receipt of its 
next transmission window between 400 microseconds and 500 milliseconds after the receipt of the last window.  */

#if defined(ESP8266) || defined(ESP32)
#define XNetTransmissionWindow 3000	//wait longer = slower, because software serial interrupt
#else
#define XNetTransmissionWindow 500	//max time to wait until data will be received
#endif

//XpressNet Buffer length (send and receive):	
#define XNetBufferSize 5	//max Data Pakets (max: 4 Bit = 16!)

//XpressNet Mode (Master/Slave)
#define XNetSlaveCycle 0xFF	//max (255) cycles to Stay in SLAVE MODE when no CallByte is received

//Fahrstufen:
#define Loco14 0x00		//FFF = 000 = 14 speed step
#define Loco27 0x01		//FFF = 001 = 27 speed step
#define Loco28 0x02		//FFF = 010 = 28 speed step
#define Loco128 0x04	//FFF = 100 = 128 speed step

// XPressnet Call Bytes.
// broadcast to everyone, we save the incoming data and process it later.
#define GENERAL_BROADCAST 0x60	//0x160
#define FB_BROADCAST 0xA0		//0x1A0
#define MY_ADDRESS 0x5F	//only for SLAVE-MODE Adr=31 (0x5F) or Adr=26 (0x5A)!

#define ACK_REQ 0x9A	//Acknowledge-Anforderungen beantworten mit 0x20 0x20

// certain global XPressnet status indicators
#define csNormal 0x00 // Normal Operation Resumed ist eingeschaltet
#define csEmergencyStop 0x01 // Der Nothalt ist eingeschaltet
#define csTrackVoltageOff 0x02 // Die Gleisspannung ist abgeschaltet
#define csShortCircuit 0x04 // Kurzschluss
#define csServiceMode 0x08 // Der Programmiermodus ist aktiv - Service Mode

//XpressNet Befehl, with max. 7 data bytes!
#define XNetCallByte 0		//CallByte
#define XNetheader	1		//Header
#define XNetdata1	2		//Databyte1
#define XNetdata2	3		//Databyte2
#define XNetdata3	4		//Databyte3
#define XNetdata4	5		//Databyte4
#define XNetdata5	6		//Databyte5
#define XNetdata6	7		//Databyte6
#define XNetdata7	8		//Databyte7
#define XNetCRC		9		//Checksumme

#define XNetBufferMaxData 10		//max Bytes over all for each Paket

typedef struct	//Msg Seicher
{
	uint8_t length;			//Speicher f�r Datenl�nge
	uint8_t data[XNetBufferMaxData];	//zu sendende Daten
} XNetMessage;

typedef struct	//Buffer
{
	XNetMessage msg[XNetBufferSize];
	uint8_t get;	//position we are with reading
	uint8_t put;	//position we are with writing
	uint8_t pos;	//byte position for write
} XNetBuffer;

// library interface description
class XpressNetMasterClass
{
  // user-accessible "public" interface
  public:
    XpressNetMasterClass(void);	//Constuctor
	#if defined(ESP8266) || defined(ESP32)
	void setup(uint8_t FStufen, uint8_t XNetPort, uint8_t XControl, bool XnModeAuto = true);  //Initialisierung Serial
	#else
	void setup(uint8_t FStufen, uint8_t XControl, bool XnModeAuto = true);  //Initialisierung Serial
	#endif
	bool update(void);  			//Set new Data on the Dataline
	
	bool getOperationModeMaster(void);	//gibt TRUE zur�ck wenn aktuelle Master Mode aktiv ist!

	void setPower(byte Power);		//Zustand Gleisspannung Melden
	void setBCFeedback(byte data1, byte data2);	//R�ckmeldedaten an Clients verteilen

	//Control Loco
	void ReqLocoBusy(uint16_t Adr);	//Lok Adresse besetzt melden
	void SetLocoBusy(uint8_t UserOps, uint16_t Adr);	//Lok besetzt melden
	
	void getStatus();					//Staus der Zentrale erfragen
	void getLocoInfo(uint16_t Adr);		//Slave Modus Lok Informationen erfragen!
	void getLocoFkt(uint16_t Adr);		//Slave Modus Lok Funktionen erfragen!
	
	void SetLocoInfo(uint8_t UserOps, uint8_t Speed, uint8_t F0, uint8_t F1);	//Lokinfo an XNet Melden
	void SetLocoInfo(uint8_t UserOps, uint8_t Steps, uint8_t Speed, uint8_t F0, uint8_t F1);	//Lokinfo an XNet Melden
	void SetFktStatus(uint8_t UserOps, uint8_t F4, uint8_t F5);	//LokFkt an XNet Melden
	void SetLocoInfoMM(uint8_t UserOps, uint8_t Steps, uint8_t Speed, uint8_t F0, uint8_t F1, uint8_t F2, uint8_t F3);
	void SetTrntStatus(uint8_t UserOps, uint16_t Address, uint8_t Data); // data=0000 00AA	A=Weichenausgang1+2 (Aktive/Inaktive);
	void SetTrntPos(uint16_t Address, uint8_t state, uint8_t active);	//�nderung der Weichenlage

	void setSpeed(uint16_t Adr, uint8_t Steps, uint8_t Speed);
	void setFunc0to4(uint16_t Adr, uint8_t G1); //Gruppe 1: 0 0 0 F0 F4 F3 F2 F1
	void setFunc5to8(uint16_t Adr, uint8_t G2); //Gruppe 2: 0 0 0 0 F8 F7 F6 F5 
	void setFunc9to12(uint16_t Adr, uint8_t G3); //Gruppe 3: 0 0 0 0 F12 F11 F10 F9 
	void setFunc13to20(uint16_t Adr, uint8_t G4); //Gruppe 4: F20 F19 F18 F17 F16 F15 F14 F13  
	void setFunc21to28(uint16_t Adr, uint8_t G5); //Gruppe 5: F28 F27 F26 F25 F24 F23 F22 F21

	void setCVReadValue(uint8_t cvAdr, uint8_t value);	//return a CV data read
	void setCVNack(void);	//no ACK
	void setCVNackSC(void); //no ACK Short Circuit
	
	// public only for easy access by interrupt handlers
	static inline void handle_RX_interrupt();		//Serial RX Interrupt bearbeiten
	static inline void handle_TX_interrupt();		//Serial TX Interrupt bearbeiten
	
private:
	bool XModeAuto;		//ON = Automatische Umschaltung Master/Slave-Mode; OFF = Slave Mode only
	uint8_t XNetSlaveMode;	// > 0 then we are working in SLAVE MODE
	uint8_t XNetSlaveInit;	//send initialize sequence
	byte Railpower;		//Data of the actual Power State
	byte Fahrstufe;	//Standard f�r Fahrstufe
	byte MAX485_CONTROL; //Port for send or receive control
	uint8_t XNetAdr;	//Adresse des Abzufragenden XNet Device
	unsigned long XSendCount;	//Zeit: Call Byte ausgesendet, data received
	
	XNetBuffer XNetRXBuffer;	//Read Buffer
	
	byte callByteParity (byte me);	// calculate the parity bit
	uint8_t CallByteInquiry;
	uint8_t RequestAck;
	uint8_t DirectedOps;

	uint16_t SlotLokUse[32];	//store loco to DirectedOps
	void SetBusy(uint8_t Slot);	//send busy message to slot that doesn't use
	void AddBusySlot(uint8_t UserOps, uint16_t Adr);	//add loco to slot
	
	void XNetRXclear(uint8_t b);	//Clear a spezial RX Message

		//Functions:
	void unknown(void);		//unbekannte Anfrage
	void getNextXNetAdr(void);	//N�CHSTE Adr of XNet Device
	bool XNetCheckXOR(void);	//Checks the XOR
	void XNetAnalyseReceived(void);		//work on received data
	
		//Serial send and receive:
#if defined(__AVR__)
	static XpressNetMasterClass *active_object;	//aktuelle aktive Object for interrupt handler	
#endif
	
	XNetBuffer XNetTXBuffer;
		
 	void XNetsend(byte *dataString, byte byteCount);	//Sende Datenarray out NOW!
	uint16_t XNetReadBuffer(void);	//read out next Buffer Data
	void getXOR (uint8_t *data, byte length); // calculate the XOR
	void XNetSendData(void);	//Sendet Daten aus dem Buffer mittels Interrupt
	void XNetSendNext(void);	//Recursives sende weiterer Daten aus dem Buffer
	void XNetReceive(void);	//Speichern der eingelesenen Daten
	
	uint16_t XNetCVAdr;		//CV Adr that was read
	uint8_t XNetCVvalue;	//read CV Value 
	
	uint16_t SlaveRequestLocoInfo = 0;		//Lok that we request for Lokdata
	uint16_t SlaveRequestLocoFkt = 0;		//Lok that we request for Lok funktions
};

#if defined (__cplusplus)
	extern "C" {
#endif
	extern void notifyXNetPower(uint8_t State) __attribute__((weak));
	extern uint8_t getPowerState() __attribute__((weak));	//give Back Actual Power State
	//Fahrbefehl:
	extern void notifyXNetgiveLocoInfo(uint8_t UserOps, uint16_t Address) __attribute__((weak));
	extern void notifyXNetLocoDrive14(uint16_t Address, uint8_t Speed) __attribute__((weak));
	extern void notifyXNetLocoDrive27(uint16_t Address, uint8_t Speed) __attribute__((weak));
	extern void notifyXNetLocoDrive28(uint16_t Address, uint8_t Speed) __attribute__((weak));
	extern void notifyXNetLocoDrive128(uint16_t Address, uint8_t Speed) __attribute__((weak));
	//Funktionsbefehl:
	extern void notifyXNetgiveLocoFunc(uint8_t UserOps, uint16_t Address) __attribute__((weak));
	extern void notifyXNetLocoFunc1(uint16_t Address, uint8_t Func1) __attribute__((weak));//Gruppe1 0 0 0 F0 F4 F3 F2 F1
	extern void notifyXNetLocoFunc2(uint16_t Address, uint8_t Func2) __attribute__((weak));//Gruppe2 0000 F8 F7 F6 F5
	extern void notifyXNetLocoFunc3(uint16_t Address, uint8_t Func3) __attribute__((weak));//Gruppe3 0000 F12 F11 F10 F9
	extern void notifyXNetLocoFuncX(uint16_t Address, uint8_t group, uint8_t Func) __attribute__((weak));//Gruppe4 F20-F13, Gruppe5 F28-F21, .....
	//Weichenbefehl:
	extern void notifyXNetTrntInfo(uint8_t UserOps, uint16_t Address, uint8_t data) __attribute__((weak));// data=0000 000N	N=Nibble N0-(0,1); N1-(2,3);
	extern void notifyXNetTrnt(uint16_t Address, uint8_t data) __attribute__((weak));// data=0000 000A	A=Weichenausgang (Aktive/Inaktive);
	//R�ckmeldung:
	extern void notifyXNetFeedback(uint16_t Address, uint8_t data) __attribute__((weak));// data=0000 000A	A=Weichenausgang (Aktive/Inaktive);
	//CV:
	extern void notifyXNetDirectCV(uint16_t CV, uint8_t data) __attribute__((weak));
	extern void notifyXNetDirectReadCV(uint16_t cvAdr) __attribute__((weak));
	//POM:
	extern void notifyXNetPOMwriteByte (uint16_t Adr, uint16_t CV, uint8_t data) __attribute__((weak));
	extern void notifyXNetPOMwriteBit (uint16_t Adr, uint16_t CV, uint8_t data) __attribute__((weak));
	//MultiMaus:
	extern void notifyXNetgiveLocoMM(uint8_t UserOps, uint16_t Address) __attribute__((weak));	

#if defined (__cplusplus)
}
#endif


#endif
