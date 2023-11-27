
#include "XpressNet.h"

#include "DIAG.h"

#define MAX485_CONTROL_PIN 9    //RS-485/422 Transceiver

/*static*/ void XpressNet::init() {

    pinMode(MAX485_CONTROL_PIN, OUTPUT);
    digitalWrite(MAX485_CONTROL_PIN, LOW);

    //setup serial port 1
    cli();
    UBRR1H = 0;
    UBRR1L = 0x0F;
    UCSR1A = 0;
    //enable reception(RXEN), enable transmission(TXEN), enable receive Int(RXCIE)
    //set 8 bit character mode (UCSZ00, UCSZ01, UCSZ02)
    UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1) | (1<<UCSZ12);
    UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);
    sei();
}

enum Msg {
    eMaxLength = 10
};

static int currentWritePos = 0;
const int maxNumberMessagesInBuffer = 8;
static uint8_t tempReceiveBuffer[Msg::eMaxLength*maxNumberMessagesInBuffer];
static int currentMsgWriteIndex = 0;
static int currentMsgReadIndex = 0;

void receiveMessages() {    //called from the interrupt (no heavy processing!)
    int b = serialReceive();
    if(b == -1) {   // bail out on error
        return;
    }
    //the nineth bit marks if a new message starts or not
    if(b >= 0x100) {
        if(currentWritePos > 0) {   //is something already in the buffer which needs to be processed?

        }
        currentWritePos = 0;

    }
}

ISR(USART1_RX_vect) {
    receiveMessages();
}

int serialReceive() {
    unsigned char status, resHigh, resLow;
    status = UCSR1A;
    while(!(status&(1<<RXC1))) {
        return -1;
    }

    //get status and 9th bit, then data
    resHigh = UCSR1B;
    resLow = UDR1;

    if(status&((1<<FE1) | (1<<DOR1)|(1<<UPE1))) {
        return -1;
    }
    resHigh = (resHigh >> 1) & 0x01;    //filter on 9th bit
    return ((resHigh << 8) | resLow);
}

void serialSend(unsigned char b) {
    while(!(UCSR1A&(1<<UDRE1))) {
    }
    UDR1 = b;
}

void sendMessage(uint8_t count, uint8_t *bytes) {
    digitalWrite(MAX485_CONTROL_PIN, HIGH);
    for(int i=0; i<count; ++i) {
        serialSend(bytes[i]);
    }
    WAIT_FOR_XMIT_COMPLETE; //AVR?
    digitalWrite(MAX485_CONTROL_PIN, LOW);
}

/*static*/ void XpressNet::loop() {

}

/*
Auszug Anmeldung LokMaus2

Hier ein Auszug der Daten aus der Anmeldung einer LokMaus 2 am XpressNet-Bus:
RX: 0x141 0x21 0x24 0x05 - Status Zentrale anfordern
TX: 0x1E1 0x62 0x22 0x00 0x40 - Status Zentrale
RX: 0x141 0x21 0x21 0x00 - Softwareversion anfordern
TX: 0x1E1 0x63 0x21 0x30 0x00 0x72 - Softwareversion Zentrale
RX: 0x141 0xE3 0x00 0x00 0x04 0xE7 - Lokdaten anfordern
TX: 0x1E1 0xE4 0x04 0x00 0x00 0x00 XOR - Normale Lokinfo

*/