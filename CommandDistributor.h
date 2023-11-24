/*
 *  © 2022 Harald Barth
 *  © 2020-2021 Chris Harlow
 *  © 2020 Gregor Baues
 *  © 2022 Colin Murdoch
 * 
 *  All rights reserved.
 *
 *  This file is part of CommandStation-EX
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef CommandDistributor_h
#define CommandDistributor_h
#include "DCCEXParser.h"
#include "RingStream.h"
#include "StringBuffer.h"
#include "defines.h"
#include "EXRAIL2.h"

#if WIFI_ON | ETHERNET_ON 
  // Command Distributor must handle a RingStream of clients
  #define CD_HANDLE_RING
#endif 

class CommandDistributor {
public:
  enum clientType: byte {
    NONE_TYPE,
    COMMAND_TYPE,
    WITHROTTLE_TYPE,
    XPRESSNET_TYPE,
  };
private:
  static void broadcastToClients(clientType type);
  static StringBuffer * broadcastBufferWriter;
  #ifdef CD_HANDLE_RING
    static RingStream * ring;
    static clientType clients[8];
  #endif
public :
  
  enum forceProtocolType: byte{
    eForceNothing = 0,
    eForceXpressNet = 1,
  };

  static void parse(byte clientId,byte* buffer, RingStream * ring, byte forceLenz = eForceNothing);
  static void broadcastLoco(byte slot);
  static void broadcastSensor(int16_t id, bool value);
  static void broadcastTurnout(int16_t id, bool isClosed);
  static void broadcastClockTime(int16_t time, int8_t rate);
  static void setClockTime(int16_t time, int8_t rate, byte opt);
  static int16_t retClockTime();
  static void broadcastPower();
  static void broadcastRaw(clientType type,char * msg);
  static void broadcastTrackState(const FSH* format,byte trackLetter,int16_t dcAddr);
  template<typename... Targs> static void broadcastReply(clientType type, Targs... msg);
  static void forget(byte clientId);
  
};

#endif
