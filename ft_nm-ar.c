#include "./ft_nm.h"

struct s_archive {
    char name[PATH_MAX];
    struct s_symbol *arr;
};

extern int ft_ar(const char *buffer, const size_t size) {
    if (!buffer) { return (0); }
    if (!size)   { return (0); }

    /* helpers... */
    char *ar_blob = 0;  /* ar_blob - blob of symbol names that exceed the size of standard 'ar_name'... */

    /* execution... */
    t_list *list = 0;

    char *cursor = (char *) buffer + SARMAG;
    while (cursor < buffer + size) {
        /* get the symbol header... */
        struct ar_hdr *ar_hdr = (struct ar_hdr *) cursor;

        /* get the size of the blob... */
        size_t ar_size = ft_atoi(ar_hdr->ar_size);

        /* get the name of the blob... */
        char *ar_name = ar_hdr->ar_name;
         /* special case: either indexed name or index blob */
        if (ar_name[0] == '/') {
            /* case: index blob... */
            if (ar_name[1] == '/') {
                cursor += sizeof(struct ar_hdr);
                ar_blob = cursor;
                cursor += ar_size;

                /* we only wanted the index blob, thus we can 'continue'... */
                continue;
            }

            /* case: indexed name... */
            else {
                /* shouldn't occur but we can still check for that... */
                if (!ar_blob) {
                    cursor += sizeof(struct ar_hdr) + ar_size;
                   continue; 
                }

                size_t ar_name_off = ft_atoi(ar_name + 1);
                char  *ar_name_ptr = ar_blob + ar_name_off;
                size_t ar_name_len = ft_strchr(ar_name_ptr, '/') - ar_name_ptr;

                ar_name = ft_calloc(ar_name_len + 1, 1);
                ar_name = ft_memcpy(ar_name, ar_name_ptr, ar_name_len);
            }
        }

        /* regular case: ar_name fits the name of the symbol */
        else {
            char  *ar_name_ptr = ar_name;
            size_t ar_name_len = ft_strchr(ar_name_ptr, '/') - ar_name_ptr;

            ar_name = ft_calloc(ar_name_len + 1, 1);
            ar_name = ft_memcpy(ar_name, ar_name_ptr, ar_name_len);
        }

        /* get the blob itself... */
        cursor += sizeof(struct ar_hdr);
    
        /* execution... */
        if (ft_elf_getMagic(cursor)) {
            /* allocate symbols... */
            struct s_symbol *tmp = 0;
            switch (ft_elf_getArch(cursor)) {
                case (ELFCLASS32): { tmp = ft_elf32(cursor); } break;
                case (ELFCLASS64): { tmp = ft_elf64(cursor); } break;
            }
            
            size_t size = 0;
            while (tmp[size].valid) { size++; }

            tmp = ft_sort(tmp, size);

            /* allocate archive... */
            struct s_archive *ar = malloc(sizeof(struct s_archive));
            if (!ar) {
                free(tmp), tmp = 0;
                ft_lstclear(&list, free), list = 0;
                return (0);
            }

            ft_strlcpy(ar->name, ar_name, PATH_MAX);
            ar->arr = tmp;
            ft_lstadd_back(&list, ft_lstnew(ar));
        }
        

        /* move to the next blob... */
        cursor += ar_size;

        /* release resources allocated on this iteration... */
        free(ar_name), ar_name = 0;
    }

    /* print every file with it's content... */
    for (t_list *item = list; item; item = item->next) {
        struct s_archive *ar = (struct s_archive *) item->content;
        const char *path = ar->name;
        
        struct s_symbol *arr = ar->arr;
        if (!arr) {
            g_errno = 4;
            ft_perror(path);
            continue;
        }

        size_t lstsize = ft_lstsize(list);
        if (lstsize > 1) {
            ft_putchar_fd(10, 1);
            ft_putstr_fd(path, 1);
            ft_putendl_fd(":", 1);
        }
            
        size_t size = 0;
        while (arr[size].valid) { size++; }

        ft_print(arr, size);

        free(arr), arr = 0;
    }

    ft_lstclear(&list, free), list = 0;
    return (1);
}
