#include "./ft_nm.h"

static int ft_getopt_help(void);

extern int ft_getopt(int ac, char **av) {
    (void) ac;
    g_prog = (const char *) *av;
    if (!g_prog) { return (0); }
    while (*++av) {
        /* process options... */
        if (**av == '-') {
            const char *opt = *av + 1;

            /* long-option... */
            if (*opt == '-') {
                opt++;

                if (!ft_strcmp(opt, "undefined-only")) { g_opt_undef = 1; }
                else if (!ft_strcmp(opt, "debug-syms")) {
                    if (!g_opt_undef) {
                        g_opt_debug = 1;
                    }   
                }
                else if (!ft_strcmp(opt, "extern-only")) {
                    if (!g_opt_undef) {
                        g_opt_extern = 1;
                    }   
                }

                else if (!ft_strcmp(opt, "no-sort")) { g_opt_sort = 0; }
                else if (!ft_strcmp(opt, "reverse-sort")) {
                    if (g_opt_sort != 0) {
                        g_opt_sort = 2;
                    }
                }
                
                else if (!ft_strcmp(opt, "help")) {
                    ft_getopt_help();
                }

                else {
                    ft_putstr_fd(g_prog, 1);
                    ft_putstr_fd(": unrecognized option '", 1);
                    ft_putstr_fd(*av, 1);
                    ft_putendl_fd("'", 1);
                    ft_getopt_help();
                }
            }
            /* short-option... */
            else {
                do {
                    switch (*opt) {
                        case ('u'): { g_opt_undef = 1; } break;
                        case ('a'): {
                            if (!g_opt_undef) {
                                g_opt_debug = 1;
                            }   
                        } break;
                        case ('g'): {
                            if (!g_opt_undef) {
                                g_opt_extern = 1;
                            }   
                        } break;

                        case ('p'): { g_opt_sort = 0; } break;
                        case ('r'): {
                            if (g_opt_sort != 0) {
                                g_opt_sort = 2;
                            }            
                        } break;

                        case ('h'): {
                            ft_getopt_help();
                        } break;

                        default: {
                            ft_putstr_fd(g_prog, 1);
                            ft_putstr_fd(": invalid option -- '", 1);
                            ft_putstr_fd(*av, 1);
                            ft_putendl_fd("'", 1);
                            ft_getopt_help();
                        }
                    }
                } while (*++opt);
            }
        }

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

static int ft_getopt_help(void) {
    /* print usage... */
    ft_putstr_fd("Usage: ", 1);
    ft_putstr_fd(g_prog, 1);
    ft_putendl_fd(" [option(s)] [file(s)]", 1);

    /* print description... */
    ft_putendl_fd(" List symbols in [file(s)] (a.out by default).", 1);
    
    /* print options... */
    ft_putendl_fd(" The options are:", 1);
    ft_putendl_fd("  -a, --debug-syms       Display debugger-only symbols", 1);
    ft_putendl_fd("  -g, --extern-only      Display only external symbols", 1);
    ft_putendl_fd("  -p, --no-sort          Do not sort the symbols", 1);
    ft_putendl_fd("  -r, --reverse-sort     Reverse the sense of the sort", 1);
    ft_putendl_fd("  -u, --undefined-only   Display only undefined symbols", 1);
    ft_putendl_fd("  -h, --help             Display this information", 1);
    ft_putendl_fd("  -V, --version          Display this program's version number", 1);
    
    /* print description... */
    ft_putstr_fd(g_prog, 1);
    ft_putendl_fd(": supported targets: elf64-x86-64 elf32-i386 elf32-iamcu elf32-x86-64 elf64-little elf64-big elf32-little elf32-big elf64-bpfle elf64-bpfbe", 1);

    /* bug reports... */
    ft_putendl_fd("Report bugs to <https://github.com/itsYakub/42-nm/issues>.", 1);
    
    /* gently exit the program... */
    if (g_paths) {
        ft_lstclear(&g_paths, free), g_paths = 0;
    }
    exit(0);
}
