#include "ft_nm.h"

static int ft_printSymbol(struct s_symbol);

extern int ft_printFile(struct s_file file) {
    for (size_t i = 0; i < file.f_size; i++) {
        struct s_symbol sym = ((struct s_symbol *) file.f_data)[i];

        /* process '-u' / '--undefined-only'... */
        if (g_opt_undef) {
            if (sym.s_shndx == SHN_UNDEF) {
                ft_printSymbol(sym);
            }
        }
        
        /* process '-g' / '--extern-only'... */
        else if (g_opt_extern) {
            if (sym.s_bind == STB_GLOBAL ||
                sym.s_bind == STB_WEAK
            ) {
                ft_printSymbol(sym);
            }
        }
        
        /* process '-a' / '--debug-syms'... */
        else if (g_opt_debug) {
            if (!*sym.s_name) {
                if (sym.s_type == STT_SECTION) {
                    /* if symbol is debug symbol... */
                    if (!ft_strncmp(sym.s_name, ".debug_", 7)) {
                        sym.s_code = 'N';
                    }
                }
            }
            ft_printSymbol(sym);
        }

        /* process default... */
        else {
            if (sym.s_shndx != SHN_LOPROC    &&
                sym.s_shndx != SHN_HIPROC    &&
                sym.s_shndx != SHN_BEFORE    &&
                sym.s_shndx != SHN_AFTER     &&
                sym.s_shndx != SHN_LOOS      &&
                sym.s_shndx != SHN_HIOS      &&
                sym.s_shndx != SHN_ABS       &&
                sym.s_shndx != SHN_COMMON    &&
                sym.s_shndx != SHN_XINDEX    &&
                sym.s_shndx != SHN_HIRESERVE
            ) {
                if (sym.s_type != STT_SECTION) {
                    ft_printSymbol(sym);
                }
            }
        }
    }

    return (1);
}

static int ft_printSymbol(struct s_symbol sym) {
    size_t addrlen = 0;
    switch (sym.s_arch) {
        case (ELFCLASS32): { addrlen = 8; } break;
        case (ELFCLASS64): { addrlen = 16; } break;
    }
    
    uintptr_t addr = sym.s_addr;
    
    size_t n_l = 0;
    for (uint64_t tmp = addr; tmp != 0; tmp /= 16) {
        n_l++;
    }

    char c = sym.s_shndx == SHN_UNDEF ? ' ' : '0';
    for (size_t i = 0; i < addrlen - n_l; i++) {
        ft_putchar_fd(c, 1);
    }

    char buffer[16] = { 0 };
    ft_utoa(addr, 16, buffer);

    ft_putstr_fd(buffer, 1);
    ft_putstr_fd(" ", 1);

    /* print code... */
    ft_putchar_fd(sym.s_code, 1);
    ft_putstr_fd(" ", 1);

    /* print name... */
    ft_putendl_fd(sym.s_name, 1);

    return (1);
}
