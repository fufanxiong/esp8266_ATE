#ifndef __CPU_H_____
#define __CPU_H_____

#ifdef IDLIB_EXPORTS
#define IDLIB_API __declspec(dllexport)
#else
#define IDLIB_API __declspec(dllimport)
#endif

EXTERN_C IDLIB_API bool getId(char *id, int len);
EXTERN_C IDLIB_API bool genIdFile(void);
EXTERN_C IDLIB_API bool verifyIdFile(void);


#endif
