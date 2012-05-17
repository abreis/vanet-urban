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

#include "City.h"

namespace ns3
{
	TypeId City::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::City").SetParent<Object>().AddConstructor<City>();
		return tid;
	}

	void City::InitCity()
	{
		/*
		 * Init m_cityGrid and fill the pattern of street/building/parking cells
		 */
		m_cityGrid.resize(m_gridSize);	//  2km city, 5m cells

		short rowmult=15, colmult=15;

		vector< vector< Cell > >::iterator rowiterator;
		for(rowiterator=m_cityGrid.begin(); rowiterator!=m_cityGrid.end(); rowiterator++)
		{
			rowiterator->resize(m_gridSize);	// resize each row to 400 columns
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

		if(!m_initVehicles.IsNull())
		{
			m_initVehicles(Ptr<City>(this));
		}

	}

	void City::Start()
	{
		InitCity();
		Simulator::Schedule(Seconds(0.0), &Step, Ptr<City>(this));
	}

	void City::Stop()
		{ }

	City::City()
	{
		/* set defaults */
		m_dt=1;
		m_gridSize=2000/5;
		m_probPark = 0.001;
		m_probTurnLeft = 0.25;
		m_probTurnRight = 0.25;
	}

	City::~City()
	{

	}

	void City::printCityStruct(void)
	{
		int rowNum=0;
		vector< vector< Cell > >::const_iterator rowiterator;
		for(rowiterator=m_cityGrid.begin(); rowiterator!=m_cityGrid.end(); rowiterator++)
		{
			cout << setw(3) << ++rowNum << ' ';
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
		int rowNum=0;
//		vector< vector< Cell > >::const_iterator rowiterator;
//		for(rowiterator=m_cityGrid.begin(); rowiterator!=m_cityGrid.end(); rowiterator++)
		// TODO this blows up
		for(int iRow = 1; iRow<=m_gridSize; iRow++)
		{
			cout << setw(3) << ++rowNum << ' ';
//			vector< Cell >::const_iterator coliterator;
//			for(coliterator=rowiterator->begin(); coliterator!=rowiterator->end(); coliterator++)
			for(int iCol = 1; iCol<=m_gridSize; iCol++)
			{
				if(m_cityGrid[iRow][iCol].type==BUILDING) cout << ' ';
					else { if(m_cityGrid[iRow][iCol].vehicle==0) cout << '0'; else cout << '1'; }
			}
			cout << '\n';
		}
	}

	void City::Step(Ptr<City> City)
	{
		ns3::Time nowtime = ns3::Simulator::Now();
		cout << nowtime.ns3::Time::GetSeconds() << " STEP\n";

		City->TranslateVehicles();
		// DEBUG
		City->printCityPointVehicles();
	}

	Ptr<Vehicle> City::CreateVehicle (void)
	{
		Ptr<Vehicle> veh = CreateObject<Vehicle>();
		veh->SetReceiveCallback(m_receiveData);
		return veh;
	}


	void City::AddVehicle(Ptr<Vehicle> veh, CellOrientation ort)
	{
		ns3::Time nowtime = ns3::Simulator::Now();
		cout << nowtime.ns3::Time::GetSeconds() << " NEW \n";

		int iRow, iCol;
		switch(ort)
		{
		case LEFTTOP:	// westbound
			for(iRow = 1; !(m_cityGrid[iRow][1].type==ROAD && m_cityGrid[iRow][2].type==ROAD && m_cityGrid[iRow][3].type==ROAD); iRow++)
			for(iCol = 1; m_cityGrid[iRow+1][iCol].vehicle!=0; iCol++);	// find empty cell on westbound lane
			m_cityGrid[iRow+1][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		case RIGHTTOP:	// eastbound
			for(iRow = 1; !(m_cityGrid[iRow][1].type==ROAD && m_cityGrid[iRow][2].type==ROAD && m_cityGrid[iRow][3].type==ROAD); iRow++)
			for(iCol = m_gridSize; m_cityGrid[iRow][iCol].vehicle!=0; iCol--);	// find empty cell on eastbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			cout << "New vehicle on " << iRow << ' ' << iCol << '\n';
//			cout << "Position is " << ( (m_cityGrid[iRow][iCol].vehicle==0)?'E':'F') << '\n';
//			cout << "Type is " << ( (m_cityGrid[iRow][iCol].type==BUILDING)?'B':'N') << '\n';
			break;
		default:
			break;
		}
	}

	void City::TranslateVehicles()
	{
		/*
		 * Rules:
		 * - Update horizontal first, vertical second
		 * - Update a single lane at a time, starting from the front vehicle to the back
		 */

		// start by locating the first horizontal road (3 consecutive RRR)
		for(int iRow = 1; iRow<=m_gridSize; iRow++)
		{
			if(m_cityGrid[iRow][1].type==ROAD && m_cityGrid[iRow][2].type==ROAD && m_cityGrid[iRow][3].type==ROAD)
			{
				// we're at a horizontal road
				// first horizontal lane from top runs eastbound, so we're at the start

				// run through each vehicle, frontmost first
				for(int iCol = 1; iCol<=m_gridSize; iCol++)
				{
					if(m_cityGrid[iRow][iCol].vehicle!=0) // there's a vehicle here
					{
						// movement rules for eastbound vehicle
						if(m_cityGrid[iRow][iCol].type==INTERSECTION)
						{
							// if current cell is marked as INTERSECTION, evaluate turning probability
							double willItTurn = randomNum.GetValue();
							if( willItTurn > (1-m_probTurnLeft) ) // top m_probTurnLeft percent
							{
								// turn left
								if(m_cityGrid[iRow+2][iCol-1].vehicle==0)
								{
									m_cityGrid[iRow+2][iCol-1].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}

							} else if( willItTurn < m_probTurnRight) // bottom m_probTurnRight percent
							{
								// turn right
								if(m_cityGrid[iRow-1][iCol].vehicle==0)	// if no-one at the turn (else halt)
								{
									m_cityGrid[iRow-1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							} else
							{
								// remainder probability, go straight ahead
								if(m_cityGrid[iRow][iCol-2].vehicle==0)
								{
									m_cityGrid[iRow][iCol-2].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							}

						} else if(m_cityGrid[iRow][iCol].vehicle->GetVelocity()==0)
						{
							// if vehicle is stopped, see if moving forward is possible
							if(m_cityGrid[iRow][iCol-1].vehicle==0)	// no vehicles ahead of us, accelerate
								m_cityGrid[iRow][iCol].vehicle->SetVelocity(1);

						} else if(iCol<=2)
						{
							// turn around at the end of the road
							int entryOnOppositeLane=1;
							while(m_cityGrid[iRow+1][entryOnOppositeLane].vehicle!=0)	// find an empty spot on the opposite lane
								entryOnOppositeLane++;
							m_cityGrid[iRow+1][entryOnOppositeLane].vehicle = m_cityGrid[iRow][iCol].vehicle;
							m_cityGrid[iRow][iCol].vehicle = 0;

						} else if(m_cityGrid[iRow][iCol-1].vehicle!=0)
						{
							// stop immediately if right next to next vehicle
							m_cityGrid[iRow][iCol].vehicle->SetVelocity(0);

						} else
						{
							// vehicle is free to move, check for frontmost vehicles and evaluate parking probability
							if( (randomNum.GetValue()<m_probPark) && (m_cityGrid[iRow-1][iCol].type==PARKING))
							{
								// park the vehicle
								m_cityGrid[iRow][iCol].vehicle->SetParked(true);
								m_cityGrid[iRow-1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
								// TODO AddParkedVehicle(m_cityGrid[iRow][iCol].vehicle);
							} else if (m_cityGrid[iRow][iCol].vehicle->GetVelocity()==1)
							{
								// move forward 1, evaluate speeding up to 2cells/step
								if( (m_cityGrid[iRow][iCol-2].vehicle==0) && (m_cityGrid[iRow][iCol-3].vehicle==0) && (m_cityGrid[iRow][iCol-2].type==ROAD))	// road ahead is clear
									m_cityGrid[iRow][iCol].vehicle->SetVelocity(2);
								m_cityGrid[iRow][iCol-1].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
							} else if(m_cityGrid[iRow][iCol].vehicle->GetVelocity()==2)
							{
								// see if we can move to iCol-2, else move to iCol-1 and decrease speed to 1
								if(m_cityGrid[iRow][iCol-2].vehicle==0 && m_cityGrid[iRow][iCol-2].type==ROAD)	// also makes vehicles slow down before intersections
								{
									m_cityGrid[iRow][iCol-2].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								} else
								{
									m_cityGrid[iRow][iCol].vehicle->SetVelocity(1);
									m_cityGrid[iRow][iCol-1].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							}
						}
 					}
				}
			}
			// now increase iRow and process westbound lane (starts on m_gridSize, vehicles in front have larger index)
			iRow++;
			// TODO

		}	// end of row iterator


		// now repeat for columns
		// this is gonna be hard, no straight vectors in columnspace. Use numerals m_cityGrid[x][y]
		// TODO this blows up
//		for(int iCol = 1; iCol<=(m_gridSize); iCol++)
//		{
//			if(m_cityGrid[1][iCol].type==ROAD && m_cityGrid[2][iCol].type==ROAD && m_cityGrid[3][iCol].type==ROAD)
//			{
//				// we're at a vertical road
//				// first vertical road is southbound, start at the end
//				for(int iRow=(m_gridSize); iRow>0; iRow--)
//				{
//					if(m_cityGrid[iRow][iCol].vehicle!=0)
//					{
//						// TODO movement rules for southbound vehicle
//					}
//				}
//
//				/* * * * * * */
//
//				// now jump to the next lane, and start from the top (northbound traffic)
//				iCol++;	// could bork if last lane is alone at the city's end
//				for(int iRow=1; iRow<=(m_gridSize); iRow++)
//				{
//					if(m_cityGrid[iRow][iCol].vehicle!=0)
//					{
//						// TODO movement rules for northbound vehicle
//					}
//				}
//			}
//		}

		// TODO evaluate parked vehicles, as they're outside road cells
		{
			// need a variable that holds pointers to parked vehicles
			// RemoveParkedVehicle(vehicle);

		}

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

	Callback<bool, Ptr<City> > City::GetInitVehiclesCallback()
		{ return m_initVehicles; }

	void City::SetInitVehiclesCallback(Callback<bool, Ptr<City> > initVehicles)
		{ m_initVehicles = initVehicles; }
}
