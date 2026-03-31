/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/vlc-channel-helper.h"
#include "ns3/vlc-device-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/log.h"
#include "ns3/vlc-mac.h"  // Inclua o header do seu MAC customizado

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("VisibleLightCommunication");

// ==================== PARÂMETROS DA SIMULAÇÃO (ajuste aqui) ====================
double simTime = 10.0;               // Tempo total de simulação (segundos)
double distance = 2.0;               // Distância entre os nós (metros)
uint32_t packetSize = 1024;          // Tamanho do pacote (bytes)
Time packetInterval = Seconds(0.05); // Intervalo entre envios de pacotes
double PhotoDetectorArea = 1.3e-5;   // Área do foto detector
double BandFactorNoiseSignal = 10.0; // Fator de ruído
// ===============================================================================

std::vector<double> Received(1, 0);
std::vector<double> theTime(1, 0);

static void RxEnd(Ptr<const Packet> p) { // used for tracing and calculating throughput
    Received.push_back(Received.back() + p->GetSize());
    theTime.push_back(Simulator::Now().GetSeconds());
}

static void TxEnd(Ptr<const Packet> p) { // also used as a trace and for calculating throughput
    Received.push_back(Received.back() + p->GetSize());
    theTime.push_back(Simulator::Now().GetSeconds());
}

// Aplicação customizada para enviar pacotes via MAC
class CustomVlcSender : public Application {
public:
    CustomVlcSender(Ptr<VlcMac> mac, Mac48Address dest, uint32_t pktSize, Time interval);
    virtual ~CustomVlcSender() {}

    void StartApplication() {
        Simulator::Schedule(Seconds(1.0), &CustomVlcSender::Send, this);
    }

private:
    void Send() {
        Ptr<Packet> p = Create<Packet>(m_pktSize);
        m_mac->Enqueue(p, m_dest);
        Simulator::Schedule(m_interval, &CustomVlcSender::Send, this);
    }

    Ptr<VlcMac> m_mac;
    Mac48Address m_dest;
    uint32_t m_pktSize;
    Time m_interval;
};

CustomVlcSender::CustomVlcSender(Ptr<VlcMac> mac, Mac48Address dest, uint32_t pktSize, Time interval)
    : m_mac(mac), m_dest(dest), m_pktSize(pktSize), m_interval(interval) {}

int main(int argc, char *argv[]) {
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // 1. Cria os dois nós
    NodeContainer nodes;
    nodes.Create(2);

    // 2. Instala mobilidade (obrigatória para o modelo VLC)
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    // Posições exemplo: nó 0 em (0,0,2.5), nó 1 em (distance,0,2.5)
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 2.5));          // Nó 0 (transmissor principal)
    positionAlloc->Add(Vector(distance, 0.0, 2.5));     // Nó 1 (receptor principal)
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(nodes);

    // 3. Cria helpers para dispositivos e canais
    VlcDeviceHelper devHelperVPPM;  // VLC Device Helper para gerenciar TX e RX
    VlcChannelHelper chHelper;      // VLC Channel Helper para gerenciar canais

    // 4. Cria transmissores e receptores para bidirecionalidade
    // Transmissor e Receptor para Nó 0
    devHelperVPPM.CreateTransmitter("TX0");  // TX principal do Nó 0
    // ---------------- Configurações específicas do TX0 (ajuste aqui seus parâmetros reais) ----------------
    devHelperVPPM.SetTXSignal("TX0", 1000, 0.5, 0, 9.25e-5, 0);
    devHelperVPPM.SetTrasmitterParameter("TX0", "Bias", 0);
    devHelperVPPM.SetTrasmitterParameter("TX0", "SemiAngle", 35);
    devHelperVPPM.SetTrasmitterParameter("TX0", "Azimuth", 0);
    devHelperVPPM.SetTrasmitterParameter("TX0", "Elevation", 180.0);
    devHelperVPPM.SetTrasmitterPosition("TX0", 0.0, 0.0, 2.5);  // Ajuste posição se necessário
    devHelperVPPM.SetTrasmitterParameter("TX0", "Gain", 70);
    devHelperVPPM.SetTrasmitterParameter("TX0", "DataRateInMBPS", 0.3);
    // -----------------------------------------------------------------------------------------------

    devHelperVPPM.CreateReceiver("RX0");  // RX para ACKs no Nó 0
    // ---------------- Configurações específicas do RX0 (ajuste aqui) ----------------
    devHelperVPPM.SetReceiverParameter("RX0", "FilterGain", 1);
    devHelperVPPM.SetReceiverParameter("RX0", "RefractiveIndex", 1.5);
    devHelperVPPM.SetReceiverParameter("RX0", "FOVAngle", 28.5);
    devHelperVPPM.SetReceiverParameter("RX0", "ConcentrationGain", 0);
    devHelperVPPM.SetReceiverParameter("RX0", "PhotoDetectorArea", PhotoDetectorArea);
    devHelperVPPM.SetReceiverParameter("RX0", "RXGain", 0);
    devHelperVPPM.SetReceiverParameter("RX0", "Beta", 1);
    // Modulação: Ajuste esquema (ex.: VlcErrorModel::VPPM)
    // devHelperVPPM.SetReceiverParameter("RX0", "SetModulationScheme", VlcErrorModel::VPPM);
    // --------------------------------------------------------------------------------

    // Transmissor e Receptor para Nó 1
    devHelperVPPM.CreateTransmitter("TX1");  // TX para ACKs no Nó 1
    // Configs semelhantes ao TX0 (ajuste se uplink for diferente)
    devHelperVPPM.SetTXSignal("TX1", 1000, 0.5, 0, 9.25e-5, 0);
    devHelperVPPM.SetTrasmitterParameter("TX1", "Bias", 0);
    devHelperVPPM.SetTrasmitterParameter("TX1", "SemiAngle", 35);
    devHelperVPPM.SetTrasmitterParameter("TX1", "Azimuth", 0);
    devHelperVPPM.SetTrasmitterParameter("TX1", "Elevation", 180.0);
    devHelperVPPM.SetTrasmitterPosition("TX1", distance, 0.0, 2.5);
    devHelperVPPM.SetTrasmitterParameter("TX1", "Gain", 70);
    devHelperVPPM.SetTrasmitterParameter("TX1", "DataRateInMBPS", 0.3);

    devHelperVPPM.CreateReceiver("RX1");  // RX principal do Nó 1
    // Configs semelhantes ao RX0 (ajuste aqui)
    devHelperVPPM.SetReceiverParameter("RX1", "FilterGain", 1);
    devHelperVPPM.SetReceiverParameter("RX1", "RefractiveIndex", 1.5);
    devHelperVPPM.SetReceiverParameter("RX1", "FOVAngle", 28.5);
    devHelperVPPM.SetReceiverParameter("RX1", "ConcentrationGain", 0);
    devHelperVPPM.SetReceiverParameter("RX1", "PhotoDetectorArea", PhotoDetectorArea);
    devHelperVPPM.SetReceiverParameter("RX1", "RXGain", 0);
    devHelperVPPM.SetReceiverParameter("RX1", "Beta", 1);
    // Modulação: Ajuste esquema

    // 5. Cria os dois canais (downlink e uplink)
    chHelper.CreateChannel("channelDown");  // Downlink: TX0 → RX1
    chHelper.SetPropagationLoss("channelDown", "VlcPropagationLoss");
    chHelper.SetPropagationDelay("channelDown", 2);
    chHelper.AttachTransmitter("channelDown", "TX0", &devHelperVPPM);
    chHelper.AttachReceiver("channelDown", "RX1", &devHelperVPPM);
    // ---------------- Configurações do canal downlink (ajuste aqui) ----------------
    chHelper.SetChannelParameter("channelDown", "TEMP", 295);
    chHelper.SetChannelParameter("channelDown", "BAND_FACTOR_NOISE_SIGNAL", BandFactorNoiseSignal);
    chHelper.SetChannelWavelength("channelDown", 380, 780);
    chHelper.SetChannelParameter("channelDown", "ElectricNoiseBandWidth", 3 * 1e5);
    // --------------------------------------------------------------------------------

    chHelper.CreateChannel("channelUp");  // Uplink: TX1 → RX0 (para ACKs)
    chHelper.SetPropagationLoss("channelUp", "VlcPropagationLoss");
    chHelper.SetPropagationDelay("channelUp", 2);
    chHelper.AttachTransmitter("channelUp", "TX1", &devHelperVPPM);
    chHelper.AttachReceiver("channelUp", "RX0", &devHelperVPPM);
    // Configs semelhantes ao downlink (ajuste se uplink for diferente)
    chHelper.SetChannelParameter("channelUp", "TEMP", 295);
    chHelper.SetChannelParameter("channelUp", "BAND_FACTOR_NOISE_SIGNAL", BandFactorNoiseSignal);
    chHelper.SetChannelWavelength("channelUp", 380, 780);
    chHelper.SetChannelParameter("channelUp", "ElectricNoiseBandWidth", 3 * 1e5);

    // 6. Instala dispositivos nos nós
    Ptr<VlcTxNetDevice> tx0Handle = DynamicCast<VlcTxNetDevice>(devHelperVPPM.GetTransmitter("TX0"));
    Ptr<VlcRxNetDevice> rx0Handle = DynamicCast<VlcRxNetDevice>(devHelperVPPM.GetReceiver("RX0"));
    nodes.Get(0)->AddDevice(tx0Handle);
    nodes.Get(0)->AddDevice(rx0Handle);

    Ptr<VlcTxNetDevice> tx1Handle = DynamicCast<VlcTxNetDevice>(devHelperVPPM.GetTransmitter("TX1"));
    Ptr<VlcRxNetDevice> rx1Handle = DynamicCast<VlcRxNetDevice>(devHelperVPPM.GetReceiver("RX1"));
    nodes.Get(1)->AddDevice(tx1Handle);
    nodes.Get(1)->AddDevice(rx1Handle);

    // 7. Instala MAC em ambos os nós
    Ptr<VlcMac> mac0 = CreateObject<VlcMac>();
    mac0->SetAddress(Mac48Address::Allocate());
    mac0->SetTxDevice(tx0Handle);
    mac0->SetRxDevice(rx0Handle);
    tx0Handle->SetMac(mac0);
    rx0Handle->SetMac(mac0);

    Ptr<VlcMac> mac1 = CreateObject<VlcMac>();
    mac1->SetAddress(Mac48Address::Allocate());
    mac1->SetTxDevice(tx1Handle);
    mac1->SetRxDevice(rx1Handle);
    tx1Handle->SetMac(mac1);
    rx1Handle->SetMac(mac1);

    // 8. Instala stack IP (opcional, se precisar de IP para outras partes)
    InternetStackHelper internet;
    internet.Install(nodes);

    // Atribua IPs (exemplo)
    /*
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(NetDeviceContainer(tx0Handle, tx1Handle, rx0Handle, rx1Handle));
    */

    NetDeviceContainer downlinkDevices;
    downlinkDevices.Add(tx0Handle); // TX → RX
    downlinkDevices.Add(rx0Handle);

    NetDeviceContainer uplinkDevices;
    uplinkDevices.Add(rx1Handle);   // RX → TX
    uplinkDevices.Add(tx1Handle);

    Ipv4AddressHelper ipv4;

    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer downlinkIfs =
    ipv4.Assign(downlinkDevices);

    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer uplinkIfs =
    ipv4.Assign(uplinkDevices);



    // 9. Instala aplicação customizada no Nó 0 (envia para MAC1 via MAC0)
    Ptr<CustomVlcSender> sender = CreateObject<CustomVlcSender>(mac0, mac1->GetAddress(), packetSize, packetInterval);
    nodes.Get(0)->AddApplication(sender);
    sender->SetStartTime(Seconds(1.0));
    sender->SetStopTime(Seconds(simTime - 1.0));

    // 10. Configura traces para throughput
    rx1Handle->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&RxEnd));
    tx0Handle->TraceConnectWithoutContext("PhyTxEnd", MakeCallback(&TxEnd));

    // 11. Habilita animação e traces ASCII (opcional)
    AnimationInterface anim("vlc-arq-example.xml");
    //AsciiTraceHelper ascii;
    //ascii.EnableAsciiAll(ascii.CreateFileStream("vlc-arq-example.tr"));

    // 12. Executa a simulação
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // 13. Calcula throughput/goodput
    double throughput = (Received.back() * 8) / theTime.back();
    std::cout << "Throughput value is " << throughput << std::endl;

    double goodput = rx1Handle->ComputeGoodPut() * 8 / theTime.back() / 1024;
    std::cout << "Distance between TX and RX is " << distance << " meters" << std::endl;
    std::cout << "Goodput value is " << goodput << std::endl;

    Simulator::Destroy();
    Received.clear();
    theTime.clear();

    return 0;
}