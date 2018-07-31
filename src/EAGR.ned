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

package src;

import inet.physicallayer.unitdisk.UnitDiskRadioMedium;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import src.nodes.Sink;
import src.nodes.Sensor;

network EAGR {
    
      parameters:
        int numHosts;
        double density;

        double area = numHosts/density;
        double side = sqrt(area);

        @display("bgb=$side,$side;bgi=background/terrain");

    submodules:

        visualizer: IntegratedCanvasVisualizer {
            @display("p=580,125");
        }

        radioMedium: UnitDiskRadioMedium {
            @display("p=580,275");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=580,200");
        }

        sink: Sink {
            parameters:
                *.initialX = (side/2)*1m;
                *.initialY = (side/2)*1m;
                @display("p=149,94");
        }
        
        sensor[numHosts]: Sensor {
            
            parameters:
				*.numHosts = numHosts;
				*.constraintAreaMaxX = side*1m;
				*.constraintAreaMaxY = side*1m;
				*.constraintAreaMinX = 0;
				*.constraintAreaMinY = 0;
            	@display("p=87.02667,155.34666");
        }
}
