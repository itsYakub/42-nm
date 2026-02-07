/* TODO:
 * 1. [X] perror should return output string
 * 2. [X] output string should be either printed directly (regular binary files) or appended to the output (archives)
 * 3. [ ] improve efficiency of execution (it's to damn slow)
 * */

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

        struct s_file *arr = ft_file(path);
        if (!arr) {
            ft_lstclear(&list, free), list = 0;
            return (1);
        }

        size_t size = 0;
        while (arr[size].valid) { size++; }

        arr = ft_sort(arr, size);
        if (!arr) {
            ft_lstclear(&list, free), list = 0;
            return (1);
        }

        ft_print(arr, size);

        free(arr), arr = 0;
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
