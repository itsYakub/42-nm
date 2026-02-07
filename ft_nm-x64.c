#include "./ft_nm.h"

/* SECTION: static functions
 * */

static void *ft_elf64_getStrtab(Elf64_Ehdr *, Elf64_Shdr *, const char *, const char *, const char *);

static int ft_elf64_getLetterCode(Elf64_Shdr *, Elf64_Sym);

/* SECTION: api
 * */

extern struct s_symbol *ft_elf64(const char *buffer) {
    if (!buffer) { return (0); }

    /* output pointer... */
    size_t size = 0;
    struct s_symbol *arr = 0;

    /* ehdr... */
    Elf64_Ehdr *ehdr = ft_elf_extract(buffer, sizeof(Elf64_Ehdr), 0);
    if (!ehdr) { goto ft_elf64_exit; }

    /* shdr table... */
    Elf64_Shdr *shdr_tb = ft_elf_extract(buffer, ehdr->e_shnum * ehdr->e_shentsize, ehdr->e_shoff);
    if (!shdr_tb) { goto ft_elf64_exit; }

    /* extract STRTAB's... */
    char *shstrtab = ft_elf_extract(buffer, shdr_tb[ehdr->e_shstrndx].sh_size, shdr_tb[ehdr->e_shstrndx].sh_offset);
    if (!shstrtab) { goto ft_elf64_exit; }

    char *strtab = ft_elf64_getStrtab(ehdr, shdr_tb, buffer, shstrtab, ".strtab");

    /* extract symbol table... */
    Elf64_Sym *sym_tb = 0;
    for (size_t i = 0; i < ehdr->e_shnum; i++) {
        Elf64_Shdr shdr = shdr_tb[i];
        if (shdr.sh_type != SHT_SYMTAB) {
            continue;
        }

        sym_tb= ft_elf_extract(buffer, shdr.sh_size, shdr.sh_offset);
        if (!sym_tb) {
            goto ft_elf64_exit; 
        }

        size = shdr.sh_size / sizeof(*sym_tb);
    }
    if (!sym_tb) {
        g_errno = 4;
        goto ft_elf64_exit;
    }

    arr = ft_calloc(size + 1, sizeof(struct s_symbol));
    if (!arr) {
        goto ft_elf64_exit; 
    }

    for (size_t i = 0; i < size; i++) {
        Elf64_Sym sym = sym_tb[i];
        char  st_code = ft_elf64_getLetterCode(shdr_tb, sym);
        char *st_name = strtab + sym.st_name;
        if (!*st_name) {
            if (g_opt_debug) {
                if (ELF64_ST_TYPE(sym.st_info) == STT_SECTION) {
                    st_name = shstrtab + shdr_tb[sym.st_shndx].sh_name;
                }
            }
        } 
        
        ft_strlcpy(arr[i].name, st_name, PATH_MAX);
        arr[i].arch = ELFCLASS64;
        arr[i].type = ELF64_ST_TYPE(sym.st_info);
        arr[i].bind = ELF64_ST_BIND(sym.st_info);
        arr[i].shndx = sym.st_shndx;
        arr[i].addr = sym.st_value;
        arr[i].code = st_code;
        arr[i].valid = 1;
    }

ft_elf64_exit:
    if (shstrtab) { free((void *) shstrtab), shstrtab = 0; }
    if (strtab)   { free((void *) strtab), strtab = 0; }
    if (shdr_tb)  { free(shdr_tb), shdr_tb = 0; }
    if (sym_tb)   { free(sym_tb), sym_tb = 0; }
    if (ehdr)     { free(ehdr), ehdr = 0; }
    return (arr);
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

static int ft_elf64_getLetterCode(Elf64_Shdr *shdr_tb, Elf64_Sym sym) {
    /* null-check... */
    if (!shdr_tb) { return (0); }
    
    /* result */
    int c = 0;
    
    /* Weak symbols... */
    const uint16_t st_shndx = sym.st_shndx;
    const uint8_t st_type   = ELF64_ST_TYPE(sym.st_info);
    const uint8_t st_bind   = ELF64_ST_BIND(sym.st_info);

    /* special indices... */
    switch (st_shndx) {
        case (SHN_ABS):    { if (!c) { c = 'A'; } } break;
        case (SHN_UNDEF):  { if (!c) { c = 'U'; } } break;
        case (SHN_COMMON): { if (!c) { c = 'C'; } } break;
    }

    /* symbol binding... */
    switch (st_bind) {
        case (STB_GNU_UNIQUE): { c = 'u'; } break;
        case (STB_WEAK): {
            if (st_shndx == SHN_UNDEF) {
                c = (st_type == STT_OBJECT ? 'v' : 'w');
            }
            else {
                c = (st_type == STT_OBJECT ? 'V' : 'W');
            }
        } break;
    }

    /* symbol types... */
    switch (st_type) {
        case (STT_GNU_IFUNC): { c = 'i'; } break;
        case (STT_FILE): { c = 'a'; } break;
    }

    /* proceed if letter code wasn't found... */
    if (!c) {
        /* Section type/flags... */
        Elf64_Shdr shdr = shdr_tb[st_shndx];
        
        const uint32_t sh_type = shdr.sh_type;
        switch (sh_type) {
            case (SHT_NOBITS): {
                c = 'B';
            } break;

            default: {
                if (!(shdr.sh_flags & SHF_ALLOC)) { c = 'N'; }
                switch (shdr.sh_flags) {
                    case (SHF_ALLOC):
                    case (SHF_ALLOC | SHF_MERGE):
                    case (SHF_ALLOC | SHF_MERGE | SHF_STRINGS): { c = 'R'; } break;

                    case (SHF_WRITE):
                    case (SHF_WRITE | SHF_ALLOC):
                    case (SHF_WRITE | SHF_ALLOC | SHF_TLS):
                    case (SHF_WRITE | SHF_ALLOC | SHF_GNU_RETAIN): { c = 'D'; } break;

                    case (SHF_EXECINSTR):
                    case (SHF_EXECINSTR | SHF_ALLOC):
                    case (SHF_EXECINSTR | SHF_ALLOC | SHF_GROUP):
                    case (SHF_EXECINSTR | SHF_ALLOC | SHF_WRITE): { c = 'T'; } break;
                }
            } break;
        }
    }

    /* check if symbol is global / local... */
    return (st_bind == STB_LOCAL ? ft_tolower(c) : c);
}
