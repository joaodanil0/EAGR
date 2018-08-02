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

#ifndef __GREEDYFORWARD_5_4_1_SENSORNETLAYER_H_
#define __GREEDYFORWARD_5_4_1_SENSORNETLAYER_H_

#include "inet/networklayer/base/NetworkProtocolBase.h"
#include "inet/networklayer/contract/INetworkProtocol.h"
#include "inet/networklayer/contract/IArp.h"
#include "inet/mobility/contract/IMobility.h"
#include "src/messages/RoutingMessage_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/common/ProtocolGroup.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/power/contract/IEpEnergyStorage.h"

#include <functional>
#include <sstream>
#include <string>

using namespace inet;
using namespace power;
using std::map;
using std::make_pair;
using std::string;
using std::function;
using std::pair;
using std::set;
using std::cout;

class SensorNetLayer : public NetworkProtocolBase, public INetworkProtocol {

private:
    SensorNetLayer(const SensorNetLayer&);
    SensorNetLayer& operator=(const SensorNetLayer&);
    typedef map<Coord, double> neighborsTable_t;
    void neighbors(Coord position, double energy);
    L3Address getDest();
protected:

public:
    SensorNetLayer() {}
    virtual void initialize(int stage) override;
    virtual void handleSelfMessage(cMessage *msg) override;
    virtual void handleLowerPacket(Packet *packet) override;
    virtual void handleUpperPacket(Packet *msg) override;
    const Protocol& getProtocol() const override;
    virtual void setDownControlInfo(Packet *const pMsg, const MacAddress& pDestAddr);

private:
    IArp *arp;
    IMobility *mobility;
    int headerLength = 0;
    cMessage *routingMessage;
    Coord destPosition;

    L3Address myNetworkAddress;
    L3Address sinkAddress;
    MacAddress myMacAddress;
    bool isConfigured = false;
    double sinkDistance;
    neighborsTable_t neighborsTable;

    enum messagesTypes {
        ROUTING,
        DATA
    };

    IEpEnergyStorage *energyStorage = nullptr;

protected:
public:
};

#endif
