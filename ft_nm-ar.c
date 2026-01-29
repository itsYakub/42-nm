#include "./ft_nm.h"

extern char *ft_ar(const char *buffer, const size_t size) {
    if (!buffer) { return (0); }
    if (!size)   { return (0); }

    /* output string... */
    char *output = ft_strdup("");
    char *cursor = (char *) buffer + SARMAG;
    while (cursor < buffer + size) {
        /* get the symbol header... */
        struct ar_hdr *ar_hdr = (struct ar_hdr *) cursor;
        size_t ar_size = ft_atoi(ar_hdr->ar_size);

        /* get the symbol itself... */
        cursor += sizeof(struct ar_hdr);
        if (ft_elf_getMagic(cursor)) {
            /* extract ELF file... */
            char *ar_name = ft_calloc(sizeof(ar_hdr->ar_name), 1);
            ar_name = ft_memcpy(ar_name, ar_hdr->ar_name, sizeof(ar_hdr->ar_name));
            *ft_strrchr(ar_name, '/') = 0;

            /* repeat the steps of the regular ELF file parsing... */
            char *tmp0 = 0;
            switch (ft_elf_getArch(cursor)) {
                case (ELFCLASS32): { tmp0 = ft_elf32(cursor, ar_name); } break;
                case (ELFCLASS64): { tmp0 = ft_elf64(cursor, ar_name); } break;
            }

            /* ...and append the results to the output string */
            output = ft_strjoin_free(output, tmp0);
            free(tmp0), tmp0 = 0;
        }

        /* move to the next symbol... */
        cursor += ar_size;
    }

    return (output);
}
