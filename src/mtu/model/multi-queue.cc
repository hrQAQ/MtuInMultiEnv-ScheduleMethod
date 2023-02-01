#include "multi-queue.h"
// #include "ns3/log.h"
#include "priority-tag.h"
#include "ns3/mtu-utility.h"

namespace ns3
{
    // NS_LOG_COMPONENT_DEFINE("MultiQueue");
    NS_OBJECT_ENSURE_REGISTERED(MultiQueue);

    TypeId MultiQueue::GetTypeId(void)
    {
        TypeId tid = TypeId("ns3::MultiQueue")
                         .SetParent<Queue<Packet>>()
                         .SetGroupName("mtu")
                         .AddConstructor<MultiQueue>();
        return tid;
    }

    MultiQueue::MultiQueue()
        : pri_number(2)
    {
        SetNumPriority(2);
    }

    MultiQueue::~MultiQueue()
    {
        // NS_LOG_FUNCTION(this);
    }

    void MultiQueue::SetNumPriority(uint16_t number)
    {

        //update the number of priorities
        pri_number = number;
        uint16_t current_number = m_queues.size();
        if (current_number < pri_number)
        {
            for (uint16_t i = current_number; i < (pri_number - current_number); i++)
            {
                std::queue<Ptr<Packet>> single_queue;
                m_queues.push_back(single_queue);
            }
        }
    }

    bool MultiQueue::Enqueue(Ptr<Packet> packet)
    {
        //peek the priority
        PriorityTag tag;
        bool tagFound = packet->PeekPacketTag(tag);
        // std::cout << "enqueue here" << std::endl;
        // FindFirstMatchingByteTag(tag);
        if (tagFound)
        {
            //save to the unique FCFS queue
            m_queues[1].push(packet);
            return true;
        }
        else
        {
            //if not found --> mice ack --> highest priority
            m_queues[0].push(packet);
            return true;
        }
    } // namespace ns3

    Ptr<Packet> MultiQueue::Dequeue()
    {
        Ptr<Packet> packet;
        if (m_queues[0].size() > 0)
        {
            //packets exists in highest priority ACK
            packet = m_queues[0].front();
            m_queues[0].pop();
        }
        else
        {
            if (m_queues[1].size() == 0)
                return 0;
            packet = m_queues[1].front();
            m_queues[1].pop();
        }
        return packet;
    }

    Ptr<const Packet> MultiQueue::Peek(void) const
    {
        // NS_LOG_FUNCTION(this);
        for (int i = 0; i < pri_number; i++)
        {
            if (m_queues[i].size() != 0)
            {
                return m_queues[i].front();
            }
        }
        return 0;
    }

    Ptr<Packet> MultiQueue::Remove()
    {
        if (GetTotalNumber() == 0)
        {
            std::cout << "No packets in queue" << std::endl;
            return 0;
        }
        Ptr<Packet> packet;
        for (uint16_t i = 0; i < m_queues.size(); i++)
        {
            if (m_queues[i].size() != 0)
            {
                packet = m_queues[i].front();
            }
        }
        return packet;
    }

    uint32_t MultiQueue::GetTotalNumber()
    {
        uint32_t total_num = 0;

        for (uint16_t i = 0; i < m_queues.size(); i++)
        {
            std::queue<Ptr<Packet>> q = m_queues[i];
            while (q.size() != 0)
            {
                total_num += q.front()->GetSize();
                q.pop();
            }
        }
        // for (uint16_t i = 0; i < m_queues.size(); i++)
        // {
        //     total_num += m_queues[i].size();
        // }
        return total_num;
    }

} // namespace ns3
