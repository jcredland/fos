

enum KLogLevel
{
    KL_ERROR,
    KL_DEBUG
};

inline void klog(KLogLevel level, const KString & log)
{
    vga.writeln(log); 
}

