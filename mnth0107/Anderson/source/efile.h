/* file I/O for OS/2 Programmer's Editor */

/* return types for local file I/O functions */
#define CANTREAD (-1L)
#define CANTWRITE (-2L)
#define TOOLONG (-3L)
#define NOMEMORY (-4L)

LONG ReadFile (char *fn, PCHAR *b);
LONG MakeWriteBuffer (LONG cb, PCHAR *b);
LONG WriteFile (char *fn, LONG cb, PCHAR b);
VOID Release (PCHAR b);
