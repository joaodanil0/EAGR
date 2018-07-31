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

#include "../mobility/Grid.h"

Define_Module(Grid);

void Grid::setInitialPosition(){
    int numHosts = par("numHosts");
    double marginX = par("marginX");
    double marginY = par("marginY");
    double separationX = par("separationX");
    double separationY = par("separationY");
    int columns = par("columns");
    int rows = par("rows");
    if (numHosts > rows * columns)
        throw cRuntimeError("parameter error: numHosts > rows * columns");

    int index = subjectModule->getIndex();

    int row = index / columns;
    int col = index % columns;
    lastPosition.x = constraintAreaMin.x + marginX + (col + 0.5) * separationX;
    lastPosition.x += normal(0,1);

    if(lastPosition.x > constraintAreaMax.x)
        lastPosition.x = constraintAreaMax.x;
    if(lastPosition.x < 0)
        lastPosition.x = 0;

    lastPosition.y = constraintAreaMin.y + marginY + (row + 0.5) * separationY;
    lastPosition.y += normal(0,1);

    if(lastPosition.y > constraintAreaMax.y)
        lastPosition.y = constraintAreaMax.y;
    if(lastPosition.y < 0)
        lastPosition.y = 0;

    lastPosition.z = par("initialZ");
    recordScalar("x", lastPosition.x);
    recordScalar("y", lastPosition.y);
    recordScalar("z", lastPosition.z);
}
