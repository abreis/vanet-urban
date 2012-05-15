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

#ifndef CLASS_VEHICLE_
#define CLASS_VEHICLE_

#include "ns3/ptr.h"
#include "ns3/object.h"
#include "VanetHeader.h"
#include <list>

namespace ns3
{
	class Vehicle;
	typedef Callback<void, Ptr<Vehicle>, VanetHeader> VehicleReceiveCallback; // define type VehicleReceiveCallback.

	class Vehicle : public ns3::Object
	{
	private:
		bool is_alive;				// vehicle is active
		bool is_rsu;					// vehicle is an RSU
		bool is_silent;				// silent: no rebroadcast
		bool is_parked;
		double m_velocity;            // vehicle's velocity.
		int m_vehicleId;              // vehicle's id
		list<VanetHeader> p_buffer;	// packet storage

		VehicleReceiveCallback m_receive; // Catches the event when a packet is received

	public:
		static TypeId GetTypeId (void); // Override TypeId

		Vehicle();
		~Vehicle();

		int GetVehicleId();
		void SetVehicleId(int value);
		double GetVelocity();
		void SetVelocity(double value);
		bool IsAlive();
		void SetAlive(bool status);
		bool IsParked();
		void SetParked(bool parked);
		bool IsRSU();
		void SetRSU(bool rsu);

		list<VanetHeader> GetPacketList();
		void AddPacket(VanetHeader header);

		VehicleReceiveCallback GetReceiveCallback(); // returns the vehicle's receive callback.
		void SetReceiveCallback(VehicleReceiveCallback receive); // sets the Vehicle's receive callback which is being used to handle the receive data event.
		bool ReceivePacket(VanetHeader packet); // ReceiverPacket handler. This is the handler which catches the Vehicle's Receive Data event.

	protected:

  };
};
#endif
