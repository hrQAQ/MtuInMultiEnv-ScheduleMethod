#include "multi-queue.h"
// #include "ns3/log.h"
#include "ns3/mtu-utility.h"
#include "priority-tag.h"
#define QUEUE_NUMBER 3005
namespace ns3 {
// NS_LOG_COMPONENT_DEFINE("MultiQueue");
NS_OBJECT_ENSURE_REGISTERED(MultiQueue);

TypeId MultiQueue::GetTypeId(void) {
    TypeId tid = TypeId("ns3::MultiQueue").SetParent<Queue<Packet>>().SetGroupName("mtu").AddConstructor<MultiQueue>();
    return tid;
}

MultiQueue::MultiQueue() : pri_number(QUEUE_NUMBER) {
    // NS_LOG_FUNCTION(this);
    queue_index = 0;
    for (int i = 0; i < QUEUE_NUMBER; i++) {
        bitmap[i] = 0;
    }
    SetNumPriority(QUEUE_NUMBER);
}

MultiQueue::~MultiQueue() {
    // NS_LOG_FUNCTION(this);
}

void MultiQueue::SetNumPriority(uint16_t number) {
    // update the number of priorities
    pri_number = number;
    // update the weight vector
    if (number < 1) {
        // NS_LOG_ERROR("The number of prioroty queues cannot be less than 1");
        return;
    }
    pri_number = number;

    uint16_t current_number = m_queues.size();
    if (current_number < pri_number) {
        for (uint16_t i = current_number; i < (pri_number - current_number); i++) {
            std::queue<Ptr<Packet>> single_queue;
            m_queues.push_back(single_queue);
        }
    } else if (current_number > pri_number) {
        std::vector<Ptr<Packet>> vecTemp;
        uint16_t indexStart = pri_number;
        for (uint16_t i = indexStart; i < current_number; i++) {
            std::queue<Ptr<Packet>> queTemp = m_queues[i];
            while (queTemp.size() > 0) {
                vecTemp.push_back(queTemp.front());
                queTemp.pop();
            }
        }
        m_queues.resize(pri_number);
        std::queue<Ptr<Packet>> queBack = m_queues.back();
        for (uint32_t i = 0; i < vecTemp.size(); i++) {
            queBack.push(vecTemp[i]);
        }
    }
}

bool MultiQueue::Enqueue(Ptr<Packet> packet) {
    // peek the priority
    PriorityTag tag;
    bool tagFound = packet->PeekPacketTag(tag);
    // std::cout << "enqueue here" << std::endl;
    // FindFirstMatchingByteTag(tag);
    if (tagFound) {
        // std::cout << "found tag" << std::endl;
        uint16_t priority = tag.GetPriorityTag();

        // save to the corresponding queue
        if (priority >= m_queues.size()) {
            m_queues.back().push(packet);
            bitmap[QUEUE_NUMBER - 1] = 1;
            return true;
        } else {
            bitmap[priority] = 1;
            m_queues[priority].push(packet);
            // std::cout << "queue_index: " << priority  << " queue size: " << m_queues[priority].size() << std::endl;
            return true;
        }
    } else {
        // if not found --> highest priority
        bitmap[0] = 1;
        m_queues[0].push(packet);
        // m_queues[m_queues.size() - 1].push(packet);

        return true;
    }
}  // namespace ns3

Ptr<Packet> MultiQueue::Dequeue() {
    Ptr<Packet> packet;
    if (bitmap[0] == 1) {
        // packets exists in highest priority
        packet = m_queues[0].front();
        m_queues[0].pop();
        if (m_queues[0].empty()) {
            bitmap[0] = 0;
        }
    } else {
        // no packets in highest priority
        GetNextIndex();
        if (queue_index == 0) {
            // no packet in device
            return 0;
        }
        // std::cout << "queue_index: " << queue_index << std::endl;
        packet = m_queues[queue_index].front();
        m_queues[queue_index].pop();
        if (m_queues[queue_index].empty()) {
            bitmap[queue_index] = 0;
        }
    }
    return packet;
}

Ptr<const Packet> MultiQueue::Peek(void) const {
    // NS_LOG_FUNCTION(this);
    for (int i = 0; i < pri_number; i++) {
        if (m_queues[i].size() != 0) {
            return m_queues[i].front();
        }
    }
    return 0;
}

Ptr<Packet> MultiQueue::Remove() {
    if (GetTotalNumber() == 0) {
        std::cout << "No packets in queue" << std::endl;
        return 0;
    }
    Ptr<Packet> packet;
    for (uint16_t i = 0; i < m_queues.size(); i++) {
        if (m_queues[i].size() != 0) {
            packet = m_queues[i].front();
        }
    }
    return packet;
}

uint32_t MultiQueue::GetTotalNumber() {
    uint32_t total_num = 0;

    for (uint16_t i = 0; i < m_queues.size(); i++) {
        std::queue<Ptr<Packet>> q = m_queues[i];
        while (q.size() != 0) {
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

void MultiQueue::GetNextIndex() {
    uint16_t cur_index = queue_index;
    // nxt idx > cur idx
    for (uint16_t i = cur_index + 1; i < QUEUE_NUMBER; i++) {
        if (bitmap[i]) {
            queue_index = i;
            return;
        }
    }
    for (uint16_t i = 1; i <= cur_index; i++) {
        if (bitmap[i]) {
            queue_index = i;
            return;
        }
    }
    // no packet in queue
    queue_index = 0;
}

}  // namespace ns3
