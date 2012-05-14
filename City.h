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
#include "LaneChange.h"
#include <vector>

namespace ns3
{

	// Setup a grid-like system for cars to travel in
	class City: public ns3::Object
	{
		private:
			/*
			 * Grid size and cell size are hardcoded to 2km, 5m, 160000 cells
			 */
			enum CellType {ROAD, INTERSECTION, BUILDING, PARKING};
			typedef struct
			{
				Ptr<Vehicle> vehicle;
				CellType type;
			} Cell;

			double m_dt;                          // the mobility step interval. (duraion between each step)
			double m_range;						// vehicle radio range

			std::vector< std::vector< Cell > > m_cityGrid;

			void InitCity();
			void TranslateVehicles();

			Callback<bool, Ptr<City> ,Ptr<Vehicle> , double> m_controlVehicle;
			Callback<bool, Ptr<City>, int&> m_initVehicle;
			VehicleReceiveCallback m_receiveData;


		public:
			static TypeId GetTypeId (void);		// Override TypeId
			City();
			~City();

			void Start();
			void Stop();

			double GetDeltaT(void);
			void SetDeltaT(double value);

			void printCityStruct(void);
			void printCityPointVehicles(void);

			VehicleReceiveCallback GetReceiveDataCallback();
			void SetReceiveDataCallback(VehicleReceiveCallback receiveData);
			Callback<bool, Ptr<City> ,Ptr<Vehicle> , double> GetControlVehicleCallback();
			void SetControlVehicleCallback(Callback<bool, Ptr<City> ,Ptr<Vehicle> , double> controlVehicle);
			Callback<bool, Ptr<City>, int&> GetInitVehicleCallback();
			void SetInitVehiclesCallback(Callback<bool, Ptr<City>, int&> initVehicle);
			/**
			* Runs one mobility Step for the given City.
			* This function is called each interval dt to simulated the mobility through TranslateVehicles().
			*/
			static void Step(Ptr<City> City);

	};
};
#endif
