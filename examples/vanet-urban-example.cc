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
#include "ns3/core-module.h"
#include "ns3/City.h"
#include "ns3/VanetHeader.h"

using namespace ns3;
using namespace std;

static bool InitVehicles(Ptr<City> City);
static bool ControlVehicle(Ptr<City> City, Ptr<Vehicle> vehicle, double dt);
static void ReceiveData(Ptr<Vehicle> vehicle, VanetHeader packet);

// this declaration allows for global access to the City object
static Ptr<City> g_City=CreateObject<City>();

// aliases to enable scheduling
static void Start(Ptr<City> City) { City->Start(); }
static void Stop(Ptr<City> City) { City->Stop(); }


/* * * * * * *
 * * * * * * *
 * * * * * * */

static bool InitVehicles(Ptr<City> City)
{
	// print city map
	g_City->printCityStruct();

	// initialize vehicles in this routine
	Ptr<Vehicle> testDummy01 = g_City->CreateVehicle();
//	VanetHeader vHeader;
//	vHeader.SetID(1337);
//	testDummy01->AddPacket(vHeader);
	Simulator::Schedule(Seconds(0.5), &ns3::City::AddVehicle, g_City, testDummy01, RIGHTTOP);

	return(true);
}

static bool ControlVehicle(Ptr<City> City, Ptr<Vehicle> vehicle, double dt)
{
	// called for every vehicle, on each time step
	return(true);
}

static void ReceiveData(Ptr<Vehicle> vehicle, VanetHeader packet)
{
	// called on packet reception
}

int main (int argc, char *argv[])
{ 
	// Default values
	float simTime=1000.0;				// simulation time
	int runNumber=1;					// run number
	double deltaT=1;					// simulation step, max resolution is 1 step per deltaT
	int gridSize=400;					// grid size^2, default 400x400 (5m cells, 2x2km)
	double parkProb=0.001;

	// Process command-line args
	CommandLine cmd;
	cmd.AddValue ("time", "simulation time", simTime);
	cmd.AddValue ("rn", "run number", runNumber);
	cmd.AddValue ("gridsize", "sqrt(cells)", gridSize);
	cmd.AddValue ("parkprob", "parking probability (per vehicle per second)", parkProb);
	cmd.Parse(argc, argv);

	// Setup a City
	g_City->SetDeltaT(deltaT);
	g_City->SetGridSize(gridSize);
	g_City->SetParkProb(parkProb);

	// Bind the City/Vehicle events to the event handlers
	g_City->SetInitVehiclesCallback(MakeCallback(&InitVehicles));
	g_City->SetControlVehicleCallback(MakeCallback(&ControlVehicle));
	g_City->SetReceiveDataCallback(MakeCallback(&ReceiveData));

	// Setup seed and run-number (to affect random variable outcome of different runs)
	if(runNumber < 1) runNumber=1;
	SeedManager::SetSeed(1);
	SeedManager::SetRun(runNumber);

	// Schedule and run City
	Simulator::Schedule(Seconds(0.0), &Start, g_City);		// Invokes Start(City)
	Simulator::Schedule(Seconds(simTime), &Stop, g_City);
	Simulator::Stop(Seconds(simTime));
	Simulator::Run();
	Simulator::Destroy();

	return(0);
}

