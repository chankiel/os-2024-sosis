#include <stdint.h>
#include "header/filesystem/fat32.h"

struct FAT32DirectoryTable cwd_table;

struct ShellState{
    char cur_dir[100];
    int idx;
    int cur_cluster;
};

static struct ShellState shell_state = {
    .cur_dir = "Root/",
    .idx = 5,
    .cur_cluster = 2,
};

void *memset2(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;  // Pointer to the memory block
    while (n--) {  // Decrement `n` each iteration
        *p++ = (unsigned char)c;  // Set the byte and advance the pointer
    }
    return s;  // Return the original pointer
}

int memcmp2(const void *s1, const void *s2, size_t n) {
    const uint8_t *buf1 = (const uint8_t*) s1;
    const uint8_t *buf2 = (const uint8_t*) s2;
    for (size_t i = 0; i < n; i++) {
        if (buf1[i] < buf2[i])
            return -1;
        else if (buf1[i] > buf2[i])
            return 1;
    }

    return 0;
}

int strlen2(const char *str)
{
    int len = 0;
    while (*str != '\0')
    {
        len++;
        str++;
    }
    return len;
}

int strcmp2(const char *str1, const char *str2) {
    uint16_t i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i])
            return 0;
        i++;
    }
    if (str1[i] != str2[i])
        return 0;
    return 1;
}

int strncmp2(const char *str1, const char *str2, uint16_t n) {
    uint16_t i = 0;
    while (i < n && str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i])
            return 0;
        i++;
    }
    if (i < n && str1[i] != str2[i])
        return 0;
    return 1;
}

char *strtok2(char *str, const char *delim) {
    static char *last_token = NULL;
    char *tmp;
    const char *tmp_delim;

    if (str != NULL) {
        last_token = str;
    }

    if (last_token == NULL) {
        return NULL;
    }

    char* ret;
    tmp_delim = delim;
    while (*tmp_delim != '\0') {
        tmp = last_token;
        while (*tmp != '\0') {
            if (*tmp == *tmp_delim) {
                *tmp = '\0';
                ret = last_token;
                last_token = tmp + 1;
                return ret;
            }
            tmp++;
        }
        tmp_delim++;
    }

    tmp = last_token;
    last_token = NULL;
    return tmp;
}

void addTrailingNull2(char *str, uint16_t start, uint16_t end) {
    for (uint16_t i = start; i < end; i++)
        str[i] = '\0';
}

int parseFileName2(char *filename, char *name, char *ext) {
    int i = 0;
    while (i < 8 && filename[i] != '.' && filename[i] != '\0'){
        name[i] = filename[i];
        i++;
    }
    addTrailingNull2(name, i, 8);

    if (filename[i] == '\0') {
        addTrailingNull2(ext, 0, 3);
        return 0;
    }

    if (filename[i] != '.')
        return 1;
    
    int fnlen = i;
    i++;
    while (i <= fnlen + 3 && filename[i] != '\0'){
        ext[i-fnlen-1] = filename[i];
        i++;
    }
    addTrailingNull2(ext, i - fnlen - 1, 3);

    if (filename[i] != '\0')
        return 1;

    return 0;
}

int countWords2(const char* str) {
    int ctr = 0;
    for (int i = 0; i<strlen2(str)-1; i++) {
        if (i == 0 && str[i] != ' ') {
            ctr++;
        }
        else if (str[i] ==' ' && str[i+1] != ' ') {
            ctr++;
        }
    }
    return ctr;
}

int wordLen2(const char* str, uint16_t idx) {
    int ctr = 0;
    int word_start = -1;
    int word_end = -1;

    for (int i = 0; i<strlen2(str)-1; i++) {
        if (word_start != -1 && word_end == -1 && str[i] == ' ') {
            word_end = i;
        }
        if (i == 0 && str[i] != ' ') {
            ctr++;
            if (ctr == idx + 1) {
                word_start = i;
            }
        }
        else if (str[i] ==' ' && str[i+1] != ' ') {
            ctr++;
            if (ctr == idx + 1) {
                word_start = i + 1;
            }
        }
    }

    if (word_end == -1)
        word_end = strlen2(str);

    uint16_t n = word_end - word_start;
    return n;
}

void getWord2(const char* str, uint16_t idx, char* buf) {
    int ctr = 0;
    int word_start = -1;

    for (int i = 0; i<strlen2(str)-1; i++) {
        if (i == 0 && str[i] != ' ') {
            ctr++;
            if (ctr == idx + 1) {
                word_start = i;
            }
        }
        else if (str[i] ==' ' && str[i+1] != ' ') {
            ctr++;
            if (ctr == idx + 1) {
                word_start = i + 1;
            }
        }
    }
    
    uint16_t n = wordLen2(str, idx);
    for (int i = 0; i < n; i++) {
        buf[i] = str[word_start + i];
    }
    buf[n] = '\0';
}

char* custom_strtok2(char* str, const char* delim) {
    static char* saved_str = NULL;
    char* token;

    if (str != NULL) {
        saved_str = str;
    }

    token = saved_str;
    while (*saved_str != '\0') {
        const char* d = delim;
        while (*d != '\0') {
            if (*saved_str == *d) {
                *saved_str = '\0';
                saved_str++;
                if (*token != '\0') {
                    return token;
                } else {
                    token = saved_str;
                    break;
                }
            }
            d++;
        }
        saved_str++;
    }

    if (*token == '\0') {
        return NULL;
    } else {
        saved_str = NULL;
        return token;
    }
}

void* memcpy2(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;  
}

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

void puts(char* val, uint32_t color)
{
    syscall(6, (uint32_t) val, strlen2(val), color);
}

void strcpy2(char *destination, const char *source) {
    while (*source != '\0') {
        *destination =* source; 
        destination++;
        source++; 
    }
    *destination = '\0';
}

// MAIN COMMANDS

void remove(char* name, char* ext, uint32_t parent_number)
{
    int8_t ret;

    struct ClusterBuffer data_buf;
    struct FAT32DriverRequest request = {
        .buf = &data_buf,
        .name = "\0\0\0\0\0\0\0",
        .ext = "\0\0",
        .buffer_size = 0,
        .parent_cluster_number = parent_number
    };

    memcpy2(request.name, name, 8);
    memcpy2(request.ext, ext, 3);
    syscall(3, (uint32_t)&request, (uint32_t)&ret, 0);

    memcpy2(request.name, name, 8);
    memcpy2(request.ext, ext, 3);
    request.parent_cluster_number = parent_number;
    syscall(3, (uint32_t)&request, (uint32_t)&ret, 0);
}

void copy(char* src_name, char* src_ext, uint32_t src_parent_number, char* target_name, char* target_ext, uint32_t target_parent_number) {
    struct ClusterBuffer cl = {0};
    int8_t t_retcode;
    struct FAT32DriverRequest t_request = {
        .buf = &cl,
        .name = "\0\0\0\0\0\0\0",
        .ext = "\0\0",
        .parent_cluster_number = target_parent_number,
        .buffer_size = sizeof(struct FAT32DirectoryEntry)
    };

    memcpy2(t_request.name, target_name, 8);
    memcpy2(t_request.ext, target_ext, 3);
    syscall(1, (uint32_t)&t_request, (uint32_t)&t_retcode, 0);

    if (t_retcode != 2) {
        remove(target_name, target_ext, target_parent_number);
    }
    uint32_t src_size;
    bool is_dir = 0;
    struct FAT32DirectoryTable src_table;
    uint32_t src_cluster_number;
    struct FAT32DirectoryTable src_parent_table;
    syscall(99, (uint32_t)&src_parent_table, src_parent_number, 0);
    
    for (int32_t i = 0; i < (int32_t)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++) {
        // puts("aaa", 0x07);
        // puts((char*) (src_parent_table.table[i].name), 0x07);
        if (memcmp2(src_parent_table.table[i].name, src_name, 4) == 0 &&
            memcmp2(src_parent_table.table[i].ext, src_ext, 3) == 0) {
            src_size = src_parent_table.table[i].filesize;
            // puts((char*) (src_size), 0x07);
            src_cluster_number = (src_parent_table.table[i].cluster_high << 16) | src_parent_table.table[i].cluster_low;
            is_dir = src_parent_table.table[i].attribute == ATTR_SUBDIRECTORY;
            if (is_dir){
                src_size = sizeof(struct FAT32DirectoryTable);
            break;
            }
        }
    }

    // struct ClusterBuffer data_buf[(src_size + CLUSTER_SIZE - 1) / CLUSTER_SIZE];
    uint32_t num_clusters = (src_size + CLUSTER_SIZE - 1) / CLUSTER_SIZE;
    // puts((char*) (num_clusters), 0x07);

    // Allocate an array of ClusterBuffer of the correct size
    struct ClusterBuffer data_buf[num_clusters];


    struct FAT32DriverRequest request = {
        .buf = &data_buf,
        .name = "\0\0\0\0\0\0\0",
        .ext = "\0\0",
        .buffer_size = src_size,
        .parent_cluster_number = src_parent_number
    };

    memcpy2(request.name, src_name, 8);
    memcpy2(request.ext, src_ext, 3);
    
    int8_t retcode;
    if (is_dir) {
        request.buf = &src_table;
        syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);
    } else{
        syscall(0, (uint32_t)&request, (uint32_t)&retcode, 0);}

    if (is_dir) {
        memcpy2(request.name, target_name, 8);
        memcpy2(request.ext, target_ext, 3);
        request.parent_cluster_number = target_parent_number;
        request.buffer_size = 0;
        syscall(2, (uint32_t)&request, (uint32_t)&retcode, 0);

        uint32_t target_cluster_number;
        struct FAT32DirectoryTable target_parent_table;

        syscall(7, (uint32_t)&target_parent_table, target_parent_number, 0);

        for (int32_t i = 0; i < (int32_t)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++) {
            if (memcmp2(target_parent_table.table[i].name, target_name, 8) == 0 &&
                memcmp2(target_parent_table.table[i].ext, target_ext, 3) == 0 &&
                target_parent_table.table[i].user_attribute == UATTR_NOT_EMPTY) {
                target_cluster_number = (target_parent_table.table[i].cluster_high << 16) | target_parent_table.table[i].cluster_low;
            }
        }

        for (int32_t i = 1; i < (int32_t)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++) {
            if (src_table.table[i].user_attribute == UATTR_NOT_EMPTY) {
                copy(src_table.table[i].name, src_table.table[i].ext, src_cluster_number, src_table.table[i].name, src_table.table[i].ext, target_cluster_number);
            }
        }
    } else {
        memcpy2(request.name, target_name, 8);
        memcpy2(request.ext, target_ext, 3);
        request.parent_cluster_number = target_parent_number;
        syscall(2, (uint32_t)&request, (uint32_t)&retcode, 0);
        if (retcode != 0)
            puts("Error writing to file", 0x07);
    }
}

void cp(char* command) {
    uint16_t n_words = countWords2(command);
    int16_t recursive = -1;
    int8_t retcode = 0;

    struct FAT32DirectoryTable table_buf = {0};
    struct FAT32DriverRequest request = {
        .buf = &table_buf,
        .name = "\0\0\0\0\0\0\0",
        .ext = "\0\0",
        .parent_cluster_number = 2,
        .buffer_size = sizeof(struct FAT32DirectoryEntry)
    };
    for (uint16_t i = 1; i < n_words; i++) {
        uint16_t n = wordLen2(command, i);
        char word[n + 1];
        getWord2(command, i, word);
        if (strcmp2(word, "-r"))
            recursive = i;
    }

    if ((recursive == -1 && n_words < 3) || (recursive != -1 && n_words < 4)) {
        // puts("hi", 0x07);
        // puts((char*) recursive, 0x07);
        puts(": missing file operands\n", 0x07);
        return;
    }
    uint16_t target_idx;
    if (recursive == n_words - 1)
        target_idx = n_words - 2;
    else
        target_idx = n_words - 1;

    uint16_t target_n = wordLen2(command, target_idx);
    char target_filename[target_n + 1];
    getWord2(command, target_idx, target_filename);

    if (2 == 2 && strcmp2(target_filename, "..")) {
        puts("root folder does not have parent\n", 0x07);
        return;
    }
    // check if all source exists
    for (uint16_t i = 1; i < n_words; i++) {
        if (recursive == i || target_idx == i) continue;
        char filename[12];
        getWord2(command, i, filename);

        if (strcmp2(filename, "..")) {
            puts("cannot copy a directory, '..', into itself\n", 0x07);
            return;
        }

        char name[9];
        char ext[4];

        // if filename is too long
        if (parseFileName2(filename, name, ext)) {
            puts(filename, 0x07);
            puts(": filename invalid, name or extension may be too long\n", 0x07);
            return;
        }

        memcpy2(request.name, name, 8);
        memcpy2(request.ext, ext, 3);
        syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);
        if (retcode == 2) {
            puts(filename, 0x07);
            puts(": file not found\n", 0x07);
            return;
        }
        if (retcode == 0 && recursive == -1) {
            puts(filename, 0x07);
            puts(": is a directory;  -r not specified\n", 0x07);
            return;
        }
        if (retcode == 0 && strcmp2(target_filename, "..") && memcmp2(name, cwd_table.table[0].name, 8) == 0) {
            puts(filename, 0x07);
            puts(": cannot copy into itself\n", 0x07);
            return;
        } 
    }
    char target_name[9];
    char target_ext[4];

    if (strcmp2(target_filename, "..")) {
        retcode = 0;
    } else {
        if (parseFileName2(target_filename, target_name, target_ext)) {
            puts(target_filename, 0x07);
            puts(": filename invalid, name or extension may be too long\n", 0x07);
            return;
        }

        memcpy2(request.name, target_name, 8);
        memcpy2(request.ext, target_ext, 3);
        syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);
    }
    // target is an existing directory
    if (retcode == 0) {
        uint32_t target_cluster_number;

        if (!strcmp2(target_filename, "..")) {
            for (int32_t i = 0; i < (int32_t)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++) {
                if (memcmp2(cwd_table.table[i].name, target_name, 8) == 0 &&
                    memcmp2(cwd_table.table[i].ext, target_ext, 3) == 0) {
                    target_cluster_number = (cwd_table.table[i].cluster_high << 16) | cwd_table.table[i].cluster_low;
                }
            }
        } else {
            target_cluster_number = (cwd_table.table[0].cluster_high << 16) | cwd_table.table[0].cluster_low;
        }

        for (int16_t i = 1; i < n_words; i++) {
            if (i == recursive || i == target_idx) continue;
            char filename[12];
            getWord2(command, i, filename);
            char name[9];
            char ext[4];
            parseFileName2(filename, name, ext);

            copy(name, ext, 2, name, ext, target_cluster_number);
        }
    } else if (retcode == 1 || retcode == 2) {
        if ((recursive == -1 && n_words > 3) || (recursive != -1 && n_words > 4)) {
            puts(target_filename, 0x07);
            puts(": is not a folder\n", 0x07);
            return;
        } else {
            for (int16_t i = 1; i < n_words; i++) {
                if (i == recursive || i == target_idx) continue;
                char filename[12];
                getWord2(command, i, filename);
                char name[9];
                char ext[4];
                parseFileName2(filename, name, ext);
                copy(name, ext, 2, target_name, target_ext, 2);
            }
        }
    }
}

void rm(char* command) {
    uint16_t n_words = countWords2(command);
    int16_t recursive = -1;
    int8_t retcode = 0;

    struct FAT32DirectoryTable table_buf = {0};
    struct FAT32DriverRequest request = {
        .buf = &table_buf,
        .name = "\0\0\0\0\0\0\0",
        .ext = "\0\0",
        .parent_cluster_number = 2, // could be a variable
        .buffer_size = sizeof(struct FAT32DirectoryTable)
    };

    for (uint16_t i = 1; i < n_words; i++) {
        uint16_t n = wordLen2(command, i);
        char word[n + 1];
        getWord2(command, i, word);
        if (strcmp2(word, "-r"))
            recursive = i;
    }

    if ((recursive == -1 && n_words < 2) || (recursive != -1 && n_words < 3)) {
        puts(": missing file operands\n", 0x07);
        return;
    }

    // check if all files exists
    for (uint16_t i = 1; i < n_words; i++) {
        if (recursive == i) continue;
        char filename[12];
        getWord2(command, i, filename);

        char name[9];
        char ext[4];

        // if filename is too long
        if (parseFileName2(filename, name, ext)) {
            puts(filename, 0x07);
            puts(": filename invalid, name or extension may be too long\n", 0x07);
            return;
        }

        memcpy2(request.name, name, 8);
        memcpy2(request.ext, ext, 3);
        syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);
        if (retcode == 2) {
            puts(filename, 0x07);
            puts(": file not found\n", 0x07);
            return;
        }
        if (retcode == 0 && recursive == -1) {
            puts(filename, 0x07);
            puts(": is a directory;  -r not specified\n", 0x07);
            return;
        }
    }

    for (uint16_t i = 1; i < n_words; i++) {
        if (recursive == i) continue;
        char filename[12];
        getWord2(command, i, filename);

        char name[9];
        char ext[4];

        // if filename is too long
        if (parseFileName2(filename, name, ext)) {
            puts(filename, 0x07);
            puts(": filename invalid, name or extension may be too long\n", 0x07);
            return;
        }

        remove(name, ext, 2); // 2 could be a variable
    }
}


void mkdir(char *command) {
    uint16_t n_words = countWords2(command);
    if (n_words < 2) {
        puts("mkdir: missing operand\n", 0x07);
        return;
    }

    char filename[12];
    getWord2(command, 1, filename);  
    
    char name[9], ext[4];
    if (parseFileName2(filename, name, ext)) {
        puts(filename, 0x07);
        puts(": invalid directory name\n", 0x07);
        return;
    }

    struct FAT32DriverRequest request = {
        .buf = NULL,
        .name = "\0\0\0\0\0\0\0\0",
        .ext = "\0\0\0",
        .buffer_size = 0,
        .parent_cluster_number = shell_state.cur_cluster
    };
    memcpy2(request.name, name, 8);
    memcpy2(request.ext, ext, 3);

    int8_t retcode;
    puts("Name: ", 0x07);
    puts(name, 0x07);
    puts("\nExt: ", 0x07);
    puts(ext, 0x07);
    puts("\n", 0x07);

    puts("Attempting to create directory...\n", 0x07);
    syscall(2, (uint32_t)&request, (uint32_t)&retcode, 0);
    puts("Syscall returned: ", 0x07);
    puts("\n", 0x07);

    if (retcode == 0) {
        puts("Directory created successfully\n", 0x07);
    } else {
        puts("Failed to create directory, error code: ", 0x07);
        puts("\n", 0x07);
    }
}



void mv(char* command) {
    uint16_t n_words = countWords2(command);
    if (n_words != 3) {
        puts("Usage: mv <source> <destination>\n", 0x07);
        return;
    }

    char source_filename[12];
    char destination_filename[12];
    getWord2(command, 1, source_filename);
    getWord2(command, 2, destination_filename);

    char source_name[9];
    char source_ext[4];
    char dest_name[9];
    char dest_ext[4];
    
    if (parseFileName2(source_filename, source_name, source_ext)) {
        puts(source_filename, 0x07);
        puts(": Invalid source file name or extension.\n", 0x07);
        return;
    }
    if (parseFileName2(destination_filename, dest_name, dest_ext)) {
        puts(destination_filename, 0x07);
        puts(": Invalid destination file name or extension.\n", 0x07);
        return;
    }

    uint32_t parent_cluster_number = 2;
    copy(source_name, source_ext, parent_cluster_number, dest_name, dest_ext, parent_cluster_number);

    struct FAT32DriverRequest request = {0};
    int8_t retcode;
    request.buf = NULL;
    memcpy2(request.name, dest_name, 8);
    memcpy2(request.ext, dest_ext, 3);
    request.parent_cluster_number = parent_cluster_number;
    syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);

    if (retcode == 0) { 
        remove(source_name, source_ext, parent_cluster_number);
    } else {
        puts("Move failed: unable to verify copy.\n", 0x07);
    }
}

void findShell(char* command){
    char name[8] = "\0\0\0\0\0\0\0\0", ext[3] = "\0\0\0";
    uint16_t n_words = countWords2(command);
    if(n_words>2){
        puts("find: excess operand\n",0x07);
        return;
    }
    char filename[12];
    getWord2(command,1,filename);
    if (parseFileName2(filename, name, ext)) {
        puts(filename, 0x07);
        puts(": invalid directory name\n", 0x07);
        return;
    }

    syscall(9,(uint32_t)name,(uint32_t)ext,0);
}

void readCluster(int cluster){
    syscall(10,(uint32_t)&cwd_table,cluster,0);
}

void addPath(char *new_add){
    int idx = 0;
    while(new_add[idx]!='\0'){
        shell_state.cur_dir[shell_state.idx] = new_add[idx];
        idx++;
        shell_state.idx++;
    }
    shell_state.cur_dir[shell_state.idx] = '/';
    shell_state.idx++;
}

void removePath(){
    shell_state.idx--;
    shell_state.cur_dir[shell_state.idx] = '\0';
    while(shell_state.cur_dir[shell_state.idx-1]!='/'){
        shell_state.idx--;
        shell_state.cur_dir[shell_state.idx]='\0';
    }
}

void cd(char *command){
    char name[8],ext[3],path[11]="";
    getWord2(command,1,path);

    if(strcmp2(path,"..")){
        if(shell_state.cur_cluster==2){
            puts("Current directory sudah berada di root!\n",0xF);
            return;
        }
        removePath();
        shell_state.cur_cluster = (cwd_table.table[1].cluster_high<<16)|(cwd_table.table[1].cluster_low);
        readCluster(shell_state.cur_cluster);
        return;
    }

    parseFileName2(path,name,ext);
    struct FAT32DriverRequest request = {
        .buf = NULL,
        .name = "\0\0\0\0\0\0\0\0",
        .ext = "\0\0\0",
        .parent_cluster_number = shell_state.cur_cluster,
        .buffer_size = 0
    };
    memcpy2(request.name, name, 8);
    memcpy2(request.ext, ext, 3);

    int32_t cluster_cd;
    syscall(11,(uint32_t)&request,(uint32_t)&cluster_cd,0);
    if(cluster_cd==-9999){
        puts("Masukan path CD anda invalid! Tidak ditemukan path tersebut!",0xF);
        return;
    }
    
    readCluster(cluster_cd);
    shell_state.cur_cluster = cluster_cd;
    addPath(path);
}

int main(void) {
    char name[100] = "\ns0sis@OS-IF2230:";
    readCluster(2);

    syscall(7,0,0,0);

    // mkdir("mkdir haha");
    // cd("cd haha");
    // mkdir("mkdir hihi");
    // findShell("find hihi");

    while(true){
        char command[100] = "";
        int idx = 0;
        char *buf = '\0';

        puts(name, 0xA);
        puts(shell_state.cur_dir, 0x9);
        puts("$ ", 0xF);

        while(*buf!='\n'){
            syscall(4,(uint32_t)buf,0,0);
            if(*buf!='\0' && *buf!='\n'){
                if (*buf == '\b') {  // Handle backspace
                    if (idx > 0) {  // Ensure there's something to delete
                        idx--; 
                        command[idx] = '\0';
                        syscall(5, (uint32_t)"\b \b", 0xF, 0);
                    }
                }
                else
                {
                    command[idx]=*buf;
                    idx++;
                    syscall(5,(uint32_t)buf,0xF,0);
                }
            }
            
        }

        char *cmdtyped = '\0';
        getWord2(command, 0, cmdtyped);
        puts("\n", 2);

        // DO STUFF
        if (strcmp2(cmdtyped, "rm")) {
            rm(command);
            puts("command found", 0x07);
        }
        else if (strcmp2(cmdtyped, "cp")) {
            cp(command);
            puts("command found", 0x07);
        }   
        else if (strcmp2(cmdtyped, "mkdir")) {
            mkdir(command);
            puts("command found", 0x07);
        }
        else if (strcmp2(cmdtyped, "mv")) {
            mv(command);
            puts("command found", 0x07);
        }
        else if (strcmp2(cmdtyped,"find")){
            findShell(command);
        }else if(strcmp2(cmdtyped,"clear")){
            syscall(8,0,0,0);
        }else if(strcmp2(cmdtyped,"cd")){
            cd(command);
        }
        else {
            puts(cmdtyped, 0x07);
            puts(": command not found\n", 0x07);
        }   
    }

    return 0;
}