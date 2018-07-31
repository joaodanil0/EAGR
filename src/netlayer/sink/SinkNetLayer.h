//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __GREEDYFORWARD_5_4_1_SINKNETLAYER_H_
#define __GREEDYFORWARD_5_4_1_SINKNETLAYER_H_

#include "inet/networklayer/base/NetworkProtocolBase.h"
#include "inet/networklayer/contract/INetworkProtocol.h"
#include "inet/networklayer/contract/IArp.h"
#include "inet/mobility/contract/IMobility.h"
#include "src/messages/RoutingMessage_m.h"




#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/common/ProtocolGroup.h"
#include "inet/applications/pingapp/PingApp.h"

using namespace inet;
using std::cout;

class SinkNetLayer : public NetworkProtocolBase, public INetworkProtocol {

private:
    SinkNetLayer(const SinkNetLayer&);
    SinkNetLayer& operator=(const SinkNetLayer&);
protected:

public:
    SinkNetLayer() {}
    virtual void initialize(int stage) override;
    virtual void handleSelfMessage(cMessage *msg) override;
    virtual void handleLowerPacket(Packet *msg) override;
    const Protocol& getProtocol() const override;
    virtual void setDownControlInfo(Packet *const pMsg, const MacAddress& pDestAddr);

private:
    IArp *arp = nullptr;
    cMessage *routingMessage;
    IMobility *mobility;
    Coord sinkPosition;
    L3Address myNetwAddr;
    int headerLength = 0;

    Protocol *protocol = new Protocol("SinkNetwork", "SinkNetwork", Protocol::NetworkLayer);
    ProtocolGroup *xa = &ProtocolGroup::ethertype;


    enum messageType {
        ROUTING,
        DATA
    };

protected:
public:
};

#endif
