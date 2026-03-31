#ifndef VLC_PACKET_TAG_H
#define VLC_PACKET_TAG_H

#include "ns3/tag.h"

namespace ns3 {

class VlcPacketTag : public Tag
{
public:
  VlcPacketTag ();

  void SetId (uint32_t id);
  uint32_t GetId () const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

private:
  uint32_t m_id;
};

} // namespace ns3

#endif
