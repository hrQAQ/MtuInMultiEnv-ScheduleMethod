#include "ns3/mtu-net-device.h"
#include "ns3/priority-tag.h"
#include "ns3/multi-queue.h"
#include "ns3/simulator.h"
#include <fstream>
#include <iostream>
namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("MtuNetDevice");
    NS_OBJECT_ENSURE_REGISTERED(MtuNetDevice);

    TypeId MtuNetDevice::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::MtuNetDevice")
                                .SetParent<PointToPointNetDevice>()
                                .SetGroupName("mtu")
                                .AddConstructor<MtuNetDevice>();
        return tid;
    }

    MtuNetDevice::MtuNetDevice()
        : loss_rate(0.0)
    {
        NS_LOG_FUNCTION(this);
    }

    MtuNetDevice::~MtuNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    void MtuNetDevice::setLossRate(double lossRate)
    {
        loss_rate = lossRate;
    }

    double MtuNetDevice::getLossRate() const
    {
        return loss_rate;
    }

    bool MtuNetDevice::Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
    {
        PriorityTag tag;
        if (packet == 0)
        {
            std::cout << "packet is null " << std::endl;
        }

        if (loss_rate == 0)
        {
            return PointToPointNetDevice::Send(packet, dest, protocolNumber);
        }
        else
        {
            srand(time(0));
            double loss = double(rand()) / double(RAND_MAX);
            if (loss < loss_rate)
                return true;
            else
                return PointToPointNetDevice::Send(packet, dest, protocolNumber);
        }
    }

} // namespace ns3