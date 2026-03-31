#include "ns3/vlc-rx-net-device.h"
//#include "ns3/vlc-mac.h"
#include "vlc-mac-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("vlcNetDeviceRX");

NS_OBJECT_ENSURE_REGISTERED (VlcRxNetDevice);

ns3::TypeId VlcRxNetDevice::GetTypeId(void)	// returns meta-information about VlcErrorModel class
		{ 	// including parent class, group name, constructor, and attributes

	static ns3::TypeId tid = ns3::TypeId("VlcRxNetDevice").SetParent<
			VlcNetDevice>().AddConstructor<VlcRxNetDevice>().AddAttribute(
			"FilterGain", "filter gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_filterGain),
			MakeDoubleChecker<double>()).AddAttribute("PhotoDetectorArea",
			"photo detector area for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_photodetectorArea),
			MakeDoubleChecker<double>()).AddAttribute("FOVAngle",
			"field of view angle for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_FOVangle),
			MakeDoubleChecker<double>()).AddAttribute("RefractiveIndex",
			"refractive index of the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_refIndex),
			MakeDoubleChecker<double>()).AddAttribute("AngleOfIncidence",
			"angle of incidence of the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_angleOfIncidence),
			MakeDoubleChecker<double>()).AddAttribute("ConcentrationGain",
			"concentration gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_concentrationGain),
			MakeDoubleChecker<double>()).AddAttribute("RXGain",
			"RX gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_RXGain),
			MakeDoubleChecker<double>()).AddAttribute("Bias",
			"Bias for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcRxNetDevice::m_bias),
			MakeDoubleChecker<double>())

			;

	return tid;

}

VlcRxNetDevice::VlcRxNetDevice()
	: codedBuffer(5),
	uncodedBuffer(5) 
{

	////NS_LOG_FUNCTION(this);

	m_filterGain = 0;
	m_photodetectorArea = 0;
	m_FOVangle = 0;
	m_refIndex = 0;
	m_angleOfIncidence = 0;
	m_concentrationGain = 0;
	m_RXGain = 0;
	m_bias = 0;
	m_error = ns3::CreateObject<ns3::VlcErrorModel>();
	//m_mac = NULL;

}

void ns3::VlcRxNetDevice::AttachChannel(Ptr<VlcChannel> channel) {
	m_channel = channel;
}

//use this function after setting up FOV and refractive index
void VlcRxNetDevice::SetConcentrationGain() {
	//NS_LOG_FUNCTION(this);
	this->m_concentrationGain = std::pow(this->m_refIndex, 2)
			/ std::pow((float) std::sin((float) m_FOVangle), 2);
}

void VlcRxNetDevice::AddRXOpticalPowerSignal(double power) {
	//NS_LOG_FUNCTION(this<<power);
	this->m_RXOpticalPower.push_back(power);
}
std::vector<double>& VlcRxNetDevice::GetRXOpticalPowerSignal() {
	//NS_LOG_FUNCTION(this);
	return this->m_RXOpticalPower;
}
double VlcRxNetDevice::GetOpticalPowerSignalAtInstant(int time) {
	//NS_LOG_FUNCTION(this<<time);
	return this->m_RXOpticalPower.at(time);
}
void VlcRxNetDevice::SetRXOpticalPowerSignal(std::vector<double> &powerSignal) {
	//NS_LOG_FUNCTION(this<<powerSignal.size());
	this->m_RXOpticalPower = powerSignal;
}

void VlcRxNetDevice::SetCapacity(int size) {
	//NS_LOG_FUNCTION(this<<size);
	this->m_signal.reserve(size);
	this->m_RXOpticalPower.reserve(size);
}

double VlcRxNetDevice::GetFilterGain() {
	//NS_LOG_FUNCTION(this);
	return this->m_filterGain;
}
void VlcRxNetDevice::SetFilterGain(double fgain) {
	//NS_LOG_FUNCTION(this<<fgain);
	this->m_filterGain = fgain;
}

double VlcRxNetDevice::GetPhotoDetectorArea() {
	//NS_LOG_FUNCTION(this);
	return this->m_photodetectorArea;
}

void VlcRxNetDevice::SetPhotoDectectorArea(double pArea) {
	//NS_LOG_FUNCTION(this<<pArea);
	this->m_photodetectorArea = pArea;
}

double VlcRxNetDevice::GetFOVAngle() {
	//NS_LOG_FUNCTION(this);
	return this->m_FOVangle;
}

void VlcRxNetDevice::SetFOVAngle(double angle) {
	//NS_LOG_FUNCTION(this<<angle);
	this->m_FOVangle = angle * M_PI / 180;
}

double VlcRxNetDevice::GetRefractiveIndex() {
	//NS_LOG_FUNCTION(this);
	return this->m_refIndex;
}
void VlcRxNetDevice::SetRefractiveIndex(double angle) {
	//NS_LOG_FUNCTION(this << angle);
	this->m_refIndex = angle;
}

double VlcRxNetDevice::GetConcentrationGain() {
	//NS_LOG_FUNCTION(this);
	return this->m_concentrationGain;
}

double VlcRxNetDevice::GetRXGain() {
	//NS_LOG_FUNCTION(this);
	return this->m_RXGain;
}
void VlcRxNetDevice::SetRXGain() {
	//NS_LOG_FUNCTION(this);
	this->m_RXGain = std::cos(this->m_angleOfIncidence);
}

void VlcRxNetDevice::SetIncidenceAngle(double angle) {
	//NS_LOG_FUNCTION(this << angle);
	this->m_angleOfIncidence = angle * M_PI / 180;
}

ns3::VlcErrorModel::ModScheme VlcRxNetDevice::GeModulationtScheme(void) const {	// returns the modulation scheme used
	//NS_LOG_FUNCTION(this);
	return this->m_error->GetScheme();
}

void VlcRxNetDevice::SetScheme(VlcErrorModel::ModScheme scheme) { // sets the value of the modulation scheme used
	//NS_LOG_FUNCTION(this<<scheme);
	this->m_error->SetScheme(scheme);

}

void VlcRxNetDevice::SetRandomVariableForErrorModel(
	ns3::Ptr<ns3::RandomVariableStream> ranVar) {// assigns a random variable stream to be used by this model
	//NS_LOG_FUNCTION(this<<ranVar);
	this->m_error->SetRandomVariable(ranVar);
}

int64_t VlcRxNetDevice::AssignStreamsForErrorModel(int64_t stream) {// assigns a fixed stream number to the random variables used by this model
	//NS_LOG_FUNCTION(this);
	return this->m_error->AssignStreams(stream);
}

double VlcRxNetDevice::GetSNRFromErrorModel(void) const {// returns the signal-to-noise ratio (SNR)
	//NS_LOG_FUNCTION(this);
	return this->m_error->GetSNR();
}

void VlcRxNetDevice::SetSNRForErrorModel(double snr) {	// sets the SNR value
	//NS_LOG_FUNCTION(this<<snr);
	this->m_error->SetSNR(snr);
}

double VlcRxNetDevice::CalculateErrorRateForErrorModel() {// calculates the error rate value according to modulation scheme
	//NS_LOG_FUNCTION(this);
	return this->m_error->CalculateErrorRate();
}

bool VlcRxNetDevice::IsCorrupt(ns3::Ptr<ns3::Packet> pkt) {	// determines if the packet is corrupted according to the error model
	//NS_LOG_FUNCTION(this<<pkt);
	return this->m_error->IsCorrupt(pkt);
}

// methods for PAM
int VlcRxNetDevice::GetModulationOrder(void) const {// returns the modulation order (M)
	//NS_LOG_FUNCTION(this);
	return this->m_error->GetModulationOrder();
}

void VlcRxNetDevice::SetModulationOrder(int m_order) {// sets the modulation order value
	//NS_LOG_FUNCTION(this<<m_order);
	return this->m_error->SetModulationOrder(m_order);
}

// methods for VPPM
double VlcRxNetDevice::GetAlpha(void) const {	// returns alpha value
	//NS_LOG_FUNCTION(this);
	return this->m_error->GetAlpha();
}
void VlcRxNetDevice::SetAlpha(double a) {		// sets alpha value
	//NS_LOG_FUNCTION(this);
	if( m_error->GetScheme()!=VlcErrorModel::VPPM ){
		throw std::logic_error("Alpha should not be set for these modulation schemes \n");
	}
	this->m_error->SetAlpha(a);
}

double VlcRxNetDevice::GetBeta(void) const {		// returns beta value
	//NS_LOG_FUNCTION(this);
	return this->m_error->GetBeta();
}
void VlcRxNetDevice::SetBeta(double b) {		// sets beta value
	//NS_LOG_FUNCTION(this<<b);
	return this->m_error->SetBeta(b);
}

double VlcRxNetDevice::GetIncidenceAngle() {
	//NS_LOG_FUNCTION(this);
	return this->m_angleOfIncidence;
}

ns3::Ptr<ns3::VlcErrorModel> VlcRxNetDevice::GetErrorModel() {
	//NS_LOG_FUNCTION(this);
	return this->m_error;
}

PacketCircularBuffer::PacketCircularBuffer(uint32_t capacity)
    : m_capacity(capacity),
      m_head(0),
      m_tail(0),
      m_size(0)
{
    m_buffer.resize(capacity);
}

bool PacketCircularBuffer::PushBack(ns3::Ptr<ns3::Packet> p)
{
    /*if (Full()) {
        return false; // buffer cheio
    }*/
    uint32_t old_tail = m_tail;
    m_buffer[old_tail] = p;
    m_tail = (m_tail + 1) % m_capacity;
    //std::cout << "Inseriu no buffer na posicao " << old_tail << std::endl;
    //m_size++;
    return true;
}


//coded = 1 , uncoded = 0
Ptr<Packet>
PacketCircularBuffer::GetByPacketId(uint32_t id1, uint32_t id2)
{
    for (uint32_t i = 0; i < 5; ++i)
    {
        uint32_t realIndex = (m_head + i) % m_capacity;
		//std::cout << "Real time index " << realIndex <<std::endl;
        Ptr<Packet> p = m_buffer[realIndex];

        if (!p)
            continue;

        //NUNCA remover header do pacote armazenado
        Ptr<Packet> copy = p->Copy();

        VlcMacHeader hdr;
        copy->RemoveHeader(hdr);
		/*
		Logica dos ifs abaixo feita para distinguir que tipo de pacote deve buscar 
		*/
		//std::cout << "-ID1- " << hdr.GetPacketId1() << " -ID2- " << hdr.GetPacketId2() << std::endl;


		if(id2 != 0){
			if(hdr.GetPacketId1() == id2){
				return p; // retorna o pacote ORIGINAL
			}
		}else{
			if (hdr.GetPacketId2() == id1)
			{
				return p; // retorna o pacote ORIGINAL
			}
		}
    }

    return 0;
}


bool
PacketCircularBuffer::Remove(uint32_t index)
{
    if (index >= m_size)
    {
        return false;
    }

    // desloca os elementos seguintes para "fechar o buraco"
    for (uint32_t i = index; i < 5 - 1; ++i)
    {
        uint32_t from = (m_head + i + 1) % m_capacity;
        uint32_t to   = (m_head + i) % m_capacity;
        m_buffer[to] = m_buffer[from];
    }

    // ajusta tail
    m_tail = (m_tail + m_capacity - 1) % m_capacity;
    m_buffer[m_tail] = 0;
    m_size--;
    return true;
}

uint32_t
PacketCircularBuffer::Size() const
{
    return m_size;
}

uint32_t
PacketCircularBuffer::Capacity() const
{
    return m_capacity;
}

bool
PacketCircularBuffer::Empty() const
{
    return m_size == 0;
}

bool
PacketCircularBuffer::Full() const
{
    return m_size == m_capacity;
}

void
PacketCircularBuffer::PrintBuffer () const
{
    std::cout << "==== PacketCircularBuffer ====" << std::endl;
    std::cout << "Size: " << m_size
              << " Capacity: " << m_capacity
              << " Head: " << m_head
              << std::endl;

    for (uint32_t i = 0; i < 5; ++i)
    {
        uint32_t realIndex = (m_head + i) % m_capacity;
        Ptr<Packet> p = m_buffer[realIndex];

        std::cout << "[" << i << "] (real " << realIndex << "): ";

        if (!p)
        {
            std::cout << "<empty>" << std::endl;
            continue;
        }

        // Copiar para não destruir header
        Ptr<Packet> copy = p->Copy();

        VlcMacHeader hdr;
        copy->RemoveHeader(hdr);

        std::cout << "Size=" << p->GetSize()
                  << " ID1=" << hdr.GetPacketId1()
                  << " ID2=" << hdr.GetPacketId2()
                  << std::endl;
    }

    std::cout << "==============================" << std::endl;
}


//função de recepção
void VlcRxNetDevice::EnqueueDataPacketAfterCorruption(Ptr<Packet> p,
		bool corruptFlag, bool isACK) {
	//NS_LOG_FUNCTION(this<<p<<corruptFlag);
	uint32_t id1,id2;
	Ptr<Packet> auxPacket1,auxPacket2;
	this->dataPool.push_back(p);
	this->packetCorruptionState.push_back(corruptFlag);
  	VlcMacHeader hdr;
	p->RemoveHeader(hdr);

	if(!corruptFlag && !isACK){

		id1 = hdr.GetPacketId1();
		id2 = hdr.GetPacketId2();

		if(id2 != 0){//significa que é um pacote codificado
			//só precisa verificar o id2, pois nunca vai chegar um pacote codificado que precisara do pacote id1
			auxPacket1 = uncodedBuffer.GetByPacketId(id1,id2);
			//std::cout << std::endl << " Pacote " << auxPacket1 << std::endl;
			if(auxPacket1 != 0){
				auxPacket2 = VlcNetDevice::XorPackets(p, auxPacket1,p->GetSize());
				//std::cout << " +1 " << std::endl;
			}else{
				p->AddHeader (hdr);
				codedBuffer.PushBack(p);
				//std::cout<<std::endl <<"----------Buffer codificado----------------" <<std::endl;
				//codedBuffer.PrintBuffer();
			}
			//verifica se existe pacote correspondente no buffer de pacotes nao codificados
			//se sim, decodifica
			//se nao, guarda no buffer de pacotes codificados (verifica limite de tamanho do buffer)

		}else{//pacote nao codificadoauxPacket1 = uncodedBuffer.Get(id2);
			//só precisa verificar o id2, pois nunca vai chegar um pacote decodificado que precisara do pacote id2
			auxPacket1 = codedBuffer.GetByPacketId(id1,id2);
			//std::cout << std::endl << " Pacote " << auxPacket1 << std::endl;
			if(auxPacket1 != 0){
				auxPacket2 = VlcNetDevice::XorPackets(p, auxPacket1,p->GetSize());
				//std::cout << " +1 " << std::endl;
			}else{
				//std::cout <<"----------Ta entrando no else nao codificado----------------" <<std::endl;
				p->AddHeader (hdr);
				//std::cout<<std::endl <<"----------Buffer nao codificado----------------" <<std::endl;
				uncodedBuffer.PushBack(p);
				//uncodedBuffer.PrintBuffer();
			}
			//verifica se existe pacote correspondente no buffer de pacotes codificados
			//se sim, decodifica
			//se nao, guarda no buffer de pacotes nao codificados (verifica limite de tamanho do buffer)

		}

	}
}

int VlcRxNetDevice::ComputeGoodPut() {
	//NS_LOG_FUNCTION(this);
	int goodPacketSize = 0;
	int qntNoCorruptedPackets = 0;
	for (uint32_t i = 0; i < this->dataPool.size(); i++) {
		if (!this->packetCorruptionState.at(i)) {
			qntNoCorruptedPackets++;
			goodPacketSize += this->dataPool.at(i)->GetSize();
		}
	}

	std::cout << "Number of received packets: " << this->dataPool.size()
			<< std::endl;
			
	std::cout <<" Number of good packets: " << qntNoCorruptedPackets
			<< std::endl;

	return goodPacketSize;

}

VlcRxNetDevice::~VlcRxNetDevice() {
	// TODO Auto-generated destructor stub
}
/*
void VlcRxNetDevice::SetMac(Ptr<VlcMac> mac) {
  m_mac = mac;
  if (m_mac) m_mac->SetRxDevice(this);  // Configura o MAC para usar este TX
}
Ptr<VlcMac> VlcRxNetDevice::GetMac() const { return m_mac; }
*/
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////

} /* namespace vlc */
