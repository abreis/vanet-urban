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

#include <iostream>
#include "City.h"
#include "ns3/simulator.h"
#include <math.h>

namespace ns3
{
	TypeId City::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::City").SetParent<Object>().AddConstructor<City>();
		return tid;
	}

	void City::InitCity()
	{

	}


	City::~City()
	{
		// init m_cityGrid
		m_cityGrid.resize(2000/5);	//  2km city, 5m cells
		// get an iterator and add lines of cells

	}

	void City::Step(Ptr<City> City)
	{
		City->TranslateVehicles();
	}

	void City::Start()
	{
		m_stopped=false;
		InitCity();
		Simulator::Schedule(Seconds(0.0), &Step, Ptr<City>(this));
	}

	void City::Stop()
	{
		m_stopped=true;
	}
 
	double City::GetDeltaT()
	{
		return m_dt;
	}

	void City::SetDeltaT(double value)
	{
		if(value<=0)
			value=0.1;

		m_dt=value;
	}

	Callback<void, Ptr<Vehicle>, VanetHeader> City::GetReceiveDataCallback()
	{
		return m_receiveData;
	}

	void City::SetReceiveDataCallback(Callback<void, Ptr<Vehicle>, VanetHeader> receiveData)
	{
		m_receiveData = receiveData;
	}

	Callback<bool, Ptr<City>, Ptr<Vehicle>, double> City::GetControlVehicleCallback()
	{
		return m_controlVehicle;
	}

	void City::SetControlVehicleCallback(Callback<bool, Ptr<City>, Ptr<Vehicle>, double> controlVehicle)
	{
		m_controlVehicle = controlVehicle;
	}

	Callback<bool, Ptr<City>, int&> City::GetInitVehicleCallback()
	{
		return m_initVehicle;
	}

	void City::SetInitVehicleCallback(Callback<bool, Ptr<City>, int&> initVehicle)
	{
		m_initVehicle = initVehicle;
	}
}
