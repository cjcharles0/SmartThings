#pragma once
/// <summary>
/// Fixed size queue (does not call new/delete, so will not lead to memory fragmentation (important on embeded systems)).
/// Items must be POD suitable for memcpy.
/// </summary>
template<typename T, int QCNT>
class FixedSizeQueue {
    struct Node
    {
        T item;
        bool used;
    };

    Node m_queue[QCNT];

 public:
    FixedSizeQueue (){ clear(); }

    void clear()
    {
        memset(&m_queue, 0, sizeof(m_queue));
    }

    bool push (const T& i)
    {
        int pos = findEmpty();
        if(pos == -1)
        {
            return false;
        }

        memcpy(&m_queue[pos].item, &i, sizeof(T));
        m_queue[pos].used = true;
        return true;
    }

    // pop an item from the queue.
    T pop ()
    {
        T res = m_queue[0].item;

        //shift all down:
        for(int ix=1; ix<QCNT; ix++)
        {
            memcpy(&m_queue[ix-1], &m_queue[ix], sizeof(Node));
            memset(&m_queue[ix], 0, sizeof(Node));
        }

        return res;
    }

    const T& peek () const { return m_queue[0].item; }
    bool isEmpty ()  const { return m_queue[0].used == false; }
    bool isFull  ()  const { return m_queue[QCNT-1].used; }

    // get the number of items in the queue.
    int count () const
    {
        for(int ix=0; ix<QCNT; ix++)
        {
            if(m_queue[ix].used == false)
            {
                return ix;
            }
        }

        return QCNT;
    }

private:
    int findEmpty() const
    {
        for(int ix=0; ix<QCNT; ix++)
        {
            if(m_queue[ix].used == false)
            {
                return ix;
            }
        }

        return -1;
    }
};
