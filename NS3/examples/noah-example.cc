#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"       // Para stack IP se quiser usar aplicações
#include "ns3/applications-module.h"   // Para OnOffApplication, UdpEcho, etc.
#include "ns3/vlc-module.h"            // Seu módulo VLC (ajuste o nome se for diferente)

using namespace ns3;

int main(int argc, char *argv[])
{
    // ==================== PARÂMETROS DA SIMULAÇÃO (ajuste aqui) ====================
    double simTime = 10.0;               // Tempo total de simulação (segundos)
    double distance = 2.0;               // Distância entre TX e RX (metros)
    // ===============================================================================

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

    // 3. Cria os dois canais VLC (downlink e uplink reverso)
    Ptr<VlcChannel> channelDown = CreateObject<VlcChannel>();
    Ptr<VlcChannel> channelUp   = CreateObject<VlcChannel>();

    // ---------------- Configurações comuns do canal (ajuste conforme necessário) ----------------
    // channelDown->SetPropagationLossModel(...);
    // channelDown->SetPropagationDelayModel(CreateObject<ConstantSpeedPropagationDelayModel>());
    // channelDown->SetAveragePower(20.0);  // exemplo em dBm ou mW
    // channelUp->Set... (pode ser igual ou diferente para uplink)
    // -----------------------------------------------------------------------------------------

    // 4. Cria os dispositivos em cada nó (TX + RX em ambos para bidirecionalidade)

    // Nó 0 (transmissor principal, mas também recebe ACKs)
    Ptr<VlcTxNetDevice> tx0 = CreateObject<VlcTxNetDevice>();
    Ptr<VlcRxNetDevice> rx0 = CreateObject<VlcRxNetDevice>();
    tx0->SetMobilityModel(nodes.Get(0)->GetObject<MobilityModel>());
    rx0->SetMobilityModel(nodes.Get(0)->GetObject<MobilityModel>());

    // Configurações específicas do TX0 (ajuste aqui seus parâmetros reais)
    // tx0->SetSemiangle(60.0);               // em graus
    // tx0->SetBias(0.5);
    // tx0->SetTXOpticalPowerSignal( ... );   // seu vetor de potência, se usar

    nodes.Get(0)->AddDevice(tx0);
    nodes.Get(0)->AddDevice(rx0);

    // Nó 1 (receptor principal, mas também envia ACKs)
    Ptr<VlcTxNetDevice> tx1 = CreateObject<VlcTxNetDevice>();
    Ptr<VlcRxNetDevice> rx1 = CreateObject<VlcRxNetDevice>();
    tx1->SetMobilityModel(nodes.Get(1)->GetObject<MobilityModel>());
    rx1->SetMobilityModel(nodes.Get(1)->GetObject<MobilityModel>());
ut
    // Configurações específicas do RX1 (ajuste aqui)
    // rx1->SetFOVAngle(60.0);
    // rx1->SetPhotoDectectorArea(1e-4);      // área em m²
    // rx1->SetRefractiveIndex(1.5);
    // rx1->SetConcentrationGain();           // chama após FOV e índice

    nodes.Get(1)->AddDevice(tx1);
    nodes.Get(1)->AddDevice(rx1);

    // 5. Conecta os dispositivos aos canais corretos
    channelDown->Attach(tx0);   // TX0 envia downlink
    channelDown->Attach(rx1);   // RX1 recebe downlink

    channelUp->Attach(tx1);     // TX1 envia uplink (ACKs e possivelmente outros dados)
    channelUp->Attach(rx0);     // RX0 recebe uplink

    // 6. Instala o MAC em ambos os nós
    Ptr<VlcMac> mac0 = CreateObject<VlcMac>();
    mac0->SetAddress(Mac48Address::Allocate());
    mac0->SetTxDevice(tx0);     // TX principal
    mac0->SetRxDevice(rx0);     // RX para ACKs recebidos

    Ptr<VlcMac> mac1 = CreateObject<VlcMac>();
    mac1->SetAddress(Mac48Address::Allocate());
    mac1->SetTxDevice(tx1);     // TX reverso (para enviar ACKs)
    mac1->SetRxDevice(rx1);     // RX principal

    // Vincula os MACs aos dispositivos (para que eles saibam quem chamar)
    tx0->SetMac(mac0);
    rx0->SetMac(mac0);
    tx1->SetMac(mac1);
    rx1->SetMac(mac1);

    // 7. (Opcional) Instala stack IP se quiser usar aplicações padrão
    InternetStackHelper stack;
    stack.Install(nodes);

    // Atribua endereços IP (exemplo simples)
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(nodes);

    // 8. (Opcional) Instale uma aplicação de teste
    // Exemplo: OnOffApplication enviando para o MAC0 (nó transmissor principal)
    /*
    OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(interfaces.GetAddress(1), 9)));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate("500kbps")));
    onoff.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer apps = onoff.Install(nodes.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(simTime - 1.0));
    */

    // Para usar com o MAC customizado, você precisaria de uma aplicação que chame diretamente:
    // mac0->Enqueue(packet, mac1->GetAddress());

    // 9. Habilita traces (opcional, para debug)
    // AsciiTraceHelper ascii;
    // ascii.EnableAsciiAll(ascii.CreateFileStream("vlc-arq.tr"));

    // 10. Executa a simulação
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}