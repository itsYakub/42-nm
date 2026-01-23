#include "./ft_nm.h"

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

                if (!ft_strncmp(opt, "undefined-only", 15)) { g_opt_undef = 1; }
                else if (!ft_strncmp(opt, "debug-syms", 10)) {
                    if (!g_opt_undef) {
                        g_opt_debug = 1;
                    }   
                }
                else if (!ft_strncmp(opt, "extern-only", 11)) {
                    if (!g_opt_undef) {
                        g_opt_extern = 1;
                    }   
                }

                else if (!ft_strncmp(opt, "no-sort", 8)) { g_opt_sort = 0; }
                else if (!ft_strncmp(opt, "reverse-sort", 12)) {
                    if (g_opt_sort != 0) {
                        g_opt_sort = 2;
                    }
                }

                else {
                    printf("%s: unknown command: %s\n", g_prog, *av);
                    return (0);
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

                        default: {
                            printf("%s: unknown command: %s\n", g_prog, *av);
                            return (0);
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
