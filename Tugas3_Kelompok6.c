#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 101

// Struktur Node untuk Linked List (Separate Chaining)
typedef struct Node {
    char ktm[15];
    char nama[100];
    struct Node* next;
} Node;

// Array Hash Table
Node* hashTable[SIZE];

// Inisialisasi hash table
void inisialisasiTable() {
    for (int i = 0; i < SIZE; i++) {
        hashTable[i] = NULL;
    }
}

int hitungHash(char* ktm) {
    unsigned long hash = 0;
    int posisi = 1;
    
    // Abaikan prefix "KTM-" karena seragam
    char* ptr = ktm;
    if (strncmp(ktm, "KTM-", 4) == 0) {
        ptr = ktm + 4;
    }
    
    while (*ptr != '\0') {
        int ascii = *ptr;
        
        hash = hash * 131 + (ascii * posisi * posisi * posisi);
        
        // Operasi XOR untuk menambah keacakan
        hash = hash ^ (hash >> 8);
        
        posisi++;
        ptr++;
    }
    
    // Wajib modulo dengan ukuran array (101)
    return hash % SIZE;
}

void insert(char* ktm, char* nama) {
    int indeks = hitungHash(ktm);
    
    // Jika KTM sudah ada, update namanya lalu keluar
    Node* curr = hashTable[indeks];
    while (curr != NULL) {
        if (strcmp(curr->ktm, ktm) == 0) {
            strncpy(curr->nama, nama, sizeof(curr->nama) - 1);
            curr->nama[sizeof(curr->nama) - 1] = '\0';
            return; 
        }
        curr = curr->next;
    }
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Alokasi memori gagal!\n");
        return;
    }
    
    // Batasi penyalinan string agar tidak overflow
    strncpy(newNode->ktm, ktm, sizeof(newNode->ktm) - 1);
    newNode->ktm[sizeof(newNode->ktm) - 1] = '\0';
    
    strncpy(newNode->nama, nama, sizeof(newNode->nama) - 1);
    newNode->nama[sizeof(newNode->nama) - 1] = '\0';
    
    // Insert di HEAD
    newNode->next = hashTable[indeks];
    hashTable[indeks] = newNode;
}

// Membaca file data latih
void bacaFile() {
    FILE* file = fopen("Data_Latih.txt", "r");
    if (file == NULL) {
        printf("File Data_Latih.txt tidak ditemukan!\n");
        return;
    }
    
    char tempKtm[20];
    char tempNama[150];
    int jumlahData = 0;
    
    // Gunakan feof agar program terus membaca sampai isi file benar-benar habis
    while (!feof(file)) {
        // Coba baca format KTM,Nama
        int hasilKecocokan = fscanf(file, " %14[^,],%99[^\r\n]", tempKtm, tempNama);
        
        // Bersihkan sisa karakter/newline di baris aktif saat ini
        int c;
        while ((c = fgetc(file)) != '\n' && c != EOF);
        
        // Jika format cocok (bernilai 2), masukkan ke hash table
        if (hasilKecocokan == 2) {
            insert(tempKtm, tempNama);
            jumlahData++;
        }
        // Jika format rusak (seperti teks source), loop akan skip baris itu dan lanjut ke baris bawahnya
    }
    
    fclose(file);
    printf("Sukses memuat %d data dari Data_Latih.txt\n\n", jumlahData);
}

// Menghitung statistik
void hitungMetrik() {
    int indeksTerisi = 0;
    int totalCollision = 0;
    int totalData = 0;
    
    for (int i = 0; i < SIZE; i++) {
        int count = 0;
        Node* curr = hashTable[i];
        
        while (curr != NULL) {
            count++;
            curr = curr->next;
        }
        
        if (count > 0) {
            indeksTerisi++;
            totalCollision += (count - 1);
            totalData += count;
        }
    }
    

    double score = ((double)indeksTerisi / 101) * 
                   (1 - (double)abs(totalCollision - 399) / 500) * 100;
    
    if (score < 0) score = 0;
    
    printf("==================================================\n");
    printf("Total Tabrakan (Collision)   : %d Kali\n", totalCollision);
    printf("Jumlah Indeks Laci Terisi    : %d dari 101 Slot\n", indeksTerisi);
    printf("Nilai Score                  : %.2f %%\n", score);
    printf("==================================================\n");
    
    // Verifikasi
    if (totalData == indeksTerisi + totalCollision) {
        printf("%d data = %d terisi + %d collision\n", totalData, indeksTerisi, totalCollision);
    }
}

// Membebaskan memori
void bebaskanMemori() {
    for (int i = 0; i < SIZE; i++) {
        Node* curr = hashTable[i];
        while (curr != NULL) {
            Node* temp = curr;
            curr = curr->next;
            free(temp);
        }
        hashTable[i] = NULL;
    }
}

int main() {
    inisialisasiTable();
    bacaFile();
    hitungMetrik();
    bebaskanMemori();
    
    return 0;
}
