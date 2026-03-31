// vlc-mac.h
#ifndef VLC_MAC_H
#define VLC_MAC_H

#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/mac48-address.h"
#include "ns3/timer.h"
#include "ns3/traced-callback.h"
#include "vlc-tx-net-device.h"  // Include your TX device
#include "vlc-rx-net-device.h"  // Include your RX device

namespace ns3 {


class VlcRxNetDevice;
class VlcTxNetDevice;

class VlcMac : public Object {
public:
  static TypeId GetTypeId(void);
  VlcMac();
  virtual ~VlcMac();

  void SetTxDevice(Ptr<VlcTxNetDevice> tx);
  void SetRxDevice(Ptr<VlcRxNetDevice> rx);
  void SetAddress(Mac48Address addr);
  Mac48Address GetAddress() const;

  // Enqueue packet from upper layer (e.g., IP/UDP)
  void Enqueue(Ptr<Packet> packet, Mac48Address dest);

  // Callback for when packet is received from channel
  void Receive(Ptr<Packet> packet, bool corrupted);

private:
  enum State { IDLE, TX, WAIT_ACK };
  State m_state;

  Ptr<VlcTxNetDevice> m_txDevice;
  Ptr<VlcRxNetDevice> m_rxDevice;
  Mac48Address m_address;

  uint32_t m_seqNum;  // Sequence number for ARQ
  Ptr<Packet> m_currentPkt;  // Packet waiting for ACK
  Timer m_ackTimer;  // Timeout for retransmission
  uint8_t m_retryCount;  // Retry limit (e.g., max 3 retries)
  uint8_t m_maxRetries;

  void StartTransmission(Ptr<Packet> packet);
  void SendAck(Mac48Address src, uint32_t seqNum);
  void HandleAckTimeout();
  void NotifyTxSuccess();
  void NotifyTxFailure();

  // Traces for debugging
  TracedCallback<Ptr<const Packet> > m_txTrace;
  TracedCallback<Ptr<const Packet> > m_rxTrace;
  TracedCallback<Ptr<const Packet> > m_dropTrace;
};

}  // namespace ns3

#endif