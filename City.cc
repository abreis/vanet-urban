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
		{ }

	void City::Start()
	{
		InitCity();
		Simulator::Schedule(Seconds(0.0), &Step, Ptr<City>(this));
	}

	void City::Stop()
		{ }

	City::City()
	{
		m_dt=0.1;

		/*
		 * Init m_cityGrid and fill the pattern of street/building/parking cells
		 */
		m_cityGrid.resize(2000/5);	//  2km city, 5m cells

		short rowmult=15, colmult=15;

		vector< vector< Cell > >::iterator rowiterator;
		for(rowiterator=m_cityGrid.begin(); rowiterator!=m_cityGrid.end(); rowiterator++)
		{
			rowiterator->resize(2000/5);	// resize each row to 400 columns
			vector< Cell >::iterator coliterator;
			for(coliterator=rowiterator->begin(); coliterator!=rowiterator->end(); coliterator++)
			{
				switch(rowmult)
				{
					case 1:
					case 25:
						switch(colmult)
						{
							case 1:
							case 25:
								coliterator->type=INTERSECTION; break;
							default:
								coliterator->type=ROAD; break;
						}
						break;
					case 2:
					case 24:
						switch(colmult)
						{
							case 1:
							case 25:
								coliterator->type=ROAD; break;
							case 2:
							case 24:
								coliterator->type=BUILDING; break;
							default:
								coliterator->type=PARKING; break;
						}
						break;
					default:
						switch(colmult)
						{
							case 1:
							case 25:
								coliterator->type=ROAD; break;
							case 2:
							case 24:
								coliterator->type=PARKING; break;
							default:
								coliterator->type=BUILDING; break;
						}
						break;
				}
				if(colmult==25) colmult=1; else colmult++;
			}	// end column iterator
			if(rowmult==25) rowmult=1; else rowmult++;
		}	// end row iterator
	}

	City::~City()
	{

	}

	void City::printCityStruct(void)
	{
		vector< vector< Cell > >::const_iterator rowiterator;
		for(rowiterator=m_cityGrid.begin(); rowiterator!=m_cityGrid.end(); rowiterator++)
		{
			vector< Cell >::const_iterator coliterator;
			for(coliterator=rowiterator->begin(); coliterator!=rowiterator->end(); coliterator++)
			{
				switch(coliterator->type)
				{
					case ROAD: cout << 'R'; break;
					case BUILDING: cout << 'B'; break;
					case INTERSECTION: cout << 'I'; break;
					case PARKING: cout << 'P'; break;
				}
			}
			cout << '\n';
		}
	}

	void City::printCityPointVehicles(void)
	{
		vector< vector< Cell > >::const_iterator rowiterator;
		for(rowiterator=m_cityGrid.begin(); rowiterator!=m_cityGrid.end(); rowiterator++)
		{
			vector< Cell >::const_iterator coliterator;
			for(coliterator=rowiterator->begin(); coliterator!=rowiterator->end(); coliterator++)
			{
				if(coliterator->type==BUILDING) cout << ' ';
					else{ if(coliterator->vehicle==0) cout << '0';
						else cout << '1'; }
			}
			cout << '\n';
		}
	}

	void City::Step(Ptr<City> City)
	{
		City->TranslateVehicles();
	}

	void City::TranslateVehicles()
	{
		/*
		 * Rules:
		 * - Update horizontal first, vertical second
		 * - Update a single lane at a time, starting from the front vehicle to the back
		 */

		// start by locating the first horizontal road (3 consecutive RRR)
		vector< vector< Cell > >::const_iterator rowiterator;
		for(rowiterator=m_cityGrid.begin(); rowiterator!=m_cityGrid.end(); rowiterator++)
		{
			vector< Cell >::const_iterator coliterator=rowiterator->begin();
			if(coliterator->type==ROAD && (coliterator+1)->type==ROAD && (coliterator+2)->type==ROAD)
			{
				// we're at a horizontal road
				// first horizontal lane from top runs eastbound, so we're at the start

				// find first vehicle in lane
				for(coliterator=rowiterator->begin(); coliterator!=rowiterator->end(); coliterator++)
				{
					if(coliterator->vehicle!=0) // there's a vehicle here
					{
						// TODO movement rules for eastbound vehicle
					}
				}	// finished running through eastbound lane

				/* * * * * * */

				// now jump to the next lane, and start from the end (westbound traffic)
				rowiterator++;
				for(coliterator=rowiterator->end(); coliterator!=rowiterator->begin(); coliterator--)
				{
					if(coliterator->vehicle!=0) // there's a vehicle here
					{
						// TODO movement rules for westbound vehicle
					}
				}	// finished running through westbound lane
			}

		}	// end of row iterator

		// now repeat for columns

		Simulator::Schedule(Seconds(m_dt), &City::Step, Ptr<City>(this));
	}

	double City::GetDeltaT()
		{ return m_dt; }

	void City::SetDeltaT(double value)
		{ m_dt=value; }

	/* Callbacks */

	Callback<void, Ptr<Vehicle>, VanetHeader> City::GetReceiveDataCallback()
		{ return m_receiveData; }

	void City::SetReceiveDataCallback(Callback<void, Ptr<Vehicle>, VanetHeader> receiveData)
		{ m_receiveData = receiveData; }

	Callback<bool, Ptr<City>, Ptr<Vehicle>, double> City::GetControlVehicleCallback()
		{ return m_controlVehicle; }

	void City::SetControlVehicleCallback(Callback<bool, Ptr<City>, Ptr<Vehicle>, double> controlVehicle)
		{ m_controlVehicle = controlVehicle; }

	Callback<bool, Ptr<City>, int&> City::GetInitVehicleCallback()
		{ return m_initVehicle; }

	void City::SetInitVehiclesCallback(Callback<bool, Ptr<City>, int&> initVehicle)
		{ m_initVehicle = initVehicle; }
}
