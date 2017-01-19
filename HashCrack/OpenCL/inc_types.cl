#define PACKED __attribute__ ((packed))

typedef uchar  u8;
typedef ushort u16;
typedef uint   u32;
typedef ulong  u64;

#define CONCAT_NAME(a, b)       a##b
#define VECT_TYPE(type, width) CONCAT_NAME(type, width)

#if (VECT_SIZE == 1)
    typedef uchar   u8x;
    typedef ushort  u16x;
    typedef uint    u32x;
    typedef ulong   u64x;
#else
    typedef VECT_TYPE(uchar,  VECT_SIZE)  u8x;
    typedef VECT_TYPE(ushort, VECT_SIZE) u16x;
    typedef VECT_TYPE(uint,   VECT_SIZE) u32x;
    typedef VECT_TYPE(ulong,  VECT_SIZE) u64x;
#endif

struct PACKED charset_t
{
    uchar data[256];

    uint size;
};

struct PACKED message_t
{
    uint data[16];

    uint size;
};

struct PACKED message_digest_t
{
    uint data[4];
};

struct PACKED message_prefix_t
{
    uint data;
};

struct PACKED message_index_t
{
    uint found;
    uint msg_batch_pos;
    uint inner_loop_pos;
};
