#include "ns3/vlc-tx-net-device.h"
//#include "ns3/vlc-mac.h"
#include "vlc-mac-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("vlcNetDeviceTX");
NS_OBJECT_ENSURE_REGISTERED (VlcTxNetDevice);

ns3::TypeId VlcTxNetDevice::GetTypeId(void)	// returns meta-information about VLC_ErrorModel class
		{ 	// including parent class, group name, constructor, and attributes

	static ns3::TypeId tid = ns3::TypeId("VlcTxNetDevice").SetParent<
			VlcNetDevice>().AddConstructor<VlcTxNetDevice>().AddAttribute(
			"SemiAngle", "Semi angle for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcTxNetDevice::m_semiangle),
			MakeDoubleChecker<double>()).AddAttribute("AngleOfRadiance",
			"Angle of radiance for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcTxNetDevice::m_angleOfRadiance),
			MakeDoubleChecker<double>()).AddAttribute("LambertianOrder",
			"Lambertian Order for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcTxNetDevice::m_lOrder),
			MakeDoubleChecker<double>()).AddAttribute("Gain",
			"TX gain for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcTxNetDevice::m_TXGain),
			MakeDoubleChecker<double>()).AddAttribute("Bias",
			"Biasing voltage for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&VlcTxNetDevice::m_bias),
			MakeDoubleChecker<double>());
	return tid;

}

VlcTxNetDevice::VlcTxNetDevice() :
	m_TMAX(1) {
	m_semiangle = 0;
	m_angleOfRadiance = 0;
	m_lOrder = 1;
	m_TXGain = 0;
	m_bias = 0;
	m_pktId = 1;
}

VlcTxNetDevice::~VlcTxNetDevice() {

}

void ns3::VlcTxNetDevice::AttachChannel(Ptr<VlcChannel> channel) {
	m_channel = channel;
}

double VlcTxNetDevice::GetLambertianOrder() {
	return this->m_lOrder;
}

//before setting Lambertian Order make sure the semiangle value is set
//Need to setup error handling when semiangle is not set
void VlcTxNetDevice::SetLambertainOrder() {
	this->m_lOrder = (-1 * log(2)) / (log(cos(this->m_semiangle)));
}

void VlcTxNetDevice::AddTXOpticalPowerSignal(double power) {
	this->m_TXOpticalPower.push_back(power);
}

std::vector<double>& VlcTxNetDevice::GetTXOpticalPowerSignal() {
	return this->m_TXOpticalPower;
}

double VlcTxNetDevice::GetOpticalPowerSignalAtInstant(int time) {
	return this->m_TXOpticalPower.at(time);
}

void VlcTxNetDevice::SetTXOpticalPowerSignal(std::vector<double> &powerSignal) {
	this->m_TXOpticalPower = powerSignal;
}

double VlcTxNetDevice::GetTXPowerMax() {
	return this->m_TMAX;
}

void VlcTxNetDevice::SetCapacity(int size) {
	m_TXOpticalPower.reserve(size);
	m_signal.reserve(size);
}

double VlcTxNetDevice::GetSemiangle() {
	return this->m_semiangle;
}

void VlcTxNetDevice::SetSemiangle(double angle) {
	this->m_semiangle = angle * M_PI / 180;
	this->SetLambertainOrder();
}

void VlcTxNetDevice::SetAngleOfRadiance(double angle) {
	this->m_angleOfRadiance = angle * M_PI / 180;
}

double VlcTxNetDevice::GetAngleOfRadiance() {
	return this->m_angleOfRadiance;
}

double VlcTxNetDevice::GetTXGain() {
	return this->m_TXGain;
}
void VlcTxNetDevice::SetTXGain() {
	//Channel loss is divided into TX gain and RX gain, which are used to calculate the loss.
	this->m_TXGain = ((this->m_lOrder + 1) / (2 * M_PI))
			* std::pow(std::cos((long double) this->m_angleOfRadiance),
					this->m_lOrder);
}

void VlcTxNetDevice::AddSignal(double signal) {
	this->m_signal.push_back(signal);
}

std::vector<double>& VlcTxNetDevice::GetSignal() {
	return this->m_signal;
}

double VlcTxNetDevice::GetSignalAtInstant(int time) {
	return this->m_signal.at(time);
}

void VlcTxNetDevice::SetSignal(std::vector<double> &signal) {
	this->m_signal = signal;
}

void VlcTxNetDevice::SetBias(double bias) {
	this->m_bias = bias;
}

double VlcTxNetDevice::GetBias() {
	return m_bias;
}

void VlcTxNetDevice::BoostSignal() {

	m_TXOpticalPower.clear();

	for (unsigned int i = 0; i < m_signal.size(); i++) {
		m_TXOpticalPower.push_back((double) (m_signal.at(i) + m_bias));
		if (m_TXOpticalPower.at(i) > m_TMAX) {
			m_TXOpticalPower.at(i) = m_TMAX;
		}
	}
}

double VlcTxNetDevice::GetAveragePowerSignalPower() {
	double pMax = *std::max_element(this->m_TXOpticalPower.begin(), this->m_TXOpticalPower.end());
	double pMin = *std::min_element(this->m_TXOpticalPower.begin(), this->m_TXOpticalPower.end());
	return (pMax + pMin) / 2;
}

double VlcTxNetDevice::GetAverageSignal() {
	double pMax = *std::max_element(this->m_signal.begin(), this->m_signal.end());
	double pMin = *std::min_element(this->m_signal.begin(), this->m_signal.end());
	return (pMax + pMin) / 2;
}


//função de envio
bool VlcTxNetDevice::EnqueueDataPacket(Ptr<Packet> p/*,bool isACK, bool packetCoded, bool firstPacket*/) {

	VlcMacHeader hdr;
	bool dropped = false;

	
	//std::cout <<"Codificado? "<< packetCoded << std::endl;
	//if(isACK){	
		/*if(packetCoded){
			hdr.SetPacketId (m_pktId,0);
		}
		else{
			hdr.SetPacketId (m_pktId,m_pktId+1);
		}*/
	//}else{
		//if(firstPacket){
			//std::cout <<"First Packet"<<std::endl;
			//hdr.SetPacketId (m_pktId,0);
		//}
		//else if(packetCoded){
			//std::cout <<"Pacote codificado"<<std::endl;
			//hdr.SetPacketId (m_pktId,m_pktId-1);
		//}
		//else{
			//std::cout <<"Else"<<std::endl;
			hdr.SetPacketId (m_pktId,0);
		//}

	//}
		

	p->AddHeader (hdr);

	
	dropped = m_channel->TransmitDataPacket(p);
	if(!dropped){
		std::cout << "------FIM DO PACOTE-------" << std::endl;
	}

	
	//if(!dropped && isACK){
	m_pktId++;
	//}
	std::cout << "ID1= " << hdr.GetPacketId1 ()<< " ID2= " << hdr.GetPacketId2 ()/* << " ACK? " << isACK*/ << std::endl;


	return dropped;
}

/*
void VlcTxNetDevice::SetMac(Ptr<VlcMac> mac) {
  m_mac = mac;
  if (m_mac) m_mac->SetTxDevice(this);  // Configura o MAC para usar este TX
}

Ptr<VlcMac> VlcTxNetDevice::GetMac() const { return m_mac; }
*/

} /* namespace vlc */
