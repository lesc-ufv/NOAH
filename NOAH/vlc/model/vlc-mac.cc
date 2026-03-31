// vlc-mac.cc
#include "vlc-mac.h"
#include "vlc-mac-header.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/vlc-channel-model.h"

namespace ns3 {

Ptr<VlcChannel> ch;

NS_LOG_COMPONENT_DEFINE("VlcMac");
NS_OBJECT_ENSURE_REGISTERED(VlcMac);
NS_OBJECT_ENSURE_REGISTERED(VlcMacHeader);

TypeId VlcMac::GetTypeId(void) {
  static TypeId tid = TypeId("ns3::VlcMac")
    .SetParent<Object>()
    .SetGroupName("Vlc")
    .AddConstructor<VlcMac>()
    .AddTraceSource("Tx", "Trace on packets sent by MAC", MakeTraceSourceAccessor(&VlcMac::m_txTrace))
    .AddTraceSource("Rx", "Trace on packets received by MAC", MakeTraceSourceAccessor(&VlcMac::m_rxTrace))
    .AddTraceSource("Drop", "Trace on dropped packets", MakeTraceSourceAccessor(&VlcMac::m_dropTrace));
  return tid;
}

VlcMac::VlcMac() : m_state(IDLE), m_seqNum(0), m_retryCount(0), m_maxRetries(3) {
  m_ackTimer.SetFunction(&VlcMac::HandleAckTimeout, this);
}

VlcMac::~VlcMac() {}

void VlcMac::SetTxDevice(Ptr<VlcTxNetDevice> tx) { m_txDevice = tx; }
void VlcMac::SetRxDevice(Ptr<VlcRxNetDevice> rx) { m_rxDevice = rx; }
void VlcMac::SetAddress(Mac48Address addr) { m_address = addr; }
Mac48Address VlcMac::GetAddress() const { return m_address; }

void VlcMac::Enqueue(Ptr<Packet> packet, Mac48Address dest) {
  if (m_state != IDLE) {
    NS_LOG_LOGIC("MAC busy; dropping packet");
    m_dropTrace(packet);
    return;
  }
  std::cout << "Enqueued" << std::endl;
  // Add MAC header
  VlcMacHeader header;
  header.SetType(0);  // DATA
  header.SetSeqNum(m_seqNum++);
  header.SetSrc(m_address);
  header.SetDst(dest);
  packet->AddHeader(header);

  m_currentPkt = packet;
  //StartTransmission(packet);
  ch->TransmitDataPacket(packet);
}

void VlcMac::StartTransmission(Ptr<Packet> packet) {
  m_state = TX;
  m_txTrace(packet);
  m_txDevice->EnqueueDataPacket(packet);  // Send via your VLC TX device/channel
  m_ackTimer.Schedule(Seconds(0.1));  // Timeout (adjust based on your delay model)
}

void VlcMac::Receive(Ptr<Packet> packet, bool corrupted) {
  if (corrupted) {

    m_dropTrace(packet);
    return;
  }

  std::cout << "Received" << std::endl;

  VlcMacHeader header;
  packet->RemoveHeader(header);

  if (header.GetDst() != m_address) return;  // Not for us

  m_rxTrace(packet);

  if (header.GetType() == 0) {  // DATA
    // Deliver to upper layer (e.g., via callback)
    // m_rxCallback(packet, header.GetSrc());  // Add your own callback if needed
    // Send ACK
    SendAck(header.GetSrc(), header.GetSeqNum());
  } else if (header.GetType() == 1) {  // ACK
    if (m_state == WAIT_ACK && header.GetSeqNum() == m_seqNum - 1) {
      m_ackTimer.Cancel();
      m_state = IDLE;
      m_retryCount = 0;
      NotifyTxSuccess();
    }
  }
}

void VlcMac::SendAck(Mac48Address src, uint32_t seqNum) {
  Ptr<Packet> ack = Create<Packet>(0);
  VlcMacHeader header;
  header.SetType(1);  // ACK
  header.SetSeqNum(seqNum);
  header.SetSrc(m_address);
  header.SetDst(src);
  ack->AddHeader(header);
  m_txDevice->EnqueueDataPacket(ack);
}

void VlcMac::HandleAckTimeout() {
  if (++m_retryCount > m_maxRetries) {
    m_state = IDLE;
    NotifyTxFailure();
    return;
  }
  // Retransmit
  //StartTransmission(m_currentPkt);
  ch->TransmitDataPacket(m_currentPkt);
  m_state = WAIT_ACK;
}

void VlcMac::NotifyTxSuccess() { NS_LOG_LOGIC("TX success"); /* Clear current pkt */ }
void VlcMac::NotifyTxFailure() { NS_LOG_LOGIC("TX failure after retries"); /* Drop */ }




// VlcMacHeader implementation (omitted for brevity; standard NS-3 header serialize/deserialize)
//TypeId VlcMacHeader::GetTypeId(void) { /* Implement as usual */ }
// ... (Add Serialize, Deserialize, etc.)

}  // namespace ns3