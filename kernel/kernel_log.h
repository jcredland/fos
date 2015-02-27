

enum KLogLevel
{
    KL_ERROR,
    KL_DEBUG
};

inline void klog(KLogLevel level, const KString & log)
{
    vga.writeln(log); 
}

inline void kdebug(const KString & log)
{
    klog(KL_DEBUG, log); 
}

inline void kerror(const KString & log)
{
    klog(KL_ERROR, log); 
}
