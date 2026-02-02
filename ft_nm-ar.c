#include "./ft_nm.h"

extern char *ft_ar(const char *buffer, const size_t size) {
    if (!buffer) { return (0); }
    if (!size)   { return (0); }

    /* output string... */
    char *output = ft_strdup("");

    /* helpers... */
    char *ar_blob = 0;  /* ar_blob - blob of symbol names that exceed the size of standard 'ar_name'... */

    /* execution... */
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
        if (ft_elf_getMagic(cursor)) {
            /* repeat the steps of the regular ELF file parsing... */
            char *tmp0 = 0;
            switch (ft_elf_getArch(cursor)) {
                case (ELFCLASS32): { tmp0 = ft_elf32(cursor); } break;
                case (ELFCLASS64): { tmp0 = ft_elf64(cursor); } break;
            }

            if (!tmp0) {
                /* ...and append the results to the output string, even if faulty */
                tmp0 = ft_perror(ar_name);
                output = ft_strjoin_free(output, tmp0);
                free(tmp0), tmp0 = 0;
            }
            else {
                /* ...and append the results to the output string */
                output = ft_strjoin_free(output, "\n");
                output = ft_strjoin_free(output, ar_name);
                output = ft_strjoin_free(output, ":\n");
                output = ft_strjoin_free(output, tmp0);
                free(tmp0), tmp0 = 0;
            }
        }

        /* move to the next blob... */
        cursor += ar_size;

        /* release resources allocated on this iteration... */
        free(ar_name), ar_name = 0;
    }

    return (output);
}
