/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005-2009 Old Dominion University [ARBABI]
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
 * Author: Hadi Arbabi <marbabi@cs.odu.edu>
 */

#ifndef CLASS_VEHICLE_
#define CLASS_VEHICLE_

#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/vector.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "VanetHeader.h"
#include <list>

namespace ns3
{
  class Vehicle;
  
  /// define type VehicleReceiveCallback.
//  typedef Callback<void, Ptr<Vehicle>, Ptr<const Packet>, Address> VehicleReceiveCallback;
  typedef Callback<void, Ptr<Vehicle>, VanetHeader> VehicleReceiveCallback;

  /**
  * \brief Vehicle is a mobile Object which follows the given IDM/MOBIL mobility Model and LaneChange rules.
  *
  * Each Vehicle has the dimension [length and width] and can be positioned by a Vector.
  * The position of the Vehicle is the position of its node which it owns.
  * A Vehicle can have directions +1 or -1; and it can belong to a particular lane of the roadway [Highway].
  * Its current acceleration and velocity can be calculated based on the IDM car following Model and LaneChange rules.
  * Vehicle is able to communicate (send and receive) through a common wifi channel with the capablilty of the device it owns.
  * The behavior of a vehicle depends on the IDM/MOBIL models assigned to them.
  * Mac/Phy layer messages including the receive data messages and events can be captured by setting the appropriate callbacks (event handlers). 
  */
  class Vehicle : public ns3::Object
  {
    private: 
    	
	  bool is_alive;				// vehicle is active
      bool is_rsu;					// vehicle is an RSU
      bool is_silent;				// silent: no rebroadcast
      double m_velocity;            // vehicle's velocity.
      int m_vehicleId;              // vehicle's id

      list<VanetHeader> p_buffer;	// packet storage

	  /// Catching an event when a packet is received.
      VehicleReceiveCallback m_receive;
    		
    public:

      /// Override TypeId.
      static TypeId GetTypeId (void);
      /// Constructor to initialize values of all variables to zero except VehicleId to one.
      Vehicle();
	  /// Destructor [does nothing].
      ~Vehicle();
      /**
      * \returns the Vehicle Id.
	  *
      */
      int GetVehicleId();
      /**
      * \param value a Vehicle Id.
	  *
	  * A Vehicle can have an Id. It is good that this Id be unique in the VANETs Highway.  
      */
      void SetVehicleId(int value);
      /**
      * \returns the current velocity (speed) of the Vehicle.
      */
      double GetVelocity();
      /**
      * \param value the current velocity (speed) of the Vehicle.
      */
      void SetVelocity(double value);
      /**
      * \param dt an interval dt. [passed time or time to pass]
      *
      * Translates the Vehicle to the next position in the roadway by knowing interval dt, velocity, and the direction.
      * Position += Velocity * dt * Direction.
      */
//      virtual void TranslatePosition(double dt);
      /**
      * \param dt the interval dt. [passed time or time to pass].
      *
      * Translates the Vehicle to the next velocity(speed) by knowing interval dt, velocity, and acceleration.
      * Velocity += Acceleration * dt.
      */
      virtual void TranslateVelocity(double dt);

      /**
      * \param the vehicle's desired status.
      */
      void SetAlive(bool status);
      /**
      * \returns the vehicle's status.
	  *
	  * A terminated vehicle should return false.
      */
      bool IsAlive();


      list<VanetHeader> GetPacketList();
      void AddPacket(VanetHeader header);


      /// returns the vehicle's receive callback.
      VehicleReceiveCallback GetReceiveCallback();
      /// sets the Vehicle's receive callback which is being used to handle the receive data event.
      void SetReceiveCallback(VehicleReceiveCallback receive);

      /// ReceiverPacket handler. This is the handler which catches the Vehicle's Receive Data event.
	  bool ReceivePacket(VanetHeader packet);
//      bool ReceivePacket(Ptr<NetDevice> device,Ptr<const Packet> packet,uint16_t protocol,const Address& address);
    protected:

  };
};
#endif
