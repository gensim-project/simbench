#define NOINLINE __attribute__((noinline))
#define ALIGN __attribute__((aligned(4096)))

#define FN(x,y) void ipd_fn_##y(); void NOINLINE ALIGN ipd_fn_##x() { ipd_fn_##y(); }
