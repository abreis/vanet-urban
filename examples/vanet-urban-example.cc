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
	float simTime=3600.0;				// simulation time
	int runNumber=1;					// run number
	double deltaT=1;					// simulation step, max resolution is 1 step per deltaT
	int gridSize=400;					// grid size^2, default 400x400 (5m cells, 2x2km)
	double parkProb=0.02;
	bool debug=false;
	int nVehicles=320;					// number of vehicles to introduce
	float interval=1.0;					// delay in seconds between each new vehicle
	float turningprob=0.10;				// turning (left and right) probability
	short algorithm=0;
	float percent=0;					// algorithm decision criteria
	bool coveragemap=false;
	bool citymap=false;
	bool stat=true;

	// Process command-line args
	CommandLine cmd;
	cmd.AddValue ("time", "simulation time", simTime);
	cmd.AddValue ("rn", "run number", runNumber);
	cmd.AddValue ("gridsize", "sqrt(cells)", gridSize);
	cmd.AddValue ("parkprob", "parking probability (per vehicle per second)", parkProb);
	cmd.AddValue ("debug", "enable debug output", debug);
	cmd.AddValue ("nvehicles", "vehicles to insert", nVehicles);
	cmd.AddValue ("interval", "delay between each new vehicle", interval);
	cmd.AddValue ("turning", "turning probability at each intersection", turningprob);
	cmd.AddValue ("algorithm", "RSU election algorithm", algorithm);
	cmd.AddValue ("criteria", "RSU algorithm criteria", percent);
	cmd.AddValue ("printCoverage", "print coverage map every step", coveragemap);
	cmd.AddValue ("printCity", "print city map every step", citymap);
	cmd.AddValue ("stat", "print statistics", stat);
	cmd.Parse(argc, argv);

	// Setup a City
	g_City->SetDeltaT(deltaT);
	g_City->SetGridSize(gridSize);
	g_City->SetParkProb(parkProb);
	g_City->SetDebug(debug);
	g_City->SetNumberOfVehicles(nVehicles);
	g_City->SetInterval(interval);
	g_City->SetTurningProb(turningprob);
	g_City->SetAlgorithm(algorithm);
	g_City->SetAlgorithmPercentage(percent);
	g_City->SetMapPrinting(coveragemap, citymap);
	g_City->SetStatPrinting(stat);

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

