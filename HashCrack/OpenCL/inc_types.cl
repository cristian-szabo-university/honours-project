#define PACKED __attribute__ ((packed))

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
