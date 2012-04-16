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
#include "Model.h"
#include "LaneChange.h"
#include "VanetHeader.h"
#include <list>

namespace ns3
{
  class LaneChange;
  class Model;
  class HadiHeader;
  
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
	  int m_lane;                   // vehicle's lane.
      double m_length;              // vehicle's length.
      double m_width;               // vehicle's width.
      double m_velocity;            // vehicle's velocity.
      double m_acceleration;        // vehicle's acceleration.
      int m_direction;              // vehicle's direction.
      Ptr<Model> m_model;           // vehicle's IDM mobility model.
      Ptr<LaneChange> m_laneChange; // vehicle's IDM/MOBIL lanechange Model.
      int m_vehicleId;              // vehicle's id
      Ptr<Node> m_node;             // vehicle has a node
      Ptr<NetDevice> m_device;      // vehicle has a device
      bool m_brake;					// vehicle's braking status.
      bool m_control;				// vehicle is manually controlled
      list<VanetHeader> p_buffer;	// packet storage
      double m_brakingAccel;		// sudden braking acceleration

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
      * \param wifi a WifiHelper.
      * \param phy  a YansWifiPhyHelper.
      * \param mac  a NqosWifiMacHelper.
      *
      * Setups the Vehicle wifi given the appropriate helpers.
	  * This function may need to be modified later if stable WAVE/DSRC standards added to ns-3.
      */
//      void SetupWifi(const WifiHelper &wifi, const YansWifiPhyHelper &phy, const NqosWifiMacHelper &mac);
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
      * \returns the IDM mobility Model of Vehicle.
      */
      Ptr<Model> GetModel();
      /**
      * \param value a IDM mobility Model.
	  *
	  * A Vehicle behavior in car following models is based on the parameters set in the Model.
      */
      void SetModel(Ptr<Model> value);
      /**
      * \returns the MOBIL LaneChange model.
      */
      Ptr<LaneChange> GetLaneChange();
      /**
      * \param value a LaneChange model.
	  *
	  * LaneChange model of each vehicle is applied to consider possibility of changing lanes in multilane roadways.
      */
      void SetLaneChange(Ptr<LaneChange> value);
      /**
      * \returns the position of Vehicle's Node which is loacted at the center back of the Vehicle. 
      */
      Vector GetPosition();
      /**
      * \param value a position Vector.
	  *
	  * This function sets the position of Vehicle's Node. Vehicle's position is its Node's position.
	  * This position Vector must point to the center back of the Vehicle.
      */
      void SetPosition(Vector value);
      /**
      * \returns the length of the Vehicle.
      */
      double GetLength();
      /**
      * \param value the length of the Vehicle. 
      */
      void SetLength(double value);
      /**
      * \returns the width of the Vehicle.
      */
      double GetWidth();
      /**
      * \param value the width of the Vehicle.
      */
      void SetWidth(double value);
      /**
      * \returns the current velocity (speed) of the Vehicle.
      */
      double GetVelocity();
      /**
      * \param value the current velocity (speed) of the Vehicle.
      */
      void SetVelocity(double value);
      /**
      * \param value the current acceleration of the Vehicle.
      */
      void SetAcceleration(double value);
      /**
      * \returns the current accelration of the Vehicle.
      */
      double GetAcceleration(void);
      /**
      * \returns the lane number to which the Vehicle is assigned to.
      */
      int GetLane();
      /**
      * \param value the lane number to which the Vehicle must belong.
      */
      void SetLane(int value);
      /**
      * \returns the braking decision of the Vehicle.
      */
      bool GetBrake();
      /**
      * \param value whether the vehicle brakes [true/false]
      */
      void SetBrake(bool value);
      /**
      * \returns the direction of the Vehicle in verbose 'E' (1) or 'W' (-1).
      */
      char GetCharDirection();
      /**
      * \returns the direction of the Vehicle.
      */
      int GetDirection();
      /**
      * \param value the direction of the Vehicle. Usually (+1) or (-1) used in the Highway. 
      */
      void SetDirection(int value);
      /**
      * \param vwd the Vehicle in front.
      *
      * set the Vehicle acceleration based on the front Vehicle calculated by the Model Acceleration.
      */
      virtual void Accelerate(Ptr<Vehicle> vwd);
      /**
      * \param vwd the Vehicle in front.
      * \returns the calculated acceleration for the Vehicle based on the Vehicle in front and by IDM Model rules and accelration.  
      */
      virtual double Acceleration(Ptr<Vehicle> vwd);
      /**
      * \param dt an interval dt. [passed time or time to pass]
      *
      * Translates the Vehicle to the next position in the roadway by knowing interval dt, velocity, and the direction.
      * Position += Velocity * dt * Direction.
      */
      virtual void TranslatePosition(double dt);
      /**
      * \param dt the interval dt. [passed time or time to pass].
      *
      * Translates the Vehicle to the next velocity(speed) by knowing interval dt, velocity, and acceleration.
      * Velocity += Acceleration * dt.
      */
      virtual void TranslateVelocity(double dt);
      /**
      * \param frontOld the Vehicle in the front at the same lane.
      * \param frontNew the Vehicle in the front at the target lane.
      * \param backNew the Vehicle in back at the target lane.
      * \param toLeft the inclination for the target lane. True: target lane is left, False: target lane is right.
      * \returns true if the change of the lane is possible for the Vehicle.
      */
      virtual bool CheckLaneChange(Ptr<Vehicle> frontOld, Ptr<Vehicle> frontNew, Ptr<Vehicle> backNew, bool toLeft);
      /**
      * \return the Wifi address of the Vehicle.
	  *
	  * Vehicles can use this address to communicate to each other in an unicast fashion.
      */
      Address GetAddress();
      /**
      * \returns the Wifi broadcast address.
	  *
	  * A Vehicle can broadcast messages using this address.
      */
      Address GetBroadcastAddress();
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
      /**
      * \param address the destination address. (Wifi address of the target Vehicle)
      * \param packet the packet to send.
      * \returns ture if the enqueuing/sending was successful, otherwise false.
      */
      bool SendTo(Address address, Ptr<Packet> packet);
      /**
      * \returns vehicle is being controlled manually (true/false)
      */
      bool GetManualControl();
      /**
      * \param vehicle is being controlled manually (true/false)
      */
      void SetManualControl(bool control);

      list<VanetHeader> GetPacketList();
      void AddPacket(VanetHeader header);
	  void SetBrakingAccel(double acc);
	  double GetBrakingAccel(void);

      /// is used for purpose of sorting vehicles based on their positions in a list or a queue.
      static bool Compare(Ptr<Vehicle> v1, Ptr<Vehicle> v2);
      /// returns the vehicle's receive callback.
      VehicleReceiveCallback GetReceiveCallback();
      /// sets the Vehicle's receive callback which is being used to handle the receive data event.
      void SetReceiveCallback(VehicleReceiveCallback receive);
	  /// used for purpose of considering market penetration rate.
	  bool IsEquipped;  

      /// ReceiverPacket handler. This is the handler which catches the Vehicle's Receive Data event.
	  bool ReceivePacket(VanetHeader packet);
//      bool ReceivePacket(Ptr<NetDevice> device,Ptr<const Packet> packet,uint16_t protocol,const Address& address);
    protected:

  };
};
#endif
