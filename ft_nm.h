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
# include <linux/limits.h>
#
# include "libft/libft.h"

extern char *g_prog;

extern int g_opt_debug;
extern int g_opt_extern;
extern int g_opt_undef;
extern int g_opt_sort;

extern int g_errno;

struct s_file {
    uint64_t arch;

    uint64_t type;
    uint64_t bind;
    uint64_t shndx;
    
    uintptr_t addr;
    char name[PATH_MAX];
    char code;

    /* file validity flag... */
    char valid;
};

/* ./ft_nm-file.c */

extern struct s_file *ft_file(const char *);

extern struct s_file *ft_sort(struct s_file *, const size_t);

/* ./ft_nm-print.c */

extern int ft_print(struct s_file *, const size_t);

/* ./ft_nm-sort.c */

extern struct s_file *ft_qsort(struct s_file *, const size_t, const size_t, int (*)(struct s_file, struct s_file));

/* ./ft_nm-getopt.c */

extern int ft_getopt(int, char **);

/* ./ft_nm-x32.c */

extern struct s_file *ft_elf32(const char *);

/* ./ft_nm-x64.c */

extern struct s_file *ft_elf64(const char *);

/* ./ft_nm-ar.c */

/* extern char *ft_ar(const char *, const size_t); */

/* ./ft_nm-util.c */

extern int ft_elf_getMagic(const char *);

extern int ft_ar_getMagic(const char *);

extern int ft_elf_getArch(const char *);

extern void *ft_elf_extract(const char *, const size_t, const size_t);

extern void *ft_memdup(void *, const size_t);

extern void *ft_memjoin(void *, void *, const size_t, const size_t);

extern int ft_strcmp(const char *, const char *);

extern int ft_islower(int);

extern char *ft_utoa(uint64_t, uint8_t, char [16]);

extern char *ft_strjoin_free(char *, const char *);

extern char *ft_perror(const char *);

#endif /* _ft_nm_h_ */
