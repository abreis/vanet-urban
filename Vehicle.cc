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

#include "Vehicle.h"

namespace ns3
{	
	TypeId Vehicle::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::Vehicle")
			.SetParent<Object> ()
			.AddConstructor<Vehicle> ();
		return tid;
	}

	Vehicle::Vehicle()
	{
		m_vehicleId = 1;
		m_velocity = 0.0;
		is_alive=true;
	}

	Vehicle::~Vehicle()
	{ }

	int Vehicle::GetVehicleId()
		{ return m_vehicleId; }

	void Vehicle::SetVehicleId(int value)
		{ m_vehicleId=value; }

	double Vehicle::GetVelocity()
		{ return m_velocity; }

	void Vehicle::SetVelocity(double value)
		{ m_velocity=value; }

	bool Vehicle::ReceivePacket(VanetHeader packet)
	{
		if(!m_receive.IsNull())
			m_receive(Ptr<Vehicle>(this), packet);
		return true;
	}

	void Vehicle::SetAlive(bool status)
		{ is_alive = status; }

	bool Vehicle::IsAlive()
		{ return is_alive; }

	list<VanetHeader> Vehicle::GetPacketList()
		{ return p_buffer; }

	void Vehicle::AddPacket(VanetHeader header)
		{ p_buffer.push_back(header); }

	VehicleReceiveCallback Vehicle::GetReceiveCallback()
		{ return m_receive; }

	void Vehicle::SetReceiveCallback(VehicleReceiveCallback receive)
		{ m_receive = receive; }
}
