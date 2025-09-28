#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/random-variable-stream.h"
#include <time.h>
#include <iomanip>  
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NetworkSimulation");

int main(int argc, char *argv[])
{
	time_t t1 ;
	Time::SetResolution(Time::MS);
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
	srand ( (unsigned) time (&t1));
	// Create Nodes: 7 workstations and 4 routers
	NodeContainer workstations, routers;
	workstations.Create(7);
	routers.Create(4);

	// Create Point-to-Point Links with capacities and delays
	PointToPointHelper p2p;
	NetDeviceContainer devices[11];

	// A -> R1
	p2p.SetDeviceAttribute("DataRate", StringValue("500Kbps"));
	p2p.SetChannelAttribute("Delay", StringValue("2ms"));
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("10p"));
	devices[0] = p2p.Install(workstations.Get(0), routers.Get(0));

	// Add packet loss on A->R1
	Ptr<RateErrorModel> errorModelA = CreateObject<RateErrorModel>();
	errorModelA->SetAttribute("ErrorRate", DoubleValue(0.01));
	devices[0].Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModelA));

	// B -> R2
	p2p.SetDeviceAttribute("DataRate", StringValue("500Kbps"));
	p2p.SetChannelAttribute("Delay", StringValue("3ms"));
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("20p"));
	devices[1] = p2p.Install(workstations.Get(1), routers.Get(1));
    
	// Add packet loss on B->R2
	Ptr<RateErrorModel> errorModelB = CreateObject<RateErrorModel>();
	errorModelB->SetAttribute("ErrorRate", DoubleValue(0.05));
	devices[1].Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModelB));

	// C -> R3
	p2p.SetDeviceAttribute("DataRate", StringValue("500Kbps"));
	p2p.SetChannelAttribute("Delay", StringValue("4ms"));
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("6p"));
	devices[2] = p2p.Install(workstations.Get(2), routers.Get(2));

	// Add packet loss on C->R3
	Ptr<RateErrorModel> errorModelC = CreateObject<RateErrorModel>();
	errorModelC->SetAttribute("ErrorRate", DoubleValue(0.03));
	devices[2].Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModelC));

	// D -> R4
	p2p.SetDeviceAttribute("DataRate", StringValue("500Kbps"));
	p2p.SetChannelAttribute("Delay", StringValue("5ms"));
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("3p"));
	devices[3] = p2p.Install(workstations.Get(3), routers.Get(3));

	// Add packet loss on D->R4
	Ptr<RateErrorModel> errorModelD = CreateObject<RateErrorModel>();
	errorModelD->SetAttribute("ErrorRate", DoubleValue(0.02));
	devices[3].Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModelD));

	p2p.SetDeviceAttribute("DataRate", StringValue("500Kbps"));
	p2p.SetChannelAttribute("Delay", StringValue("6ms"));
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("10p"));
	devices[4] = p2p.Install(workstations.Get(4), routers.Get(1));

	// Add packet loss on E->R5
	Ptr<RateErrorModel> errorModelE = CreateObject<RateErrorModel>();
	errorModelE->SetAttribute("ErrorRate", DoubleValue(0.04));
	devices[4].Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModelE));

	// F -> R6
	p2p.SetDeviceAttribute("DataRate", StringValue("500Kbps"));
	p2p.SetChannelAttribute("Delay", StringValue("7ms"));
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("12p"));
	devices[5] = p2p.Install(workstations.Get(5), routers.Get(0));
	//..
	p2p.SetDeviceAttribute("DataRate", StringValue("1000Kbps"));
	p2p.SetChannelAttribute("Delay", StringValue("3ms"));
	p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("200p"));
	devices[1] = p2p.Install(routers.Get(3), routers.Get(1));


   	 
	InternetStackHelper internet;
	internet.Install(workstations);
	internet.Install(routers);
	Ipv4AddressHelper address;
	Ipv4InterfaceContainer workstationInterfaces;
	for (uint32_t i = 0; i < 7; ++i)
	{
    	std::ostringstream subnet;
    	subnet << "10.1." << i + 1 << ".0";
    	address.SetBase(subnet.str().c_str(), "255.255.255.0");
    	workstationInterfaces.Add(address.Assign(devices[i]));
	}

  Ipv4StaticRoutingHelper staticRouting;

	for (uint32_t i = 0; i < routers.GetN(); ++i)
{
	Ptr<Ipv4> ipv4 = routers.Get(i)->GetObject<Ipv4>();
	Ptr<Ipv4StaticRouting> routing = staticRouting.GetStaticRouting(ipv4);

	if (i == 0) // Router R1
	{
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"), 1); // R2
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.3.0"), Ipv4Mask("255.255.255.0"), 2); // R3
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.4.0"), Ipv4Mask("255.255.255.0"), 3); // R4
	}
	else if (i == 1) // Router R2
	{
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"), 1); // R1
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.3.0"), Ipv4Mask("255.255.255.0"), 2); // R3
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.4.0"), Ipv4Mask("255.255.255.0"), 3); // R4
	}
	else if (i == 2) // Router R3
	{
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"), 1); // R1
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"), 2); // R2
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.4.0"), Ipv4Mask("255.255.255.0"), 3); // R4
	}
	else if (i == 3) // Router R4
	{
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"), 1); // R1
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"), 2); // R2
    	routing->AddNetworkRouteTo(Ipv4Address("10.1.3.0"), Ipv4Mask("255.255.255.0"), 3); // R3
	}
}
    

	// Define traffic matrix
    	uint32_t trafficMatrix[7][7];
	for (uint32_t i = 0; i < 7; ++i)
	{
	for (uint32_t j = 0; j < 7; ++j)
	{
    	if (i != j)
    	{
        	trafficMatrix[i][j] = (rand() %1000) +1 ;
    	}
    	else
    	{
        	trafficMatrix[i][j] = 0;
    	}
	}
}

	// Create UDP echo applications (client-server)
	ApplicationContainer serverApps, clientApps;
	for (uint32_t i = 0; i < 7; ++i)
	{
    	UdpEchoServerHelper echoServer(9);
    	serverApps.Add(echoServer.Install(workstations.Get(i)));

    	for (uint32_t j = 0; j < 7; ++j)
    	{
        	if (trafficMatrix[i][j] > 0 && i != j)
        	{
            	Ipv4Address destIp = workstationInterfaces.GetAddress(j, 0);
            	UdpEchoClientHelper echoClient(destIp, 9);
            	echoClient.SetAttribute("MaxPackets", UintegerValue(trafficMatrix[i][j]));
            	echoClient.SetAttribute("Interval", TimeValue(Seconds(0.1)));
            	echoClient.SetAttribute("PacketSize", UintegerValue(512));
            	clientApps.Add(echoClient.Install(workstations.Get(i)));
        	}
    	}
	}
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(6000.0));
	clientApps.Start(Seconds(2.0));
	clientApps.Stop(Seconds(6000.0));

	// Enable tracing
	AsciiTraceHelper ascii;
	p2p.EnableAsciiAll(ascii.CreateFileStream("racksetup.tr"));
	p2p.EnablePcapAll("racksetup");

	// Install flow monitor
	FlowMonitorHelper flowHelper;
	Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

	// Run the simulation
	Simulator::Stop(Seconds(600.0));
	Simulator::Run();

	// Analyze flow monitor results
	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

	std::cout << "Source-Destination Matrix (Delays in ms):\n";
    

for (const auto& flow : stats)
{
	Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(flow.first);
	double avgDelay = flow.second.delaySum.GetSeconds() / (flow.second.rxPackets+1); // in seconds
	double variance = 0.0;

	if (flow.second.rxPackets > 0)
	{
    	double meanDelaySquared = avgDelay * avgDelay;
    	variance = (flow.second.delaySum.GetSeconds() * avgDelay) - meanDelaySquared;
	}

	avgDelay *= 1000; // Convert to milliseconds
	variance *= 1000; // Convert to ms^2
	if (avgDelay != 0){
    
   	std::cout << "Flow : "
          	<< " (" << tuple.sourceAddress << " -> " << tuple.destinationAddress << ")\n"
          	<< "  Avg Delay: " << std::fixed << std::setprecision(12) << avgDelay << " ms, "
          	<< "Variance: " << std::fixed << std::setprecision(12) << variance << " ms^2\n";
	}
}

	std::cout << "\nPacket Drops \n";
	for (const auto& flow : stats)
	{
    	if (flow.second.lostPackets != 0)
    	{
        	Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(flow.first);
        	std::cout << "Flow : " << flow.first
                  	<< " (" << tuple.sourceAddress << " -> " << tuple.destinationAddress << ")\n"
                  	<< "  Packet Drops: " << flow.second.lostPackets << "\n";
    	}
	}

	Simulator::Destroy();
	return 0;
}


