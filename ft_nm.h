#if !defined (_ft_nm_h_)
# define _ft_nm_h_ 1
#
# include <stdio.h>      /* perror() */
# include <stdlib.h>     /* malloc(), free(), exit()  */
#
# include <errno.h>
# include <string.h>     /* strerror() */
#
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

/* ./ft_nm.c */

extern int ft_file(const char *);

/* ./ft_nm-getopt.c */

extern int ft_getopt(int, char **);

/* ./ft_nm-x64.c */

extern int ft_elf32(const char *);

/* ./ft_nm-x64.c */

extern int ft_elf64(const char *);

extern Elf64_Sym *ft_elf64_sort(Elf64_Sym *, const size_t, const char *, int (*)(Elf64_Sym, Elf64_Sym, const char *));

/* ./ft_nm-util.c */

extern int ft_elf_getMagic(const char *);

extern int ft_elf_getArch(const char *);

extern void *ft_elf_extract(const char *, const size_t, const size_t);

extern void *ft_memdup(void *, const size_t);

extern void *ft_memjoin(void *, void *, const size_t, const size_t);

extern int ft_strcmp(const char *, const char *);

#endif /* _ft_nm_h_ */
