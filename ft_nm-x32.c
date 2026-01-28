#include "./ft_nm.h"

/* SECTION: static functions
 * */


static void *ft_elf32_getStrtab(Elf32_Ehdr *, Elf32_Shdr *, const char *, const char *, const char *);

static Elf32_Sym *ft_elf32_extractSymbol(Elf32_Ehdr *, Elf32_Shdr *, const char *, size_t *);

static Elf32_Sym *ft_elf32_sort(Elf32_Sym *, const size_t, const char *, int (*)(Elf32_Sym, Elf32_Sym, const char *));

static int ft_elf32_comparea(Elf32_Sym, Elf32_Sym, const char *);

static int ft_elf32_compared(Elf32_Sym, Elf32_Sym, const char *);

static int ft_elf32_printSymbols(Elf32_Shdr *, Elf32_Sym *, const size_t, const char *);

static int ft_elf32_printSymbol(Elf32_Sym, const char *, const char);

static int ft_elf32_getLetterCode(Elf32_Shdr *, Elf32_Sym);

/* SECTION: api
 * */

extern int ft_elf32(const char *buffer, const char *path) {
    int exitcode = 1;

    /* ehdr... */
    Elf32_Ehdr *ehdr = ft_elf_extract(buffer, sizeof(Elf32_Ehdr), 0);
    if (!ehdr) {
        exitcode = 0; goto ft_elf32_exit;
    }

    /* shdr table... */
    Elf32_Shdr *shdr_tb = ft_elf_extract(buffer, ehdr->e_shnum * ehdr->e_shentsize, ehdr->e_shoff);
    if (!shdr_tb) {
        exitcode = 0; goto ft_elf32_exit;
    }

    /* extract STRTAB's... */
    const char *shstrtab = ft_elf_extract(buffer, shdr_tb[ehdr->e_shstrndx].sh_size, shdr_tb[ehdr->e_shstrndx].sh_offset);
    const char *dynstr = ft_elf32_getStrtab(ehdr, shdr_tb, buffer, shstrtab, ".dynstr");
    const char *strtab = ft_elf32_getStrtab(ehdr, shdr_tb, buffer, shstrtab, ".strtab");

    /* extract symbol table... */
    size_t sym_tb_s = 0;
    Elf32_Sym *sym_tb = ft_elf32_extractSymbol(ehdr, shdr_tb, buffer, &sym_tb_s);
    if (!sym_tb) {
        ft_putstr_fd(g_prog, 1);
        ft_putstr_fd(": ", 1);
        ft_putstr_fd(path, 1);
        ft_putendl_fd(": no symbols", 1);
        goto ft_elf32_exit;
    }

    /* sort symbol table... */
    switch (g_opt_sort) {
        case (1): { sym_tb = ft_elf32_sort(sym_tb, sym_tb_s, strtab, ft_elf32_comparea); } break;
        case (2): { sym_tb = ft_elf32_sort(sym_tb, sym_tb_s, strtab, ft_elf32_compared); } break;
        default:  { /* ...don't sort... */ } break;
    }

    /* print symbol table...
     * */
    if (!ft_elf32_printSymbols(shdr_tb, sym_tb, sym_tb_s, strtab)) {
        exitcode = 0; goto ft_elf32_exit;
    }

ft_elf32_exit:
    if (shstrtab) { free((void *) shstrtab), shstrtab = 0; }
    if (strtab)   { free((void *) strtab), strtab = 0; }
    if (dynstr)   { free((void *) dynstr), dynstr = 0; }
    if (shdr_tb)  { free(shdr_tb), shdr_tb = 0; }
    if (sym_tb)   { free(sym_tb), sym_tb = 0; }
    if (ehdr)     { free(ehdr), ehdr = 0; }
    return (exitcode);
}

/* SECTION: static functions
 * */

static void *ft_elf32_getStrtab(Elf32_Ehdr *ehdr, Elf32_Shdr *shdr_tb, const char *buffer, const char *shstrtab, const char *name) {
    /* null-check... */
    if (!ehdr)     { return (0); }
    if (!shdr_tb)  { return (0); }
    if (!buffer)   { return (0); }
    if (!shstrtab) { return (0); }
    if (!name)     { return (0); }

    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        /* section header object... */
        Elf32_Shdr shdr = shdr_tb[i];

        if (!strcmp(shstrtab + shdr.sh_name, name)) {
            return (ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset));
        }
    }

    return (0);
}

static Elf32_Sym *ft_elf32_extractSymbol(Elf32_Ehdr *ehdr, Elf32_Shdr *shdr_tb, const char *buffer, size_t *sizeptr) {
    /* null-check... */
    if (!ehdr)    { return (0); }
    if (!shdr_tb) { return (0); }
    if (!buffer)  { return (0); }
    if (!sizeptr) { return (0); }
   
    Elf32_Sym *sym_tb = 0;
    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        Elf32_Shdr shdr = shdr_tb[i];
        if (shdr.sh_type != SHT_SYMTAB) {
            continue;
        }

        Elf32_Sym *tmp = ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset);
        if (!tmp) {
            return (0);
        }

        if (!sym_tb) {
            sym_tb = ft_memdup(tmp, shdr.sh_size);
            if (!sym_tb) {
                free(tmp), tmp = 0;
                return (0);
            }
            *sizeptr = shdr.sh_size / sizeof(Elf32_Sym);
        }
        else {
            void *tmp1 = sym_tb;
            sym_tb = ft_memjoin(sym_tb, tmp, shdr.sh_size, *sizeptr);
            if (!sym_tb) {
                free(tmp), tmp = 0;
                free(sym_tb), sym_tb = 0;
                return (0);
            }
            *sizeptr += shdr.sh_size / sizeof(Elf32_Sym);
            free(tmp1), tmp1 = 0;
        }

        free(tmp), tmp = 0;
    }

    if (!*sizeptr) {
        return (0);
    }

    return (sym_tb);
}

static Elf32_Sym *ft_elf32_sort(Elf32_Sym *sym_tb, const size_t size, const char *strtab, int (*compare)(Elf32_Sym, Elf32_Sym, const char *)) {
    /* safety-check... */
    if (!sym_tb) { return (0); }
    if (!strtab) { return (0); }
    if (!size)   { return (0); }

    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - 1 - i; j++) {
            if (compare(sym_tb[j], sym_tb[j + 1], strtab)) {
                Elf32_Sym tmp = sym_tb[j];
                sym_tb[j] = sym_tb[j + 1];
                sym_tb[j + 1] = tmp;
            }
        }
    }
    return (sym_tb);
}

static int ft_elf32_comparea(Elf32_Sym sym0, Elf32_Sym sym1, const char *strtab) {
    const char *name0 = strtab + sym0.st_name;
    const char *name1 = strtab + sym1.st_name;

    while (*name0 || *name1) {
        while (*name0 && !ft_isalnum(*name0)) { name0++; }
        while (*name1 && !ft_isalnum(*name1)) { name1++; }
        if (ft_tolower(*name0) != ft_tolower(*name1)) {
            return (ft_tolower(*name0) > ft_tolower(*name1));
        }

        if (*name0) { name0++; }
        if (*name1) { name1++; }
    }

    return (ft_strcmp(strtab + sym0.st_name, strtab + sym1.st_name) > 0);
}

static int ft_elf32_compared(Elf32_Sym sym0, Elf32_Sym sym1, const char *strtab) {
    const char *name0 = strtab + sym0.st_name;
    const char *name1 = strtab + sym1.st_name;

    while (*name0 || *name1) {
        while (*name0 && !ft_isalnum(*name0)) { name0++; }
        while (*name1 && !ft_isalnum(*name1)) { name1++; }
        if (ft_tolower(*name0) != ft_tolower(*name1)) {
            return (ft_tolower(*name0) < ft_tolower(*name1));
        }

        if (*name0) { name0++; }
        if (*name1) { name1++; }
    }

    return (ft_strcmp(strtab + sym0.st_name, strtab + sym1.st_name) < 0);
}

static int ft_elf32_printSymbols(Elf32_Shdr *shdr_tb, Elf32_Sym *sym_tb, const size_t size, const char *strtab) {
    /* null-check... */
    if (!shdr_tb) { return (0); }
    if (!sym_tb)  { return (0); }
    if (!strtab)  { return (0); }

    for (size_t i = 0; i < size; i++) {
        Elf32_Sym sym = sym_tb[i];
        const char  st_code = ft_elf32_getLetterCode(shdr_tb, sym);
        const char *st_name = strtab + sym.st_name;

        /* check if the symbol is null-symbol... */
        if (!sym.st_name  &&
            !sym.st_info  &&
            !sym.st_other &&
            !sym.st_shndx &&
            !sym.st_value &&
            !sym.st_size
        ) {
            continue;
        }
        
        /* process '-u' / '--undefined-only'... */
        if (g_opt_undef) {
            if (sym.st_shndx == SHN_UNDEF) {
                ft_elf32_printSymbol(sym, st_name, st_code);
            }
        }
        
        /* process '-g' / '--extern-only'... */
        else if (g_opt_extern) {
            const uint8_t st_bind = ELF32_ST_BIND(sym.st_info);
            if (st_bind == STB_GLOBAL ||
                st_bind == STB_WEAK
            ) {
                ft_elf32_printSymbol(sym, st_name, st_code);
            }
        }
        
        /* process '-a' / '--debug-syms'... */
        else if (g_opt_debug) {
            ft_elf32_printSymbol(sym, st_name, st_code);
        }

        /* process default... */
        else {
            const uint16_t st_shndx = sym.st_shndx;
            if (st_shndx != SHN_LOPROC    &&
                st_shndx != SHN_HIPROC    &&
                st_shndx != SHN_BEFORE    &&
                st_shndx != SHN_AFTER     &&
                st_shndx != SHN_LOOS      &&
                st_shndx != SHN_HIOS      &&
                st_shndx != SHN_ABS       &&
                st_shndx != SHN_COMMON    &&
                st_shndx != SHN_XINDEX    &&
                st_shndx != SHN_HIRESERVE
            ) {
                const uint8_t st_type = ELF32_ST_TYPE(sym.st_info);
                if (st_type != STT_SECTION) {
                    ft_elf32_printSymbol(sym, st_name, st_code);
                }
            }
        }
    }

    return (1);
}
static int ft_elf32_printSymbol(Elf32_Sym sym, const char *st_name, const char st_code) {
    /* print address... */
    if (sym.st_shndx == SHN_UNDEF) {
        ft_putstr_fd("                 ", 1);
    }
    else {
        size_t numlen = ft_numlen(sym.st_value, 16);
        for (size_t i = 0; i < 16 - numlen; i++) {
            ft_putchar_fd('0', 1);
        }
        if (sym.st_value > 0) {
            ft_puthex_fd(sym.st_value, 1);
        }
        ft_putchar_fd(' ', 1);
    }

    /* print code... */
    ft_putchar_fd(st_code, 1);
    ft_putchar_fd(' ', 1);

    /* print name... */
    if (!st_name && !st_name[0]) {
        ft_putchar_fd('\n', 1);
    }
    else {
        ft_putendl_fd((char *) st_name, 1);
    }

    return (1);
}

static int ft_elf32_getLetterCode(Elf32_Shdr *shdr_tb, Elf32_Sym sym) {
    /* null-check... */
    if (!shdr_tb) { return (0); }
    
    /* result */
    int c = 0;
    
    /* Weak symbols... */
    const uint16_t st_shndx = sym.st_shndx;
    const uint8_t st_type   = ELF32_ST_TYPE(sym.st_info);
    const uint8_t st_bind   = ELF32_ST_BIND(sym.st_info);
    switch (st_bind) {
        case (STB_GNU_UNIQUE): { return ('u'); }
        case (STB_WEAK): {
            if (st_shndx == SHN_UNDEF) {
                c = (st_type == STT_OBJECT ? 'v' : 'w');
            }
            else {
                c = (st_type == STT_OBJECT ? 'V' : 'W');
            }
        }
    }

    /* special indices... */
    switch (st_shndx) {
        case (SHN_ABS):    { if (!c) { c = 'A'; } } break;
        case (SHN_UNDEF):  { if (!c) { c = 'U'; } } break;
        case (SHN_COMMON): { if (!c) { c = 'C'; } } break;
    }

    /* proceed if letter code wasn't found... */
    if (!c) {
        /* Section type/flags... */
        Elf32_Shdr shdr = shdr_tb[st_shndx];
        
        const uint32_t sh_type = shdr.sh_type;
        switch (sh_type) {
            case (SHT_NOBITS): {
                if (shdr.sh_flags == (SHF_ALLOC | SHF_WRITE)) {
                    c = 'B';
                }
            } break;

            default: {
                switch (shdr.sh_flags) {
                    case (SHF_ALLOC):
                    case (SHF_ALLOC | SHF_MERGE):
                    case (SHF_ALLOC | SHF_MERGE | SHF_STRINGS): { c = 'R'; } break;

                    case (SHF_ALLOC | SHF_WRITE):
                    case (SHF_ALLOC | SHF_WRITE | SHF_TLS): { c = 'D'; } break;

                    case (SHF_ALLOC | SHF_EXECINSTR):
                    case (SHF_ALLOC | SHF_EXECINSTR | SHF_GROUP):
                    case (SHF_ALLOC | SHF_EXECINSTR | SHF_WRITE): { c = 'T'; } break;
                }
            } break;
        }
    }
    /* check if symbol is global / local... */
    return (st_bind == STB_LOCAL ? ft_tolower(c) : c);
}
