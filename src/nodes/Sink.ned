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

package src.nodes;

import inet.node.base.ApplicationLayerNodeBase;
import inet.mobility.static.StationaryMobility;

module Sink extends ApplicationLayerNodeBase {
   
    parameters:         
        
        numWlanInterfaces = default(1);   
        numLoInterfaces = default(0);   
        wlan[*].typename = default("AckingWirelessInterface");
        generic.typename = default("SinkNetwork");
        
        hasUdp = false;
		hasTcp = false;
		hasSctp = false;
		
		hasIpv4 = false;
		hasIpv6 = false;
		hasGn = true;
		
        forwarding = default(true);  // disable routing by default
        //ipv4.proxyARP = default(false);
        //*.routingTableModule = default("^.ipv4.routingTable"); 
        @figure[submodules];
        @display("i=device/antennatower");        
}
