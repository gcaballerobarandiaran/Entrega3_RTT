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
simple Nodo
{
    parameters:
        @display("i=block/switch");
        double probability;
    gates:
        input inPort[];
        output outPort[];
}
simple End
{
    parameters:
        @display("i=block/sink");
    gates:
        input inPort[];
        output outPort[];
}


simple Fuente
{
    parameters:
        @display("i=block/source");
        double meanPacketLength;
    gates:
        output outPort;
}

network StopAndWait
{
    @display("bgb=484.52997,225.98999");
    types:
        channel Channel extends ned.DatarateChannel
        {
            datarate = 1000bps;
            delay = 100us;
            per = 0.25;
        }
    submodules:




        fuente1: Fuente {
            @display("p=25.109999,78.119995");
            meanPacketLength = 1000/3;
        }
        fuente2: Fuente {
            @display("p=25.109999,21.39");
            meanPacketLength = 1000/3;
        }
        fuente3: Fuente {
            @display("p=25.109999,142.29");
            meanPacketLength = 1000/3;
        }
        nodo1: Nodo {
            @display("p=112.53,78.119995");
            probability = 1/4;
            gates:
                inPort[3];
                outPort[2];
        }
        nodo2: Nodo {
            @display("p=194.37,21.39");
            probability = 1/3;
            gates:
                inPort[5];
                outPort[4];
        }
        nodo3: Nodo {
            @display("p=305.03998,21.39");
            probability = 0.01;
            gates:
                inPort[2];
                outPort[2];
        }
        nodo4: Nodo {
            @display("p=305.03998,142.29");
            probability = 0.99;
            gates:
                inPort[3];
                outPort[3];
        }

        nodo5: Nodo {
            @display("p=194.37,142.29");
            probability = 1/2;
            gates:
                inPort[4];
                outPort[3];
        }
        end1: End {
            @display("p=435.24,21.39");
            gates:
                inPort[1];
                outPort[1];
        }
        end2: End {
            @display("p=435.24,142.29");
            gates:
                inPort[1];
                outPort[1];
        }
    connections:

        fuente1.outPort --> nodo1.inPort[2];
        fuente2.outPort --> nodo2.inPort[4];
        fuente3.outPort --> nodo5.inPort[3];

        nodo1.outPort[0] --> Channel --> nodo2.inPort[2];
        nodo1.inPort[0] <-- Channel <-- nodo2.outPort[2];

        nodo1.outPort[1] --> Channel --> nodo5.inPort[2];
        nodo1.inPort[1] <-- Channel <-- nodo5.outPort[2];

        nodo2.outPort[0] --> Channel --> nodo3.inPort[0];
        nodo2.inPort[0] <-- Channel <-- nodo3.outPort[0];

        nodo2.outPort[1] --> Channel --> nodo4.inPort[2];
        nodo2.inPort[1] <-- Channel <-- nodo4.outPort[2];

        nodo5.outPort[0] --> Channel --> nodo2.inPort[3];
        nodo5.inPort[0] <-- Channel <-- nodo2.outPort[3];

        nodo5.outPort[1] --> Channel --> nodo4.inPort[1];
        nodo5.inPort[1] <-- Channel <-- nodo4.outPort[1];

        nodo3.outPort[1] --> Channel --> end1.inPort[0];
        nodo3.inPort[1] <-- Channel <-- end1.outPort[0];

        nodo4.outPort[0] --> Channel --> end2.inPort[0];
        nodo4.inPort[0] <-- Channel <-- end2.outPort[0];

}
