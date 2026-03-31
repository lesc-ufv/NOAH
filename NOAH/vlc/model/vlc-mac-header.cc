#include "vlc-mac-header.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("VlcMacHeader");

NS_OBJECT_ENSURE_REGISTERED (VlcMacHeader);

VlcMacHeader::VlcMacHeader ()
  : m_packetId1 (0),
  m_packetId2 (0)
{
}

TypeId
VlcMacHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VlcMacHeader")
    .SetParent<Header> ()
    .AddConstructor<VlcMacHeader> ();
  return tid;
}

TypeId
VlcMacHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
VlcMacHeader::SetPacketId (uint32_t id1,uint32_t id2)
{
  m_packetId1 = id1;
  m_packetId2 = id2;
}

uint32_t
VlcMacHeader::GetPacketId1 (void) const
{
  return m_packetId1;
}
uint32_t
VlcMacHeader::GetPacketId2 (void) const
{
  return m_packetId2;
}

uint32_t
VlcMacHeader::GetSerializedSize (void) const
{
  return 8;
}

void
VlcMacHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU32 (m_packetId1);
  start.WriteHtonU32 (m_packetId2);
}

uint32_t
VlcMacHeader::Deserialize (Buffer::Iterator start)
{
  m_packetId1 = start.ReadNtohU32 ();
  m_packetId2 = start.ReadNtohU32 ();


  return GetSerializedSize ();
}

void
VlcMacHeader::Print (std::ostream &os) const
{
  //os << "packetId=" << m_packetId;
}

} // namespace ns3