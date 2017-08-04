#pragma once
#include <string.h>
//content is populated only once, on download, so it's OK to use dynamic memory allocation even on embeded devices
#define MAX_PAGES_CNT 27
#define MAX_PAGE_SIZE 0x100
class MemoryMap
{
    unsigned char* m_tblPages[MAX_PAGES_CNT];

    //prevent copy operations:
    MemoryMap(const MemoryMap& rhs);
    MemoryMap& operator=(const MemoryMap& rhs);

    int Access(bool read, int iPage, int iIndex, int iToAccessBytes, unsigned char* buffer)
    {
        if(read && Exist(iPage) == false)
        {
            return 0;
        }

        int totalAccessed = 0;
        while(iToAccessBytes > 0)
        {
            unsigned char* pPage = GetPage(iPage);
            if(pPage == NULL)
            {
                //DEBUG(LOG_ERR, "Requested too large page: %d", iPage);
                break;
            }

            if(iIndex + iToAccessBytes <= MAX_PAGE_SIZE)
            {
                //read/write fits the page
                if(read)
                {
                    memcpy(buffer, pPage+iIndex, iToAccessBytes);
                }
                else
                {
                    memcpy(pPage+iIndex, buffer, iToAccessBytes);
                }
                
                totalAccessed += iToAccessBytes;
                buffer += iToAccessBytes;
                iToAccessBytes = 0;
            }
            else
            {
                //read/write spans more than one page, let's remaining ammount from current one
                int iFree = MAX_PAGE_SIZE - iIndex;
                if(read)
                {
                    memcpy(buffer, pPage+iIndex, iFree);
                }
                else
                {
                    memcpy(pPage+iIndex, buffer, iFree);
                }

                totalAccessed += iFree;
                buffer += iFree;
                iToAccessBytes -= iFree;

                //Set it to the next page and index to 0
                iPage += 1;
                iIndex = 0;
            }
        }

        return totalAccessed;
    }

public:
    MemoryMap()
    {
        memset(m_tblPages, 0, sizeof(m_tblPages));
    }

    ~MemoryMap()
    {
        Clear();
    }

    void Clear()
    {
        for(int ix=0; ix<MAX_PAGES_CNT; ix++)
        {
            delete [] m_tblPages[ix];
            m_tblPages[ix] = NULL;
        }
    }

    bool Exist(int iPage) const
    {
        if(iPage<0 || iPage>=MAX_PAGES_CNT)
        {
            return false;
        }

        return m_tblPages[iPage] != NULL;
    }

    unsigned char* GetPage(int iPage)
    {
        if(iPage<0 || iPage>=MAX_PAGES_CNT)
        {
            return NULL;
        }

        if(m_tblPages[iPage] != NULL)
        {
            return m_tblPages[iPage];
        }

        m_tblPages[iPage] = new unsigned char[MAX_PAGE_SIZE];
        memset(m_tblPages[iPage], 0xFF, MAX_PAGE_SIZE);
        return m_tblPages[iPage];
    }

    int Read(int iPage, int iIndex, int iToReadBytes, void* bufferOut)
    {
        return Access(true, iPage, iIndex, iToReadBytes, (unsigned char*)bufferOut);
    }

    int Write(int iPage, int iIndex, int iToWriteBytes, const void* bufferIn)
    {
        return Access(false, iPage, iIndex, iToWriteBytes, (unsigned char*)bufferIn);
    }
};
