#define MD5_F(x,y,z)    (bitselect ((z), (y), (x)))
#define MD5_G(x,y,z)    (bitselect ((y), (x), (z)))
#define MD5_H(x,y,z)    ((x) ^ (y) ^ (z))
#define MD5_I(x,y,z)    (bitselect (0xffffffffU, (x), (z)) ^ (y))

#define MD5_STEP(f,a,b,c,d,x,K,s)       \
{                                       \
  a += K;                               \
  a += x;                               \
  a += f (b, c, d);                     \
  a  = rotate(a, s);                    \
  a += b;                               \
}

#define MD5_STEP_FAST(f,a,b,c,d,K,s)    \
{                                       \
  a += K;                               \
  a += f (b, c, d);                     \
  a  = rotate(a, s);                    \
  a += b;                               \
}

#define MD5_STEP_REV(f,a,b,c,d,x,t,s)   \
{                                       \
  a -= b;                               \
  a  = rotate(a, 32 - s);               \
  a -= f (b, c, d);                     \
  a -= x;                               \
  a -= t;                               \
}

#define MD5_STEP_REV1(f,a,b,c,d,x,t,s)  \
{                                       \
  a -= b;                               \
  a  = rotate(a, 32 - s);               \
  a -= x;                               \
  a -= t;                               \
}
