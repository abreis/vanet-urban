/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Andre Braga Reis
 *
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
 * Author: Andre Braga Reis <abreis@cmu.edu>
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
		.AddConstructor<VanetHeader> ();
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
