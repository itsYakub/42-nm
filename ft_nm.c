#include "./ft_nm.h"

/* SECTION: api
 * */

int main(int ac, char **av) {
    int status = ft_getopt(ac, av);
    if (!status) {
        return (1);
    }

    t_list *list = 0;
    for (size_t i = 1; i < (size_t) ac; i++) {
        if (*av[i] != '-') {
            char *content = ft_strdup(av[i]);
            t_list *new = ft_lstnew(content);

            ft_lstadd_back(&list, new);
        }
    }

    if (!list) {
        char *content = ft_strdup("a.out");
        t_list *new = ft_lstnew(content);

        ft_lstadd_back(&list, new);
    }

    for (t_list *item = list; item; item = item->next) {
        const char *path = item->content;

        struct s_file *file = ft_file(path);
        if (!file) {
            continue;
        }
        
        size_t lstsize = ft_lstsize(list);
        if (lstsize > 1) {
            if (list != item ||
                file->f_type == 2
            ) {
                ft_putchar_fd(10, 1);
            }

            ft_putstr_fd(path, 1);
            ft_putendl_fd(":", 1);
        }

        switch (file->f_type) {
            /* case: regular file */
            case (1): {
                struct s_symbol *arr = (struct s_symbol *) file->f_data;
                arr = ft_sort(arr, file->f_size);
                
                ft_printFile(*file);
                free(arr);
            } break;

            /* case: archive file */
            case (2): {
                struct s_file *files = (struct s_file *) file->f_data;

                /* for every file in archive... */
                for (size_t i = 0; i < file->f_size; i++) {
                    struct s_symbol *arr = (struct s_symbol *) files[i].f_data;
                    if (!arr) {
                        if (*files[i].f_name) {
                            g_errno = 4;
                            ft_perror(files[i].f_name);
                        }
                    }
                    else {
                        arr = ft_sort(arr, files[i].f_size);
                        
                        ft_putendl_fd("", 1);
                        ft_putstr_fd(files[i].f_name, 1);
                        ft_putendl_fd(":", 1);
                        ft_printFile(files[i]);
                    }
                    free(arr);
                }

                free(files);
            } break;
        }

        free(file);
    }
    
    ft_lstclear(&list, free), list = 0;
    return (0);
}


/* g_prog - name of the program
 * */
char *g_prog = 0;

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


/* g_errno - my own errno
 * > 0 - success
 * > 1 - no_such_file
 * > 2 - is_a_directory
 * > 3 - file_format_no_recognized
 * > 4 - no_symbols
 * */
int g_errno = 0;
