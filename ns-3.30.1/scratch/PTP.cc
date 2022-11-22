#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3; //Используется пространство имен ns‑3

NS_LOG_COMPONENT_DEFINE ("PTP"); //Определяется компонент журналирования

int main (int argc, char *argv[])
{
    CommandLine cmd;
    cmd.Parse (argc, argv);

    Time::SetResolution (Time::NS);
    LogComponentEnable("PTP", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    
    /* Создаются два узла, 
    которые мы будем связывать через соединение точка-точка */
    NodeContainer nodes;
    nodes.Create (2);

    /* Мы создаем экземпляр PointToPointHelper и устанавливаем связанные атрибуты так, чтобы на устройствах, 
    созданных с помощью помощника, получить передачу со скоростью пять мегабит в секунду 
    и задержкой пять наносекунд в канале. */
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ns"));
    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);
    
    // Создаем стек протокол
    InternetStackHelper stack;
    stack.Install (nodes);
    // Далее нужно назначить IP-адресс
    Ipv4AddressHelper address;
    address.SetBase ("192.168.0.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign (devices);
    /* Создаем экземпляр сервера на одном из узлов с устройством
    и клиенте на узле, имеющем только двухточечное устройство */
    UdpEchoServerHelper echoServer (4445); //Настройка эха сервера
    ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (10.0));
    /* Мы передаем необходимые атрибуты в конструктор UdpEchoClientHelper. 
    Мы говорим клиенту отправлять пакеты на сервер.
    Мы устанавливаем клиент на первой точке (nodes 0). */
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 4445);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
    echoClient.SetFill (clientApps.Get (0), "Hello World");
    echoClient.SetFill (clientApps.Get (0), 0xa5, 1024);
    uint8_t fill[] = {1, 0, 1, 0, 0, 1, 0, 1};
    echoClient.SetFill (clientApps.Get (0), fill, sizeof(fill), 1024);
    uint8_t N = sizeof(fill);
   
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));
    
    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("PTP.txt"));
    pointToPoint.EnablePcapAll ("PTP-PTP", true);

    Simulator::Run ();
    for(uint8_t i = 0; i < N; i++) 
    {
        if (fill[i] == 1)
        {
           std::cout << "Тут у нас = 1"  << "\n";
        }
        else if (fill[i] == 0)
        {
           std::cout << "Тут у нас = 0"  << "\n";
        }
    }
    Simulator::Stop ();
}