/*
  Hatari - version.h

  Name and version for window title (and other outputs)
*/

#if defined(__SWITCH__)
#define HATARI_VERSION "1.00"
#define PROG_NAME "Hatari Switch v" HATARI_VERSION
#else
/* devel */
#define HATARI_VERSION "2.6.1-devel (" __DATE__ ")"

/* release */
//#define HATARI_VERSION "2.6.1"

#define PROG_NAME "Hatari v" HATARI_VERSION
#endif
