#pragma once
#include <std_types.h>
#include <hw/cpu.h>

extern "C" {
/** These routines probably shouldn't be used directly. */
void __kspin_lock(uint32 *p);
};

/** 
 * SpinLock provides a simple locking mechanism based around an atomic
 * exchange of a variable in memory. 
 *
 * The SpinLock loops, swapping the value 1 for the value in lock. 
 * Each time it checks to see if the value retrieved from lock is now 0, 
 * if it is it knows that lock now contains 1, and it holds the lock.
 *
 * To ensure all spinlocks reside in a unique cache line the spinlock
 * is aligned to 64 bytes and is 64 bytes in size.  This avoids two 
 * processes competing for ownership of the same 64 byte cache line,
 * which could be bad for performance.
 *
 * A SpinLock shouldn't be required when running with a single processor.
 * It should be sufficent to clear the interrupt flags. 
 */
class __attribute__((aligned(64))) SpinLock 
{
public:
    SpinLock()
        : lock_value(0)
    {}

    void lock()
    {
        if_flag_state = CPU::get_interrupt_flag_state();
        CPU::set_interrupt_flag(false); 
        __kspin_lock(&lock_value); 
    }

    void release()
    {
        __kspin_lock_release(&lock_value); 
        CPU::set_interrupt_flag(if_flag_state); 
    }

    bool is_locked()
    {
        return lock_value == 1; 
    }


private:
    bool if_flag_state; 
    static void __kspin_lock_release(uint32 *p) 
    { 
        *p = 0; 
    }

    /* Difficult to know whether it makes sense to copy a spinlock. 
     * It might be practical to copy it but always 0 the lock for
     * the copy. */
    SpinLock(const SpinLock &) = delete; 
    SpinLock & operator=(const SpinLock &) = delete; 
    /* padded to ensure it's on a unique cache line. */
    uint32 lock_value;
    uint32 saved_flags;
    char reserved[64 - sizeof(uint32)];
};

/** RAII wrapper around a spin-lock. Designed to reduce programmer error
 * by forcing a release of the lock when the ScopedSpinLock goes out of scope. 
 */

class ScopedSpinLock
{
public:
    ScopedSpinLock(SpinLock & _lock)
        :
        lock(_lock)
    {
        lock.lock(); 
    }

    ~ScopedSpinLock()
    {
        lock.release(); 
    }

private:
    SpinLock & lock; 
};

