/*
 * Fuente.cc
 *
 *  Created on: Jan 14, 2021
 *      Author: gcab
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <random>
#include "custom_packet_m.h"

using namespace omnetpp;

class Fuente : public cSimpleModule
{
    private:
        double longmedia;
        int m = 100;
        int sequenceNumber = 0;
        double landa = 2;
    protected:
        virtual void initialize() override;
        virtual std::vector<double> getDepartures(double landa, int m);
        virtual std::vector<double> getLengths(double mu, int m);
        virtual CustomPacket* getPacket();
        virtual void handleMessage(cMessage *msg) override;
};
Define_Module(Fuente);

void Fuente::initialize() {
    //inicializacion
    //coge la longitud media desde el paquete
    longmedia = (double) par("meanPacketLength");

    // Conseguir salidas y longitudes

    std::vector<double> salidas = getDepartures(landa, m);
    std::vector<double> lengths = getLengths(longmedia, m);
    for(int i = 0; i < salidas.size(); i++) {
        // Scheduled packets will arrived to the same module at
        // departures[i]
        CustomPacket *pkt = getPacket();
        pkt -> setBitLength(lengths[i]);
        scheduleAt(salidas[i], pkt);
    }
}


std::vector<double> Fuente::getDepartures(double landa, int m) {
    //genera distribucion exponencial de longitud m y tasa landa
    std::uniform_real_distribution<double> randomReal(0.0, 1.0);
    std::default_random_engine generator(1);//time(NULL));
    std::vector<double> departures(m);
    for(int i = 0; i < departures.size(); i++) {
        double randomNumber = randomReal(generator);
        double number = (-1/landa) * log(randomNumber);
        if (i != 0)
            departures[i] = departures[i - 1] + number;
        else
            departures[i] = number;
    }
    return departures;
}

std::vector<double> Fuente::getLengths(double meanPacketLength, int samples) {
    //genera distribucion exponencial de las longitudes
    std::uniform_real_distribution<double> randomReal(0.0, 1.0);
    std::default_random_engine generator(time(NULL));
    std::vector<double> lengths(samples);
    for(int i = 0; i < lengths.size(); i++) {
        double randomNumber = randomReal(generator);
        double number = (-meanPacketLength) * log(randomNumber);
        lengths[i] = number;
    }
    return lengths;
}

CustomPacket* Fuente::getPacket() {
    std::string packetName = "paquete::" + std::to_string(getId()) + "::" + std::to_string(sequenceNumber);
    char *charPacketName = new char[packetName.length() + 1];
    strcpy(charPacketName, packetName.c_str());
    CustomPacket *pkt = new CustomPacket(charPacketName);
    pkt -> setFromSource(true);
    pkt -> setKind(1);
    pkt -> setSequenceNumber(sequenceNumber);
    pkt -> setOrigin(getId());
    sequenceNumber++;
    return pkt;
}

void Fuente::handleMessage(cMessage *msg) {
    // Send scheduled packet
    CustomPacket *pkt = check_and_cast<CustomPacket *> (msg);
    send(pkt, "outPort");
}
