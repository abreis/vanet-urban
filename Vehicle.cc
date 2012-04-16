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

#include "Vehicle.h"

namespace ns3
{	
  TypeId Vehicle::GetTypeId (void)
  {
    static TypeId tid = TypeId ("ns3::Vehicle")
    .SetParent<Object> ()
    .AddConstructor<Vehicle> ()
    ;
    return tid;
  }

  Vehicle::Vehicle()
  {
    m_node=CreateObject<Node>();
	MobilityHelper mobility;
	mobility.Install(m_node);
    m_vehicleId = 1;
    m_lane = 0;
    m_direction= 0;
    m_velocity = 0.0;
    m_acceleration= 0.0;
    m_model = 0;
    m_laneChange = 0;
    m_length = 0;
    m_width = 0;
	IsEquipped=true;
	is_alive=true;
	m_control=false;
  }

  Vehicle::~Vehicle()
  {
  }

//  void Vehicle::SetupWifi(const WifiHelper &wifi, const YansWifiPhyHelper &phy, const NqosWifiMacHelper &mac)
//  {
//    if(IsEquipped==false) return;
//    NetDeviceContainer d;
//	NodeContainer n(m_node);
//    d = wifi.Install(phy, mac, n);
//
//    m_device = d.Get(0);
//    m_device->SetReceiveCallback(MakeCallback(&Vehicle::ReceivePacket, this));
//  }

  void Vehicle::SetDirection(int value)
  {
    m_direction=value;
  }

  void Vehicle::SetBrake(bool value)
  {
	  m_brake=value;
  }

  int Vehicle::GetDirection()
  {
    return m_direction;
  }

  bool Vehicle::GetBrake()
  {
	  return m_brake;
  }

  char Vehicle::GetCharDirection()
  {
	return m_direction==1 ? 'E' : 'W';
  }

  int Vehicle::GetVehicleId()
  {
    return m_vehicleId;
  }

  void Vehicle::SetVehicleId(int value)
  {
    m_vehicleId=value;
  }

  Ptr<Model> Vehicle::GetModel()
  {
    return m_model;
  }

  void Vehicle::SetModel(Ptr<Model> value)
  {
    m_model=value;
  }

  Ptr<LaneChange> Vehicle::GetLaneChange()
  {
    return m_laneChange;
  }

  void Vehicle::SetLaneChange(Ptr<LaneChange> value)
  {
    m_laneChange=value;
  }

  Vector Vehicle::GetPosition()
  {
    return m_node->GetObject<MobilityModel>()->GetPosition();
  }

  void Vehicle::SetPosition(Vector value)
  {
    m_node->GetObject<MobilityModel>()->SetPosition(value);
  }

  double Vehicle::GetLength()
  {
    return m_length;
  }

  void Vehicle::SetLength(double value)
  {
    if(value < 0) 
	  value=0;
    m_length=value;
  }

  double Vehicle::GetWidth()
  {
    return m_width;
  }

  void Vehicle::SetWidth(double value)
  {
    if(value < 0)
	  value=0;
    m_width=value;
  }

  double Vehicle::GetVelocity()
  {
    return m_velocity;
  }

  void Vehicle::SetVelocity(double value)
  {
    m_velocity=value;
  }

  double Vehicle::GetAcceleration()
  {
    return m_acceleration;
  }

  void Vehicle::SetAcceleration(double acc)
  {
    m_acceleration=acc;
  }

  int Vehicle::GetLane()
  {
    return m_lane;
  }

  void Vehicle::SetLane(int value)
  {
    m_lane=value;
  }

  void Vehicle::Accelerate(Ptr<Vehicle> vwd)
  {
    m_acceleration = Acceleration(vwd);
  }

  double Vehicle::Acceleration(Ptr<Vehicle> vwd)
  {
    return m_model->CalculateAcceleration(Ptr<Vehicle>(this), vwd);       
  }

  void Vehicle::TranslatePosition(double dt)
  {
    Vector v = this->GetPosition();
    v.x += dt * m_velocity * m_direction;
    this->SetPosition(v);
  }

  void Vehicle::TranslateVelocity(double dt)
  {
    m_velocity += (m_acceleration * dt);
    if(m_velocity<=0.0)
      {
	    m_velocity=0.0;
      }
  }

  bool Vehicle::CheckLaneChange(Ptr<Vehicle> frontOld, Ptr<Vehicle> frontNew, Ptr<Vehicle> backNew, bool toLeft)
  {
    return m_laneChange->CheckLaneChange(Ptr<Vehicle>(this), frontOld, frontNew, backNew, toLeft);
  }

  bool Vehicle::Compare(Ptr<Vehicle> v1, Ptr<Vehicle> v2)
  {
    if(v1->GetDirection() == 1)
      {
	    if (v1->GetPosition().x > v2->GetPosition().x) return true;
	    else return false;
      }
    else
      {
	    if (v1->GetPosition().x < v2->GetPosition().x) return true;
	    else return false;
      }
  }

//  bool Vehicle::ReceivePacket(Ptr<NetDevice> device, Ptr<const Packet> packet,uint16_t protocol,const Address& address)
//  {
//    if(!m_receive.IsNull())
//	  m_receive(Ptr<Vehicle>(this), packet, address);
//    return true;
//  }

	bool Vehicle::ReceivePacket(VanetHeader packet)
	{
	  if(!m_receive.IsNull())
		  m_receive(Ptr<Vehicle>(this), packet);
	  return true;
	}


  Address Vehicle::GetAddress()
  {
    return m_device->GetAddress();
  }

  Address Vehicle::GetBroadcastAddress()
  {
    return m_device->GetBroadcast();
  }

  void Vehicle::SetAlive(bool status)
  {
	is_alive = status;
  }

  bool Vehicle::IsAlive()
  {
    return is_alive;
  }

  bool Vehicle::GetManualControl()
  {
	  return m_control;
  }

  void Vehicle::SetManualControl(bool control)
  {
	  m_control = control;
  }

  list<VanetHeader> Vehicle::GetPacketList()
  {
    return p_buffer;
  }

  void Vehicle::AddPacket(VanetHeader header)
  {
    p_buffer.push_back(header);
  }

  void Vehicle::SetBrakingAccel(double acc)
  {
	  m_brakingAccel = acc;
  }

  double Vehicle::GetBrakingAccel(void)
  {
	  return m_brakingAccel;
  }

  bool Vehicle::SendTo(Address address, Ptr<Packet> packet)
  {
    return m_device->Send(packet, address, 1);
  }

  VehicleReceiveCallback Vehicle::GetReceiveCallback()
  {
    return m_receive;
  }

  void Vehicle::SetReceiveCallback(VehicleReceiveCallback receive)
  {
    m_receive = receive;
  }
}
