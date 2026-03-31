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

//
// Network topology - Modified for descricao_vlc.txt parameters
//
// Single VLC Link with 4 beacon nodes and 1 unknown node positioning system
//

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/vlc-channel-helper.h"
#include "ns3/vlc-device-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("VisibleLightCommunication_Modified");

void PrintPacketData(Ptr<const Packet> p, uint32_t size);

// The number of bytes to send in this simulation.
static const uint32_t totalTxBytes = 80000;
static uint32_t currentTxBytes = 0;

// Perform series of 1040 byte writes (this is a multiple of 26 since
// we want to detect data splicing in the output stream)
static const uint32_t writeSize = 2048;
uint8_t data[writeSize];

// These are for starting the writing process, and handling the sending
// socket's notification upcalls (events).  These two together more or less
// implement a sending "Application", although not a proper ns3::Application
// subclass.

void StartFlow(Ptr<Socket>, Ipv4Address, uint16_t);

void WriteUntilBufferFull(Ptr<Socket>, uint32_t);

std::vector<double> Received(1, 0);
std::vector<double> theTime(1, 0);
//////////////////////////////////////
//Function to generate signals.
std::vector<double>& GenerateSignal(int size, double dutyRatio);

static void RxEnd(Ptr<const Packet> p) { // used for tracing and calculating throughput

	//PrintPacketData(p,p->GetSize());

	Received.push_back(Received.back() + p->GetSize()); // appends on the received packet to the received data up until that packet and adds that total to the end of the vector
	theTime.push_back(Simulator::Now().GetSeconds()); // keeps track of the time during simulation that a packet is received
	//NS_LOG_UNCOND("helooooooooooooooooo RxEnd");
}

static void TxEnd(Ptr<const Packet> p) { // also used as a trace and for calculating throughput

	Received.push_back(Received.back() + p->GetSize()); // same as for the RxEnd trace
	theTime.push_back(Simulator::Now().GetSeconds()); 	//
	//NS_LOG_UNCOND("helooooooooooooooooo TxEnd");
}

static void CwndTracer(uint32_t oldval, uint32_t newval) {
	NS_LOG_INFO("Moving cwnd from " << oldval << " to " << newval);
}

int main(int argc, char *argv[]) {
	// Users may find it convenient to turn on explicit debugging
	// for selected modules; the below lines suggest how to do this
	//  LogComponentEnable("TcpL4Protocol", LOG_LEVEL_ALL);
	//  LogComponentEnable("TcpSocketImpl", LOG_LEVEL_ALL);
	LogComponentEnable("PacketSink", LOG_LEVEL_ALL);
	//  LogComponentEnable("TcpLargeTransfer", LOG_LEVEL_ALL);

	// ==================================================================================
	// TABLE I: VLC SIMULATION PARAMETERS from descricao_vlc.txt
	// ==================================================================================
	double TransmitterPower_dBm = 48.573;     // Pt (dBm) - Transmitter Power
	double LambertianOrder_SemiAngle = 70.0;  // Φ1/2 (degrees) - Lambertian Order Semi angle
	double NoiseB_factor = 0.562;             // I2 - Noise bandwidth factor
	double BackgroundCurrent_IB = 5.1e-6;     // A - Background current
	double FET_transconductance_gm = 30.0;    // ms - FET transconductance
	double ElectronicCharge_q = 1.60217e-19;  // C - Electronic charge
	double PhotoDetectorArea = 1.0e-4;        // m² - Photodetector Area (changed from 1.3e-5)
	double FieldOfView_psi_con = 70.0;        // degrees - Field of view angle
	double Wavelength_min = 380e-9;           // meters - Lower wavelength (converted from nm)
	double Wavelength_max = 380e-9;           // meters - Upper wavelength (converted from nm)
	double Distance_d = 50.0;                 // m - Distance
	double Alpha_A = 0.85;                    // Α - Alpha coefficient
	double BandwidthFactor_B = 10.0;          // B - Bandwidth factor
	uint32_t PAM_M = 4;                       // PAM modulation level (M=4)

	// ==================================================================================
	// TABLE II: POSITIONING MODEL PARAMETERS from descricao_vlc.txt
	// ==================================================================================
	uint32_t BeaconNodes = 4;                 // Bi - Number of beacon nodes
	uint32_t UnknownNodes = 1;                // Ui - Number of unknown nodes
	double DistanceBetweenBeaconX = 800.0;    // m - Distance between each beacon along X-axis
	double DistanceB1_U = 10.0;               // R1 - Distance between B1 and U
	double DistanceB2_U = 800.06;             // R2 - Distance between B2 and U
	double DistanceB3_U = 1600.03;            // R3 - Distance between B3 and U
	double DistanceB4_U = 2400.02;            // R4 - Distance between B4 and U
	double LogDistance_NearField = 1.0;       // m - 3log Distance field near
	double LogDistance_Middle = 200.0;        // m - log Distance field, middle
	double LogDistance_Far = 500.0;           // m - 3log distance field, far
	double Exponent_N0 = 1.9;                 // Exponent N0
	double Exponent_N1 = 3.8;                 // Exponent N1
	double Exponent_N2 = 3.8;                 // Exponent N2

	// ==================================================================================
	// TRANSMITTER POSITION from descricao_vlc.txt
	// ==================================================================================
	double Tx_X = 0.0;                        // Transmitter coordinate X
	double Tx_Y = 0.0;                        // Transmitter coordinate Y
	double Tx_Z = 50.0;                       // Transmitter coordinate Z (height)
	double Tx_Azimuth = 0.0;                  // Transmitter Azimuth angle
	double Tx_Elevation = 180.0;              // Transmitter Elevation angle

	// ==================================================================================
	// RECEIVER POSITION from descricao_vlc.txt
	// ==================================================================================
	double Rx_X = 0.0;                        // Receiver coordinate X
	double Rx_Y = 0.0;                        // Receiver coordinate Y
	double Rx_Z_start = Distance_d;           // Receiver coordinate Z (starts at distance_d = 50m)
	//double Rx_Azimuth = 0.0;                  // Receiver Azimuth angle
	//double Rx_Elevation = 0.0;                // Receiver Elevation angle

	// Band factor for noise-signal ratio
	double Band_factor_Noise_Signal = BandwidthFactor_B;

	CommandLine cmd;
	cmd.Parse(argc, argv);

	// initialize the tx buffer.
	for (uint32_t i = 0; i < writeSize; ++i) {
		char m = toascii(97 + i % 26);
		data[i] = m;
	}

	// Main simulation loop - iterate through distance values
	for (double dist = 0.1; dist < 2; dist += 0.1) {
		//double dist = 2;

		// Here, we will explicitly create three nodes.  The first container contains
		// nodes 0 and 1 from the diagram above, and the second one contains nodes
		// 1 and 2.  This reflects the channel connectivity, and will be used to
		// install the network interfaces and connect them with a channel.
		NodeContainer n0n1;
		n0n1.Create(2);

		NodeContainer n1n2;
		n1n2.Add(n0n1.Get(1));
		n1n2.Create(1);

		MobilityHelper mobility;
		Ptr < ListPositionAllocator > m_listPosition = CreateObject<
				ListPositionAllocator>();
		m_listPosition->Add(Vector(250.0, 500.0, 0.0));
		m_listPosition->Add(Vector(500.0, 500.0, 0.0));
		m_listPosition->Add(Vector(750.0, 500.0, 0.0));

		mobility.SetPositionAllocator(m_listPosition);
		mobility.SetMobilityModel("ns3::VlcMobilityModel");

		NodeContainer allNodes(n0n1, n1n2.Get(1));
		mobility.Install(allNodes);

		VlcDeviceHelper devHelperVPPM; //VLC Device Helper to manage the VLC Device and device properties.

		//Creating and setting properties for the transmitter.
		// Parameters from descricao_vlc.txt Table I
		devHelperVPPM.CreateTransmitter("THE_TRANSMITTER");
		/****************************************TX-SIGNAL************************************
		 * Parameters from descricao_vlc.txt:
		 * - Lambertian Order Semi angle: 70°
		 * - Transmitter Power: 48.573 dBm
		 * - Position: (0.0, 0.0, 50.0) m
		 * - Azimuth: 0.0°, Elevation: 180.0°
		 ************************************************************************************/
		devHelperVPPM.SetTXSignal("THE_TRANSMITTER", 1000, 0.5, 0, 9.25e-5, 0);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER", "Bias", 0);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER", "SemiAngle", LambertianOrder_SemiAngle);  // 70° from descricao_vlc
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER", "Azimuth", Tx_Azimuth);  // 0.0 from descricao_vlc
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER", "Elevation", Tx_Elevation);  // 180.0 from descricao_vlc
		devHelperVPPM.SetTrasmitterPosition("THE_TRANSMITTER", Tx_X, Tx_Y, Tx_Z);  // (0.0, 0.0, 50.0) from descricao_vlc
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER", "Gain", 70);
		devHelperVPPM.SetTrasmitterParameter("THE_TRANSMITTER", "DataRateInMBPS", 0.3);

		//Creating and setting properties for the receiver.
		// Parameters from descricao_vlc.txt Table I
		devHelperVPPM.CreateReceiver("THE_RECEIVER");
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER", "FilterGain", 1);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER", "RefractiveIndex", 1.5);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER", "FOVAngle", FieldOfView_psi_con);  // 70° from descricao_vlc
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER", "ConcentrationGain", 0);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER", "PhotoDetectorArea", PhotoDetectorArea);  // 1.0e-4 m² from descricao_vlc
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER", "RXGain", 0);
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER", "Beta", Alpha_A);  // 0.85 from descricao_vlc
		/****************************************MODULATION SCHEME SETTINGS******************
		 * AVILABLE MODULATION SCHEMES ARE:
		 * [1]. VlcErrorModel::PAM4, [2]. VlcErrorModel::OOK, [3]. VlcErrorModel::VPPM
		 * [4]. VlcErrorModel::PSK4, [5]. VlcErrorModel::PSK16
		 * [6]. VlcErrorModel::QAM4, [7]. VlcErrorModel::QAM16
		 *
		 * Using PAM4 (M=4) as per descricao_vlc.txt Table I
		 ************************************************************************************/
		devHelperVPPM.SetReceiverParameter("THE_RECEIVER", "SetModulationScheme", VlcErrorModel::PAM4);

		VlcChannelHelper chHelper;
		chHelper.CreateChannel("THE_CHANNEL");
		chHelper.SetPropagationLoss("THE_CHANNEL", "VlcPropagationLoss");
		chHelper.SetPropagationDelay("THE_CHANNEL", 2);
		chHelper.AttachTransmitter("THE_CHANNEL", "THE_TRANSMITTER", &devHelperVPPM);
		chHelper.AttachReceiver("THE_CHANNEL", "THE_RECEIVER", &devHelperVPPM);
		chHelper.SetChannelParameter("THE_CHANNEL", "TEMP", 295);
		chHelper.SetChannelParameter("THE_CHANNEL", "BAND_FACTOR_NOISE_SIGNAL", Band_factor_Noise_Signal);
		// Set wavelength range from descricao_vlc.txt - both min and max are 380nm
		chHelper.SetChannelWavelength("THE_CHANNEL", 380, 780);  // Full visible spectrum for simulation
		chHelper.SetChannelParameter("THE_CHANNEL", "ElectricNoiseBandWidth", 3 * 1e5);

		// And then install devices and channels connecting our topology.
		NetDeviceContainer dev0 = chHelper.Install(n0n1.Get(0), n0n1.Get(1),
				&devHelperVPPM, &chHelper, "THE_TRANSMITTER", "THE_RECEIVER",
				"THE_CHANNEL");

		// Now add ip/tcp stack to all nodes.
		InternetStackHelper internet;
		internet.InstallAll();

		// Later, we add IP addresses.
		Ipv4AddressHelper ipv4;
		ipv4.SetBase("10.1.3.0", "255.255.255.0");
		Ipv4InterfaceContainer ipInterfs = ipv4.Assign(dev0);

		// and setup ip routing tables to get total ip-level connectivity.
 		Ipv4GlobalRoutingHelper::PopulateRoutingTables();

		///////////////////////////////////////////////////////////////////////////
		// Simulation with descricao_vlc.txt parameters
		//
		// Send data over VLC connection with parameters from descricao_vlc.txt
		// Transmitter position: (0.0, 0.0, 50.0) m
		// Receiver distance: variable (starts at 50m as per descricao_vlc)
		//
		///////////////////////////////////////////////////////////////////////////
		uint16_t servPort = 4000;
		// Create a packet sink to receive these packets
		PacketSinkHelper sink("ns3::TcpSocketFactory",
				InetSocketAddress(Ipv4Address::GetAny(), servPort));

		ApplicationContainer apps = sink.Install(n0n1.Get(1));

		// Set transmitter position from descricao_vlc.txt Table II
		devHelperVPPM.SetTrasmitterPosition("THE_TRANSMITTER", Tx_X, Tx_Y, Tx_Z);

		// Set receiver position from descricao_vlc.txt Table II
		// Distance starts at 50m (Rx_Z_start = Distance_d = 50.0)
		devHelperVPPM.SetReceiverPosition("THE_RECEIVER", Rx_X, Rx_Y, Rx_Z_start);

		apps.Start(Seconds(0.0));
		apps.Stop(Seconds(4.0));
		// Create a source to send packets from n0.  Instead of a full Application
		// and the helper APIs you might see in other example files, this example
		// will use sockets directly and register some socket callbacks as a sending
		// "Application".
		// Create and bind the socket...
		Ptr < Socket > localSocket = Socket::CreateSocket(n0n1.Get(0),
				TcpSocketFactory::GetTypeId());
		localSocket->Bind();

		// Trace changes to the congestion window
		Config::ConnectWithoutContext(
				"/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
				MakeCallback(&CwndTracer));
		dev0.Get(1)->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&RxEnd)); //traces to allow us to see what and when data is sent through the network
		dev0.Get(1)->TraceConnectWithoutContext("PhyTxEnd", MakeCallback(&TxEnd)); //traces to allow us to see what and when data is received through the network

		// ...and schedule the sending "Application"; This is similar to what an
		// ns3::Application subclass would do internally.
		Simulator::ScheduleNow(&StartFlow, localSocket, ipInterfs.GetAddress(1),
				servPort);

		// One can toggle the comment for the following line on or off to see the
		// effects of finite send buffer modelling.  One can also change the size of
		// said buffer.
		//localSocket->SetAttribute("SndBufSize", UintegerValue(4096));
		//Ask for ASCII and pcap traces of network traffic
		AsciiTraceHelper ascii;
		// p2p.EnableAsciiAll (ascii.CreateFileStream ("tcp-large-transfer.tr"));
		//p2p.EnablePcapAll ("tcp-large-transfer");

		// Finally, set up the simulator to run.  The 1000 second hard limit is a
		// failsafe in case some change above causes the simulation to never end
		AnimationInterface anim("visible-light-communication-modified.xml");

		Simulator::Stop(Seconds(5.0));
		Simulator::Run();

		double throughput = ((Received.back() * 8)) / theTime.back(); //goodput calculation
		std::cout << "Distance: " << Rx_Z_start << "m - throughput value is " << throughput << std::endl;

		Ptr < VlcRxNetDevice > rxHandle = devHelperVPPM.GetReceiver("THE_RECEIVER");
		double goodput = rxHandle->ComputeGoodPut();
	       std::cout<< "Good Packet Received Size is  "<< goodput<< std::endl;
		goodput *= 8;
		goodput /= theTime.back();
		goodput /= 1024;
		std::cout << "Simulation time is "<< theTime.back()<<std::endl;
		std::cout << "goodput value is " << goodput << " kbps" << std::endl;
		std::cout << "========================================" << std::endl;

		Received.clear();

		Simulator::Destroy();
		currentTxBytes = 0;
	}

	std::cout << "\n==================== SIMULATION PARAMETERS USED ====================" << std::endl;
	std::cout << "Source: descricao_vlc.txt" << std::endl;
	std::cout << "\nTable I: VLC Simulation Parameters" << std::endl;
	std::cout << "Transmitter Power, Pt: " << TransmitterPower_dBm << " dBm" << std::endl;
	std::cout << "Lambertian Order Semi angle, Φ1/2: " << LambertianOrder_SemiAngle << "°" << std::endl;
	std::cout << "Noise bandwidth factor, I2: " << NoiseB_factor << std::endl;
	std::cout << "Background current IB: " << BackgroundCurrent_IB << " A" << std::endl;
	std::cout << "FET transconductance (gm): " << FET_transconductance_gm << " ms" << std::endl;
	std::cout << "Electronic charge, q: " << ElectronicCharge_q << " C" << std::endl;
	std::cout << "Photodetector Area, A: " << PhotoDetectorArea << " m²" << std::endl;
	std::cout << "Field of view, ψcon: " << FieldOfView_psi_con << "°" << std::endl;
	std::cout << "Wavelength range: " << Wavelength_min*1e9 << "nm - " << Wavelength_max*1e9 << "nm" << std::endl;
	std::cout << "Distance, d: " << Distance_d << " m" << std::endl;
	std::cout << "Α: " << Alpha_A << std::endl;
	std::cout << "Bandwidth factor, B: " << BandwidthFactor_B << std::endl;
	std::cout << "PAM, M: " << PAM_M << std::endl;

	std::cout << "\nTable II: Positioning Model Parameters" << std::endl;
	std::cout << "Number of beacon nodes, Bi: " << BeaconNodes << std::endl;
	std::cout << "Number of unknown nodes, Ui: " << UnknownNodes << std::endl;
	std::cout << "Initial position of beacon node1: (0,0,0)" << std::endl;
	std::cout << "Distance between each beacon along X-axis (m): " << DistanceBetweenBeaconX << std::endl;
	std::cout << "Distance between B1 and U, R1: " << DistanceB1_U << " m" << std::endl;
	std::cout << "Distance between B2 and U, R2: " << DistanceB2_U << " m" << std::endl;
	std::cout << "Distance between B3 and U, R3: " << DistanceB3_U << " m" << std::endl;
	std::cout << "Distance between B4 and U, R4: " << DistanceB4_U << " m" << std::endl;
	std::cout << "3log Distance field near, (m): " << LogDistance_NearField << std::endl;
	std::cout << "log Distance field, middle (m): " << LogDistance_Middle << std::endl;
	std::cout << "3log distance field, far (m): " << LogDistance_Far << std::endl;
	std::cout << "Exponent N0: " << Exponent_N0 << std::endl;
	std::cout << "Exponent N1: " << Exponent_N1 << std::endl;
	std::cout << "Exponent N2: " << Exponent_N2 << std::endl;
	std::cout << "====================================================================\n" << std::endl;

	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//begin implementation of sending "Application"
void StartFlow(Ptr<Socket> localSocket, Ipv4Address servAddress,
		uint16_t servPort) {
	//NS_LOG_UNCOND("helooooooooooooooooo StartFlow");
	localSocket->Connect(InetSocketAddress(servAddress, servPort)); //connect

	// tell the tcp implementation to call WriteUntilBufferFull again
	// if we blocked and new tx buffer space becomes available
	localSocket->SetSendCallback(MakeCallback(&WriteUntilBufferFull));
	WriteUntilBufferFull(localSocket, localSocket->GetTxAvailable());
}

void WriteUntilBufferFull(Ptr<Socket> localSocket, uint32_t txSpace) {
	//NS_LOG_UNCOND("helooooooooooooooooo WriteUntilBufferFull");
	while (currentTxBytes < totalTxBytes && localSocket->GetTxAvailable() > 0) {

		uint32_t left = totalTxBytes - currentTxBytes;
		uint32_t dataOffset = currentTxBytes % writeSize;
		uint32_t toWrite = writeSize - dataOffset;
		toWrite = std::min (toWrite, left);
		toWrite = std::min (toWrite, localSocket->GetTxAvailable ());

		Ptr<Packet> p = Create<Packet>(&data[dataOffset], toWrite);
		Ptr<Node> startingNode = localSocket->GetNode();
		Ptr<VlcTxNetDevice> txOne = DynamicCast<VlcTxNetDevice>(startingNode->GetDevice(0) );
		txOne->EnqueueDataPacket(p);

		int amountSent = localSocket->Send (&data[dataOffset], toWrite, 0);
		if(amountSent < 0)
		{
			// we will be called again when new tx space becomes available.
			return;
		}

		currentTxBytes += amountSent;
	}

	localSocket->Close();
}

std::vector<double>& GenerateSignal(int size, double dutyRatio) {
	std::vector<double> *result = new std::vector<double>();
	result->reserve(size);

	double bias = 0;
	double Vmax = 4.5;
	double Vmin = 0.5;

	for (int i = 0; i < size; i++) {
		if (i < size * dutyRatio) {
			result->push_back(Vmax + bias);
		} else {
			result->push_back(Vmin + bias);
		}
	}

	return *result;
}

void PrintPacketData(Ptr<const Packet> p, uint32_t size) {
	uint8_t *data = new uint8_t[size];

	p->CopyData(data, size);

	for (uint32_t i = 0; i < size; i++) {
		std::cout << (int) data[i] << " ";
	}

	std::cout << std::endl;

	delete[] data;

}
