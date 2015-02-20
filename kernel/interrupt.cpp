
InterruptDriver::InterruptDriver(void(*defaultHandler)(int))
{
    memset((char*)idt, 0, sizeof(idt));

    registerAllHandlers();

    for (int i = 0; i < MAX_INTERRUPT_VECTORS; ++i)
        handler[i] = defaultHandler;

    idtr.base = (uint32_t) idt;
    idtr.length = sizeof(idt) - 1;

    asm volatile ( "lidt (%0)" : : "p"(&idtr) );
}

extern char interrupt_stub_1; 
extern char interrupt_stub_0;

void InterruptDriver::registerAllHandlers()
{
    ptrdiff_t size = &interrupt_stub_1 - &interrupt_stub_0;
    char * h = &interrupt_stub_0;
    for (int i = 0; i < MAX_INTERRUPT_VECTORS; ++i)
    {
        setVectorStub(i, h);
        h += size;
    }
}
