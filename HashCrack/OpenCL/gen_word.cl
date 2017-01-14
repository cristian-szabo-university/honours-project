inline void generate_word(
    uint pw_buf[16], 
    __global struct charset_t* css_buf, 
    const uint css_count,
    const uint msg_prefix_size, 
    const uint msg_suffix_size,
    const uint mask, 
    const uint bit14,
    const uint bit15,
    ulong val)
{
    uint i;
    uint j;

    for (i = 0, j = msg_prefix_size; i < msg_suffix_size; i++, j++)
    {
        __global struct charset_t* cs = &css_buf[j];

        const uint len = cs->size;

        const ulong next = val / len;
        const ulong pos = val % len;

        val = next;

        const uint key = cs->data[pos];

        const uint jd4 = j / 4;
        const uint jm4 = j % 4;

        pw_buf[jd4] |= key << (jm4 << 3);
    }

    const uint jd4 = j / 4;
    const uint jm4 = j % 4;

    pw_buf[jd4] |= (0xff << (jm4 << 3)) & mask;

    if (bit14) pw_buf[14] = (msg_prefix_size + msg_suffix_size) << 3;
    if (bit15) pw_buf[15] = (msg_prefix_size + msg_suffix_size) << 3;
}

__kernel void generate_word_suffix(
    __global struct message_t* msg,
    __global struct charset_t* css_buf,
    const uint css_count,
    const uint msg_batch_size,
    const ulong msg_batch_offset,
    const uint msg_prefix_size, 
    const uint msg_suffix_size, 
    const uint mask, 
    const uint bit14,
    const uint bit15)
{
    const uint gid = get_global_id(0);

    if (gid >= msg_batch_size) return;

    uint pw_buf[16] = { 0 };

    generate_word(pw_buf, css_buf, css_count, msg_prefix_size, msg_suffix_size, mask, bit14, bit15, msg_batch_offset + gid);

    msg[gid].data[0] = pw_buf[0];
    msg[gid].data[1] = pw_buf[1];
    msg[gid].data[2] = pw_buf[2];
    msg[gid].data[3] = pw_buf[3];
    msg[gid].data[4] = pw_buf[4];
    msg[gid].data[5] = pw_buf[5];
    msg[gid].data[6] = pw_buf[6];
    msg[gid].data[7] = pw_buf[7];
    msg[gid].data[8] = pw_buf[8];
    msg[gid].data[9] = pw_buf[9];
    msg[gid].data[10] = pw_buf[10];
    msg[gid].data[11] = pw_buf[11];
    msg[gid].data[12] = pw_buf[12];
    msg[gid].data[13] = pw_buf[13];
    msg[gid].data[14] = pw_buf[14];
    msg[gid].data[15] = pw_buf[15];

    msg[gid].size = msg_prefix_size + msg_suffix_size;
}

__kernel void generate_word_prefix(
    __global struct message_prefix_t* msg_prefix,
    __global struct charset_t* css_buf,
    const uint css_count,
    const uint msg_batch_size,
    const uint msg_batch_offset,
    const uint msg_prefix_size,
    const uint msg_suffix_size,
    const uint mask,
    const uint bit14,
    const uint bit15)
{
    const uint gid = get_global_id(0);

    if (gid >= msg_batch_size) return;

    uint pw_buf[16] = { 0 };

    generate_word(pw_buf, css_buf, css_count, msg_prefix_size, msg_suffix_size, mask, bit14, bit15, msg_batch_offset + gid);

    msg_prefix[gid].data = pw_buf[0];
}
