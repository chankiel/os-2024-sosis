<h1 align="middle"> SOSIS</h1>  
<h2 align="middle"> Simple Operating System using QEMU with C Language </h1>

<img src="image.jpg">

## How to Run
1. First, run build using makefile to insert storage bin and compile the OS  
```
make build
```
2. Then, run the OS using this command  
```
make run
```
3. If you want to use the previous saved state of the OS, just run `make run` the next time you want to use the OS. If you want to reset the storage bin, run `make build` first.

                                        
## Contributors
|   NIM    |                  Nama                  |
| -------- | -------------------------------------- |
| 13522029 |       Ignatius Jhon Hezkiel Chan       |
| 13522043 |       Daniel Mulia Putra Manurung      |
| 13522053 |       Erdianti Wiga Putri Andini       |
| 13522093 |       Matthew Vladimir Hutabarat       |

## Checkpoint
| Tugas                            | Deskripsi                                                                             | Checklist |
| -------------------------------- | ------------------------------------------------------------------------------------- | --------- |
| Text Framebuffer                 | Membuat fungsi untuk menulis pada layar                                               |     V     |
|                                  | Membuat fungsi untuk membersihkan layar                                               |     V     |
|                                  | Membuat fungsi untuk menggerakkan kursor                                              |     V     |
| Interrupt                        | Membuat IDT                                                                           |     V     |
|                                  | Melakukan PIC Remapping                                                               |     V     |
|                                  | Membuat Interrupt Service Routine                                                     |     V     |
|                                  | Memuat IDT pada CPU                                                                   |     V     |
| Keyboard Driver                  | Menyalakan IRQ Keyboard                                                               |     V     |
|                                  | Membuat driver untuk menerima dan menerjemahkan input keyboard                        |     V     |
| Filesystem                       | Menambahkan hard drive pada virtual machine                                           |     V     |
|                                  | Membuat driver untuk I/O pada hard drive                                              |     V     |
|                                  | Membuat struktur data untuk file system FAT32                                         |     V     |
|                                  | Membuat operasi write pada filesystem                                                 |     V     |
|                                  | Membuat operasi read pada filesystem                                                  |     V     |
|                                  | Membuat operasi delete pada filesystem                                                |     V     |
| Manajemen memory                 | Membuat struktur data paging                                                          |     V     |
|                                  | Memuat kernel pada alamat memori tinggi                                               |     V     |
|                                  | Mengaktifkan paging                                                                   |     V     |
| Separasi kernel-user space       | Membuat inserter file ke file system                                                  |     V     |
|                                  | Membuat GDT entry untuk user dan Task State Segment                                   |     V     |
|                                  | Membuat alokator memori sederhana                                                     |     V     |     
|                                  | Membuat aplikasi user sederhana                                                       |     V     | 
|                                  | Memasuki user mode pada aplikasi sederhana                                            |     V     |  
| Shell                            | Membuat system call                                                                   |     V     |
|                                  | Mengembangkan aplikasi sederhana menjadi shell                                        |     V     |
|                                  | Membuat perintah - perintah pada shell                                                |     V     |
| Menyiapkan struktur untuk proses | Membuat struct Process Control Block (PCB)                                            |     V     |
|                                  | Membuat struktur data yang dibutuhkan proses                                          |     V     |
| Task scheduler & context switch  | Membuat algoritma penjadwalan task yang ada                                           |     V     |
|                                  | Membuat mekanisme perpindahan konteks                                                 |     V     |
| Membuat perintah shell tambahan  | exec                                                                                  |     V     |
|                                  | ps                                                                                    |     V     |
|                                  | kill                                                                                  |     V     |
| Menjalankan Multitasking         |Menjalankan program yang dapat menggambarkan konkurensi telah berjalan                 |     V     |
