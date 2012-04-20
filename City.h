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

#ifndef CLASS_CITY_
#define CLASS_CITY_

#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/random-variable.h"
#include "ns3/vector.h"
#include "Vehicle.h"
#include "Model.h"
#include "LaneChange.h"
#include <vector>

namespace ns3
{

	// Setup a grid-like system for cars to travel in
	class City: public ns3::Object
	{
		private:

			std::vector< std::vector< Ptr<Vehicle> > > m_cityGrid;
			/*
			 * Then for each line fill the pattern of street/building/parking cells
			 */

			/// Initializes the City and raises the event InitVehicle
			void InitCity();


		public:
			/// Override TypeId
			static TypeId GetTypeId (void);
			/// Constructor to set default values
			City();
			/// Destructor
			~City();
			/**
			* Starts the City
			*/
			void Start();
			/**
			* Stops the City
			*/
			void Stop();

			/// Returns the City's Receive Data callback.
			VehicleReceiveCallback GetReceiveDataCallback();
			/// Sets the City's Receive Data callback.
			void SetReceiveDataCallback(VehicleReceiveCallback receiveData);
			/// Returns the City Control Vehicle callback.
			Callback<bool, Ptr<City> ,Ptr<Vehicle> , double> GetControlVehicleCallback();
			/// Sets the City Control Vehicle callback.
			void SetControlVehicleCallback(Callback<bool, Ptr<City> ,Ptr<Vehicle> , double> controlVehicle);
			/// Returns the City Init Vehicle callback.
			Callback<bool, Ptr<City>, int&> GetInitVehicleCallback();
			/// Sets the City Init Vehicle callback.
			void SetInitVehicleCallback(Callback<bool, Ptr<City>, int&> initVehicle);
			/**
			* Runs one mobility Step for the given City.
			* This function is called each interval dt to simulated the mobility through TranslateVehicles().
			*/
			static void Step(Ptr<City> City);

	};
};
#endif
