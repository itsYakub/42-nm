#include "ft_nm.h"

static int ft_printExact(struct s_symbol);

extern int ft_print(struct s_symbol *arr, const size_t size) {
    /* null-check... */
    if (!arr)  { return (0); }
    if (!size) { return (0); }

    for (size_t i = 0; i < size; i++) {
        struct s_symbol file = arr[i];

        /* process '-u' / '--undefined-only'... */
        if (g_opt_undef) {
            if (file.shndx == SHN_UNDEF) {
                ft_printExact(file);
            }
        }
        
        /* process '-g' / '--extern-only'... */
        else if (g_opt_extern) {
            if (file.bind == STB_GLOBAL ||
                file.bind == STB_WEAK
            ) {
                ft_printExact(file);
            }
        }
        
        /* process '-a' / '--debug-syms'... */
        else if (g_opt_debug) {
            if (!*file.name) {
                if (file.type == STT_SECTION) {
                    /* if symbol is debug symbol... */
                    if (!ft_strncmp(file.name, ".debug_", 7)) {
                        file.code = 'N';
                    }
                }
            }
            ft_printExact(file);
        }

        /* process default... */
        else {
            if (file.shndx != SHN_LOPROC    &&
                file.shndx != SHN_HIPROC    &&
                file.shndx != SHN_BEFORE    &&
                file.shndx != SHN_AFTER     &&
                file.shndx != SHN_LOOS      &&
                file.shndx != SHN_HIOS      &&
                file.shndx != SHN_ABS       &&
                file.shndx != SHN_COMMON    &&
                file.shndx != SHN_XINDEX    &&
                file.shndx != SHN_HIRESERVE
            ) {
                if (file.type != STT_SECTION) {
                    ft_printExact(file);
                }
            }
        }
    }

    return (1);
}

static int ft_printExact(struct s_symbol file) {
    size_t addrlen = 0;
    switch (file.arch) {
        case (ELFCLASS32): { addrlen = 8; } break;
        case (ELFCLASS64): { addrlen = 16; } break;
    }
    
    uintptr_t addr = file.addr;
    
    size_t n_l = 0;
    for (uint64_t tmp = addr; tmp != 0; tmp /= 16) {
        n_l++;
    }

    char c = file.shndx == SHN_UNDEF ? ' ' : '0';
    for (size_t i = 0; i < addrlen - n_l; i++) {
        ft_putchar_fd(c, 1);
    }

    char buffer[16] = { 0 };
    ft_utoa(addr, 16, buffer);

    ft_putstr_fd(buffer, 1);
    ft_putstr_fd(" ", 1);

    /* print code... */
    ft_putchar_fd(file.code, 1);
    ft_putstr_fd(" ", 1);

    /* print name... */
    ft_putendl_fd(file.name, 1);

    return (1);
}
