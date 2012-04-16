/*
 * VanetHeader.cc
 *
 *  Created on: Sep 26, 2011
 *      Author: andrer
 */

#include "VanetHeader.h"

using namespace ns3;

VanetHeader::VanetHeader()
{
}

VanetHeader::~VanetHeader()
{
}

TypeId VanetHeader::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::VanetHeader")
	.SetParent<Header> ()
	.AddConstructor<VanetHeader> ()
	;
	return tid;
}

TypeId
VanetHeader::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

void
VanetHeader::Print (std::ostream &os) const
{
	// This method is invoked by the packet printing
	// routines to print the content of my header.
	os << "src=" << m_src << std::endl;
	os << "id=" << m_id << std::endl;
	os << "timestamp=" << m_timestamp << std::endl;
//	os << "posRX=" << m_posRX<< std::endl;
//	os << "posTX=" << m_posTX << std::endl;
}

uint32_t
VanetHeader::GetSerializedSize (void) const
{
	// we reserve 12 bytes for our header (2*16+1*64).
	return 12;
}

void
VanetHeader::Serialize (Buffer::Iterator start) const
{
	// we can serialize bytes at the start of the buffer.
	// we write them in network byte order.
	start.WriteHtonU16 (m_src);
	start.WriteHtonU16 (m_id);
	start.WriteHtonU64 (m_timestamp);
//	start.WriteHtonU64 (m_posRX);
//	start.WriteHtonU64 (m_posTX);
}

uint32_t
VanetHeader::Deserialize (Buffer::Iterator start)
{
	// we can deserialize bytes from the start of the buffer.
	// we read them in network byte order and store them
	// in host byte order.
	m_src = start.ReadNtohU16 ();
	m_id = start.ReadNtohU16 ();
	m_timestamp = start.ReadNtohU64 ();
//	m_posRX = start.ReadNtohU64 ();
//	m_posTX = start.ReadNtohU64 ();

	// we return the number of bytes effectively read.
	return 12;
}

void VanetHeader::SetSource(int src)
{
	m_src = (int16_t)src;
}

void VanetHeader::SetID(unsigned int id)
{
	m_id = (uint16_t)id;
}

void VanetHeader::SetTimestamp(double time)
{
	// store a double as a longlong, 6 digits precision
	m_timestamp = (uint64_t) (time*1000000);
}

//void VanetHeader::SetPosRX(double pos)
//{
//	// store a double as a longlong, 6 digits precision
//	m_posRX = (uint64_t) (pos*1000000);
//}
//
//void VanetHeader::SetPosTX(double pos)
//{
//	// store a double as a longlong, 6 digits precision
//	m_posTX = (uint64_t) (pos*1000000);
//}

int VanetHeader::GetSource(void) const
{
	return (int)m_src;
}

unsigned int VanetHeader::GetID(void) const
{
	return (unsigned int)m_id;
}

double VanetHeader::GetTimestamp(void) const
{
	return ((double) m_timestamp)/1000000.0;
}

//double VanetHeader::GetPosRX(void)
//{
//	return ((double) m_posRX)/1000000.0;
//}
//
//double VanetHeader::GetPosTX(void)
//{
//	return ((double) m_posTX)/1000000.0;
//}
