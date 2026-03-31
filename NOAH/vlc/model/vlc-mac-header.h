#ifndef VLC_MAC_HEADER_H
#define VLC_MAC_HEADER_H

#include "ns3/header.h"
#include "ns3/type-id.h"
#include <ostream>

namespace ns3 {

/**
 * \ingroup vlc
 * \brief Header MAC simples para VLC
 *
 * Este header carrega um identificador lógico de pacote
 * que atravessa o canal e pode ser recuperado no nó receptor.
 */
class VlcMacHeader : public Header
{
public:
  VlcMacHeader ();

  /**
   * \brief Define o ID lógico do pacote
   * \param id identificador do pacote
   */
  void SetPacketId (uint32_t id1,uint32_t id2);

  /**
   * \brief Retorna o ID lógico do pacote
   * \return identificador do pacote
   */
  uint32_t GetPacketId1 (void) const;
  uint32_t GetPacketId2 (void) const;

  /**
   * \brief Retorna o TypeId da classe
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Retorna o TypeId da instância
   */
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * \brief Tamanho do header serializado (em bytes)
   */
  virtual uint32_t GetSerializedSize (void) const;

  /**
   * \brief Serializa o header no buffer
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Desserializa o header do buffer
   * \return número de bytes lidos
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * \brief Imprime o conteúdo do header
   */
  virtual void Print (std::ostream &os) const;

private:
  uint32_t m_packetId1;
  uint32_t m_packetId2; //!< ID lógico do pacote
};

} // namespace ns3

#endif /* VLC_MAC_HEADER_H */