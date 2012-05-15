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

#ifndef VANETHEADER_H_
#define VANETHEADER_H_

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/header.h"
#include "ns3/wifi-module.h"
#include <iostream>

using namespace ns3;

class VanetHeader : public Header
{
	private:
		int16_t m_src;
		uint16_t m_id;
		uint64_t m_timestamp;
	public:
		VanetHeader();
		virtual ~VanetHeader();

		void SetSource(int src);
		void SetID(unsigned int id);
		void SetTimestamp(double time);
		int GetSource(void) const;
		unsigned int GetID(void) const;
		double GetTimestamp(void) const;

		static TypeId GetTypeId (void);
		virtual TypeId GetInstanceTypeId (void) const;
		virtual void Print (std::ostream &os) const;
		virtual void Serialize (Buffer::Iterator start) const;
		virtual uint32_t Deserialize (Buffer::Iterator start);
		virtual uint32_t GetSerializedSize (void) const;
};
#endif /* VANETHEADER_H_ */
