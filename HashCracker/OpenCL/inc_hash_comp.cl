inline void mark_hash(__global struct message_index_t* msg_index, const u32 msg_batch_pos, const u32 inner_loop_pos)
{
    msg_index[0].found = 1;
    msg_index[0].msg_batch_pos = msg_batch_pos;
    msg_index[0].inner_loop_pos = inner_loop_pos;
}

#if VECTOR_SIZE == 1

#define EQUAL_VECTOR(a,b)   \
    ((a) == (b))
    
#define EQUAL_SCALAR(a,b)   \
    ((a) == (b))

#define COMPARE_DIGEST(h0,h1,h2,h3)                                                                 \
{                                                                                                   \
    if (((h0) == search[0]) && ((h1) == search[1]) && ((h2) == search[2]) && ((h3) == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos);                                        \
}
  
#endif

#if VECTOR_SIZE == 2

#define EQUAL_VECTOR(a,b)   \
    (((a).s0 == (b).s0) ||  \
    ((a).s1 == (b).s1))
    
#define EQUAL_SCALAR(a,b)   \
    (((a).s0 == (b)) ||     \
    ((a).s1 == (b)))

#define COMPARE_DIGEST(h0,h1,h2,h3)                                                                             \
{                                                                                                               \
    if (((h0).s0 == search[0]) && ((h1).s0 == search[1]) && ((h2).s0 == search[2]) && ((h3).s0 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos);                                                    \
    if (((h0).s1 == search[0]) && ((h1).s1 == search[1]) && ((h2).s1 == search[2]) && ((h3).s1 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 1);                                                \
}
    
#endif

#if VECTOR_SIZE == 4

#define EQUAL_VECTOR(a,b)   \
    (((a).s0 == (b).s0) ||  \
    ((a).s1 == (b).s1) ||   \
    ((a).s2 == (b).s2) ||   \
    ((a).s3 == (b).s3))
    
#define EQUAL_SCALAR(a,b)   \
    (((a).s0 == (b)) ||     \
    ((a).s1 == (b)) ||      \
    ((a).s2 == (b)) ||      \
    ((a).s3 == (b)))

#define COMPARE_DIGEST(h0,h1,h2,h3)                                                                             \
{                                                                                                               \
    if (((h0).s0 == search[0]) && ((h1).s0 == search[1]) && ((h2).s0 == search[2]) && ((h3).s0 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos);                                                    \
    if (((h0).s1 == search[0]) && ((h1).s1 == search[1]) && ((h2).s1 == search[2]) && ((h3).s1 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 1);                                                \
    if (((h0).s2 == search[0]) && ((h1).s2 == search[1]) && ((h2).s2 == search[2]) && ((h3).s2 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 2);                                                \
    if (((h0).s3 == search[0]) && ((h1).s3 == search[1]) && ((h2).s3 == search[2]) && ((h3).s3 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 3);                                                \
}
    
#endif

#if VECTOR_SIZE == 8

#define EQUAL_VECTOR(a,b)   \
    (((a).s0 == (b).s0) ||  \
    ((a).s1 == (b).s1) ||   \
    ((a).s2 == (b).s2) ||   \
    ((a).s3 == (b).s3) ||   \
    ((a).s4 == (b).s4) ||   \
    ((a).s5 == (b).s5) ||   \
    ((a).s6 == (b).s6) ||   \
    ((a).s7 == (b).s7))
    
#define EQUAL_SCALAR(a,b)   \
    (((a).s0 == (b)) ||     \
    ((a).s1 == (b)) ||      \
    ((a).s2 == (b)) ||      \
    ((a).s3 == (b)) ||      \
    ((a).s4 == (b)) ||      \
    ((a).s5 == (b)) ||      \
    ((a).s6 == (b)) ||      \
    ((a).s7 == (b)))

#define COMPARE_DIGEST(h0,h1,h2,h3)                                                                             \
{                                                                                                               \
    if (((h0).s0 == search[0]) && ((h1).s0 == search[1]) && ((h2).s0 == search[2]) && ((h3).s0 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos);                                                    \
    if (((h0).s1 == search[0]) && ((h1).s1 == search[1]) && ((h2).s1 == search[2]) && ((h3).s1 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 1);                                                \
    if (((h0).s2 == search[0]) && ((h1).s2 == search[1]) && ((h2).s2 == search[2]) && ((h3).s2 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 2);                                                \
    if (((h0).s3 == search[0]) && ((h1).s3 == search[1]) && ((h2).s3 == search[2]) && ((h3).s3 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 3);                                                \
    if (((h0).s4 == search[0]) && ((h1).s4 == search[1]) && ((h2).s4 == search[2]) && ((h3).s4 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 4);                                                \
    if (((h0).s5 == search[0]) && ((h1).s5 == search[1]) && ((h2).s5 == search[2]) && ((h3).s5 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 5);                                                \
    if (((h0).s6 == search[0]) && ((h1).s6 == search[1]) && ((h2).s6 == search[2]) && ((h3).s6 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 6);                                                \
    if (((h0).s7 == search[0]) && ((h1).s7 == search[1]) && ((h2).s7 == search[2]) && ((h3).s7 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 7);                                                \
}
    
#endif

#if VECTOR_SIZE == 16

#define EQUAL_VECTOR(a,b) \
    (((a).s0 == (b).s0) ||  \
    ((a).s1 == (b).s1) ||   \
    ((a).s2 == (b).s2) ||   \
    ((a).s3 == (b).s3) ||   \
    ((a).s4 == (b).s4) ||   \
    ((a).s5 == (b).s5) ||   \
    ((a).s6 == (b).s6) ||   \
    ((a).s7 == (b).s7) ||   \
    ((a).s8 == (b).s8) ||   \
    ((a).s9 == (b).s9) ||   \
    ((a).sa == (b).sa) ||   \
    ((a).sb == (b).sb) ||   \
    ((a).sc == (b).sc) ||   \
    ((a).sd == (b).sd) ||   \
    ((a).se == (b).se) ||   \
    ((a).sf == (b).sf))

#define EQUAL_SCALAR(a,b) \
    (((a).s0 == (b)) ||     \
    ((a).s1 == (b)) ||      \
    ((a).s2 == (b)) ||      \
    ((a).s3 == (b)) ||      \
    ((a).s4 == (b)) ||      \
    ((a).s5 == (b)) ||      \
    ((a).s6 == (b)) ||      \
    ((a).s7 == (b)) ||      \
    ((a).s8 == (b)) ||      \
    ((a).s9 == (b)) ||      \
    ((a).sa == (b)) ||      \
    ((a).sb == (b)) ||      \
    ((a).sc == (b)) ||      \
    ((a).sd == (b)) ||      \
    ((a).se == (b)) ||      \
    ((a).sf == (b)))

#define COMPARE_DIGEST(h0,h1,h2,h3)                                                                             \
{                                                                                                               \
    if (((h0).s0 == search[0]) && ((h1).s0 == search[1]) && ((h2).s0 == search[2]) && ((h3).s0 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos);                                                    \
    if (((h0).s1 == search[0]) && ((h1).s1 == search[1]) && ((h2).s1 == search[2]) && ((h3).s1 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 1);                                                \
    if (((h0).s2 == search[0]) && ((h1).s2 == search[1]) && ((h2).s2 == search[2]) && ((h3).s2 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 2);                                                \
    if (((h0).s3 == search[0]) && ((h1).s3 == search[1]) && ((h2).s3 == search[2]) && ((h3).s3 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 3);                                                \
    if (((h0).s4 == search[0]) && ((h1).s4 == search[1]) && ((h2).s4 == search[2]) && ((h3).s4 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 4);                                                \
    if (((h0).s5 == search[0]) && ((h1).s5 == search[1]) && ((h2).s5 == search[2]) && ((h3).s5 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 5);                                                \
    if (((h0).s6 == search[0]) && ((h1).s6 == search[1]) && ((h2).s6 == search[2]) && ((h3).s6 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 6);                                                \
    if (((h0).s7 == search[0]) && ((h1).s7 == search[1]) && ((h2).s7 == search[2]) && ((h3).s7 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 7);                                                \
    if (((h0).s8 == search[0]) && ((h1).s8 == search[1]) && ((h2).s8 == search[2]) && ((h3).s8 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 8);                                                \
    if (((h0).s9 == search[0]) && ((h1).s9 == search[1]) && ((h2).s9 == search[2]) && ((h3).s9 == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 9);                                                \
    if (((h0).sa == search[0]) && ((h1).sa == search[1]) && ((h2).sa == search[2]) && ((h3).sa == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 10);                                               \
    if (((h0).sb == search[0]) && ((h1).sb == search[1]) && ((h2).sb == search[2]) && ((h3).sb == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 11);                                               \
    if (((h0).sc == search[0]) && ((h1).sc == search[1]) && ((h2).sc == search[2]) && ((h3).sc == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 12);                                               \
    if (((h0).sd == search[0]) && ((h1).sd == search[1]) && ((h2).sd == search[2]) && ((h3).sd == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 13);                                               \
    if (((h0).se == search[0]) && ((h1).se == search[1]) && ((h2).se == search[2]) && ((h3).se == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 14);                                               \
    if (((h0).sf == search[0]) && ((h1).sf == search[1]) && ((h2).sf == search[2]) && ((h3).sf == search[3]))   \
        mark_hash(msg_index, msg_batch_pos, inner_loop_pos + 15);                                               \
}

#endif

#define NOT_EQUAL_VECTOR(a,b) !(EQUAL_VECTOR ((a), (b)))
#define NOT_EQUAL_SCALAR(a,b) !(EQUAL_SCALAR ((a), (b)))
