#if !defined (_ft_nm_h_)
# define _ft_nm_h_ 1
#
# include <stdio.h>      /* perror() */
# include <stdlib.h>     /* malloc(), free(), exit()  */
#
# include <errno.h>
# include <string.h>     /* strerror() */
#
# include <ar.h>
# include <elf.h>
# include <fcntl.h>      /* open(), close() */
# include <unistd.h>     /* write(), getpagesize() */
# include <sys/stat.h>   /* fstat() */
# include <sys/mman.h>   /* mmap(), munmap() */
#
# include "libft/libft.h"

extern t_list *g_paths;
extern const char *g_prog;

extern int g_opt_debug;
extern int g_opt_extern;
extern int g_opt_undef;
extern int g_opt_sort;

extern int g_errno;

/* ./ft_nm.c */

extern char *ft_file(const char *);

/* ./ft_nm-getopt.c */

extern int ft_getopt(int, char **);

/* ./ft_nm-x32.c */

extern char *ft_elf32(const char *);

/* ./ft_nm-x64.c */

extern char *ft_elf64(const char *);

/* ./ft_nm-ar.c */

extern char *ft_ar(const char *, const size_t);

/* ./ft_nm-util.c */

extern int ft_elf_getMagic(const char *);

extern int ft_ar_getMagic(const char *);

extern int ft_elf_getArch(const char *);

extern void *ft_elf_extract(const char *, const size_t, const size_t);

extern void *ft_memdup(void *, const size_t);

extern void *ft_memjoin(void *, void *, const size_t, const size_t);

extern int ft_strcmp(const char *, const char *);

extern int ft_islower(int);

extern size_t ft_numlen(long long, int);

extern char *ft_utoa_hex(unsigned long long);

extern char *ft_strjoin_free(char *, const char *);

extern char *ft_perror(const char *);

#endif /* _ft_nm_h_ */
