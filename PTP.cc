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

    /* Опять же, мы передаем необходимые атрибуты в конструктор UdpEchoClientHelper. 
    Мы говорим клиенту отправлять пакеты на сервер.
    Мы устанавливаем клиент на первой точке (nodes 0). */
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 4445);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    Simulator::Run ();
    Simulator::Stop ();
}