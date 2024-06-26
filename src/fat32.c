#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/stdlib/string.h"
#include "header/filesystem/fat32.h"

static struct FAT32DriverState fat32_driver_state;

struct ShellState shell_state = {
    .cur_dir = "Root/",
    .idx = 5,
    .cur_cluster = 2,
};

struct FAT32DirectoryTable cwd_table;

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C',
    'o',
    'u',
    'r',
    's',
    'e',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'D',
    'e',
    's',
    'i',
    'g',
    'n',
    'e',
    'd',
    ' ',
    'b',
    'y',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'L',
    'a',
    'b',
    ' ',
    'S',
    'i',
    's',
    't',
    'e',
    'r',
    ' ',
    'I',
    'T',
    'B',
    ' ',
    ' ',
    'M',
    'a',
    'd',
    'e',
    ' ',
    'w',
    'i',
    't',
    'h',
    ' ',
    '<',
    '3',
    ' ',
    ' ',
    ' ',
    ' ',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '2',
    '0',
    '2',
    '4',
    '\n',
    [BLOCK_SIZE - 2] = 'O',
    [BLOCK_SIZE - 1] = 'k',
};

void removePath(){
    shell_state.idx--;
    shell_state.cur_dir[shell_state.idx] = '\0';
    while(shell_state.cur_dir[shell_state.idx-1]!='/'){
        shell_state.idx--;
        shell_state.cur_dir[shell_state.idx]='\0';
    }
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

void update_shell_dir(int cluster,char* path,bool addDir){
    read_clusters(&cwd_table,cluster,1);
    shell_state.cur_cluster = cluster;
    char empty[1] = "";
    if(addDir){
        if(memcmp(path,empty,sizeof(char))==0){
            return;
        }   
        addPath(path);
    }else{
        removePath();
    }
}

int get_shell_cluster(){
    return shell_state.cur_cluster;
}

void copy_dir(char* dir){
    for(int i=0;i<shell_state.idx;i++){
        dir[i] = shell_state.cur_dir[i];
    }
    for(int i=shell_state.idx;i<100;i++){
        dir[i] = '\0';
    }
}

struct FAT32DirectoryTable get_cwd_table(){
    return cwd_table;
}

uint32_t cluster_to_lba(uint32_t cluster)
{
    return cluster * CLUSTER_BLOCK_COUNT;
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    read_blocks(ptr, cluster_number * CLUSTER_BLOCK_COUNT, cluster_count * CLUSTER_BLOCK_COUNT);
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    write_blocks(ptr, cluster_number * CLUSTER_BLOCK_COUNT, cluster_count * CLUSTER_BLOCK_COUNT);
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    for (int i = 0; i < 8; i++)
    {
        dir_table->table[0].name[i] = name[i];
    }
    dir_table->table[1].name[0] = '.';
    dir_table->table[1].name[1] = '.';

    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[1].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;
    dir_table->table[1].user_attribute = UATTR_NOT_EMPTY;

    dir_table->table[1].cluster_high = (uint16_t)(parent_dir_cluster >> 16);
    dir_table->table[1].cluster_low = (uint16_t)(parent_dir_cluster & 0xFFFF);

    dir_table->table[0].filesize = 0;
    dir_table->table[1].filesize = 0;
    for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
    {
        dir_table->table[i].user_attribute = FAT32_FAT_EMPTY_ENTRY;
    }
}

void create_fat32(void)
{
    // Write fs signature to boot sector
    write_blocks(fs_signature, 0, 1);

    // Initialize cluster 0,1,2 in FAT
    fat32_driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fat32_driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    fat32_driver_state.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;
    for (int i = 3; i < CLUSTER_MAP_SIZE; i++)
    {
        // Inisialisasi Kosong
        fat32_driver_state.fat_table.cluster_map[i] = 0;
    }

    struct FAT32DirectoryTable root_dir = {0};
    init_directory_table(&root_dir, "Root\0\0\0\0", ROOT_CLUSTER_NUMBER);
    root_dir.table[0].cluster_high = 0;
    root_dir.table[0].cluster_low = ROOT_CLUSTER_NUMBER;

    write_clusters(&fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
    write_clusters(&root_dir, ROOT_CLUSTER_NUMBER, 1);
}

bool is_empty_storage(void)
{
    struct BlockBuffer boot_sector;
    read_blocks(boot_sector.buf, BOOT_SECTOR, 1);
    return memcmp(boot_sector.buf, fs_signature, BLOCK_SIZE);
}

void initialize_filesystem_fat32(void)
{
    if (is_empty_storage())
    {
        create_fat32();
    }
    else
    {
        read_clusters(&fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
    }
}

int findEntry(struct FAT32DirectoryTable dir_table, char name[8], char ext[3])
{
    int i = 2;
    while (i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)))
    {
        if (memcmp(dir_table.table[i].name, name, 8) == 0 && memcmp(dir_table.table[i].ext, ext, 3) == 0)
        {
            return i;
        }
        i++;
    }
    return -9999;
}

int findCluster(struct FAT32DriverRequest request){
    read_clusters(&fat32_driver_state.dir_table_buf.table, request.parent_cluster_number, 1);
    int idx = findEntry(fat32_driver_state.dir_table_buf, request.name, request.ext);
    if(idx==-9999){
        return 9999;
    }
    if(fat32_driver_state.dir_table_buf.table[idx].attribute!=ATTR_SUBDIRECTORY){
        return 9999;
    }
    return (fat32_driver_state.dir_table_buf.table[idx].cluster_high<<16)|fat32_driver_state.dir_table_buf.table[idx].cluster_low;
}

void find(char name[8],char ext[3],int result[50], int *n_res){
    int queue[100], front=0, back=0;
    queue[0] = 2;
    while(front<=back){
        read_clusters(&fat32_driver_state.dir_table_buf.table, queue[front], 1);
        int idxEntry = findEntry(fat32_driver_state.dir_table_buf,name,ext);

        if(idxEntry!=-9999){
            result[*n_res] = queue[front];
            (*n_res)++;
        }

        for(int i=2;i<(int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry));i++){
            if(fat32_driver_state.dir_table_buf.table[i].user_attribute!=UATTR_NOT_EMPTY){
                continue;
            }

            if(fat32_driver_state.dir_table_buf.table[i].attribute!=ATTR_SUBDIRECTORY){
                continue;
            }

            uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[i].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[i].cluster_low;
            back++;
            queue[back] = cluster_number;
        }
        front++;
    }
}

int8_t read(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf.table, request.parent_cluster_number, 1);
    int idx = findEntry(fat32_driver_state.dir_table_buf, request.name, request.ext);
    if (idx == -9999)
    {
        return 3;
    }
    // Not enough buffer size
    if (request.buffer_size < fat32_driver_state.dir_table_buf.table[idx].filesize)
    {
        return 2;
    }
    // Is a Folder, not a file
    if (fat32_driver_state.dir_table_buf.table[idx].attribute == ATTR_SUBDIRECTORY)
    {
        return 1;
    }

    uint32_t cluster_number = (fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16) | fat32_driver_state.dir_table_buf.table[idx].cluster_low;
    struct ClusterBuffer* buffer = request.buf;
    while (cluster_number != FAT32_FAT_END_OF_FILE)
    {
        read_clusters(buffer, cluster_number, 1);
        buffer++;
        cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
    }
    return 0;
}


int8_t read_directory(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    int idx = findEntry(fat32_driver_state.dir_table_buf, request.name, request.ext);
    // Not Found
    if (idx == -9999)
    {
        return 2;
    }
    // Is a file, not a folder
    if (fat32_driver_state.dir_table_buf.table[idx].attribute != ATTR_SUBDIRECTORY)
    {
        return 1;
    }
    uint32_t entry = (fat32_driver_state.dir_table_buf.table[idx].cluster_high << 16) | (fat32_driver_state.dir_table_buf.table[idx].cluster_low);
    read_clusters(request.buf, entry, 1);
    return 0;
}

int ceil(double x)
{
    int intPart = (int)x;
    if (x > (double)intPart)
    {
        intPart++;
    }
    return intPart;
}

int findEmptySpace(struct FAT32FileAllocationTable *fat)
{
    for (int i = 0; i < CLUSTER_MAP_SIZE; i++)
    {
        if (fat->cluster_map[i] == 0)
        {
            return i;
        }
    }
    return -9999;
}

int findIdxEmptyEntry(struct FAT32DirectoryTable *dir)
{
    for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
    {
        if (dir->table[i].user_attribute != UATTR_NOT_EMPTY)
        {
            return i;
        }
    }
    return -9999;
}

void addEntry(struct FAT32DirectoryTable *parent_dir, struct FAT32DriverRequest request, int idxEntry, int cluster_idx)
{
    memcpy(parent_dir->table[idxEntry].name, request.name, 8 * sizeof(char));
    memcpy(parent_dir->table[idxEntry].ext, request.ext, 3 * sizeof(char));
    parent_dir->table[idxEntry].user_attribute = UATTR_NOT_EMPTY;
    if (request.buffer_size == 0)
    {
        parent_dir->table[idxEntry].attribute = ATTR_SUBDIRECTORY;
    }
    parent_dir->table[idxEntry].cluster_high = (uint16_t)(cluster_idx >> 16);
    parent_dir->table[idxEntry].cluster_low = (uint16_t)(cluster_idx & 0xFFFF);
    parent_dir->table[idxEntry].filesize = request.buffer_size;
}

int8_t write(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    // Jika folder request tidak valid
    if (fat32_driver_state.dir_table_buf.table[0].attribute != ATTR_SUBDIRECTORY)
    {
        return 2;
    }
    int idxFound = findEntry(fat32_driver_state.dir_table_buf, request.name, request.ext);
    // Jika udah ada folder/file dengan nama dan ext sama
    if (idxFound != -9999)
    {
        return 1;
    }
    int idxEntryParent = findIdxEmptyEntry(&fat32_driver_state.dir_table_buf);
    // Jika directory table parent udah penuh
    if (idxEntryParent == -9999)
    {
        return -1;
    }
    // Jika FAT sudah penuh
    int idx = findEmptySpace(&fat32_driver_state.fat_table);
    if (idx == -9999)
    {
        return -1;
    }
    uint8_t* ptr = (uint8_t*)request.buf;
    addEntry(&fat32_driver_state.dir_table_buf, request, idxEntryParent, idx);
    write_clusters(&fat32_driver_state.dir_table_buf.table, request.parent_cluster_number, 1);
    // Jika yang dibuat adalah folder
    if (request.buffer_size == 0)
    {
        struct FAT32DirectoryTable new_dir = {0};
        init_directory_table(&new_dir, request.name, request.parent_cluster_number);
        write_clusters(&new_dir, idx, 1);

        fat32_driver_state.fat_table.cluster_map[idx] = FAT32_FAT_END_OF_FILE;
        write_clusters(&fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
    }
    else
    {
        int total_cluster = ceil((double)request.buffer_size / CLUSTER_SIZE);
        int prev = -9999;
        idx = -9999;
        for (int i = 0; i < total_cluster; i++)
        {
            prev = idx;
            idx = findEmptySpace(&fat32_driver_state.fat_table);
            if (idx == -9999)
            {
                fat32_driver_state.fat_table.cluster_map[prev] = FAT32_FAT_END_OF_FILE;
                return -1;
            }
            else
            {
                write_clusters(ptr + (i * CLUSTER_SIZE), (uint32_t)idx, 1);
                if (prev != -9999)
                {
                    fat32_driver_state.fat_table.cluster_map[prev] = (uint32_t)(idx);
                }
                fat32_driver_state.fat_table.cluster_map[idx] = FAT32_FAT_END_OF_FILE;
            }
        }
        write_clusters(&fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
    }
    return 0;
}

bool isEmptyDir(struct FAT32DirectoryTable dir)
{
    for (int i = 2; i < (int)(CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
    {
        if (dir.table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            return false;
        }
    }
    return true;
}

void deleteEntry(struct FAT32DirectoryTable *parent_dir, int idxEntry)
{
    memcpy(parent_dir->table[idxEntry].name, "\0\0\0\0\0\0\0\0", 8 * sizeof(char));
    memcpy(parent_dir->table[idxEntry].ext, "\0\0\0", 3 * sizeof(char));
    parent_dir->table[idxEntry].user_attribute = FAT32_FAT_EMPTY_ENTRY;

    parent_dir->table[idxEntry].cluster_high = 0;
    parent_dir->table[idxEntry].cluster_low = 0;
    parent_dir->table[idxEntry].filesize = 0;
}

int8_t delete(struct FAT32DriverRequest request)
{
    read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
    int idxEntry = findEntry(fat32_driver_state.dir_table_buf, request.name, request.ext);
    if (idxEntry == -9999)
    {
        return 1;
    }
    uint32_t idxCluster = (fat32_driver_state.dir_table_buf.table[idxEntry].cluster_high << 16) | (fat32_driver_state.dir_table_buf.table[idxEntry].cluster_low);
    if (fat32_driver_state.dir_table_buf.table[idxEntry].attribute == ATTR_SUBDIRECTORY)
    {
        struct FAT32DirectoryTable dirChild = {0};
        read_clusters(&dirChild, idxCluster, 1);
        if (!isEmptyDir(dirChild))
        {
            return 2;
        }
        struct FAT32DirectoryTable emptyDir = {0};
        write_clusters(&emptyDir, idxCluster, 1);
        deleteEntry(&fat32_driver_state.dir_table_buf, idxEntry);
        fat32_driver_state.fat_table.cluster_map[idxCluster] = 0;
    }
    else
    {
        deleteEntry(&fat32_driver_state.dir_table_buf, idxEntry);

        uint8_t emptyCluster[CLUSTER_SIZE] = {[0 ... CLUSTER_SIZE - 1] = 0};
        while (fat32_driver_state.fat_table.cluster_map[idxCluster] != FAT32_FAT_END_OF_FILE)
        {
            uint32_t temp = idxCluster;
            idxCluster = fat32_driver_state.fat_table.cluster_map[idxCluster];
            fat32_driver_state.fat_table.cluster_map[temp] = 0;
            write_clusters(&emptyCluster, temp, 1);
        }
        write_clusters(&emptyCluster, idxCluster, 1);
        fat32_driver_state.fat_table.cluster_map[idxCluster] = 0;
    }
    write_clusters(&fat32_driver_state.dir_table_buf.table, request.parent_cluster_number, 1);
    write_clusters(&fat32_driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);
    return 0;
}