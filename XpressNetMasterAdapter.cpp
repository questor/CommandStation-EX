
#include "XpressNetMaster.h"

#include "DIAG.h"

#if defined (__cplusplus)
	extern "C" {
#endif

void notifyXNetPower(uint8_t State) {
	DIAG(F("notifyXNetPower"));
	switch(State) {
	case csNormal:
		break;
	case csEmergencyStop: break;
	case csTrackVoltageOff: break;
	case csShortCircuit: break;
	case csServiceMode: break;
	default:
		break;
	}
}

//Fahrbefehl
void notifyXNetgiveLocoInfo(uint8_t UserOps, uint16_t Address) {
	DIAG(F("notifyXNetgiveLocoInfo"));

  //XNetReturnLoco |= 0x01;
  //XNetUserOps = UserOps;
  #if defined(DCC) 
  //dcc.getLocoStateFull(Address, false); //request for XpressNet only!
  uint8_t ldata[6];
  dcc.getLocoData(Address, ldata);  //uint8_t Steps[0], uint8_t Speed[1], uint8_t F0[2], uint8_t F1[3], uint8_t F2[4], uint8_t F3[5]
  if (ldata[0] == 0x03)  //128 Steps?
      ldata[0]++;  //set Steps to 0x04
  XpressNet.SetLocoInfo(UserOps, ldata[0], ldata[1], ldata[2], ldata[3]); //UserOps,Steps,Speed,F0,F1
  #endif

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
	//keine Antwort notwendig
}
void notifyXNetLocoDrive128(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive128"));
	//keine Antwort notwendig
}

//MultiMaus
void notifyXNetgiveLocoMM(uint8_t UserOps, uint16_t Address) {
	DIAG(F("notifyXNetgiveLocoMM"));


  //XNetReturnLoco |= 0x04;
  //XNetUserOps = UserOps;
  #if defined(DCC) 
  //dcc.getLocoStateFull(Address, false); //request for XpressNet only!
  uint8_t ldata[6];
  dcc.getLocoData(Address, ldata);  //uint8_t Steps[0], uint8_t Speed[1], uint8_t F0[2], uint8_t F1[3], uint8_t F2[4], uint8_t F3[5]
  if (ldata[0] == 0x03)  //128 Steps?
      ldata[0]++;  //set Steps to 0x04
  XpressNet.SetLocoInfoMM(UserOps, ldata[0], ldata[1], ldata[2], ldata[3], ldata[4], ldata[5]); //Steps,Speed,F0,F1,F2,F3
  #endif
}

//Funktionsbefehl
void notifyXNetgiveLocoFunc(uint8_t UserOps, uint16_t Address) {
	DIAG(F("notifyXNetgiveLocoFunc"));
  //XNetReturnLoco |= 0x02;
  //XNetUserOps = UserOps;
  #if defined(DCC) 
  //dcc.getLocoStateFull(Address, false); //request for XpressNet only!
  XpressNet.SetFktStatus(UserOps, dcc.getFunktion13to20(Address), dcc.getFunktion21to28(Address)); //Fkt4, Fkt5
  #endif

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

<* READ C3 E4 12 00 03 84 71 : OK *>
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
