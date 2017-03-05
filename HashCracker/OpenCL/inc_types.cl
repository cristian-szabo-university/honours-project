#define PACKED __attribute__ ((packed))

typedef uchar  u8;
typedef ushort u16;
typedef uint   u32;
typedef ulong  u64;

#define CONCAT_NAME(a, b) a##b
#define VECT_TYPE(type, width) CONCAT_NAME(type, width)

#if (VECTOR_SIZE == 1)
    typedef uchar   u8x;
    typedef ushort  u16x;
    typedef uint    u32x;
    typedef ulong   u64x;
#else
    typedef VECT_TYPE(uchar,  VECTOR_SIZE)  u8x;
    typedef VECT_TYPE(ushort, VECTOR_SIZE) u16x;
    typedef VECT_TYPE(uint,   VECTOR_SIZE) u32x;
    typedef VECT_TYPE(ulong,  VECTOR_SIZE) u64x;
#endif

struct PACKED charset_t
{
    uchar data[256];

    u32 size;
};

struct PACKED message_t
{
    u32 data[16];

    u32 size;
};

struct PACKED message_digest_t
{
    u32 data[DIGEST_SIZE];
};

struct PACKED message_prefix_t
{
    u32x data;
};

struct PACKED message_index_t
{
    u32 found;
    u32 msg_batch_pos;
    u32 inner_loop_pos;
};
