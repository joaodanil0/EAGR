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

#include "SensorAppLayer.h"

Define_Module(SensorAppLayer);

SensorAppLayer::~SensorAppLayer(){
    cancelAndDelete(timer);
}

void SensorAppLayer::initialize(int stage) {

    cSimpleModule::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {


        startTime = par("startTime");
        sendIntervalPar = par("sendInterval");
        packetLengthPar = par("packetLength");
    }
    else if (stage == INITSTAGE_APPLICATION_LAYER) {

        timer = new cMessage("sendTimer");
        timer->setKind(1);

        xa->addProtocol(protocol->getId(), protocol);
        registerService(*protocol, nullptr, gate("socketIn"));
        registerProtocol(*protocol, gate("socketOut"), nullptr);

        scheduleAt(simTime() + startTime + uniform(0,1), timer);

        const char *destAddrs = par("destAddresses");
        cStringTokenizer tokenizer(destAddrs);
        const char *token;

        while ((token = tokenizer.nextToken()) != nullptr) {

            L3Address result;
            L3AddressResolver().tryResolve(token, result);
            if (result.isUnspecified())
                EV_ERROR << "cannot resolve destination address: " << token << endl;
            else
                destAddresses.push_back(result);
        }

    }
}

void SensorAppLayer::handleMessage(cMessage *msg) {

    if (msg->getKind() == 1) {

        Packet *packet = new Packet("DATA");
        const auto& payload = makeShared<ByteCountChunk>(B(packetLengthPar));
        packet->insertAtBack(payload);

        L3Address destAddr = chooseDestAddr();


        packet->addTag<PacketProtocolTag>()->setProtocol(protocol);
        packet->addTag<DispatchProtocolReq>()->setProtocol(Protocol::getProtocol("SinkNetwork"));
        packet->addTag<L3AddressReq>()->setDestAddress(destAddr);

        EV_INFO << "Sending packet: ";
        send(packet, "socketOut");

        timer = new cMessage("sendTimer");
        timer->setKind(1);
        scheduleAt(simTime() + sendIntervalPar + uniform(0,1), timer);
    }

    delete msg;

}

L3Address SensorAppLayer::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    return destAddresses[k];
}

bool SensorAppLayer::isNodeUp()
{
    return !nodeStatus || nodeStatus->getState() == NodeStatus::UP;
}

bool SensorAppLayer::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();
    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if (static_cast<NodeStartOperation::Stage>(stage) == NodeStartOperation::STAGE_APPLICATION_LAYER){
            startTime = par("startTime");
            scheduleAt(simTime() + sendIntervalPar, timer);
        }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if (static_cast<NodeShutdownOperation::Stage>(stage) == NodeShutdownOperation::STAGE_APPLICATION_LAYER){
            cancelEvent(timer);

        }
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if (static_cast<NodeCrashOperation::Stage>(stage) == NodeCrashOperation::STAGE_CRASH){
            cancelEvent(timer);
        }
    }
    else
        throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName());


    return true;
}
