#pragma once

#define F_DECLARE_NO_COPY(ClassName) ClassName(const ClassName &) = delete;

#define F_PACKED __attribute__((packed))

/* Returns true if the specified bit mask is set. */
#define bit_set(x, flag) ((x & (flag)) > 0)

/* Functions that absolutely must be called with CDECL calling convention. */
#define CDECL

#define kassert(x)  if (!(x)) kerror("assertation failure: " + KString(__PRETTY_FUNCTION__));

static inline void outb (uint16_t port, uint8_t val)
{
    asm volatile ("outb %0, %1" : : "a" (val), "Nd" (port));
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
}

static inline void outw (uint16_t port, uint16_t val)
{
    asm volatile ("outw %0, %1" : : "a" (val), "Nd" (port));
}

static inline void outl (uint16_t port, uint32_t val)
{
    asm volatile ("outl %0, %1" : : "a" (val), "Nd" (port));
}

static inline uint8_t inb (uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
    return ret;
}

static inline uint16_t inw (uint16_t port)
{
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

static inline uint32_t inl (uint16_t port)
{
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

static inline void io_wait (void)
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    asm volatile ("outb %%al, $0x80" : : "a" (0));
    /* TODO: Is there any reason why al is forced? */
}

static inline bool are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ("pushf\n\t"
                  "pop %0"
                  : "=g" (flags));
    return flags & (1 << 9);
}

static inline uint64_t rdtsc()
{
    uint64_t ret;
    asm volatile ("rdtsc" : "=A" (ret));
    return ret;
}

/*
 * Inline assembler - %0, %1 refer to register in order they are assigned to variables. 
 * asm ("...." : outputs : inputs : clobbered); 
 */

/** Returns the index of the least significant bit that's set. Undefined 
 * result if value is 0. It's important to get the right bit length. 
 *
 * For any odd input this returns 0, because the first bit will be set. An
 * input of 0x8000 returns 15. 
 */
static inline uint32_t bit_scan_forward_32(uint32_t value)
{
    uint32_t ret; 
    asm volatile ("bsf %1, %0" : "=r" (ret) : "mr" (value)); 
    return ret; 
}

/** 
 * Returns the index of the most significant bit that's set. See bit scan forward.
 *
 * For an input of 0x8000 returns 15. 
 */
static inline uint32_t bit_scan_reverse_32(uint32_t value)
{
    uint32_t ret; 
    asm volatile ("bsr %1, %0" : "=r" (ret) : "mr" (value)); 
    return ret; 
}


