#ifndef CRIMSON_MUTEX_H
#define CRIMSON_MUTEX_H

#pragma once

class CMutex
{
    friend class Lock;
public:
    CMutex () { InitializeCriticalSection (& _critSection); }
    ~CMutex () { DeleteCriticalSection (& _critSection); }

    void Acquire ()
    {
        EnterCriticalSection (& _critSection);
    }
    void Release ()
    {
        LeaveCriticalSection (& _critSection);
    }

    CRITICAL_SECTION _critSection;
};

#endif