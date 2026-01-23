/* TODO:
 * 1. [X] Store the list of symbols to be printed as a seperate pointer.
 *    We can, technically speaking, memdup the whole symbol table of .symtab section and that should work fine.
 * 2. [X] Implement sort and reverse sort for symbols
 * 3. [X] Find out a type of each symbol based on requirements from man
 * 4. [ ] Get rid of printf in favor of something else (could be the simplest libft or ft_printf because it handles most of the cases pretty well)
 * 4. [ ] Implement proper getopt
 * 5. [ ] Implement -a flag
 * 6. [ ] Implement -g flag
 * 7. [ ] Implement -u flag
 * 8. [X] Implement -r flag
 * 9. [X] Implement -p flag
 * 10. [ ] Implement -h
 * 11. [ ] Invalid flag should default to 'h'
 * 12. [ ] Check if everything works on other compilers
 * */

#include "./ft_nm.h"

/* SECTION: api
 * */

int main(int ac, char **av) {
    if (!ft_getopt(ac, av)) {
        if (g_paths) {
            ft_lstclear(&g_paths, free), g_paths = 0;
        }
        return (1);
    }
    
    if (!g_paths || !ft_lstsize(g_paths)) {
        g_paths = ft_lstnew(ft_strdup("a.out"));
        if (!g_paths) {
            return (1);
        }
    }

    for (t_list *path = g_paths; path; path = path->next) {
        const char *name = path->content;
        if (!name) {
            ft_lstclear(&g_paths, free), g_paths = 0;
            return (1);
        }

        if (ft_lstsize(g_paths) > 1) { printf("\n%s:\n", name); }

        if (!ft_file(name)) {
            ft_lstclear(&g_paths, free), g_paths = 0;
            return (1);
        }
    }

    ft_lstclear(&g_paths, free), g_paths = 0;
    return (0);
}

int ft_file(const char *path) {
    /* setup... */
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("%s: '%s': No such file\n", g_prog, path);
        return (0);
    }

    struct stat stat = { 0 };
    if (fstat(fd, &stat) == -1) {
        perror(g_prog);
        return (0);
    }
    
    if (S_ISDIR(stat.st_mode)) {
        printf("%s: Warning: '%s' is a directory\n", g_prog, path);
        return (0);
    }

    /* read file to a buffer... */
    char *buffer = ft_calloc(stat.st_size + 1, sizeof(char));
    if (!buffer) {
        close(fd), fd = 0;
        return (0);
    }

    if (read(fd, buffer, stat.st_size) != stat.st_size) {
        free(buffer), buffer = 0;
        close(fd), fd = 0;
        return (0);
    }
    
    close(fd), fd = 0;
    
    /* execution... */
    if (ft_elf_getMagic(buffer)) {
        switch (ft_elf_getArch(buffer)) {
            case (ELFCLASS32): { ft_elf32(buffer); } break;
            case (ELFCLASS64): { ft_elf64(buffer); } break;
        }
    }
    else {
        printf("%s: %s: file format not recognized\n", g_prog, path);
    }
    
    /* cleanup... */
    free(buffer), buffer = 0;

    return (1);
}



/* g_paths - list of files to process
 * */
t_list *g_paths = 0;

/* g_prog - name of the program
 * */
const char *g_prog = 0;

/* g_opt_debug - show debug symbols
 * > 0 - disabled
 * > 1 - enabled
 * */
int g_opt_debug = 0;

/* g_opt_extern - show extern symbols
 * > 0 - disabled
 * > 1 - enabled
 * */
int g_opt_extern = 0;

/* g_opt_undef - show undefined symbols
 * > 0 - disabled
 * > 1 - enabled
 * */
int g_opt_undef = 0;

/* g_opt_sort - select sorting method
 * > 0 - none
 * > 1 - ascending (a - z)
 * > 2 - descending (z - a)
 * */
int g_opt_sort = 1;
