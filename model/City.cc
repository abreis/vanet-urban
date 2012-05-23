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
		return(tid);
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
		m_gridSize=100;
		m_probPark = 0.001;
		m_probTurnLeft = 0.25;
		m_probTurnRight = 0.25;
		m_debug=false;
		m_algorithm=0;
		m_percent=0.5;
		m_printCoverage=false;
		m_printCity=false;
		m_stat=true;
	}

	City::~City()
	{

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
				coliterator->coverage=0;
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

		RandomAddVehicles(this, m_nvehicles, m_interval);	// get vehicle insertion going

	}

	void City::PrintCityStruct(void)
	{
		int rowNum=0;
		for(int iRow = 0; iRow<m_gridSize; iRow++)
		{
			cout << setw(3) << rowNum++ << ' ';
			for(int iCol = 0; iCol<m_gridSize; iCol++)
			{
				switch(m_cityGrid[iRow][iCol].type)
				{
					case ROAD: cout << 'R'; break;
					case BUILDING: cout << ' '; break;
					case INTERSECTION: cout << 'I'; break;
					case PARKING: cout << 'P'; break;
				}
			}
			cout << '\n';
		}
	}

	void City::PrintCityPointVehicles(void)
	{
		int rowNum=0;
		for(int iRow = 0; iRow<m_gridSize; iRow++)
		{
			cout << setw(3) << rowNum++ << ' ';
			for(int iCol = 0; iCol<m_gridSize; iCol++)
			{
				if(m_cityGrid[iRow][iCol].type==BUILDING) cout << ' ';
				else if(m_cityGrid[iRow][iCol].vehicle!=0)
					{ if(m_cityGrid[iRow][iCol].vehicle->IsParked()) cout << 'P'; else cout << '*'; }
				else if(m_cityGrid[iRow][iCol].type==PARKING) cout << '-';
				else cout << ' ';
			}
			cout << '\n';
		}
	}

	void City::PrintCityCoverageMap(void)
	{
		int rowNum=0;
		for(int iRow = 0; iRow<m_gridSize; iRow++)
		{
			cout << setw(3) << rowNum++ << ' ';
			for(int iCol = 0; iCol<m_gridSize; iCol++)
				if(m_cityGrid[iRow][iCol].coverage==0) cout << ' ';
				else cout << setw(1) << m_cityGrid[iRow][iCol].coverage;
			cout << '\n';
		}
	}

	void City::Step(Ptr<City> City)
	{
		ns3::Time nowtime = ns3::Simulator::Now();
		if(City->GetDebug()) cout << nowtime.ns3::Time::GetSeconds() << " STEP\n";

		City->TranslateVehicles();
		City->PrintStatistics();
	}

	Ptr<Vehicle> City::CreateVehicle (void)
	{
		Ptr<Vehicle> veh = CreateObject<Vehicle>();
		veh->SetReceiveCallback(m_receiveData);
		return(veh);
	}

	void City::RandomAddVehicles(Ptr<City> City, int number, double interval)
	{
		// add vehicles entering in the city randomly
		// get a random entry point (enum can be equaled to int)
		int randomPoint = (int)( round(City->GetRandomNum().GetValue()*7.0) );
		CellOrientation entryPoint = (CellOrientation)randomPoint;
		Ptr<Vehicle> newVeh = City->CreateVehicle();
		City->AddVehicle(newVeh, entryPoint);

		if(number>0)
			Simulator::Schedule(Seconds(interval), &City::RandomAddVehicles, City, (--number), interval);
	}

	void City::AddVehicle(Ptr<Vehicle> veh, CellOrientation ort)
	{
		int iRow=0, iCol=0;
		switch(ort)
		{
		case LEFTTOP:	// eastbound
			for(iRow = 0; !(m_cityGrid[iRow][0].type==ROAD && m_cityGrid[iRow][1].type==ROAD && m_cityGrid[iRow][2].type==ROAD); iRow++);
			iRow++;	// bottom lane is the entry point
			for(iCol = 0; m_cityGrid[iRow][iCol].vehicle!=0; iCol++);	// find empty cell on eastbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		case RIGHTTOP:	// westbound
			for(iRow = 0; !(m_cityGrid[iRow][0].type==ROAD && m_cityGrid[iRow][1].type==ROAD && m_cityGrid[iRow][2].type==ROAD); iRow++);
			for(iCol = (m_gridSize-1); m_cityGrid[iRow][iCol].vehicle!=0; iCol--);	// find empty cell on westbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		case LEFTBOTTOM:	// eastbound
			for(iRow = (m_gridSize-1); !(m_cityGrid[iRow][0].type==ROAD && m_cityGrid[iRow][1].type==ROAD && m_cityGrid[iRow][2].type==ROAD); iRow--);
			for(iCol = 0; m_cityGrid[iRow][iCol].vehicle!=0; iCol++);	// find empty cell on eastbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		case RIGHTBOTTOM:	// westbound
			for(iRow = (m_gridSize-1); !(m_cityGrid[iRow][0].type==ROAD && m_cityGrid[iRow][1].type==ROAD && m_cityGrid[iRow][2].type==ROAD); iRow--);
			iRow--; // top lane is the entry point
			for(iCol = (m_gridSize-1); m_cityGrid[iRow][iCol].vehicle!=0; iCol--);	// find empty cell on westbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		case TOPLEFT:
			for(iCol=0; !(m_cityGrid[0][iCol].type==ROAD && m_cityGrid[1][iCol].type==ROAD && m_cityGrid[2][iCol].type==ROAD); iCol++);
			for(iRow=0; m_cityGrid[iRow][iCol].vehicle!=0; iRow++);	// find empty cell on southbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		case TOPRIGHT:
			for(iCol=(m_gridSize-1); !(m_cityGrid[0][iCol].type==ROAD && m_cityGrid[1][iCol].type==ROAD && m_cityGrid[2][iCol].type==ROAD); iCol--);
			iCol--; // left lane is the entry point
			for(iRow=0; m_cityGrid[iRow][iCol].vehicle!=0; iRow++);	// find empty cell on southbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		case BOTTOMLEFT:
			for(iCol=0; !(m_cityGrid[0][iCol].type==ROAD && m_cityGrid[1][iCol].type==ROAD && m_cityGrid[2][iCol].type==ROAD); iCol++);
			iCol++; // right lane is the entry point
			for(iRow=(m_gridSize-1); m_cityGrid[iRow][iCol].vehicle!=0; iRow--);	// find empty cell on southbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		case BOTTOMRIGHT:
			for(iCol=(m_gridSize-1); !(m_cityGrid[0][iCol].type==ROAD && m_cityGrid[1][iCol].type==ROAD && m_cityGrid[2][iCol].type==ROAD); iCol--);
			for(iRow=(m_gridSize-1); m_cityGrid[iRow][iCol].vehicle!=0; iRow--);	// find empty cell on southbound lane
			m_cityGrid[iRow][iCol].vehicle=veh;
			m_Vehicles.push_back(veh);
			break;

		default:
			break;
		}

		ns3::Time nowtime = ns3::Simulator::Now();
		if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " NEW   [" << iRow << "][" << iCol << "]\n";
	}

	void City::TranslateVehicles()
	{
		ns3::Time nowtime = ns3::Simulator::Now();

		/*
		 * Rules:
		 * - Update horizontal first, vertical second
		 * - Update a single lane at a time, starting from the front vehicle to the back
		 */
		int iRow=0, iCol=0;

		// start by locating the first horizontal road (3 consecutive RRR)
		for(iRow = 0; iRow<m_gridSize; iRow++)
		{
			if(m_cityGrid[iRow][0].type==ROAD && m_cityGrid[iRow][1].type==ROAD && m_cityGrid[iRow][2].type==ROAD)
			{
				// we're at a horizontal road
				// first horizontal lane from top runs westbound, so we're at the start

				// run through each vehicle, frontmost first
				for(iCol = 0; iCol<m_gridSize; iCol++)
				{
					if(m_cityGrid[iRow][iCol].vehicle!=0 &&
							!(m_cityGrid[iRow][iCol].type==INTERSECTION && m_cityGrid[iRow][iCol+1].type==INTERSECTION)
							) // there's a vehicle here and it belongs to this lane already
					{
						// movement rules for westbound vehicle
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

						} else if(iCol<2)
						{
							// turn around at the end of the road
							int entryOnOppositeLane=0;
							while(m_cityGrid[iRow+1][entryOnOppositeLane].vehicle!=0)	// find an empty spot on the opposite lane
								entryOnOppositeLane++;
							m_cityGrid[iRow+1][entryOnOppositeLane].vehicle = m_cityGrid[iRow][iCol].vehicle;
							m_cityGrid[iRow][iCol].vehicle = 0;

						} else if(m_cityGrid[iRow][iCol-1].vehicle!=0)
						{
							// stop immediately if right next to a vehicle
							m_cityGrid[iRow][iCol].vehicle->SetVelocity(0);

						} else
						{
							// vehicle is free to move, check for frontmost vehicles and evaluate parking probability
							if( (randomNum.GetValue()<m_probPark) && (m_cityGrid[iRow-1][iCol].type==PARKING) && iCol<(m_gridSize-3))	// probability:space:not at edges
							{
								// park the vehicle
								m_cityGrid[iRow][iCol].vehicle->SetParked(true);
								m_cityGrid[iRow-1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
								if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " PARK  [" << (iRow-1) << "][" << (iCol) << "]\n";

								// evaluate becoming an RSU
								ElectRSU(iRow-1, iCol);

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

				// now increase iRow and process eastbound lane (starts on m_gridSize, vehicles in front have larger index)
				iRow++;
				for(iCol = (m_gridSize-1); iCol>=0; iCol--)
				{
					if(m_cityGrid[iRow][iCol].vehicle!=0 &&
							!(m_cityGrid[iRow][iCol].type==INTERSECTION && m_cityGrid[iRow][iCol-1].type==INTERSECTION)
							) // there's a vehicle here and it belongs to this lane already
					{
						// movement rules for eastbound vehicle
						if(m_cityGrid[iRow][iCol].type==INTERSECTION)
						{
							// if current cell is marked as INTERSECTION, evaluate turning probability
							double willItTurn = randomNum.GetValue();
							if( willItTurn > (1-m_probTurnLeft) ) // top m_probTurnLeft percent
							{
								// turn left
								if(m_cityGrid[iRow-2][iCol+1].vehicle==0)
								{
									m_cityGrid[iRow-2][iCol+1].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}

							} else if( willItTurn < m_probTurnRight) // bottom m_probTurnRight percent
							{
								// turn right
								if(m_cityGrid[iRow+1][iCol].vehicle==0)	// if no-one at the turn (else halt)
								{
									m_cityGrid[iRow+1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							} else
							{
								// remainder probability, go straight ahead
								if(m_cityGrid[iRow][iCol+2].vehicle==0)
								{
									m_cityGrid[iRow][iCol+2].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							}

						} else if(m_cityGrid[iRow][iCol].vehicle->GetVelocity()==0)
						{
							// if vehicle is stopped, see if moving forward is possible
							if(m_cityGrid[iRow][iCol+1].vehicle==0)	// no vehicles ahead of us, accelerate
								m_cityGrid[iRow][iCol].vehicle->SetVelocity(1);

						} else if(iCol>(m_gridSize-3))
						{
							// turn around at the end of the road
							int entryOnOppositeLane=m_gridSize-1;
							while(m_cityGrid[iRow-1][entryOnOppositeLane].vehicle!=0)	// find an empty spot on the opposite lane
								entryOnOppositeLane--;
							m_cityGrid[iRow-1][entryOnOppositeLane].vehicle = m_cityGrid[iRow][iCol].vehicle;
							m_cityGrid[iRow][iCol].vehicle = 0;

						} else if(m_cityGrid[iRow][iCol+1].vehicle!=0)
						{
							// stop immediately if right next to a vehicle
							m_cityGrid[iRow][iCol].vehicle->SetVelocity(0);

						} else
						{
							// vehicle is free to move, check for frontmost vehicles and evaluate parking probability
							if( (randomNum.GetValue()<m_probPark) && (m_cityGrid[iRow+1][iCol].type==PARKING) && iCol>2)	// probability:space:not at edges
							{
								// park the vehicle
								m_cityGrid[iRow][iCol].vehicle->SetParked(true);
								m_cityGrid[iRow+1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
								if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " PARK  [" << (iRow+1) << "][" << (iCol) << "]\n";

								// evaluate becoming an RSU
								ElectRSU(iRow+1, iCol);

							} else if (m_cityGrid[iRow][iCol].vehicle->GetVelocity()==1)
							{
								// move forward 1, evaluate speeding up to 2cells/step
								if( (m_cityGrid[iRow][iCol+2].vehicle==0) && (m_cityGrid[iRow][iCol+3].vehicle==0) && (m_cityGrid[iRow][iCol+2].type==ROAD))	// road ahead is clear
									m_cityGrid[iRow][iCol].vehicle->SetVelocity(2);
								m_cityGrid[iRow][iCol+1].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
							} else if(m_cityGrid[iRow][iCol].vehicle->GetVelocity()==2)
							{
								// see if we can move to iCol+2, else move to iCol+1 and decrease speed to 1
								if(m_cityGrid[iRow][iCol+2].vehicle==0 && m_cityGrid[iRow][iCol+2].type==ROAD)	// also makes vehicles slow down before intersections
								{
									m_cityGrid[iRow][iCol+2].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								} else
								{
									m_cityGrid[iRow][iCol].vehicle->SetVelocity(1);
									m_cityGrid[iRow][iCol+1].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							}
						}
 					}
				}
			}	// end of current lane pair
		}	// end of row iterator


		// now repeat for columns
		// start by locating the first vertical road (3 consecutive RRR)
		for(iCol = 0; iCol<m_gridSize; iCol++)
		{
			if(m_cityGrid[0][iCol].type==ROAD && m_cityGrid[1][iCol].type==ROAD && m_cityGrid[2][iCol].type==ROAD)
			{
				// we're at a vertical road
				// first vertical lane from left runs southbound, go to the end

				// run through each vehicle, frontmost first
				for(iRow = (m_gridSize-1); iRow>=0; iRow--)
				{
					if(m_cityGrid[iRow][iCol].vehicle!=0 &&
							!(m_cityGrid[iRow][iCol].type==INTERSECTION && m_cityGrid[iRow-1][iCol].type==INTERSECTION)
							) // there's a vehicle here and it belongs to this lane already
					{
						// movement rules for eastbound vehicle
						if(m_cityGrid[iRow][iCol].type==INTERSECTION)
						{
							// if current cell is marked as INTERSECTION, evaluate turning probability
							double willItTurn = randomNum.GetValue();
							if( willItTurn > (1-m_probTurnLeft) ) // top m_probTurnLeft percent
							{
								// turn left
								if(m_cityGrid[iRow+1][iCol+2].vehicle==0)
								{
									m_cityGrid[iRow+1][iCol+2].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}

							} else if( willItTurn < m_probTurnRight) // bottom m_probTurnRight percent
							{
								// turn right
								if(m_cityGrid[iRow][iCol-1].vehicle==0)	// if no-one at the turn (else halt)
								{
									m_cityGrid[iRow][iCol-1].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							} else
							{
								// remainder probability, go straight ahead
								if(m_cityGrid[iRow+2][iCol].vehicle==0)
								{
									m_cityGrid[iRow+2][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							}

						} else if(m_cityGrid[iRow][iCol].vehicle->GetVelocity()==0)
						{
							// if vehicle is stopped, see if moving forward is possible
							if(m_cityGrid[iRow+1][iCol].vehicle==0)	// no vehicles ahead of us, accelerate
								m_cityGrid[iRow][iCol].vehicle->SetVelocity(1);

						} else if(iRow>(m_gridSize-3))
						{
							// turn around at the end of the road
							int entryOnOppositeLane=m_gridSize-1;
							while(m_cityGrid[entryOnOppositeLane][iCol+1].vehicle!=0)	// find an empty spot on the opposite lane
								entryOnOppositeLane--;
							m_cityGrid[entryOnOppositeLane][iCol+1].vehicle = m_cityGrid[iRow][iCol].vehicle;
							m_cityGrid[iRow][iCol].vehicle = 0;

						} else if(m_cityGrid[iRow+1][iCol].vehicle!=0)
						{
							// stop immediately if right next to a vehicle
							m_cityGrid[iRow][iCol].vehicle->SetVelocity(0);

						} else
						{
							// vehicle is free to move, check for frontmost vehicles and evaluate parking probability
							if( (randomNum.GetValue()<m_probPark) && (m_cityGrid[iRow][iCol-1].type==PARKING) && iRow>2)	// probability:space:not at edges
							{
								// park the vehicle
								m_cityGrid[iRow][iCol].vehicle->SetParked(true);
								m_cityGrid[iRow][iCol-1].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
								if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " PARK  [" << (iRow) << "][" << (iCol-1) << "]\n";

								// evaluate becoming an RSU
								ElectRSU(iRow, iCol-1);

							} else if (m_cityGrid[iRow][iCol].vehicle->GetVelocity()==1)
							{
								// move forward 1, evaluate speeding up to 2cells/step
								if( (m_cityGrid[iRow+2][iCol].vehicle==0) && (m_cityGrid[iRow+3][iCol].vehicle==0) && (m_cityGrid[iRow+2][iCol].type==ROAD))	// road ahead is clear
									m_cityGrid[iRow][iCol].vehicle->SetVelocity(2);
								m_cityGrid[iRow+1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
							} else if(m_cityGrid[iRow][iCol].vehicle->GetVelocity()==2)
							{
								// see if we can move to iRow+2, else move to iRow+1 and decrease speed to 1
								if(m_cityGrid[iRow+2][iCol].vehicle==0 && m_cityGrid[iRow+2][iCol].type==ROAD)	// also makes vehicles slow down before intersections
								{
									m_cityGrid[iRow+2][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								} else
								{
									m_cityGrid[iRow][iCol].vehicle->SetVelocity(1);
									m_cityGrid[iRow+1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							}
						}
 					}
				}

				// now increase iCol and process northbound lane (starts on 0, vehicles in front have smaller index)
				iCol++;
				for(iRow = 0; iRow<m_gridSize; iRow++)
				{
					if(m_cityGrid[iRow][iCol].vehicle!=0 &&
							!(m_cityGrid[iRow][iCol].type==INTERSECTION && m_cityGrid[iRow+1][iCol].type==INTERSECTION)
							) // there's a vehicle here and it belongs to this lane already
					{
						// movement rules for eastbound vehicle
						if(m_cityGrid[iRow][iCol].type==INTERSECTION)
						{
							// if current cell is marked as INTERSECTION, evaluate turning probability
							double willItTurn = randomNum.GetValue();
							if( willItTurn > (1-m_probTurnLeft) ) // top m_probTurnLeft percent
							{
								// turn left
								if(m_cityGrid[iRow-1][iCol-2].vehicle==0)
								{
									m_cityGrid[iRow-1][iCol-2].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}

							} else if( willItTurn < m_probTurnRight) // bottom m_probTurnRight percent
							{
								// turn right
								if(m_cityGrid[iRow][iCol+1].vehicle==0)	// if no-one at the turn (else halt)
								{
									m_cityGrid[iRow][iCol+1].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							} else
							{
								// remainder probability, go straight ahead
								if(m_cityGrid[iRow-2][iCol].vehicle==0)
								{
									m_cityGrid[iRow-2][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							}

						} else if(m_cityGrid[iRow][iCol].vehicle->GetVelocity()==0)
						{
							// if vehicle is stopped, see if moving forward is possible
							if(m_cityGrid[iRow-1][iCol].vehicle==0)	// no vehicles ahead of us, accelerate
								m_cityGrid[iRow][iCol].vehicle->SetVelocity(1);

						} else if(iRow<2)
						{
							// turn around at the end of the road
							int entryOnOppositeLane=0;
							while(m_cityGrid[entryOnOppositeLane][iCol-1].vehicle!=0)	// find an empty spot on the opposite lane
								entryOnOppositeLane++;
							m_cityGrid[entryOnOppositeLane][iCol-1].vehicle = m_cityGrid[iRow][iCol].vehicle;
							m_cityGrid[iRow][iCol].vehicle = 0;

						} else if(m_cityGrid[iRow-1][iCol].vehicle!=0)
						{
							// stop immediately if right next to a vehicle
							m_cityGrid[iRow][iCol].vehicle->SetVelocity(0);

						} else
						{
							// vehicle is free to move, check for frontmost vehicles and evaluate parking probability
							if( (randomNum.GetValue()<m_probPark) && (m_cityGrid[iRow][iCol+1].type==PARKING) && iRow<(m_gridSize-3))	// probability:space:not at edges
							{
								// park the vehicle
								m_cityGrid[iRow][iCol].vehicle->SetParked(true);
								m_cityGrid[iRow][iCol+1].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
								if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " PARK  [" << (iRow) << "][" << (iCol+1) << "]\n";

								// evaluate becoming an RSU
								ElectRSU(iRow, iCol+1);

							} else if (m_cityGrid[iRow][iCol].vehicle->GetVelocity()==1)
							{
								// move forward 1, evaluate speeding up to 2cells/step
								if( (m_cityGrid[iRow-2][iCol].vehicle==0) && (m_cityGrid[iRow-3][iCol].vehicle==0) && (m_cityGrid[iRow-2][iCol].type==ROAD))	// road ahead is clear
									m_cityGrid[iRow][iCol].vehicle->SetVelocity(2);
								m_cityGrid[iRow-1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle = 0;
							} else if(m_cityGrid[iRow][iCol].vehicle->GetVelocity()==2)
							{
								// see if we can move to iRow-2, else move to iRow-1 and decrease speed to 1
								if(m_cityGrid[iRow-2][iCol].vehicle==0 && m_cityGrid[iRow-2][iCol].type==ROAD)	// also makes vehicles slow down before intersections
								{
									m_cityGrid[iRow-2][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								} else
								{
									m_cityGrid[iRow][iCol].vehicle->SetVelocity(1);
									m_cityGrid[iRow-1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
									m_cityGrid[iRow][iCol].vehicle = 0;
								}
							}
						}
 					}
				}
			}	// end of current lane pair
		}	// end of column iterator

		// evaluate parked vehicles, as they're outside road cells
		for(iRow=0; iRow<m_gridSize; iRow++)
			for(iCol=0; iCol<m_gridSize; iCol++)
			{
				if(m_cityGrid[iRow][iCol].vehicle!=0)	// we have a vehicle
					if(m_cityGrid[iRow][iCol].vehicle->IsParked()==true)	// it's parked
						if(randomNum.GetValue()<m_probPark)	// it'll unpark
						{
							// unpark the vehicle
							if(m_cityGrid[iRow+1][iCol].type==ROAD && m_cityGrid[iRow+1][iCol].vehicle==0)	// unpark to the south
							{
								if(m_cityGrid[iRow][iCol].vehicle->IsRSU())
								{
									UnTagCoverageRSU(iRow, iCol);
									m_cityGrid[iRow][iCol].vehicle->SetRSU(false);
								}
								m_cityGrid[iRow][iCol].vehicle->SetParked(false);
								m_cityGrid[iRow+1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle=0;
								if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " LEAVE [" << (iRow+1) << "][" << (iCol) << "]\n";
							}
							else if(m_cityGrid[iRow-1][iCol].type==ROAD && m_cityGrid[iRow-1][iCol].vehicle==0) // unpark to the north
							{
								if(m_cityGrid[iRow][iCol].vehicle->IsRSU())
								{
									UnTagCoverageRSU(iRow, iCol);
									m_cityGrid[iRow][iCol].vehicle->SetRSU(false);
								}
								m_cityGrid[iRow][iCol].vehicle->SetParked(false);
								m_cityGrid[iRow-1][iCol].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle=0;
								if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " LEAVE [" << (iRow-1) << "][" << (iCol) << "]\n";
							}
							else if(m_cityGrid[iRow][iCol+1].type==ROAD && m_cityGrid[iRow][iCol+1].vehicle==0) // unpark to the west
							{
								if(m_cityGrid[iRow][iCol].vehicle->IsRSU())
								{
									UnTagCoverageRSU(iRow, iCol);
									m_cityGrid[iRow][iCol].vehicle->SetRSU(false);
								}
								m_cityGrid[iRow][iCol].vehicle->SetParked(false);
								m_cityGrid[iRow][iCol+1].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle=0;
								if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " LEAVE [" << (iRow) << "][" << (iCol+1) << "]\n";
							}
							else if(m_cityGrid[iRow][iCol-1].type==ROAD && m_cityGrid[iRow][iCol-1].vehicle==0) // unpark to the east
							{
								if(m_cityGrid[iRow][iCol].vehicle->IsRSU())
								{
									UnTagCoverageRSU(iRow, iCol);
									m_cityGrid[iRow][iCol].vehicle->SetRSU(false);
								}
								m_cityGrid[iRow][iCol].vehicle->SetParked(false);
								m_cityGrid[iRow][iCol-1].vehicle = m_cityGrid[iRow][iCol].vehicle;
								m_cityGrid[iRow][iCol].vehicle=0;
								if(m_debug) cout << nowtime.ns3::Time::GetSeconds() << " LEAVE [" << (iRow) << "][" << (iCol-1) << "]\n";
							}
						}
			}

		Simulator::Schedule(Seconds(m_dt), &City::Step, Ptr<City>(this));
	}

	void City::TagCoverageRSU(int x, int y)
	{
		// by using the pattern in range50cell[25], tag a block of 50x50 cells (125x125m) as covered by an RSU
		// (x,y) marks the position of the RSU, and it belongs to the upper left quadrant
		int iRow, iCol;

		// upper left quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				m_cityGrid[x-iRow][y-iCol].coverage++;
				iCol++;
			}
			iRow++;
		}

		// lower left quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				m_cityGrid[x+iRow+1][y-iCol].coverage++;
				iCol++;
			}
			iRow++;
		}

		// upper right quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				m_cityGrid[x-iRow][y+iCol+1].coverage++;
				iCol++;
			}
			iRow++;
		}

		// lower right quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				m_cityGrid[x+iRow+1][y+iCol+1].coverage++;
				iCol++;
			}
			iRow++;
		}
	}

	void City::UnTagCoverageRSU(int x, int y)
	{
		// by using the pattern in range50cell[25], untag a block of 50x50 cells (125x125m) as covered by an RSU
		// (x,y) marks the position of the RSU, and it belongs to the upper left quadrant
		int iRow, iCol;

		// upper left quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				m_cityGrid[x-iRow][y-iCol].coverage--;
				iCol++;
			}
			iRow++;
		}

		// lower left quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				m_cityGrid[x+iRow+1][y-iCol].coverage--;
				iCol++;
			}
			iRow++;
		}

		// upper right quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				m_cityGrid[x-iRow][y+iCol+1].coverage--;
				iCol++;
			}
			iRow++;
		}

		// lower right quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				m_cityGrid[x+iRow+1][y+iCol+1].coverage--;
				iCol++;
			}
			iRow++;
		}
	}

	int City::CountUncoveredCells(int x, int y)
	{
		int count=0;
		int iRow, iCol;

		// upper left quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				if(m_cityGrid[x-iRow][y-iCol].coverage==0) count++;
				iCol++;
			}
			iRow++;
		}

		// lower left quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				if(m_cityGrid[x+iRow+1][y-iCol].coverage==0) count++;
				iCol++;
			}
			iRow++;
		}

		// upper right quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				if(m_cityGrid[x-iRow][y+iCol+1].coverage==0) count++;
				iCol++;
			}
			iRow++;
		}

		// lower right quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				if(m_cityGrid[x+iRow+1][y+iCol+1].coverage==0) count++;
				iCol++;
			}
			iRow++;
		}
		return(count);
	}

	/* * * * * * * * * * * *
	 * election algorithms *
	 * * * * * * * * * * * */

	void City::ElectRSU(int x, int y)
	{
		bool become=false;
		switch(m_algorithm)
		{
			case 0:
				become=true; break;
			case 1:
				become=ElectBasedOnPercentageNewCellsCovered(x, y, m_percent); break;
			case 2:
				become=ElectBasedOnDistanceToNearestRSU(x, y, m_percent); break;
			case 3:
				become=ElectBasedOnNumberOfNeighborRSUs(x, y, m_percent); break;
			default:
				break;
		}
		if(become) {
			TagCoverageRSU(x, y);
			m_cityGrid[x][y].vehicle->SetRSU(true);
		}
	}

	bool City::ElectBasedOnPercentageNewCellsCovered(int x, int y, float percent)
	{
		// go through local circle of cells, count covered, divide, return true if < percent
		int newlyCovered = CountUncoveredCells(x,y);
		float newCoverage = (float)newlyCovered/(float)TOTALCELLS;

		if(newCoverage>percent)
			return(true);
		else return(false);
	}

	bool City::ElectBasedOnDistanceToNearestRSU(int x, int y, float percent)
	{
		// percent between 0 and 1 -> 0(only elect if right next to an RSU) 1(don't elect if any RSUs in range) 0.5 (elect if no RSUs less than 12.5 cells away)
		assert(percent<1.0 && percent>0.0);

		// find RSUs, store lowest distance
		int distance = 26; // larger than range circle
		int iRow, iCol;

		// upper left quadrant
		iRow=0; float pyth=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				if(m_cityGrid[x-iRow][y-iCol].vehicle!=0 && m_cityGrid[x-iRow][y-iCol].vehicle->IsRSU() && iRow!=0 && iCol!=0)	// skip ourselves for this check
				{
					pyth = sqrt(pow((float)iRow,2)+pow((float)iCol,2)); // pythagoras
					if(pyth<distance) distance=(int)pyth;
				}
				iCol++;
			}
			iRow++;
		}

		// lower left quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				if(m_cityGrid[x+iRow+1][y-iCol].vehicle!=0 && m_cityGrid[x+iRow+1][y-iCol].vehicle->IsRSU())
				{
					pyth = sqrt(pow((float)(iRow+1),2)+pow((float)iCol,2)); // pythagoras
					if(pyth<distance) distance=(int)pyth;
				}
				iCol++;
			}
			iRow++;
		}

		// upper right quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				if(m_cityGrid[x-iRow][y+iCol+1].vehicle!=0 && m_cityGrid[x-iRow][y+iCol+1].vehicle->IsRSU())
				{
					pyth = sqrt(pow((float)iRow,2)+pow((float)(iCol+1),2)); // pythagoras
					if(pyth<distance) distance=(int)pyth;
				}
				iCol++;
			}
			iRow++;
		}

		// lower right quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				if(m_cityGrid[x+iRow+1][y+iCol+1].vehicle!=0 && m_cityGrid[x+iRow+1][y+iCol+1].vehicle->IsRSU())
				{
					pyth = sqrt(pow((float)(iRow+1),2)+pow((float)(iCol+1),2)); // pythagoras
					if(pyth<distance) distance=(int)pyth;
				}
				iCol++;
			}
			iRow++;
		}

		if(distance<(25*percent))	// if there is an RSU at less than percent% of the 25 cell radius, don't be an RSU
			return(false); else return(true);
	}

	bool City::ElectBasedOnNumberOfNeighborRSUs(int x, int y, float maxneighbors)
	{
		/* NOTE:
		 * maxneighbors here isn't a percentage, but an integer
		 * Datatype float chosen for consistency
		 */
		int count=0;

		int iRow, iCol;
		// upper left quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				if(m_cityGrid[x-iRow][y-iCol].vehicle!=0 && m_cityGrid[x-iRow][y-iCol].vehicle->IsRSU() && iRow!=0 && iCol!=0)	// skip ourselves for this check
					count++;
				iCol++;
			}
			iRow++;
		}

		// lower left quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y-iCol)>=0)
			{
				if(m_cityGrid[x+iRow+1][y-iCol].vehicle!=0 && m_cityGrid[x+iRow+1][y-iCol].vehicle->IsRSU())
					count++;
				iCol++;
			}
			iRow++;
		}

		// upper right quadrant
		iRow=0;
		while(iRow<25 && (x-iRow)>=0)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				if(m_cityGrid[x-iRow][y+iCol+1].vehicle!=0 && m_cityGrid[x-iRow][y+iCol+1].vehicle->IsRSU())
					count++;
				iCol++;
			}
			iRow++;
		}

		// lower right quadrant
		iRow=0;
		while(iRow<25 && (x+iRow+1)<m_gridSize)
		{
			iCol=0;
			while(iCol<range50cell[24-iRow] && (y+iCol+1)<m_gridSize)
			{
				if(m_cityGrid[x+iRow+1][y+iCol+1].vehicle!=0 && m_cityGrid[x+iRow+1][y+iCol+1].vehicle->IsRSU())
					count++;
				iCol++;
			}
			iRow++;
		}

		if(count>(int)maxneighbors)
			return(false);
		else return(true);
	}


	/* * * * * * * * * * * * *
	 * evaluation algorithms *
	 * * * * * * * * * * * * */

	int City::CountCoveredCells(void)
	{
		int count=0;
		for(int iRow = 0; iRow<m_gridSize; iRow++)
			for(int iCol = 0; iCol<m_gridSize; iCol++)
				if(m_cityGrid[iRow][iCol].coverage>0) count++;
		return(count);
	}

	int City::CountOvercoveredCells(void)
	{
		int count=0;
		for(int iRow = 0; iRow<m_gridSize; iRow++)
			for(int iCol = 0; iCol<m_gridSize; iCol++)
				if(m_cityGrid[iRow][iCol].coverage>1) count++;
		return(count);
	}

	int City::CountCoverageSaturation(void)
	{
		int count=0;
		for(int iRow = 0; iRow<m_gridSize; iRow++)
			for(int iCol = 0; iCol<m_gridSize; iCol++)
				if(m_cityGrid[iRow][iCol].coverage>1) count+=(m_cityGrid[iRow][iCol].coverage-1);
		return(count);
	}

	void City::PrintStatistics(void)
	{
		// ran every Step
		ns3::Time nowtime = ns3::Simulator::Now();

		// only on the first step
		if(nowtime.ns3::Time::GetSeconds()<0.1 && (m_printCoverage || m_printCity))
			PrintCityStruct();

		if(m_printCity) PrintCityPointVehicles();
		if(m_printCoverage) PrintCityCoverageMap();

		if(m_stat)
		{
			int seconds = (int)nowtime.ns3::Time::GetSeconds();
			if( seconds%10 == 0 )	// only once every 10 s
			if(nowtime.ns3::Time::GetSeconds() > m_nvehicles*m_interval)	// ensure city is filled before taking statistics
			{
				cout << nowtime.ns3::Time::GetSeconds() << " STAT "
					<< "COVERED " << CountCoveredCells()
					<< '\n';

				cout << nowtime.ns3::Time::GetSeconds() << " STAT "
					<< "OVERCOVERED " << CountOvercoveredCells()
					<< '\n';

				cout << nowtime.ns3::Time::GetSeconds() << " STAT "
					<< "OVERCOVERAGE " << setw(3) << (float)CountCoverageSaturation()/(float)TOTALCELLS
					<< '\n';
			}
		}
	}

	/* * * * * * * * * * * *
	 * setters and getters *
	 * * * * * * * * * * * */

	double City::GetDeltaT()
		{ return(m_dt); }

	void City::SetDeltaT(double value)
		{ m_dt=value; }

	void City::SetParkProb(double value)
		{ m_probPark=value; }

	void City::SetGridSize(int value)
		{ m_gridSize=value; }

	void City::SetDebug(bool value)
		{ m_debug=value; }

	bool City::GetDebug(void)
		{ return(m_debug); }

	void City::SetNumberOfVehicles(int value)
		{ m_nvehicles=value; }

	void City::SetInterval(float value)
		{ m_interval=value; }

	void City::SetTurningProb(float value)
		{ m_probTurnLeft=value; m_probTurnRight=value; }

	void City::SetAlgorithm(short value)
		{ m_algorithm=value; }

	void City::SetAlgorithmPercentage(float value)
		{ m_percent=value; }

	void City::SetMapPrinting(bool coverage, bool city)
		{ m_printCoverage=coverage; m_printCity=city; }

	void City::SetStatPrinting(bool stat)
		{ m_stat=stat; }


	UniformVariable City::GetRandomNum(void)
		{ return(randomNum); }

	/* * * * * * *
	 * callbacks *
	 * * * * * * */

	Callback<void, Ptr<Vehicle>, VanetHeader> City::GetReceiveDataCallback()
		{ return(m_receiveData); }

	void City::SetReceiveDataCallback(Callback<void, Ptr<Vehicle>, VanetHeader> receiveData)
		{ m_receiveData = receiveData; }

	Callback<bool, Ptr<City>, Ptr<Vehicle>, double> City::GetControlVehicleCallback()
		{ return(m_controlVehicle); }

	void City::SetControlVehicleCallback(Callback<bool, Ptr<City>, Ptr<Vehicle>, double> controlVehicle)
		{ m_controlVehicle = controlVehicle; }

	Callback<bool, Ptr<City> > City::GetInitVehiclesCallback()
		{ return(m_initVehicles); }

	void City::SetInitVehiclesCallback(Callback<bool, Ptr<City> > initVehicles)
		{ m_initVehicles = initVehicles; }
}
