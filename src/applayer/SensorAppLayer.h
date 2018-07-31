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

#ifndef __GREEDYFORWARD_SENSORAPPLAYER_H_
#define __GREEDYFORWARD_SENSORAPPLAYER_H_

#include "inet/common/INETDefs.h"
#include "inet/common/Protocol.h"
#include "inet/common/ProtocolGroup.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/lifecycle/NodeStatus.h"

using namespace inet;

class SensorAppLayer : public cSimpleModule, public ILifecycle {

  private:
    L3Address chooseDestAddr();
    bool isNodeUp();
  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override;
  public:
    ~SensorAppLayer();

  private:
    double startTime;
    double sendIntervalPar;
    int packetLengthPar;
    cMessage *timer = nullptr;
    Protocol *protocol = new Protocol("SensorAppLayer", "sensorAppLayer", Protocol::UnspecifiedLayer);
    ProtocolGroup *xa = &ProtocolGroup::ipprotocol;
    std::vector<L3Address> destAddresses;
    NodeStatus *nodeStatus = nullptr;
    bool isOperational = false;
};

#endif
