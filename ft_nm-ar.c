#include "./ft_nm.h"

extern struct s_file *ft_ar(const char *path, const char *buffer, const size_t size) {
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
            struct s_file *file = 0;
            switch (ft_elf_getArch(cursor)) {
                case (ELFCLASS32): { file = ft_elf32(path, cursor); } break;
                case (ELFCLASS64): { file = ft_elf64(path, cursor); } break;
            }
            
            ft_lstadd_back(&list, ft_lstnew(file));
        }
        
        /* move to the next blob... */
        cursor += ar_size;

        /* release resources allocated on this iteration... */
        free(ar_name), ar_name = 0;
    }

    /* final step: archive file... */
    size_t lstsize = ft_lstsize(list);
    struct s_file *file = malloc(sizeof(struct s_file));
    ft_strlcpy(file->f_name, path, PATH_MAX);
    file->f_size = lstsize;
    file->f_data = malloc(lstsize * sizeof(struct s_file));
    file->f_type = 2;
    for (size_t i = 0; i < lstsize; i++) {
        file->f_data = list->content;

        /* cleanup... */
        t_list *tmp = list;
        list = list->next;
        free(tmp);
    }

    return (file);
}
