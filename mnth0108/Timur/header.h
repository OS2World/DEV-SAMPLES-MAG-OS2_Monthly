// HEADER.H - don't you love these original names?!?!?

#ifndef HEADER_INCLUDED
#define HEADER_INCLUDED

typedef signed char SBYTE;        // Hmmm... is this already defined somewhere?
typedef unsigned char BOOL8;      // BOOL is a 32-bit boolean.  Give me a break!!!!!!!!!!!

#define BETWEEN(x,a,b) ( (a) < (b) ? ((x) >= (a) && (x) <= (b)) : ((x) >= (b) && (x) <= (a)) )
// returns TRUE if 'x' lies between 'a' and 'b'

#endif
