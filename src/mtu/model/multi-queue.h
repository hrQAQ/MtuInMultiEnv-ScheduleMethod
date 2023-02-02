#ifndef MULTIQUEUE_H
#define MULTIQUEUE_H

#include "ns3/queue.h"
// #include "ns3/log.h"
#include <queue>

namespace ns3 {

class MultiQueue : public Queue<Packet> {
   public:
    static TypeId GetTypeId(void);

    MultiQueue(/* args */);
    virtual ~MultiQueue();

    virtual bool Enqueue(Ptr<Packet> item);
    virtual Ptr<Packet> Dequeue(void);
    virtual Ptr<Packet> Remove(void);
    virtual Ptr<const Packet> Peek(void) const;
    void SetNumPriority(uint16_t pri_number);
    // return the size in bytes
    uint32_t GetTotalNumber(void);
    void GetNextIndex();

   private:
    // number of priorities
    // 3 or 8
    uint16_t pri_number;
    uint16_t queue_index;

    // RR queues
    std::vector<std::queue<Ptr<Packet>>> m_queues;
    int bitmap[65535];
};

}  // namespace ns3

#endif