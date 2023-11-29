
#if 0

#include "XpressNet.h"

#include "DIAG.h"

#define MAX485_CONTROL_PIN 9    //RS-485/422 Transceiver

/*static*/ void XpressNet::init() {
    DIAG(F("XPRESSNET::init"));
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

// aa cd ee .. ff
// aa - callbyte mit parity (bit 7 ist parity bit)
// c - identification of request
// d - Anzahl Bytes in Message ohne header und crc bytes
//      cd -> Header
// bis zu 15 datenbytes
// ff - Checksumme (aa bb werden nicht mit reingerechnet, aber header; xor alles zusammen)
// ee - message
//  0x24 - Status Zentrale anfordern
//  0x22 - Status Zentrale
//  0x21 - Softwareversion anfordern
//

/* Auszug Anmeldung LokMaus2

Hier ein Auszug der Daten aus der Anmeldung einer LokMaus 2 am XpressNet-Bus:
             x -> Nibble: anzahl bytes
RX: 0x141 0x21 0x24 0x05 - Status Zentrale anfordern
TX: 0x1E1 0x62 0x22 0x00 0x40 - Status Zentrale

RX: 0x141 0x21 0x21 0x00 - Softwareversion anfordern
TX: 0x1E1 0x63 0x21 0x30 0x00 0x72 - Softwareversion Zentrale

RX: 0x141 0xE3 0x00 0x00 0x04 0xE7 - Lokdaten anfordern
TX: 0x1E1 0xE4 0x04 0x00 0x00 0x00 XOR - Normale Lokinfo

*/

enum Msg {
    eCallByte = 0,
    eIdentLen = 1,
    eData = 2,
    eMaxLength = 24
};

enum CallType {
    eResponse = 0x60, //0bP11A AAAA
    eInquiry = 0x40,  //0bP10A AAAA
    eRequestAckFromDevice = 0x00, //0bP00A AAAA
    //eBroadcast is special because it has no address set

    eMask = 0x60
};

static int byteRecWritePos = 0;
const int maxNumberMessagesInBuffer = 8;
static uint8_t tempReceiveBuffer[Msg::eMaxLength*maxNumberMessagesInBuffer];
static int msgWriteIndex = 0;   //race protection needed?
static int msgReadIndex = 0;

void processPendingMessages() {
    while(msgWriteIndex != msgReadIndex) {
        int msgIndex = msgReadIndex*Msg::eMaxLength;
        uint8_t callParityAddress = tempReceiveBuffer[msgIndex + eCallByte];
        //parity bit is bit 7
        uint8_t address = callParityAddress & 0x1f; //filter lowest 5 bits

        uint8_t length = tempReceiveBuffer[msgIndex+eIdentLen] & 0xf;

        if((callParityAddress & 0x7F) == 0x60) {    //Broadcast 0xP110 0000 (only here address is set to zero!)
        }

        DIAG(F("msg %02x %02x %02x"), tempReceiveBuffer[msgIndex+0], tempReceiveBuffer[msgIndex+1], tempReceiveBuffer[msgIndex+2]);

        msgReadIndex = (msgReadIndex + 1) & (maxNumberMessagesInBuffer-1);
    }
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

void receiveMessages() {    //called from the interrupt (no heavy processing!)
    int b = serialReceive();
    if(b == -1) {   // bail out on error
        return;
    }
    //the nineth bit marks if a new message starts or not
    if(b >= 0x100) {
        if(byteRecWritePos > 0) {   //is something already in the buffer which needs to be processed?
            msgWriteIndex += 1;     //write to next message
            msgWriteIndex &= (maxNumberMessagesInBuffer-1);

            if(msgWriteIndex == msgReadIndex) {
                //OVERRUN!
            }
        }
        byteRecWritePos = 0;
    }
    tempReceiveBuffer[msgWriteIndex*Msg::eMaxLength + byteRecWritePos] = b & 0xff;
    byteRecWritePos += 1;
    if(byteRecWritePos >= Msg::eMaxLength) {
        //Hhm? more bytes received than expected?!
    }
}

ISR(USART1_RX_vect) {
    receiveMessages();
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
    while (!(UCSR1A & (1<<TXC1)));
    UCSR1A = (1<<TXC1);
    UCSR1A = 0;
    digitalWrite(MAX485_CONTROL_PIN, LOW);
}


/*static*/ void XpressNet::loop() {
    processPendingMessages();
}


#endif
