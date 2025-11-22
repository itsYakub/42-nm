#include <stdio.h>      /* perror() */
#include <stdlib.h>     /* malloc(), free(), exit()  */

#include <errno.h>
#include <string.h>     /* strerror() */

#include <fcntl.h>      /* open(), close() */
#include <unistd.h>     /* write(), getpagesize() */
#include <sys/stat.h>   /* fstat() */
#include <sys/mman.h>   /* mmap(), munmap() */


#include "libft/libft.h"

/* SECTION: api
 * */

extern int ft_getopt(int, char **);

/* SECTION: globals
 * */

extern
t_list      *g_paths;

extern
const char  *g_prog;

/* SECTION: main
 * */

int main(int ac, char **av) {
    if (!ft_getopt(ac, av)) { return (1); }
    if (!g_paths || !ft_lstsize(g_paths)) {
        g_paths = ft_lstnew(ft_strdup("a.out"));
        if (!g_paths) {
            return (1);
        }
    }

    for (t_list *path = g_paths; path; path = path->next) {
        struct stat stat;
        const char  *name;
        int         fd;

        name = path->content;
        if (!name) { return (1); }
        if (ft_lstsize(g_paths) > 1) {
            printf("\n%s:\n", name);
        }

        fd = open(name, O_RDONLY);
        if (fd == -1) { printf("%s: '%s': No such file\n", g_prog, name); return (1); }
        if (fstat(fd, &stat) == -1) { perror(g_prog); return (1); }
        if (S_ISDIR(stat.st_mode)) { printf("%s: Warning: '%s' is a directory\n", g_prog, name); return (1); }

        close(fd), fd = -1;
    }

    ft_lstclear(&g_paths, free), g_paths = 0;
    return (0);
}

/* SECTION: api
 * */

int ft_getopt(int ac, char **av) {
    (void) ac;
    g_prog = (const char *) *av;
    if (!g_prog) { return (0); }
    while (*++av) {
        /* process options... */
        if (**av == '-') { }

        /* process files... */
        else {
            t_list  *l;

            l = ft_lstnew(ft_strdup(*av));
            if (!l) { return (0); }

            ft_lstadd_back(&g_paths, l);
        }
    }
    return (1);    
}

/* SECTION: globals
 * */

t_list      *g_paths = 0;
const char  *g_prog = 0;
