#include <stdio.h>      /* perror() */
#include <stdlib.h>     /* malloc(), free(), exit()  */

#include <errno.h>
#include <string.h>     /* strerror() */

#include <unistd.h>     /* open(), close(), write(), getpagesize() */
#include <sys/stat.h>   /* fstat() */
#include <sys/mman.h>   /* mmap(), munmap() */

/* SECTION: api
 * */

extern int ft_getopt(int, char **);

/* SECTION: globals
 * */

extern const char *g_prog;

/* SECTION: main
 * */

int main(int ac, char **av) {
    if (!ft_getopt(ac, av)) { return (1); }

    return (0);
}

/* SECTION: api
 * */

int ft_getopt(int ac, char **av) {
    (void) ac;
    g_prog = (const char *) *av;

    while (*++av) {
        /* process options... */
        if (**av == '-') { }

        /* process files... */
        else { }
    }
    return (1);    
}

/* SECTION: globals
 * */

const char *g_prog;
