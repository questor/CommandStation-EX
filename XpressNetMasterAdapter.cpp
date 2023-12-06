
#include "XpressNetMaster.h"

#include "DIAG.h"
#include "CommandDistributor.h"
#include "TrackManager.h"
#include "DCC.h"

extern XpressNetMasterClass gXpressNetMaster;

#if defined (__cplusplus)
	extern "C" {
#endif

void notifyXNetPower(uint8_t State) {
	DIAG(F("notifyXNetPower(0x%x)"), State);
	switch(State) {
	case csNormal:
      TrackManager::setMainPower(POWERMODE::ON);
		break;
	case csEmergencyStop: 
	case csTrackVoltageOff:
	case csShortCircuit:
	case csServiceMode: 
      TrackManager::setMainPower(POWERMODE::OFF);
      break;
	default:
		break;
	}
   CommandDistributor::broadcastPower();
}

//Fahrbefehl
void notifyXNetgiveLocoInfo(uint8_t UserOps, uint16_t locoAddress) {
	DIAG(F("notifyXNetgiveLocoInfo(adr 0x%x)"), locoAddress);

/*
  uint8_t ldata[6];
  dcc.getLocoData(Address, ldata);  //uint8_t Steps[0], uint8_t Speed[1], uint8_t F0[2], uint8_t F1[3], uint8_t F2[4], uint8_t F3[5]
  if (ldata[0] == 0x03)  //128 Steps?
      ldata[0]++;  //set Steps to 0x04
  XpressNet.SetLocoInfo(UserOps, ldata[0], ldata[1], ldata[2], ldata[3]); //UserOps,Steps,Speed,F0,F1
*/
}
void notifyXNetLocoDrive14(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive14"));
	//keine Antwort notwendig
}
void notifyXNetLocoDrive27(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive27"));
	//keine Antwort notwendig
}
void notifyXNetLocoDrive28(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive28"));

	bool direction = (Speed & 0x40 == 0x40) ? true : false;  //bit6 codes the direction

   uint32_t adaptedSpeed;	//use 32bit to get precision when scaling to 0..128
   if(Speed == 0) {        //Fahrstufe 0
      adaptedSpeed = 0;
   } else {
      Speed = Speed & 0b11111;   //mask only speed information
      if(Speed == 1) {        //emergency without delay!
         adaptedSpeed = 1;    //1 means emergency stop in DCC
      } else {
         adaptedSpeed = ((Speed & 0b10000) >> 4) | ((Speed & 0x1)<<1) | (Speed & 0b1100);
      }
   }

   adaptedSpeed = adaptedSpeed*127 / 28;	//scale to 0..127

   DCC::setThrottle(Address, (uint8_t)adaptedSpeed, direction);

	//keine Antwort notwendig
}
void notifyXNetLocoDrive128(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive128"));

	bool direction = (Speed & 0x80 == 0x80) ? true : false;  //bit7 codes the direction

	if(Speed == 1) {	//emergency stop
		Speed = 0;
	} //else if(Speed > 1) {
//		Speed = Speed - 1;	//remove code for emergency stop
//	}

	DCC::setThrottle(Address, Speed, direction);

	//keine Antwort notwendig
}

//MultiMaus
void notifyXNetgiveLocoMM(uint8_t UserOps, uint16_t Address) {
	DIAG(F("notifyXNetgiveLocoMM"));

//   uint8_t steps = 0x03+1;   //4 == 128 steps hardcoded
   uint8_t steps = 0x02;   //2 == 28 steps hardcoded

   uint8_t speed = DCC::getThrottleSpeed(Address);
   if(speed == -1) {    //loco not found?
      DIAG(F("loco not found while getting speed"));
      return;
   }
   if(speed == 1)
      speed = 0;     //emergency stop

   uint8_t f0 = DCC::getFn(Address, 0);
   uint8_t f1 = DCC::getFn(Address, 1);
   uint8_t f2 = DCC::getFn(Address, 2);
   uint8_t f3 = DCC::getFn(Address, 3);

   gXpressNetMaster.SetLocoInfoMM(UserOps, steps, speed, f0, f1, f2, f3);
}

//Funktionsbefehl
void notifyXNetgiveLocoFunc(uint8_t UserOps, uint16_t Address) {
	DIAG(F("notifyXNetgiveLocoFunc"));
//  XpressNet.SetFktStatus(UserOps, dcc.getFunktion13to20(Address), dcc.getFunktion21to28(Address)); //Fkt4, Fkt5
}
void notifyXNetLocoFunc1(uint16_t Address, uint8_t Func1) {
	//Gruppe1 0 0 0 F0 F4 F3 F2 F1
	DIAG(F("notifyXNetLocoFunc1"));
}
void notifyXNetLocoFunc2(uint16_t Address, uint8_t Func2) {
	//Gruppe2 0000 F8 F7 F6 F5
	DIAG(F("notifyXNetLocoFunc2"));
}
void notifyXNetLocoFunc3(uint16_t Address, uint8_t Func3) {
	//Gruppe3 0000 F12 F11 F10 F9
	DIAG(F("notifyXNetLocoFunc3"));
}
void notifyXNetLocoFuncX(uint16_t Address, uint8_t group, uint8_t Func) {
	//Gruppe4 F20-F13, Gruppe5 F28-F21, .....
	DIAG(F("notifyXNetLocoFuncX"));
}

//Weichenbefehl
void notifyXNetTrntInfo(uint8_t UserOps, uint16_t Address, uint8_t data) {
	// data=0000 000N	N=Nibble N0-(0,1); N1-(2,3);
	DIAG(F("notifyXNetTrntInfo"));
}
void notifyXNetTrnt(uint16_t Address, uint8_t data) {
	// data=0000 000A	A=Weichenausgang (Aktive/Inaktive);
  DIAG(F("notifyXNetTrnt"));
}

//Rueckmeldung
void notifyXNetFeedback(uint16_t Address, uint8_t data) {
	// data=0000 000A	A=Weichenausgang (Aktive/Inaktive);
	DIAG(F("notifyXNetFeedback"));
}

//CV
void notifyXNetDirectCV(uint16_t CV, uint8_t data) {
	DIAG(F("notifyXNetDirectCV"));
}
void notifyXNetDirectReadCV(uint16_t cvAdr) {
	DIAG(F("notifyXNetDirectReadCV"));
}

//POM
void notifyXNetPOMwriteByte(uint16_t Adr, uint16_t CV, uint8_t data) {
	DIAG(F("notifyXNetPOMwriteByte"));
}
void notifyXNetPOMwriteBit(uint16_t Adr, uint16_t CV, uint8_t data) {
	DIAG(F("notifyXNetPOMwriteBit"));
}

#if defined (__cplusplus)
}
#endif

/*
<* READ C3 21 21 00 : OK *>     C3 -> Address   21 21 SoftwareVersion Zentral anfordern

<* READ C3 21 24 05 : OK *>		21 24 Status Zentrale anfordern

<* READ C3 E3 F0 00 03 10 : OK *>
<* notifyXNetgiveLocoMM *>

<* READ C3 E3 F0 00 03 10 : OK *>
<* notifyXNetgiveLocoMM *>

<* READ C3 E4 12 00 03 84 71 : OK *>   00 03 -> Addresse, 84 -> 
<* notifyXNetLocoDrive28 *>

<* READ C3 E4 12 00 03 85 70 : OK *>
<* notifyXNetLocoDrive28 *>



<* READ C3 E4 12 00 03 1F EA : OK *>
<* notifyXNetLocoDrive28 *>
<* READ C3 E4 12 00 03 1E EB : OK *>
<* notifyXNetLocoDrive28 *>
<* READ C3 E4 12 00 03 0B FE : OK *>
<* notifyXNetLocoDrive28 *>
<* READ 72 42 03 09 FC : NOT OK *>
<* READ C3 E4 12 00 03 07 F2 : OK *>
<* notifyXNetLocoDrive28 *>
<* READ C3 E4 12 00 03 03 F6 : OK *>
<* notifyXNetLocoDrive28 *>


<* notifyXNetPower *>
<* READ C3 21 80 A1 : OK *>
<* notifyXNetPower *>


* READ C3 E3 F0 00 04 17 : OK *>
<* notifyXNetgiveLocoMM *>
<* READ C3 E3 F0 00 04 17 : OK *>
<* notifyXNetgiveLocoMM *>
<* READ C3 E3 F0 00 05 16 : OK *>
<* notifyXNetgiveLocoMM *>
<* READ C3 E4 20 00 03 00 C7 : OK *>
<* notifyXNetLocoFunc1 *>
<* READ C3 E4 20 00 03 00 C7 : OK *>
<* notifyXNetLocoFunc1 *>
<* READ C3 E3 F0 00 04 17 : OK *>
<* notifyXNetgiveLocoMM *>
<* READ C3 E3 F0 00 04 17 : OK *>
<* notifyXNetgiveLocoMM *>
<* READ C3 E3 F0 00 05 16 : OK *>
<* notifyXNetgiveLocoMM *>
<* READ C3 E4 12 00 05 82 71 : OK *>
<* notifyXNetLocoDrive28 *>
<* READ C3 E4 12 00 05 80 73 : OK *>
<* notifyXNetLocoDrive28 *>
*/


/*NEWEST:
<* notifyXNetLocoDrive28 *>
<l 3 0 133 0>
<* READ C3 E4 12 00 03 8A 7F : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 6 0>
<* READ C3 E4 12 00 03 9A 6F : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 14 0>
<* READ C3 E4 12 00 03 8C 79 : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 4 0>
<* READ C3 E4 12 00 03 9E 6B : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 14 0>
<* READ C3 E4 12 00 03 8F 7A : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 135 0>
<* READ C3 E4 12 00 03 9F 6A : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 143 0>
<* READ C3 E4 12 00 03 8C 79 : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 4 0>
<* READ C3 E4 12 00 03 88 7D : OK *>
<* notifyXNetLocoDrive28 *>
<* READ C3 E4 12 00 03 93 66 : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 139 0>
<* READ C3 E4 12 00 03 82 77 : OK *>
<* notifyXNetLocoDrive28 *>
<l 3 0 2 0>
*/