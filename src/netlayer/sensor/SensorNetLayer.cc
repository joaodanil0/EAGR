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

#include "SensorNetLayer.h"

Define_Module(SensorNetLayer);

void SensorNetLayer::initialize(int stage) {

    NetworkProtocolBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        headerLength = par("headerLength");
        arp = getModuleFromPar<IArp>(par("arpModule"), this);
        mobility = check_and_cast<IMobility *>(getContainingNode(this)->getSubmodule("mobility"));
        energyStorage = check_and_cast<IEpEnergyStorage *>(findContainingNode(this)->getSubmodule("energyStorage"));
    }
    else if (stage == INITSTAGE_NETWORK_LAYER_3) {
        IInterfaceTable *interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        myNetworkAddress = interfaceTable->getInterface(0)->getNetworkAddress();
    }
}

void SensorNetLayer::handleSelfMessage(cMessage *msg){

    if(msg->getKind() == ROUTING){

        auto pkt = makeShared<RoutingMessage>();

        pkt->setChunkLength(B(headerLength));

        pkt->setSrcAddr(myNetworkAddress);
        pkt->setDestAddr(myNetworkAddress.getAddressType()->getBroadcastAddress());

        pkt->setSinkPosition(sinkPosition);
        pkt->setSinkDistance(sinkDistance);
        pkt->setNodeEnergy(energyStorage->getResidualEnergyCapacity().get());

        auto packet = new Packet("Forward Flooding", ROUTING);
        packet->insertAtBack(pkt);
        setDownControlInfo(packet, MacAddress::BROADCAST_ADDRESS);
        sendDown(packet);

    }

    delete msg;
}

void SensorNetLayer::handleLowerPacket(Packet *packet){

    if(packet->getKind() == ROUTING){
        auto netMsg = staticPtrCast<RoutingMessage>(packet->peekAtFront<RoutingMessage>()->dupShared());

        if(!isConfigured){
            isConfigured = true;

            sinkPosition = netMsg->getSinkPosition();
            sinkDistance = mobility->getCurrentPosition().distance(sinkPosition);
            message = new cMessage("Forward Flooding", ROUTING);
            scheduleAt(simTime() + truncnormal(0,0.1), message);
        }

        double sinkDistanceNeighbor = netMsg->getSinkDistance();
        double neighborEnergy = netMsg->getNodeEnergy();
        L3Address neighborAddr = netMsg->getSrcAddr();

        neighborsTable.insert(make_pair(sinkDistanceNeighbor, make_pair(neighborEnergy, neighborAddr)));

        delete packet;
    }
    else if(packet->getKind() == DATA){

        auto netMsg = staticPtrCast<RoutingMessage>(packet->peekAtFront<RoutingMessage>()->dupShared());

        if(netMsg->getDestAddr() == myNetworkAddress){
            L3Address neighborsAddr = getDest();

            netMsg->setSrcAddr(myNetworkAddress);
            netMsg->setDestAddr(neighborsAddr);
            netMsg->setNodeEnergy(energyStorage->getResidualEnergyCapacity().get());

            auto p = new Packet(packet->getName(), packet->getKind());
            packet->popAtFront<RoutingMessage>();
            p->insertAtBack(packet->peekDataAt(b(0), packet->getDataLength()));
            p->insertAtFront(netMsg);

            setDownControlInfo(p, MacAddress::BROADCAST_ADDRESS);
            sendDown(p);
        }

        updateNeighbor(netMsg->getSrcAddr(), netMsg->getNodeEnergy());

        delete packet;
    }


}

void SensorNetLayer::handleUpperPacket(Packet *msg){

    auto pkt = makeShared<RoutingMessage>();

    L3Address neighborsAddr = getDest();

    pkt->setChunkLength(B(headerLength));

    pkt->setSrcAddr(myNetworkAddress);
    pkt->setDestAddr(neighborsAddr);
    pkt->setNodeEnergy(energyStorage->getResidualEnergyCapacity().get());
    pkt->setSensorAddr(myNetworkAddress);

    msg->insertAtFront(pkt);
    msg->setKind(DATA);

    setDownControlInfo(msg, MacAddress::BROADCAST_ADDRESS);

    sendDown(msg);
}

void SensorNetLayer::setDownControlInfo(Packet *const pMsg, const MacAddress& pDestAddr){
    pMsg->addTagIfAbsent<MacAddressReq>()->setDestAddress(pDestAddr);
    pMsg->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&getProtocol());
    pMsg->addTagIfAbsent<DispatchProtocolInd>()->setProtocol(&getProtocol());
}

const Protocol& SensorNetLayer::getProtocol() const {

    static const Protocol *aux = Protocol::getProtocol("SinkNetwork");

    return *aux;
}

L3Address SensorNetLayer::getDest(){

    double energyThreshold = 0.5;
    double auxEnergy = 0;
    L3Address addr;


    for(auto it : neighborsTable){

        if((it.first <= sinkDistance) && (it.second.first > auxEnergy) && (it.second.first > energyThreshold)){
            auxEnergy = it.second.first;
            addr = it.second.second;
        }
    }


    return neighborsTable.begin()->second.second;
}

void SensorNetLayer::updateNeighbor(L3Address addr, double energy){

    for(auto it : neighborsTable){

        if(it.second.second == addr)
            (*neighborsTable.find(it.first)).second.first = energy;
    }
}


