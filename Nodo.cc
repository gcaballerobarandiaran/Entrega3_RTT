/*
 * Nodo.cc
 *
 *  Created on: Jan 14, 2021
 *      Author: gcab
 */

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "custom_packet_m.h"

using namespace omnetpp;

class Nodo : public cSimpleModule
{
    private:
        cChannel *channel[2]; // se crean dos canales
        cQueue *queue[2];  // cola para cada canal
        double probability;  // la probailidad definida
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void sendNew(CustomPacket *pkt);
        virtual void sendNext(int gateIndex);
        virtual void sendPacket(CustomPacket *pkt, int gateIndex);
};

Define_Module(Nodo);

void Nodo::initialize() {
    //inicializacion
    // Generador aleatorio de numeros con seed time
      srand(time(NULL));

    channel[0] = gate("outPort", 0) -> getTransmissionChannel();
    channel[1] = gate("outPort", 1) -> getTransmissionChannel();
    // Crear las colas
    queue[0] = new cQueue("queueZero");
    queue[1] = new cQueue("queueOne");
    // Consigue el parametro probabilidad definido en el ned
    probability = (double) par("probability");
}

void Nodo::handleMessage(cMessage *msg)
{
    CustomPacket *pkt = check_and_cast<CustomPacket *> (msg);
    //de que puerta ha llegado
    cGate *arrivalGate = pkt -> getArrivalGate();
    int arrivalGateIndex = arrivalGate -> getIndex();
    EV << "Llegada por gate " + std::to_string(arrivalGateIndex) + "\n";

    //Comprobar si el paquete llega desde la fuente si el campo fromsource esta en true
    if (pkt -> getFromSource()) {
        EV << "Llega desde la fuente\n";
        pkt -> setFromSource(false);
        //lo ruta
        sendNew(pkt);
        return;
    }
    if (pkt -> getKind() == 1) { // Comprobar si el paquete es erroneo
        if (pkt -> hasBitError()) {
            EV << "Paquete erroneo, mandando NAK\n";
            CustomPacket *nak = new CustomPacket("NAK");
            nak -> setKind(3); //tipo de pquete 3 -> NACK
            send(nak, "outPort", arrivalGateIndex);
        }
        else {
            EV << "Paquete correcto, mandando ACK\n";
            CustomPacket *ack = new CustomPacket("ACK");
            ack -> setKind(2); //tipo de paquete 2 ACK
            send(ack, "outPort", arrivalGateIndex);
            sendNew(pkt);
        }
    }
    else if (pkt -> getKind() == 2) { // paquetes tipo 2 -> ACK
        EV << "Ack recivido\n";
        if (queue[arrivalGateIndex] -> isEmpty())
            EV << "Ha llegado un ACK sin paquetes en la cola\n";
        else {

            queue[arrivalGateIndex] -> pop(); //pop hace que se borre el primer paquete de la cola
            sendNext(arrivalGateIndex);
        }
    }
    else { // Paquetes de tipo 3 -> NAK
        EV << "NAK recivido\n";
        sendNext(arrivalGateIndex);
    }
}

void Nodo::sendNew(CustomPacket *pkt) {
    int gateIndex;
    double randomNumber = ((double) rand() / (RAND_MAX));
    if (randomNumber < probability)
        gateIndex = 0;
    else
        gateIndex = 1;

    if (queue[gateIndex] -> isEmpty()) {
        EV << "Cola vacia, se envia (y inserta en la cola)\n";
        // Se inserta el paquete en la cola por si llega un Nak y tiene k volver a mandarse
        queue[gateIndex] -> insert(pkt);
        sendPacket(pkt, gateIndex);
    } else {
        EV << "Cola llena, se añade\n";
        queue[gateIndex] -> insert(pkt);
    }
}

void Nodo::sendNext(int gateIndex) {
    if (queue[gateIndex] -> isEmpty())
        EV << "No hay paquetes en la cola\n";
    else {
        CustomPacket *pkt = check_and_cast<CustomPacket *> (queue[gateIndex] -> front());// coje el paquete sin eliminarlo de la cola
        sendPacket(pkt, gateIndex);
    }
}

void Nodo::sendPacket(CustomPacket *pkt, int gateIndex) {
    if (channel[gateIndex] -> isBusy()) {
        EV << "Error\n";
    } else {
        CustomPacket *newPkt = check_and_cast<CustomPacket *> (pkt -> dup());
        send(newPkt, "outPort", gateIndex);
    }
}
