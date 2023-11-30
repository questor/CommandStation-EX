
#include "XpressNetMaster.h"

#include "DIAG.h"

#if defined (__cplusplus)
	extern "C" {
#endif

void notifyXNetPower(uint8_t State) {
	DIAG(F("notifyXNetPower"));
}
//uint8_t getPowerState() __attribute__((weak));	//give Back Actual Power State

//Fahrbefehl
void notifyXNetgiveLocoInfo(uint8_t UserOps, uint16_t Address) {
	DIAG(F("notifyXNetgiveLocoInfo"));
}
void notifyXNetLocoDrive14(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive14"));
}
void notifyXNetLocoDrive27(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive27"));
}
void notifyXNetLocoDrive28(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive28"));
}
void notifyXNetLocoDrive128(uint16_t Address, uint8_t Speed) {
	DIAG(F("notifyXNetLocoDrive128"));
}

//Funktionsbefehl
void notifyXNetgiveLocoFunc(uint8_t UserOps, uint16_t Address) {
	DIAG(F("notifyXNetgiveLocoFunc"));
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

//MultiMaus
void notifyXNetgiveLocoMM(uint8_t UserOps, uint16_t Address) {
	DIAG(F("notifyXNetgiveLocoMM"));
}

#if defined (__cplusplus)
}
#endif
