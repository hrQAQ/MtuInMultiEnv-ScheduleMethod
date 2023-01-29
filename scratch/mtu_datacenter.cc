#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mtu-module.h"
#include "ns3/mtu-utility.h"
#include "ns3/network-module.h"
#include "ns3/node.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ptr.h"

#define START_TIME 0.0
#define END_TIME 10

// 128M
#define BUFFER_SIZE 134217728           // 设置TCP的发送缓冲区大小
#define MTU 9000                        // 设置MTU
#define MSS MTU - 40                    // 设置MSS
#define TCP_PROTOCOL "ns3::TcpNewReno"  // 设置TCP协议

#define PORT_START 1000  // 设置端口号 从非周知端口开始
#define PORT_END 1100    // 设置端口号   到max端口结束

// #define DATA_RATE "1Gbps"
// #define PROPOGATION_DELAY "100us"
// #define BANDWIDTH_LINK "1Gbps"
// #define LOAD 2

// cmd传参 全局变量用于ertern
std::string PROPOGATION_DELAY = "100us";  // 传播延迟
std::string BANDWIDTH_LINK = "1Gbps";     // 链路带宽
double LOSS_RATE = 0.0;                   // 丢包率
double LOAD = 0.8;                        // 链路的负载状况
std::string SCHEDULE_METHOD = "SRPT";     // 调度算法 1. FCFS 2. SRPT 3. RR 4. SJF

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DataCenter");

int main(int argc, char *argv[]) {
    LogComponentEnable("DataCenter", LOG_INFO);
    LogComponentEnable("MtuNetDevice", LOG_INFO);
    CommandLine cmd;
    cmd.AddValue("DELAY", "延迟", PROPOGATION_DELAY);
    cmd.AddValue("LOSS_RATE", "丢包率", LOSS_RATE);
    cmd.AddValue("BANDWIDTH_LINK", "数据中心链路带宽", BANDWIDTH_LINK);
    cmd.AddValue("LOAD", "链路的负载状况", LOAD);
    cmd.AddValue("SCHEDULE_METHOD", "调度算法", SCHEDULE_METHOD);
    cmd.Parse(argc, argv);

    std::cout << "DataCenter running with SCHEDULE_METHOD " << SCHEDULE_METHOD << std::endl;

    Time::SetResolution(Time::NS);                                                    // 设置最小时间单元 ns
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(BUFFER_SIZE));     // 设置TCP的发送缓冲区大小
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(BUFFER_SIZE));     // 设置TCP的接收缓冲区大小
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(MSS));            // 设置MSS
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue(TCP_PROTOCOL));  // 设置TCP协议
    Config::SetDefault("ns3::PointToPointNetDevice::Mtu", UintegerValue(MTU));        // 设置MTU

    // set the rto
    uint64_t rtt = Time(PROPOGATION_DELAY).GetNanoSeconds() * 8;
    double rto = Time(PROPOGATION_DELAY).GetSeconds() * 8 * 5;
    Config::SetDefault("ns3::TcpSocketBase::MinRto", TimeValue(Seconds(rto)));

    // generate cdf table
    std::string cdfFileName = "./scratch/DCTCP_CDF.txt";  // CDF文件路径
    struct cdf_table *cdfTable = new cdf_table();         // 生成cdf表
    MtuUtility::init_cdf(cdfTable);                       // 初始化cdf表
    MtuUtility::load_cdf(cdfTable, cdfFileName.c_str());  // 读取cdf文件

    // string FCT file name FCT(dc)_$(propogation_delay)_$(bandwidth)_$(loss_rate)_$(load)
    std::string FCT_fileName = std::string("FCT(dc)_").append(PROPOGATION_DELAY).append(std::string("_")).append(BANDWIDTH_LINK).append(std::string("_"));
    FCT_fileName = FCT_fileName.append(std::to_string(LOSS_RATE)).append(std::string("_")).append(std::to_string(LOAD));

    // std::cout << FCT_fileName << std::endl;

    // leaf spine topology
    NodeContainer spines, leafs, ends;
    spines.Create(4);
    leafs.Create(4);
    ends.Create(32);

    // install stack 安装协议栈
    InternetStackHelper stackHelper;
    stackHelper.Install(spines);
    stackHelper.Install(leafs);
    stackHelper.Install(ends);

    // forwarding delay in switches, half for each netdevice
    // 1968 nanoseconds for 10G prots, 4587 nanoseconds for GE ports, 5928nanoseconds for 100M ports
    // uint32_t delay = 4587;  // 这个delay是Switch的存储转发延迟

    MtuNetHelper netHelper;

    netHelper.SetDeviceAttribute("DataRate", StringValue(BANDWIDTH_LINK));   // 设置链路带宽
    netHelper.SetChannelAttribute("Delay", StringValue(PROPOGATION_DELAY));  // 设置传播延迟
    netHelper.data_fileName = FCT_fileName;
    netHelper.rtt = rtt;

    NetDeviceContainer devices_e0l0 = netHelper.InstallNormalNetDevices(ends.Get(0), leafs.Get(0));
    NetDeviceContainer devices_e1l0 = netHelper.InstallNormalNetDevices(ends.Get(1), leafs.Get(0));
    NetDeviceContainer devices_e2l0 = netHelper.InstallNormalNetDevices(ends.Get(2), leafs.Get(0));
    NetDeviceContainer devices_e3l0 = netHelper.InstallNormalNetDevices(ends.Get(3), leafs.Get(0));
    NetDeviceContainer devices_e4l0 = netHelper.InstallNormalNetDevices(ends.Get(4), leafs.Get(0));
    NetDeviceContainer devices_e5l0 = netHelper.InstallNormalNetDevices(ends.Get(5), leafs.Get(0));
    NetDeviceContainer devices_e6l0 = netHelper.InstallNormalNetDevices(ends.Get(6), leafs.Get(0));
    NetDeviceContainer devices_e7l0 = netHelper.InstallNormalNetDevices(ends.Get(7), leafs.Get(0));

    NetDeviceContainer devices_e8l1 = netHelper.InstallNormalNetDevices(ends.Get(8), leafs.Get(1));
    NetDeviceContainer devices_e9l1 = netHelper.InstallNormalNetDevices(ends.Get(9), leafs.Get(1));
    NetDeviceContainer devices_e10l1 = netHelper.InstallNormalNetDevices(ends.Get(10), leafs.Get(1));
    NetDeviceContainer devices_e11l1 = netHelper.InstallNormalNetDevices(ends.Get(11), leafs.Get(1));
    NetDeviceContainer devices_e12l1 = netHelper.InstallNormalNetDevices(ends.Get(12), leafs.Get(1));
    NetDeviceContainer devices_e13l1 = netHelper.InstallNormalNetDevices(ends.Get(13), leafs.Get(1));
    NetDeviceContainer devices_e14l1 = netHelper.InstallNormalNetDevices(ends.Get(14), leafs.Get(1));
    NetDeviceContainer devices_e15l1 = netHelper.InstallNormalNetDevices(ends.Get(15), leafs.Get(1));

    NetDeviceContainer devices_e16l2 = netHelper.InstallNormalNetDevices(ends.Get(16), leafs.Get(2));
    NetDeviceContainer devices_e17l2 = netHelper.InstallNormalNetDevices(ends.Get(17), leafs.Get(2));
    NetDeviceContainer devices_e18l2 = netHelper.InstallNormalNetDevices(ends.Get(18), leafs.Get(2));
    NetDeviceContainer devices_e19l2 = netHelper.InstallNormalNetDevices(ends.Get(19), leafs.Get(2));
    NetDeviceContainer devices_e20l2 = netHelper.InstallNormalNetDevices(ends.Get(20), leafs.Get(2));
    NetDeviceContainer devices_e21l2 = netHelper.InstallNormalNetDevices(ends.Get(21), leafs.Get(2));
    NetDeviceContainer devices_e22l2 = netHelper.InstallNormalNetDevices(ends.Get(22), leafs.Get(2));
    NetDeviceContainer devices_e23l2 = netHelper.InstallNormalNetDevices(ends.Get(23), leafs.Get(2));

    NetDeviceContainer devices_e24l3 = netHelper.InstallNormalNetDevices(ends.Get(24), leafs.Get(3));
    NetDeviceContainer devices_e25l3 = netHelper.InstallNormalNetDevices(ends.Get(25), leafs.Get(3));
    NetDeviceContainer devices_e26l3 = netHelper.InstallNormalNetDevices(ends.Get(26), leafs.Get(3));
    NetDeviceContainer devices_e27l3 = netHelper.InstallNormalNetDevices(ends.Get(27), leafs.Get(3));
    NetDeviceContainer devices_e28l3 = netHelper.InstallNormalNetDevices(ends.Get(28), leafs.Get(3));
    NetDeviceContainer devices_e29l3 = netHelper.InstallNormalNetDevices(ends.Get(29), leafs.Get(3));
    NetDeviceContainer devices_e30l3 = netHelper.InstallNormalNetDevices(ends.Get(30), leafs.Get(3));
    NetDeviceContainer devices_e31l3 = netHelper.InstallNormalNetDevices(ends.Get(31), leafs.Get(3));

    NetDeviceContainer devices_l0s0 = netHelper.InstallMtuNetDevices(leafs.Get(0), spines.Get(0), LOSS_RATE);
    NetDeviceContainer devices_l0s1 = netHelper.InstallMtuNetDevices(leafs.Get(0), spines.Get(1), LOSS_RATE);
    NetDeviceContainer devices_l0s2 = netHelper.InstallMtuNetDevices(leafs.Get(0), spines.Get(2), LOSS_RATE);
    NetDeviceContainer devices_l0s3 = netHelper.InstallMtuNetDevices(leafs.Get(0), spines.Get(3), LOSS_RATE);

    NetDeviceContainer devices_l1s0 = netHelper.InstallMtuNetDevices(leafs.Get(1), spines.Get(0), LOSS_RATE);
    NetDeviceContainer devices_l1s1 = netHelper.InstallMtuNetDevices(leafs.Get(1), spines.Get(1), LOSS_RATE);
    NetDeviceContainer devices_l1s2 = netHelper.InstallMtuNetDevices(leafs.Get(1), spines.Get(2), LOSS_RATE);
    NetDeviceContainer devices_l1s3 = netHelper.InstallMtuNetDevices(leafs.Get(1), spines.Get(3), LOSS_RATE);

    NetDeviceContainer devices_l2s0 = netHelper.InstallMtuNetDevices(leafs.Get(2), spines.Get(0), LOSS_RATE);
    NetDeviceContainer devices_l2s1 = netHelper.InstallMtuNetDevices(leafs.Get(2), spines.Get(1), LOSS_RATE);
    NetDeviceContainer devices_l2s2 = netHelper.InstallMtuNetDevices(leafs.Get(2), spines.Get(2), LOSS_RATE);
    NetDeviceContainer devices_l2s3 = netHelper.InstallMtuNetDevices(leafs.Get(2), spines.Get(3), LOSS_RATE);

    NetDeviceContainer devices_l3s0 = netHelper.InstallMtuNetDevices(leafs.Get(3), spines.Get(0), LOSS_RATE);
    NetDeviceContainer devices_l3s1 = netHelper.InstallMtuNetDevices(leafs.Get(3), spines.Get(1), LOSS_RATE);
    NetDeviceContainer devices_l3s2 = netHelper.InstallMtuNetDevices(leafs.Get(3), spines.Get(2), LOSS_RATE);
    NetDeviceContainer devices_l3s3 = netHelper.InstallMtuNetDevices(leafs.Get(3), spines.Get(3), LOSS_RATE);

    Ipv4AddressHelper ipv4Helper;
    // assign ip for spines and leafs

    ipv4Helper.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l0s0 = ipv4Helper.Assign(devices_l0s0);
    ipv4Helper.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l0s1 = ipv4Helper.Assign(devices_l0s1);
    ipv4Helper.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l0s2 = ipv4Helper.Assign(devices_l0s2);
    ipv4Helper.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l0s3 = ipv4Helper.Assign(devices_l0s3);

    ipv4Helper.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l1s0 = ipv4Helper.Assign(devices_l1s0);
    ipv4Helper.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l1s1 = ipv4Helper.Assign(devices_l1s1);
    ipv4Helper.SetBase("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l1s2 = ipv4Helper.Assign(devices_l1s2);
    ipv4Helper.SetBase("10.1.8.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l1s3 = ipv4Helper.Assign(devices_l1s3);

    ipv4Helper.SetBase("10.1.9.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l2s0 = ipv4Helper.Assign(devices_l2s0);
    ipv4Helper.SetBase("10.1.10.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l2s1 = ipv4Helper.Assign(devices_l2s1);
    ipv4Helper.SetBase("10.1.11.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l2s2 = ipv4Helper.Assign(devices_l2s2);
    ipv4Helper.SetBase("10.1.12.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l2s3 = ipv4Helper.Assign(devices_l2s3);

    ipv4Helper.SetBase("10.1.13.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l3s0 = ipv4Helper.Assign(devices_l3s0);
    ipv4Helper.SetBase("10.1.14.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l3s1 = ipv4Helper.Assign(devices_l3s1);
    ipv4Helper.SetBase("10.1.15.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l3s2 = ipv4Helper.Assign(devices_l3s2);
    ipv4Helper.SetBase("10.1.16.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_l3s3 = ipv4Helper.Assign(devices_l3s3);

    ipv4Helper.SetBase("10.2.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e0l0 = ipv4Helper.Assign(devices_e0l0);
    ipv4Helper.SetBase("10.2.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e1l0 = ipv4Helper.Assign(devices_e1l0);
    ipv4Helper.SetBase("10.2.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e2l0 = ipv4Helper.Assign(devices_e2l0);
    ipv4Helper.SetBase("10.2.4.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e3l0 = ipv4Helper.Assign(devices_e3l0);
    ipv4Helper.SetBase("10.2.5.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e4l0 = ipv4Helper.Assign(devices_e4l0);
    ipv4Helper.SetBase("10.2.6.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e5l0 = ipv4Helper.Assign(devices_e5l0);
    ipv4Helper.SetBase("10.2.7.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e6l0 = ipv4Helper.Assign(devices_e6l0);
    ipv4Helper.SetBase("10.2.8.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e7l0 = ipv4Helper.Assign(devices_e7l0);

    ipv4Helper.SetBase("10.2.9.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e8l1 = ipv4Helper.Assign(devices_e8l1);
    ipv4Helper.SetBase("10.2.10.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e9l1 = ipv4Helper.Assign(devices_e9l1);
    ipv4Helper.SetBase("10.2.11.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e10l1 = ipv4Helper.Assign(devices_e10l1);
    ipv4Helper.SetBase("10.2.12.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e11l1 = ipv4Helper.Assign(devices_e11l1);
    ipv4Helper.SetBase("10.2.13.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e12l1 = ipv4Helper.Assign(devices_e12l1);
    ipv4Helper.SetBase("10.2.14.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e13l1 = ipv4Helper.Assign(devices_e13l1);
    ipv4Helper.SetBase("10.2.15.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e14l1 = ipv4Helper.Assign(devices_e14l1);
    ipv4Helper.SetBase("10.2.16.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e15l1 = ipv4Helper.Assign(devices_e15l1);

    ipv4Helper.SetBase("10.2.17.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e16l2 = ipv4Helper.Assign(devices_e16l2);
    ipv4Helper.SetBase("10.2.18.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e17l2 = ipv4Helper.Assign(devices_e17l2);
    ipv4Helper.SetBase("10.2.19.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e18l2 = ipv4Helper.Assign(devices_e18l2);
    ipv4Helper.SetBase("10.2.20.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e19l2 = ipv4Helper.Assign(devices_e19l2);
    ipv4Helper.SetBase("10.2.21.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e20l2 = ipv4Helper.Assign(devices_e20l2);
    ipv4Helper.SetBase("10.2.22.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e21l2 = ipv4Helper.Assign(devices_e21l2);
    ipv4Helper.SetBase("10.2.23.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e22l2 = ipv4Helper.Assign(devices_e22l2);
    ipv4Helper.SetBase("10.2.24.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e23l2 = ipv4Helper.Assign(devices_e23l2);

    ipv4Helper.SetBase("10.2.25.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e24l3 = ipv4Helper.Assign(devices_e24l3);
    ipv4Helper.SetBase("10.2.26.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e25l3 = ipv4Helper.Assign(devices_e25l3);
    ipv4Helper.SetBase("10.2.27.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e26l3 = ipv4Helper.Assign(devices_e26l3);
    ipv4Helper.SetBase("10.2.28.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e27l3 = ipv4Helper.Assign(devices_e27l3);
    ipv4Helper.SetBase("10.2.29.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e28l3 = ipv4Helper.Assign(devices_e28l3);
    ipv4Helper.SetBase("10.2.30.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e29l3 = ipv4Helper.Assign(devices_e29l3);
    ipv4Helper.SetBase("10.2.31.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e30l3 = ipv4Helper.Assign(devices_e30l3);
    ipv4Helper.SetBase("10.2.32.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_e31l3 = ipv4Helper.Assign(devices_e31l3);

    double request_rate = LOAD * DataRate(BANDWIDTH_LINK).GetBitRate() / (8 * MtuUtility::avg_cdf(cdfTable));
    // std::cout << request_rate << std::endl;
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    std::vector<Ipv4Address> dstAddress;
    dstAddress.push_back(interface_e0l0.GetAddress(0));
    dstAddress.push_back(interface_e1l0.GetAddress(0));
    dstAddress.push_back(interface_e2l0.GetAddress(0));
    dstAddress.push_back(interface_e3l0.GetAddress(0));
    dstAddress.push_back(interface_e4l0.GetAddress(0));
    dstAddress.push_back(interface_e5l0.GetAddress(0));
    dstAddress.push_back(interface_e6l0.GetAddress(0));
    dstAddress.push_back(interface_e7l0.GetAddress(0));
    dstAddress.push_back(interface_e8l1.GetAddress(0));
    dstAddress.push_back(interface_e9l1.GetAddress(0));
    dstAddress.push_back(interface_e10l1.GetAddress(0));
    dstAddress.push_back(interface_e11l1.GetAddress(0));
    dstAddress.push_back(interface_e12l1.GetAddress(0));
    dstAddress.push_back(interface_e13l1.GetAddress(0));
    dstAddress.push_back(interface_e14l1.GetAddress(0));
    dstAddress.push_back(interface_e15l1.GetAddress(0));
    dstAddress.push_back(interface_e16l2.GetAddress(0));
    dstAddress.push_back(interface_e17l2.GetAddress(0));
    dstAddress.push_back(interface_e18l2.GetAddress(0));
    dstAddress.push_back(interface_e19l2.GetAddress(0));
    dstAddress.push_back(interface_e20l2.GetAddress(0));
    dstAddress.push_back(interface_e21l2.GetAddress(0));
    dstAddress.push_back(interface_e22l2.GetAddress(0));
    dstAddress.push_back(interface_e23l2.GetAddress(0));
    dstAddress.push_back(interface_e24l3.GetAddress(0));
    dstAddress.push_back(interface_e25l3.GetAddress(0));
    dstAddress.push_back(interface_e26l3.GetAddress(0));
    dstAddress.push_back(interface_e27l3.GetAddress(0));
    dstAddress.push_back(interface_e28l3.GetAddress(0));
    dstAddress.push_back(interface_e29l3.GetAddress(0));
    dstAddress.push_back(interface_e30l3.GetAddress(0));
    dstAddress.push_back(interface_e31l3.GetAddress(0));

    uint32_t flowCount = 0;
    uint64_t bandwidth = DataRate(BANDWIDTH_LINK).GetBitRate();
    double delay_prop = double(Time(PROPOGATION_DELAY).GetMicroSeconds()) / 1000;
    double delay_process = 0, delay_tx = 0, delay_rx = 0;
    double end_gen_time = 64535.0 / request_rate / 32;

    netHelper.InstallAllApplicationsInDC(ends, ends, request_rate, cdfTable, dstAddress, flowCount, PORT_START, PORT_END, START_TIME, END_TIME, end_gen_time, bandwidth, delay_prop,
                                         delay_process, delay_tx, delay_rx, SCHEDULE_METHOD);
    std::cout << "Total flow count number: " << flowCount << std::endl;

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();
    Simulator::Stop(Seconds(END_TIME));
    Simulator::Run();

    std::cout << "FlowMonitor SerializeToXmlFile" << std::endl;
    flowMonitor->CheckForLostPackets();
    flowMonitor->SerializeToXmlFile("data/dc/"+ SCHEDULE_METHOD + "/" + FCT_fileName, true, true);

    Simulator::Destroy();
    MtuUtility::free_cdf(cdfTable);

    return 0;
}