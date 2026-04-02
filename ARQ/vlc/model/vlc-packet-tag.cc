#include "vlc-packet-tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

VlcPacketTag::VlcPacketTag ()
  : m_id (0)
{
}

TypeId
VlcPacketTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::VlcPacketTag")
    .SetParent<Tag> ()
    .AddConstructor<VlcPacketTag> ();
  return tid;
}

TypeId
VlcPacketTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
VlcPacketTag::SetId (uint32_t id)
{
  m_id = id;
}

uint32_t
VlcPacketTag::GetId () const
{
  return m_id;
}

uint32_t
VlcPacketTag::GetSerializedSize (void) const
{
  return 4; // uint32_t
}

void
VlcPacketTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_id);
}

void
VlcPacketTag::Deserialize (TagBuffer i)
{
  m_id = i.ReadU32 ();
}

void
VlcPacketTag::Print (std::ostream &os) const
{
  os << "id=" << m_id;
}

} // namespace ns3
