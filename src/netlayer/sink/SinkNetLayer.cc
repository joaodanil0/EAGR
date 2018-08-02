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

#include "SinkNetLayer.h"

Define_Module(SinkNetLayer);

void SinkNetLayer::initialize(int stage) {

    NetworkProtocolBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        headerLength = par("headerLength");
        arp = getModuleFromPar<IArp>(par("arpModule"), this);
        routingMessage = new cMessage("Start Flooding", ROUTING);
        mobility = check_and_cast<IMobility *>(getContainingNode(this)->getSubmodule("mobility"));
    }
    else if (stage == INITSTAGE_NETWORK_LAYER_3) {
        IInterfaceTable *interfaceTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        myNetwAddr = interfaceTable->getInterface(0)->getNetworkAddress();
        scheduleAt(simTime(), routingMessage);

        xa->addProtocol(protocol->getId(), protocol);
    }
}

void SinkNetLayer::handleSelfMessage(cMessage *msg){

    if (msg->getKind() == ROUTING) {
        auto pkt = makeShared<RoutingMessage>();

        pkt->setChunkLength(B(headerLength));
        pkt->setSrcAddr(myNetwAddr);
        pkt->setDestAddr(myNetwAddr.getAddressType()->getBroadcastAddress());

        pkt->setDestPosition(mobility->getCurrentPosition());
        pkt->setNodeEnergy(-1);
        pkt->setNodePosition(mobility->getCurrentPosition());




        auto packet = new Packet("Start Flooding", ROUTING);
        packet->insertAtBack(pkt);
        setDownControlInfo(packet, MacAddress::BROADCAST_ADDRESS);
        sendDown(packet);

    }

    delete msg;

}

void SinkNetLayer::handleLowerPacket(Packet *msg){



    if(msg->getKind() == ROUTING){
        auto netMsg = staticPtrCast<RoutingMessage>(msg->peekAtFront<RoutingMessage>()->dupShared());

       // cout << "CHEGOU ROTEAMENTO: " << netMsg->getSrcAddr() << endl;
    }
    else if(msg->getKind() == DATA){
        auto netMsg = staticPtrCast<RoutingMessage>(msg->peekAtFront<RoutingMessage>()->dupShared());

        if(hasGUI())
               getParentModule()->getParentModule()->bubble("Chegou dados");

        //cout << "CHEGOU DATA: " << netMsg->getSrcAddr() << endl;
    }
    delete msg;
}

void SinkNetLayer::setDownControlInfo(Packet *const pMsg, const MacAddress& pDestAddr){
    pMsg->addTagIfAbsent<MacAddressReq>()->setDestAddress(pDestAddr);
    pMsg->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&getProtocol());
    pMsg->addTagIfAbsent<DispatchProtocolInd>()->setProtocol(&getProtocol());
}

const Protocol& SinkNetLayer::getProtocol() const {

    static const Protocol *aux = Protocol::getProtocol("SinkNetwork");

    return *aux;
}
