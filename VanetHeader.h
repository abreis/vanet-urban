/*
 * VanetHeader.h
 *
 *  Created on: Sep 26, 2011
 *      Author: andrer
 */

#ifndef VANETHEADER_H_
#define VANETHEADER_H_

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/header.h"
#include <iostream>

using namespace ns3;

class VanetHeader : public Header
{
	private:
		int16_t m_src;
		uint16_t m_id;
		uint64_t m_timestamp;
//		int64_t m_posRX;	// vehicle position when packet was received
//		int64_t m_posTX;	// vehicle position right before packet is transmitted
	public:
		VanetHeader();
		virtual ~VanetHeader();

		void SetSource(int src);
		void SetID(unsigned int id);
		void SetTimestamp(double time);
//		void SetPosRX(double pos);
//		void SetPosTX(double pos);
		int GetSource(void) const;
		unsigned int GetID(void) const;
		double GetTimestamp(void) const;
//		double GetPosRX(void);
//		double GetPosTX(void);


		static TypeId GetTypeId (void);
		virtual TypeId GetInstanceTypeId (void) const;
		virtual void Print (std::ostream &os) const;
		virtual void Serialize (Buffer::Iterator start) const;
		virtual uint32_t Deserialize (Buffer::Iterator start);
		virtual uint32_t GetSerializedSize (void) const;
};
#endif /* VANETHEADER_H_ */
