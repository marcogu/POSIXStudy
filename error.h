#ifndef error_h
#define error_h

#include <stdio.h>

#if !defined(PRINTFLIKE)
#if defined(__GNUC__)
#define PRINTFLIKE(n,m) __attribute__((format(printf,n,m)))
#else
#define PRINTFLIKE(n,m) /* If only */
#endif /* __GNUC__ */
#endif /* PRINTFLIKE */

extern void err_abort(const char *format, ...) PRINTFLIKE(1,2);

#endif /* error_h */