/*
*
* 文件名称：Snowflake.h
* 文件标识：
* 摘    要：通过SnowFlake算法生成一个64位大小的分布式自增长id
*
*/
 
#ifndef __SNOWFLAKE_H__
#define __SNOWFLAKE_H__
 
#include <mutex>
#include <atomic>
 
//#define SNOWFLAKE_ID_WORKER_NO_LOCK
typedef unsigned int UInt;
typedef unsigned long UInt64;
 
#ifdef SNOWFLAKE_ID_WORKER_NO_LOCK
typedef std::atomic<UInt> AtomicUInt;
typedef std::atomic<UInt64> AtomicUInt64;
#else
typedef UInt AtomicUInt;
typedef UInt64 AtomicUInt64;
#endif
 
namespace service{
    class Snowflake
    {
    public:
        Snowflake(void);
        ~Snowflake(void);
 
        void setHostId(UInt HostId)
        {
            m_HostId = HostId;
        }
        void setWorkerId(UInt workerId)
        {
            m_WorkerId = workerId;
        }
        UInt64 GetId()
        {
            return GetDistributedId();
        }
 
    private:
        UInt64 GetTimeStamp();
        UInt64 tilNextMillis(UInt64 lastTimestamp);
        UInt64 GetDistributedId();
 
    private:
 
#ifndef SNOWFLAKE_ID_WORKER_NO_LOCK
        std::mutex mutex;
#endif
 
        /**
        * 开始时间截 (2019-09-30 00:00:00.000)
        */
        const UInt64 twepoch = 1569772800000;
 
        /**
        * worker进程映射id所占的位数
        */
        const UInt workerIdBits = 5;
 
        /**
        * 服务器id所占的位数
        */
        const UInt hostIdBits = 5;
 
        /**
        * 序列所占的位数
        */
        const UInt sequenceBits = 12;
 
        /**
        * worker进程映射ID向左移12位
        */
        const UInt workerIdShift = sequenceBits;
 
        /**
        * 服务器id向左移17位
        */
        const UInt hostIdShift = workerIdShift + workerIdBits;
 
        /**
        * 时间截向左移22位
        */
        const UInt timestampLeftShift = hostIdShift + hostIdBits;
 
        /**
        * 支持的worker进程映射id，结果是31
        */
        const UInt maxWorkerId = -1 ^ (-1 << workerIdBits);
 
        /**
        * 支持的服务器id，结果是31
        */
        const UInt maxHostId = -1 ^ (-1 << hostIdBits);
 
        /**
        * 生成序列的掩码，这里为4095
        */
        const UInt sequenceMask = -1 ^ (-1 << sequenceBits);
 
        /**
        * worker进程映射id(0~31)
        */
        UInt m_WorkerId;
 
        /**
        * 服务器id(0~31)
        */
        UInt m_HostId;
 
        /**
        * 毫秒内序列(0~4095)
        */
        AtomicUInt sequence{ 0 };
 
        /**
        * 上次生成ID的时间截
        */
        AtomicUInt64 lastTimestamp{ 0 };
    };
}
#endif