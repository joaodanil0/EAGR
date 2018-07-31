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

package src.netlayer.sink;

import inet.networklayer.contract.INetworkLayer;
import inet.networklayer.arp.ipv4.GlobalArp;
import inet.networklayer.wiseroute.WiseRoute;
import inet.networklayer.common.EchoProtocol;
import inet.common.MessageDispatcher;
import src.netlayer.sink.SinkNetLayer;

module SinkNetwork like INetworkLayer {
    
    parameters:
        string interfaceTableModule;
        *.interfaceTableModule = default(absPath(interfaceTableModule));
        @display("i=block/fork");

    gates:
        input ifIn @labels(INetworkHeader);
        output ifOut @labels(INetworkHeader);
        input transportIn @labels(ITransportPacket/down);
        output transportOut @labels(ITransportPacket/up);

    submodules:
        
        up: MessageDispatcher {
            parameters:
                @display("p=300,200;b=480,5");
        }

        arp: GlobalArp {
            parameters:
                @display("p=230,300");
        }
        np: SinkNetLayer {
            parameters:
                arpModule = "^.arp";
                @display("p=300,300;q=queue");
        }

        lp: MessageDispatcher {
            parameters:
                @display("p=300,400;b=480,5");
        }
    connections allowunconnected:
        up.out++ --> { @display("m=n"); } --> transportOut;
        up.in++ <-- { @display("m=n"); } <-- transportIn;

        np.transportOut --> up.in++;
        np.transportIn <-- up.out++;

       
        lp.out++ --> arp.ifIn if exists(arp);
        lp.in++ <-- arp.ifOut if exists(arp);

        lp.out++ --> np.queueIn;
        lp.in++ <-- np.queueOut;

        lp.in++ <-- { @display("m=s"); } <-- ifIn;
        lp.out++ --> { @display("m=s"); } --> ifOut;
}
