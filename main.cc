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


#include <fstream>
#include <iostream>
#include <iomanip>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/random-variable.h"
#include "math.h"
#include "City.h"
#include "VanetHeader.h"

//NS_LOG_COMPONENT_DEFINE ("HADI");

using namespace ns3;
using namespace std;

static bool InitVehicles(Ptr<City> City, int& VID);
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

static bool InitVehicles(Ptr<City> City, int& VID)
{
	// initialize vehicles in this routine


	return true;
}

static bool ControlVehicle(Ptr<City> City, Ptr<Vehicle> vehicle, double dt)
{
	// called for every vehicle, on each time step
	return true;
}

static void ReceiveData(Ptr<Vehicle> vehicle, VanetHeader packet)
{
	// called on packet reception
}

int main (int argc, char *argv[])
{ 
	ns3::PacketMetadata::Enable();
	Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
	Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));

	// Default values
	float simTime=5000.0;				// simulation time
	int runNumber=1;					// run number
	double deltaT=1;					// simulation step, max resolution is 1 step per deltaT

	// Process command-line args
	CommandLine cmd;
	cmd.AddValue ("time", "simulation time", simTime);
	cmd.AddValue ("rn", "run number", runNumber);
	cmd.Parse(argc, argv);

	// Create and setup a City
//	Ptr<City> City=CreateObject<City>();
	g_City->SetDeltaT(deltaT);

	// Bind the City/Vehicle events to the event handlers
	g_City->SetControlVehicleCallback(MakeCallback(&ControlVehicle));
	g_City->SetInitVehiclesCallback(MakeCallback(&InitVehicles));
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

