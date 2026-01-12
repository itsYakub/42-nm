#include <stdio.h>      /* perror() */
#include <stdlib.h>     /* malloc(), free(), exit()  */

#include <errno.h>
#include <string.h>     /* strerror() */

#include <elf.h>
#include <fcntl.h>      /* open(), close() */
#include <unistd.h>     /* write(), getpagesize() */
#include <sys/stat.h>   /* fstat() */
#include <sys/mman.h>   /* mmap(), munmap() */


#include "libft/libft.h"

/* SECTION: preprocessor
 * */

#if defined (__LP64__)
# define ElfEhdr    Elf64_Ehdr
# define ElfShdr    Elf64_Shdr
# define ElfPhdr    Elf64_Phdr
# define ElfSym     Elf64_Sym
#else
# define ElfEhdr    Elf32_Ehdr
# define ElfShdr    Elf32_Shdr
# define ElfPhdr    Elf32_Phdr
# define ElfSym     Elf32_Sym
#endif /* __LP64__ */

/* SECTION: api
 * */

extern int ft_getopt(int, char **);
extern int ft_procFile(const char *);
extern int ft_procELF(const char *, const char *);

extern ElfEhdr *ft_ElfEhdrLoad(const char *, const char *);

extern ElfPhdr *ft_ElfPhdrLoad(ElfEhdr *, const char *);
extern int ft_ElfPhdrPrint(ElfEhdr *, ElfPhdr *, ElfSym *, const char *);

extern ElfShdr *ft_ElfShdrLoad(ElfEhdr *, const char *);
extern int ft_ElfShdrPrint(ElfEhdr *, ElfShdr *, ElfSym *, const char *);

extern ElfSym *ft_ElfSymLoadS(ElfShdr *, const char *);
extern ElfSym *ft_ElfSymLoadP(ElfPhdr *, const char *);
extern ElfSym *ft_ElfGetStrtab(ElfEhdr *, ElfShdr *, const char *);

extern int ft_ElfUnload(void *);

/* SECTION: globals
 * */

extern t_list *g_paths;
extern const char *g_prog;

/* SECTION: main
 * */

int main(int ac, char **av) {
    if (!ft_getopt(ac, av)) { return (1); }
    if (!g_paths || !ft_lstsize(g_paths)) {
        g_paths = ft_lstnew(ft_strdup("a.out"));
        if (!g_paths) {
            return (1);
        }
    }

    for (t_list *path = g_paths; path; path = path->next) {
        const char *name = path->content;

        if (!name) { return (1); }
        if (ft_lstsize(g_paths) > 1) { printf("\n%s:\n", name); }
        if (!ft_procFile(name)) { break; }
    }

    ft_lstclear(&g_paths, free), g_paths = 0;
    return (0);
}

/* SECTION: api
 * */

int ft_getopt(int ac, char **av) {
    (void) ac;
    g_prog = (const char *) *av;
    if (!g_prog) { return (0); }
    while (*++av) {
        /* process options... */
        if (**av == '-') { }

        /* process files... */
        else {
            t_list  *l;

            l = ft_lstnew(ft_strdup(*av));
            if (!l) { return (0); }

            ft_lstadd_back(&g_paths, l);
        }
    }
    return (1);
}

int ft_procFile(const char *path) {
    /* setup... */
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("%s: '%s': No such file\n", g_prog, path);
        return (0);
    }

    struct stat stat = { 0 };
    if (fstat(fd, &stat) == -1) {
        perror(g_prog);
        return (0);
    }
    
    if (S_ISDIR(stat.st_mode)) {
        printf("%s: Warning: '%s' is a directory\n", g_prog, path);
        return (0);
    }

    /* read file to a buffer... */
    char *buffer = ft_calloc(stat.st_size + 1, sizeof(char));
    if (!buffer) {
        close(fd), fd = 0;
        return (0);
    }

    if (read(fd, buffer, stat.st_size) != stat.st_size) {
        free(buffer), buffer = 0;
        close(fd), fd = 0;
        return (0);
    }
    
    close(fd), fd = 0;
    
    /* execution... */
    if (!ft_procELF(path, buffer)) { return (0); }
    
    /* cleanup... */
    free(buffer), buffer = 0;

    return (1);
}

int ft_procELF(const char *path, const char *buffer) {
    /* ehdr... */
    ElfEhdr *ehdr = ft_ElfEhdrLoad(path, buffer);
    if (!ehdr) {
        return (0);
    }

    /* phdr table... */
    size_t phnum = ehdr->e_phnum; (void) phnum;
    ElfPhdr *phdr_tb = ft_ElfPhdrLoad(ehdr, buffer);
    if (!phdr_tb) {
        ft_ElfUnload(ehdr), ehdr = 0;
        return (0);
    }

    /* shdr table... */
    size_t shnum = ehdr->e_shnum; (void) shnum;
    ElfShdr *shdr_tb = ft_ElfShdrLoad(ehdr, buffer);
    if (!shdr_tb) {
        ft_ElfUnload(ehdr), ehdr = 0;
        ft_ElfUnload(phdr_tb), phdr_tb = 0;
        return (0);
    }

    /* .strtab ... */
    ElfSym *strtab = ft_ElfGetStrtab(ehdr, shdr_tb, buffer);
    if (!strtab) {
        ft_ElfUnload(ehdr), ehdr = 0;
        ft_ElfUnload(phdr_tb), phdr_tb = 0;
        ft_ElfUnload(shdr_tb), shdr_tb = 0;
    }

    /* phdr print... */
    for (size_t i = 0; i < phnum; i++) {
        if (!ft_ElfPhdrPrint(ehdr, &phdr_tb[i], strtab, buffer)) {
            free(strtab), strtab = 0;
            ft_ElfUnload(shdr_tb), shdr_tb = 0;
            ft_ElfUnload(phdr_tb), phdr_tb = 0;
            ft_ElfUnload(ehdr), ehdr = 0;
            return (0);
        }
    }

    /* shdr print... */
    for (size_t i = 0; i < shnum; i++) {
        if (!ft_ElfShdrPrint(ehdr, &shdr_tb[i], strtab, buffer)) {
            free(strtab), strtab = 0;
            ft_ElfUnload(shdr_tb), shdr_tb = 0;
            ft_ElfUnload(phdr_tb), phdr_tb = 0;
            ft_ElfUnload(ehdr), ehdr = 0;
            return (0);
        }
    }

    /* cleanup... */
    ft_ElfUnload(strtab), strtab = 0;
    ft_ElfUnload(shdr_tb), shdr_tb = 0;
    ft_ElfUnload(phdr_tb), phdr_tb = 0;
    ft_ElfUnload(ehdr), ehdr = 0;
    return (1);
}

ElfEhdr *ft_ElfEhdrLoad(const char *path, const char *buffer) {
    /* safety check... */
    if (!path) { return (0); }
    if (!buffer) { return (0); }

    /* allocate ehdr... */
    ElfEhdr *ehdr = malloc(sizeof(ElfEhdr));
    if (!ehdr) { return (0); }
    if (!ft_memcpy((void *) ehdr, buffer, sizeof(ElfEhdr))) {
        free(ehdr);
        return (0);
    } 

    /* check magic... */
    if (ft_memcmp(ehdr->e_ident, ELFMAG, SELFMAG)) {
        printf("%s: '%s': Not an ELF\n", g_prog, path);
        ft_ElfUnload(ehdr);
        return (0);
    }

    return (ehdr);
}

extern ElfPhdr *ft_ElfPhdrLoad(ElfEhdr *ehdr, const char *buffer) {
    /* safety check... */
    if (!ehdr) { return (0); }
    if (!buffer) { return (0); }

    /* helper variables... */
    size_t phoff = ehdr->e_phoff;
    size_t phnum = ehdr->e_phnum;
    size_t phentsize = ehdr->e_phentsize;
    const char *phbuf = buffer + phoff;

    /* allocate phdr... */
    ElfPhdr *phdr = malloc(phentsize * phnum);
    if (!phdr) { return (0); }
    if (!ft_memcpy((void *) phdr, phbuf, phnum * phentsize)) {
        free(phdr), phdr = 0;
        return (0);
    }

    return (phdr);
}

int ft_ElfPhdrPrint(ElfEhdr *ehdr, ElfPhdr *phdr, ElfSym *strtab, const char *buffer) {
    /* safety check... */
    if (!ehdr) { return (0); } 
    if (!phdr) { return (0); }
    if (!strtab) { return (0); }
    if (!phdr->p_type) { return (1); }

    /* symbols... */
    ElfSym *sym_tb = ft_ElfSymLoadP(phdr, buffer);
    if (!sym_tb) {
        return (0);
    }

    /* symbol... */
    size_t stnum = phdr->p_filesz / sizeof(ElfSym);
    for (size_t j = 0; j < stnum; j++) {
        ElfSym sym = sym_tb[j];

        if (sym.st_name == 0) {
            continue;
        }
    }

    /* cleanup... */
    ft_ElfUnload(sym_tb), sym_tb = 0;
    return (1);
}

ElfShdr *ft_ElfShdrLoad(ElfEhdr *ehdr, const char *buffer) {
    /* safety check... */
    if (!ehdr) { return (0); }
    if (!buffer) { return (0); }

    /* helper variables... */
    size_t shoff = ehdr->e_shoff;
    size_t shnum = ehdr->e_shnum;
    size_t shentsize = ehdr->e_shentsize;
    const char *shbuf = buffer + shoff;

    /* allocate shdr... */
    ElfShdr *shdr = malloc(shentsize * shnum);
    if (!shdr) { return (0); }
    if (!ft_memcpy((void *) shdr, shbuf, shnum * shentsize)) {
        free(shdr), shdr = 0;
        return (0);
    }

    return (shdr);
}

int ft_ElfShdrPrint(ElfEhdr *ehdr, ElfShdr *shdr, ElfSym *strtab, const char *buffer) {
    /* safety check... */
    if (!ehdr) { return (0); } 
    if (!shdr) { return (0); }
    if (!strtab) { return (0); }
    if (!shdr->sh_type) { return (1); }

    /* print address... */
    if (shdr->sh_addr != 0) {
        printf("%016lx ", shdr->sh_addr);
    }
    else {
        printf("%*s ", 16, " ");
    }

    /* print name... */
    printf("%s\n", (const char *) (strtab) + shdr->sh_name);

    /* symbols... */
    if (shdr->sh_type != SHT_SYMTAB) { return (1); }
    if (shdr->sh_type != SHT_DYNSYM) { return (1); }
    
    ElfSym *sym_tb = ft_ElfSymLoadS(shdr, buffer);
    if (!sym_tb) {
        return (0);
    }

    /* symbol... */
    size_t stnum = shdr->sh_size / sizeof(ElfSym);
    for (size_t j = 0; j < stnum; j++) {
        ElfSym sym = sym_tb[j];

        if (sym.st_name == 0) {
            continue;
        }
    }

    /* cleanup... */
    ft_ElfUnload(sym_tb), sym_tb = 0;
    return (1);
}

ElfSym *ft_ElfSymLoadS(ElfShdr *shdr, const char *buffer) {
    /* safety check... */
    if (!shdr) { return (0); }
    if (!buffer) { return (0); }

    /* helper variables... */
    size_t offset = shdr->sh_offset;
    size_t shsize = shdr->sh_size;
    const char *sbuf = buffer + offset;

    /* allocate sym... */
    ElfSym *sym = malloc(shsize);
    if (!sym) { return (0); }
    if (!ft_memcpy((void *) sym, sbuf, shsize)) {
        free(sym), sym = 0;
        return (0);
    }

    return (sym);
}

ElfSym *ft_ElfSymLoadP(ElfPhdr *phdr, const char *buffer) {
    /* safety check... */
    if (!phdr) { return (0); }
    if (!buffer) { return (0); }

    /* helper variables... */
    size_t offset = phdr->p_offset;
    size_t filesz = phdr->p_filesz;
    const char *pbuf = buffer + offset;

    /* allocate sym... */
    ElfSym *sym = malloc(filesz);
    if (!sym) { return (0); }
    if (!ft_memcpy((void *) sym, pbuf, filesz)) {
        free(sym), sym = 0;
        return (0);
    }

    return (sym);
}

ElfSym *ft_ElfGetStrtab(ElfEhdr *ehdr, ElfShdr *shdr, const char *buffer) {
    /* safety check... */
    if (!ehdr) { return (0); }
    if (!shdr) { return (0); }
    if (!buffer) { return (0); }

    /* get .strtab index... */
    size_t shstrndx = 0;
    if (ehdr->e_shstrndx != SHN_XINDEX) {
        shstrndx = ehdr->e_shstrndx;
    }
    else {
        /* process SH_XINDEX... */
    }

    ElfShdr section = shdr[shstrndx];
    if (section.sh_type == SHT_NULL) {
        return (0);
    }

    return (ft_ElfSymLoadS(&section, buffer));
}

int ft_ElfUnload(void *data) {
    if (!data) { return (0); }
    
    free(data);
    return (1);
}


/* SECTION: globals
 * */

t_list *g_paths = 0;
const char *g_prog = 0;
