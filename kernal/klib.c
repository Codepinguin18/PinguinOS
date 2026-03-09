/**
 * @file klib.c
 * @brief Freestanding string, memory and formatting utilities – PinguinOS.
 *
 * OPTIMISATIONS vs. previous version
 * ────────────────────────────────────
 *  memset    – 4-byte word loop for aligned ranges; byte prologue/epilogue
 *  memcpy    – 4-byte word loop when src+dst share alignment offset
 *  strlen    – Hacker's-Delight 4-byte zero-byte detect bithack
 *  __udivdi3 – fast shift-based long division (replaces bit-by-bit loop)
 *
 *  New helpers (needed by new subsystems)
 *  ───────────────────────────────────────
 *  strncat, strrchr, strstr, strdup
 *  strtoul / strtol
 *  strsplit_next   – zero-alloc tokeniser
 *  klib_checksum16 – one's-complement internet checksum (RFC 1071)
 *  klib_checksum8  – simple byte sum
 *  %b specifier in vsnprintf, left-align (%-Ns), long modifier (%lu/%lx)
 *  __divdi3 / __moddi3
 */

#include "../include/klib.h"
#include "../include/types.h"
#include "../include/heap.h"

/* ══════════════════════════════════════════════════════════════════
 *  MEMORY
 * ══════════════════════════════════════════════════════════════════ */

void *memset(void *s, int c, size_t n)
{
    uint8_t  *p8  = (uint8_t *)s;
    uint32_t  val = (uint8_t)c;
    val |= val << 8;
    val |= val << 16;

    while (n && ((uintptr_t)p8 & 3)) { *p8++ = (uint8_t)c; n--; }
    uint32_t *p32 = (uint32_t *)p8;
    size_t    w   = n >> 2;
    n &= 3;
    while (w--) *p32++ = val;
    p8 = (uint8_t *)p32;
    while (n--) *p8++ = (uint8_t)c;
    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    uint8_t       *d8 = (uint8_t *)dst;
    const uint8_t *s8 = (const uint8_t *)src;

    if (((uintptr_t)d8 & 3) == ((uintptr_t)s8 & 3)) {
        while (n && ((uintptr_t)d8 & 3)) { *d8++ = *s8++; n--; }
        uint32_t       *d32 = (uint32_t *)d8;
        const uint32_t *s32 = (const uint32_t *)s8;
        size_t w = n >> 2;
        n &= 3;
        while (w--) *d32++ = *s32++;
        d8 = (uint8_t *)d32;
        s8 = (const uint8_t *)s32;
    }
    while (n--) *d8++ = *s8++;
    return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
    uint8_t       *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    if (d == s || !n) return dst;
    if (d < s) {
        if (n >= 4 && !((uintptr_t)d & 3) && !((uintptr_t)s & 3)) {
            uint32_t       *d32 = (uint32_t *)d;
            const uint32_t *s32 = (const uint32_t *)s;
            size_t w = n >> 2;
            while (w--) *d32++ = *s32++;
            d = (uint8_t *)d32; s = (const uint8_t *)s32; n &= 3;
        }
        while (n--) *d++ = *s++;
    } else {
        d += n; s += n;
        while (n--) *--d = *--s;
    }
    return dst;
}

int memcmp(const void *a, const void *b, size_t n)
{
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    while (n--) { if (*pa != *pb) return (int)*pa - (int)*pb; pa++; pb++; }
    return 0;
}

/* ══════════════════════════════════════════════════════════════════
 *  STRINGS
 * ══════════════════════════════════════════════════════════════════ */

size_t strlen(const char *s)
{
    const char *p = s;
    while ((uintptr_t)p & 3) { if (*p == '\0') return (size_t)(p - s); p++; }
    const uint32_t *w = (const uint32_t *)p;
    for (;;) {
        uint32_t v = *w;
        if ((v - 0x01010101u) & ~v & 0x80808080u) break;
        w++;
    }
    p = (const char *)w;
    while (*p) p++;
    return (size_t)(p - s);
}

char *strcpy(char *dst, const char *src)
{ char *d = dst; while ((*d++ = *src++)); return dst; }

char *strncpy(char *dst, const char *src, size_t n)
{ char *d = dst; while (n && (*d++ = *src++)) n--; if (n) { while(--n) *d++='\0'; } return dst; }

char *strcat(char *dst, const char *src)
{ char *d = dst; while (*d) d++; while ((*d++ = *src++)); return dst; }

char *strncat(char *dst, const char *src, size_t n)
{ char *d = dst; while (*d) d++; while (n-- && (*d = *src++)) d++; *d='\0'; return dst; }

int strcmp(const char *a, const char *b)
{ while (*a && *a == *b) { a++; b++; } return (unsigned char)*a - (unsigned char)*b; }

int strncmp(const char *a, const char *b, size_t n)
{ while (n && *a && *a==*b) { a++;b++;n--; } if(!n) return 0; return (unsigned char)*a-(unsigned char)*b; }

char *strchr(const char *s, int c)
{ while (*s) { if (*s==(char)c) return (char*)s; s++; } return (c=='\0')?(char*)s:NULL; }

char *strrchr(const char *s, int c)
{ const char *l=NULL; while(*s){if(*s==(char)c)l=s;s++;} return (char*)l; }

char *strstr(const char *h, const char *n)
{
    if (!*n) return (char *)h;
    size_t nl = strlen(n);
    while (*h) { if (strncmp(h,n,nl)==0) return (char*)h; h++; }
    return NULL;
}

char *strrev(char *s)
{
    size_t len = strlen(s);
    for (size_t i = 0; i < len/2; i++) {
        char t=s[i]; s[i]=s[len-1-i]; s[len-1-i]=t;
    }
    return s;
}

char *strdup(const char *s)
{
    size_t len = strlen(s)+1;
    char *copy = (char *)kmalloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

char *strsplit_next(char **str, char delim)
{
    if (!str || !*str) return NULL;
    char *start = *str;
    while (*start == delim) start++;
    if (!*start) { *str=NULL; return NULL; }
    char *end = start;
    while (*end && *end != delim) end++;
    if (*end) { *end='\0'; *str=end+1; } else { *str=NULL; }
    return start;
}

/* ── Number parsing ──────────────────────────────────────────────── */
static INLINE int char_digit(char c, int base)
{
    int v = (c>='0'&&c<='9')? c-'0': (c>='a'&&c<='z')? c-'a'+10: (c>='A'&&c<='Z')? c-'A'+10: -1;
    return (v>=0 && v<base) ? v : -1;
}

uint32_t strtoul(const char *str, char **end, int base)
{
    while (*str==' '||*str=='\t') str++;
    bool neg=false;
    if (*str=='-'){neg=true;str++;} else if(*str=='+')str++;
    if (base==0) {
        if (str[0]=='0'&&(str[1]=='x'||str[1]=='X')){base=16;str+=2;}
        else if(str[0]=='0'){base=8;str++;}
        else base=10;
    } else if (base==16&&str[0]=='0'&&(str[1]=='x'||str[1]=='X')) str+=2;
    uint32_t r=0; const char *s=str; int d;
    while ((d=char_digit(*str,base))>=0){r=r*(uint32_t)base+(uint32_t)d;str++;}
    if (end) *end=(char*)(str==s?s:str);
    return neg?(uint32_t)(-(int32_t)r):r;
}

int32_t strtol(const char *str, char **end, int base) { return (int32_t)strtoul(str,end,base); }

/* ── Number formatting ───────────────────────────────────────────── */
int uitoa(char *buf, uint32_t val, int base, bool upper)
{
    static const char lo[]="0123456789abcdefghijklmnopqrstuvwxyz";
    static const char hi[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *d = upper ? hi : lo;
    if (base<2||base>36){buf[0]='\0';return 0;}
    int len=0;
    if (!val) { buf[len++]='0'; }
    else { char tmp[32]; int t=0; while(val){tmp[t++]=d[val%(uint32_t)base];val/=(uint32_t)base;} for(int i=t-1;i>=0;i--)buf[len++]=tmp[i]; }
    buf[len]='\0'; return len;
}

int itoa(char *buf, int32_t val)
{
    int len=0;
    if(val<0){buf[len++]='-';val=-val;}
    len+=uitoa(buf+len,(uint32_t)val,10,false);
    return len;
}

/* ── vsnprintf ───────────────────────────────────────────────────── */
int vsnprintf(char *buf, size_t size, const char *fmt, __builtin_va_list ap)
{
    size_t pos=0;
    char tmp[32];
#define EMIT(c)    do{if(pos+1<size)buf[pos]=(c);pos++;}while(0)
#define EMITS(s,n) do{for(int _i=0;_i<(n);_i++){EMIT((s)[_i]);}}while(0)

    while (*fmt) {
        if (*fmt!='%'){EMIT(*fmt++);continue;}
        fmt++;
        bool la=false; if(*fmt=='-'){la=true;fmt++;}
        char pc=' ';  if(*fmt=='0'&&!la){pc='0';fmt++;}
        int  w=0;     while(*fmt>='0'&&*fmt<='9')w=w*10+(*fmt++-'0');
        bool lng=false; if(*fmt=='l'){lng=true;fmt++;}
        switch(*fmt){
        case 'c':{char c=(char)__builtin_va_arg(ap,int);if(!la)for(int i=1;i<w;i++)EMIT(' ');EMIT(c);if(la)for(int i=1;i<w;i++)EMIT(' ');break;}
        case 's':{const char*s=__builtin_va_arg(ap,const char*);if(!s)s="(null)";int sl=(int)strlen(s);if(!la)for(int i=sl;i<w;i++)EMIT(' ');while(*s)EMIT(*s++);if(la)for(int i=sl;i<w;i++)EMIT(' ');break;}
        case 'd':case 'i':{int32_t v=lng?(int32_t)__builtin_va_arg(ap,long):__builtin_va_arg(ap,int32_t);int l=itoa(tmp,v);if(!la)for(int i=l;i<w;i++)EMIT(pc);EMITS(tmp,l);if(la)for(int i=l;i<w;i++)EMIT(' ');break;}
        case 'u':{uint32_t v=lng?(uint32_t)__builtin_va_arg(ap,unsigned long):__builtin_va_arg(ap,uint32_t);int l=uitoa(tmp,v,10,false);if(!la)for(int i=l;i<w;i++)EMIT(pc);EMITS(tmp,l);if(la)for(int i=l;i<w;i++)EMIT(' ');break;}
        case 'x':case 'X':{uint32_t v=lng?(uint32_t)__builtin_va_arg(ap,unsigned long):__builtin_va_arg(ap,uint32_t);int l=uitoa(tmp,v,16,*fmt=='X');if(!la)for(int i=l;i<w;i++)EMIT(pc);EMITS(tmp,l);if(la)for(int i=l;i<w;i++)EMIT(' ');break;}
        case 'p':{uintptr_t v=(uintptr_t)__builtin_va_arg(ap,void*);EMIT('0');EMIT('x');int l=uitoa(tmp,(uint32_t)v,16,false);for(int i=l;i<8;i++)EMIT('0');EMITS(tmp,l);break;}
        case 'b':{uint32_t v=__builtin_va_arg(ap,uint32_t);int l=uitoa(tmp,v,2,false);if(!la)for(int i=l;i<w;i++)EMIT(pc);EMITS(tmp,l);break;}
        case '%':EMIT('%');break;
        default:EMIT('%');EMIT(*fmt);break;
        }
        fmt++;
    }
    if(size>0)buf[pos<size?pos:size-1]='\0';
    return (int)pos;
#undef EMIT
#undef EMITS
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    __builtin_va_list ap;
    __builtin_va_start(ap,fmt);
    int r=vsnprintf(buf,size,fmt,ap);
    __builtin_va_end(ap);
    return r;
}

/* ── Checksums ───────────────────────────────────────────────────── */
uint16_t klib_checksum16(const void *data, size_t len)
{
    const uint16_t *p=(const uint16_t*)data; uint32_t s=0;
    while(len>1){s+=*p++;len-=2;}
    if(len)s+=*(const uint8_t*)p;
    s=(s>>16)+(s&0xFFFF); s+=(s>>16);
    return (uint16_t)~s;
}

uint8_t klib_checksum8(const void *data, size_t len)
{ const uint8_t *p=(const uint8_t*)data; uint8_t s=0; while(len--)s+=*p++; return s; }

/* ── 64-bit math helpers ─────────────────────────────────────────── */
uint64_t __udivdi3(uint64_t n, uint64_t d)
{
    if(!d||d>n) return (d==0||d>n)&&d?0:1;
    if(d==n)return 1;
    uint64_t q=0; int sh=0;
    while(d<=n&&!(d&(1ULL<<63))){d<<=1;sh++;}
    if(d>n){d>>=1;sh--;}
    for(int i=sh;i>=0;i--){if(n>=d){n-=d;q|=(1ULL<<i);}d>>=1;}
    return q;
}
uint64_t __umoddi3(uint64_t n,uint64_t d){if(!d)return 0;return n-d*__udivdi3(n,d);}
int64_t  __divdi3(int64_t n,int64_t d){bool neg=(n<0)^(d<0);uint64_t q=__udivdi3((uint64_t)(n<0?-n:n),(uint64_t)(d<0?-d:d));return neg?-(int64_t)q:(int64_t)q;}
int64_t  __moddi3(int64_t n,int64_t d){return n-d*__divdi3(n,d);}