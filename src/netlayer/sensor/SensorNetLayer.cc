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
        pkt->setDestPosition(destPosition);
        pkt->setNodePosition(mobility->getCurrentPosition());
        pkt->setNodeEnergy(energyStorage->getResidualEnergyCapacity().get());

        pkt->setSrcAddr(myNetworkAddress);
        pkt->setDestAddr(myNetworkAddress.getAddressType()->getBroadcastAddress());

        auto packet = new Packet("Forward Flooding", ROUTING);
        packet->insertAtBack(pkt);
        setDownControlInfo(packet, MacAddress::BROADCAST_ADDRESS);
        sendDown(packet);

    }
    else if(msg->getKind() == DATA){

        auto pkt = makeShared<RoutingMessage>();

        L3Address neighborsAddr = getDest();
        //MacAddress neighborsMACAddr = arp->resolveL3Address(neighborsAddr, nullptr);

        pkt->setChunkLength(B(headerLength));
        pkt->setDestPosition(destPosition);
        pkt->setNodeEnergy(energyStorage->getResidualEnergyCapacity().get());
        pkt->setDestAddr(neighborsAddr);


        dataMsg->insertAtFront(pkt);
        dataMsg->setKind(DATAFORWARD);
        setDownControlInfo(dataMsg, MacAddress::BROADCAST_ADDRESS);
        sendDown(dataMsg);


    }
    else if(msg->getKind() == DATAFORWARD){

        auto pkt = makeShared<RoutingMessage>();

        L3Address neighborsAddr = getDest();
    }

    delete msg;
}

void SensorNetLayer::handleLowerPacket(Packet *packet){

    if(packet->getKind() == ROUTING){
        auto netMsg = staticPtrCast<RoutingMessage>(packet->peekAtFront<RoutingMessage>()->dupShared());
        neighborsData_t neighborsData;

        if(!isConfigured){
            isConfigured = true;

            destPosition = netMsg->getDestPosition();

            message = new cMessage("Forward Flooding", ROUTING);
            scheduleAt(simTime() + truncnormal(0,0.1), message);
        }

        neighborsData.energy = netMsg->getNodeEnergy();
        neighborsData.destinationDistance = netMsg->getNodePosition().distance(destPosition);

        neighborsTable.insert(make_pair(netMsg->getSrcAddr(), neighborsData));

        delete packet;
    }
    else if(packet->getKind() == DATAFORWARD){

        message = new cMessage("Send Data", DATAFORWARD);
        scheduleAt(simTime() + truncnormal(0,0.1), message);

        dataMsg = packet;

//        auto netMsg = staticPtrCast<RoutingMessage>(packet->peekAtFront<RoutingMessage>()->dupShared());
//
//        Coord neighborsAddr = getDest();
//        MacAddress neighborsMacAddr = arp->resolveL3Address(neighborsAddr, nullptr);
//        const cObject* pCtrlInfo = nullptr;
//
//
//        netMsg->setDestAddr(neighborsAddr);
//
//        pCtrlInfo = packet->removeControlInfo();
//
//        auto p = new Packet(packet->getName(), packet->getKind());
//        packet->popAtFront<RoutingMessage>();
//        p->insertAtBack(packet->peekDataAt(b(0), packet->getDataLength()));
//        p->insertAtFront(netMsg);
//        setDownControlInfo(p, neighborsMacAddr);
//        sendDown(p);
//
//        cout << energyStorage->getResidualEnergyCapacity().get() << endl;
//
        delete packet;
    }


}

void SensorNetLayer::handleUpperPacket(Packet *msg){

    message = new cMessage("Send Data", DATA);
    scheduleAt(simTime() + truncnormal(0,0.1), message);

    dataMsg = msg;
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
    double auxDistance = 99999999;
    double auxEnergy = 0;
    L3Address addr;

    for(auto neighbor : neighborsTable) {

        if((neighbor.second.destinationDistance < auxDistance) && (neighbor.second.energy > auxEnergy) && (neighbor.second.energy > energyThreshold)){
            auxDistance = neighbor.second.destinationDistance;
            auxEnergy = neighbor.second.energy;
            addr = neighbor.first;
        }
    }

    return addr;
}
