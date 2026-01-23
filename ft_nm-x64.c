#include "./ft_nm.h"

/* SECTION: static functions
 * */

static int ft_elf64_comparea(Elf64_Sym, Elf64_Sym, const char *);

static int ft_elf64_compared(Elf64_Sym, Elf64_Sym, const char *);

static void *ft_elf64_getStrtab(Elf64_Ehdr *, Elf64_Shdr *, const char *, const char *, const char *);

static int ft_elf64_getLetterCode(Elf64_Shdr *, Elf64_Sym);

static int ft_elf64_print(Elf64_Shdr *, Elf64_Sym *, const size_t, const char *);

/* SECTION: api
 * */

extern int ft_elf64(const char *buffer) {
    int exitcode = 1;

    /* ehdr... */
    Elf64_Ehdr *ehdr = ft_elf_extract(buffer, sizeof(Elf64_Ehdr), 0);
    if (!ehdr) {
        exitcode = 0; goto ft_elf64_exit;
    }

    /* shdr table... */
    Elf64_Shdr *shdr_tb = ft_elf_extract(buffer, ehdr->e_shnum * ehdr->e_shentsize, ehdr->e_shoff);
    if (!shdr_tb) {
        exitcode = 0; goto ft_elf64_exit;
    }

    /* extract STRTAB's... */
    const char *shstrtab = ft_elf_extract(buffer, shdr_tb[ehdr->e_shstrndx].sh_size, shdr_tb[ehdr->e_shstrndx].sh_offset);
    const char *dynstr = ft_elf64_getStrtab(ehdr, shdr_tb, buffer, shstrtab, ".dynstr");
    const char *strtab = ft_elf64_getStrtab(ehdr, shdr_tb, buffer, shstrtab, ".strtab");

    /* extract symbol table... */
    Elf64_Sym *sym_tb = 0;
    size_t sym_tb_s = 0;
    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        Elf64_Shdr shdr = shdr_tb[i];
        if (shdr.sh_type != SHT_SYMTAB) {
            continue;
        }

        Elf64_Sym *tmp = ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset);
        if (!tmp) {
            exitcode = 0; goto ft_elf64_exit;
        }

        if (!sym_tb) {
            sym_tb = ft_memdup(tmp, shdr.sh_size);
            if (!sym_tb) {
                free(tmp), tmp = 0;
                exitcode = 0; goto ft_elf64_exit;
            }
            sym_tb_s = shdr.sh_size / sizeof(Elf64_Sym);
        }
        else {
            void *tmp1 = sym_tb;
            sym_tb = ft_memjoin(sym_tb, tmp, shdr.sh_size, sym_tb_s);
            if (!sym_tb) {
                free(tmp), tmp = 0;
                exitcode = 0; goto ft_elf64_exit;
            }
            sym_tb_s += shdr.sh_size / sizeof(Elf64_Sym);
            free(tmp1), tmp1 = 0;
        }

        free(tmp), tmp = 0;
    }

    if (sym_tb_s == 0) {
        printf("%s: no symbols\n", g_prog);
        goto ft_elf64_exit;
    }

    switch (g_opt_sort) {
        case (1): { sym_tb = ft_elf64_sort(sym_tb, sym_tb_s, strtab, ft_elf64_comparea); } break;
        case (2): { sym_tb = ft_elf64_sort(sym_tb, sym_tb_s, strtab, ft_elf64_compared); } break;
        default:  { /* ...don't sort... */ } break;
    }

    /* print symbol table...
     * */
    if (!ft_elf64_print(shdr_tb, sym_tb, sym_tb_s, strtab)) {
        exitcode = 0; goto ft_elf64_exit;
    }

ft_elf64_exit:
    if (shstrtab) { free((void *) shstrtab), shstrtab = 0; }
    if (strtab)   { free((void *) strtab), strtab = 0; }
    if (dynstr)   { free((void *) dynstr), dynstr = 0; }
    if (shdr_tb)  { free(shdr_tb), shdr_tb = 0; }
    if (sym_tb)   { free(sym_tb), sym_tb = 0; }
    if (ehdr)     { free(ehdr), ehdr = 0; }
    return (exitcode);
}

extern Elf64_Sym *ft_elf64_sort(Elf64_Sym *sym_tb, const size_t size, const char *strtab, int (*compare)(Elf64_Sym, Elf64_Sym, const char *)) {
    /* safety-check... */
    if (!sym_tb) { return (0); }
    if (!strtab) { return (0); }
    if (!size)   { return (0); }

    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - 1 - i; j++) {
            if (compare(sym_tb[j], sym_tb[j + 1], strtab)) {
                Elf64_Sym tmp = sym_tb[j];
                sym_tb[j] = sym_tb[j + 1];
                sym_tb[j + 1] = tmp;
            }
        }
    }
    return (sym_tb);
}

/* SECTION: static functions
 * */

static void *ft_elf64_getStrtab(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr_tb, const char *buffer, const char *shstrtab, const char *name) {
    /* null-check... */
    if (!ehdr)     { return (0); }
    if (!shdr_tb)  { return (0); }
    if (!buffer)   { return (0); }
    if (!shstrtab) { return (0); }
    if (!name)     { return (0); }

    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        /* section header object... */
        Elf64_Shdr shdr = shdr_tb[i];

        if (!strcmp(shstrtab + shdr.sh_name, name)) {
            return (ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset));
        }
    }

    return (0);
}

static int ft_elf64_comparea(Elf64_Sym sym0, Elf64_Sym sym1, const char *strtab) {
    const char *name0 = strtab + sym0.st_name;
    const char *name1 = strtab + sym1.st_name;

    while (*name0 && *name1) {
        while (!ft_isalnum(*name0)) { name0++; }
        while (!ft_isalnum(*name1)) { name1++; }
        if (ft_tolower(*name0) != ft_tolower(*name1)) { break; }
        name0++;
        name1++;
    }

    if (!*name0 && !*name1) {
        return (1);
    }
    return (ft_tolower(*name0) > ft_tolower(*name1));
}

static int ft_elf64_compared(Elf64_Sym sym0, Elf64_Sym sym1, const char *strtab) {
    const char *name0 = strtab + sym0.st_name;
    const char *name1 = strtab + sym1.st_name;

    while (*name0 && *name1) {
        while (!ft_isalnum(*name0)) { name0++; }
        while (!ft_isalnum(*name1)) { name1++; }
        if (ft_tolower(*name0) != ft_tolower(*name1)) { break; }
        name0++;
        name1++;
    }

    if (!*name0 && !*name1) {
        return (1);
    }
    return (ft_tolower(*name0) < ft_tolower(*name1));
}

static int ft_elf64_getLetterCode(Elf64_Shdr *shdr_tb, Elf64_Sym sym) {
    /* null-check... */
    if (!shdr_tb) { return (0); }
    
    /* Weak symbols... */
    const uint16_t st_shndx = sym.st_shndx;
    const uint8_t st_type   = ELF64_ST_TYPE(sym.st_info);
    const uint8_t st_bind   = ELF64_ST_BIND(sym.st_info);
    switch (st_bind) {
        case (STB_GNU_UNIQUE): { return ('u'); }
        case (STB_WEAK): {
            if (st_shndx == SHN_UNDEF) {
                if (st_type == STT_OBJECT) { return ('v'); }
                else {
                    return ('w');
                }
            }
            else {
                if (st_type == STT_OBJECT) { return ('V'); }
                else {
                    return ('W');
                }
            }
        }
    }

    /* special indices... */
    switch (st_shndx) {
        case (SHN_UNDEF): {
            return (st_bind == STB_LOCAL ? 'u' : 'U');
        }
        case (SHN_ABS): {
            return (st_bind == STB_LOCAL ? 'a' : 'A');
        }
        case (SHN_COMMON): {
            return (st_bind == STB_LOCAL ? 'c' : 'C');
        }
    }
    
    /* result */
    int c = 0;

    /* Section type/flags... */
    Elf64_Shdr shdr = shdr_tb[sym.st_shndx];
    
    const uint32_t sh_type = shdr.sh_type;
    switch (sh_type) {
        case (SHT_NOBITS): {
            if (shdr.sh_flags == (SHF_ALLOC | SHF_WRITE)) {
                c = 'B';
            }
        } break;

        case (SHT_PROGBITS): {
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

        default: { c = 'D'; } break;
    }

    /* check if symbol is global / local... */
    if (st_bind == STB_LOCAL) {
        c = ft_tolower(c);
    }

    return (c);
}


static int ft_elf64_print(Elf64_Shdr *shdr_tb, Elf64_Sym *sym_tb, const size_t size, const char *strtab) {
    /* null-check... */
    if (!shdr_tb) { return (0); }
    if (!sym_tb)  { return (0); }
    if (!strtab)  { return (0); }

    for (size_t i = 0; i < size; i++) {
        Elf64_Sym sym = sym_tb[i];
        const uint8_t st_type = ELF64_ST_TYPE(sym.st_info);
        if (st_type != STT_NOTYPE &&
            st_type != STT_OBJECT &&
            st_type != STT_FUNC &&
            st_type != STT_COMMON
        ) {
            continue;
        }

        const char *st_name = strtab + sym.st_name;
        if (!st_name || !st_name[0]) {
            continue;
        }

        if (g_opt_undef) {
            if (sym.st_shndx != SHN_UNDEF) {
                continue;
            }
        }

        /* print address... */
        if (sym.st_value) {
            printf("%016lx ", sym.st_value);
        }
        else {
            printf("%*c ", 16, ' ');
        }

        /* print type... */
        const char letter_code = ft_elf64_getLetterCode(shdr_tb, sym);
        printf("%c ", letter_code);

        /* print name... */
        printf("%s\n", strtab + sym.st_name);
    }

    return (1);
}
