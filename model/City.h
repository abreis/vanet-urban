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

#include <vector>
#include <list>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/object.h"
#include "ns3/random-variable.h"
#include "ns3/simulator.h"
#include "Vehicle.h"
#include "VanetHeader.h"


namespace ns3
{
	enum CellType {ROAD, INTERSECTION, BUILDING, PARKING};
	enum CellOrientation {TOPLEFT, TOPRIGHT, RIGHTTOP, RIGHTBOTTOM, BOTTOMRIGHT, BOTTOMLEFT, LEFTBOTTOM, LEFTTOP, RANDOM};

	// Setup a grid-like system for cars to travel in
	class City: public ns3::Object
	{
		private:
			/*
			 * Grid size and cell size are hardcoded to 2km, 5m, 160000 cells
			 */
			typedef struct
			{
				Ptr<Vehicle> vehicle;
				CellType type;
			} Cell;

			int m_gridSize;						// side (in cells) of cell grid
			double m_dt;						// step interval
			double m_range;						// vehicle radio range
			bool m_debug;						// debug message flag

			UniformVariable randomNum;			// generates uniform numbers between 0.0 and 1.0
			double m_probPark;					// probability of parking
			double m_probTurnLeft;				// probability of turning left
			double m_probTurnRight;				// probability of turning right

			std::vector< std::vector< Cell > > m_cityGrid;
			std::list< Ptr<Vehicle> > m_Vehicles;		// holds vehicles in the network
			std::list< Ptr<Vehicle> > m_parkedVehicles;	// holds vehicles that parked
			std::list< Ptr<Vehicle> > m_RSUs;			// holds vehicles that became RSUs

			void InitCity();
			void TranslateVehicles();

			Callback<bool, Ptr<City> ,Ptr<Vehicle> , double> m_controlVehicle;
			Callback<bool, Ptr<City> > m_initVehicles;
			VehicleReceiveCallback m_receiveData;

		public:
			static TypeId GetTypeId (void);		// Override TypeId
			City();
			~City();

			void Start();
			void Stop();

			double GetDeltaT(void);
			void SetDeltaT(double value);
			void SetParkProb(double value);
			void SetGridSize(int value);
			void SetDebug(bool value);

			Ptr<Vehicle> CreateVehicle (void);
			void AddVehicle(Ptr<Vehicle> veh, CellOrientation ort);
			void RandomAddVehicles(int number, double interval);
			void printCityStruct(void);
			void printCityPointVehicles(void);

			VehicleReceiveCallback GetReceiveDataCallback();
			void SetReceiveDataCallback(VehicleReceiveCallback receiveData);
			Callback<bool, Ptr<City> ,Ptr<Vehicle> , double> GetControlVehicleCallback();
			void SetControlVehicleCallback(Callback<bool, Ptr<City> ,Ptr<Vehicle> , double> controlVehicle);
			Callback<bool, Ptr<City> > GetInitVehiclesCallback();
			void SetInitVehiclesCallback(Callback<bool, Ptr<City> > initVehicles);
			/**
			* Runs one mobility Step for the given City.
			* This function is called each interval dt to simulated the mobility through TranslateVehicles().
			*/
			static void Step(Ptr<City> City);

		protected:

	};
};
#endif
