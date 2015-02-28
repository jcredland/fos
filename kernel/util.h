#ifndef UTIL_H_21
#define UTIL_H21


#define F_DECLARE_NO_COPY(ClassName) ClassName(const ClassName &) = delete;

#define F_PACKED __attribute__((packed))

/* Returns true if the specified bit mask is set. */
#define bit_set(x, flag) ((x & (flag)) > 0)

/* Functions that absolutely must be called with CDECL calling convention. */
#define CDECL

#define kassert(x)  if (!(x)) kerror(KString("assert:") + KString(__PRETTY_FUNCTION__));

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

#endif

