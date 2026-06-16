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
    
    char tempKtm[15];   // Pas sesuai ukuran array node asli
    char tempNama[100]; // Pas sesuai ukuran array node asli
    int jumlahData = 0;
    int gagalData = 0;
    
    while (!feof(file)) {
        // Membatasi fscanf agar hanya membaca maksimal 14 karakter KTM dan 99 karakter Nama
        int hasilKecocokan = fscanf(file, " %14[^,],%99[^\r\n]", tempKtm, tempNama);
        
        // Pembilas Buffer: Bersihkan sisa karakter/newline yang terpotong di baris aktif saat ini
        int c;
        int karakterTersisa = 0;
        while ((c = fgetc(file)) != '\n' && c != EOF) {
            karakterTersisa++; // Deteksi apakah ada sisa karakter yang terpotong di baris ini
        }
        
        if (hasilKecocokan == 2) {
            // Jika ada karakter tersisa di buffer, berarti data asli di file kepanjangan dan terpotong
            if (karakterTersisa > 0) {
                gagalData++;
                continue; // Skip, langsung lanjut ke data berikutnya tanpa di-insert
            }
            
            insert(tempKtm, tempNama);
            jumlahData++;
        } 
        else {
            // Jika baris rusak atau acak yang tidak memiliki format koma (,)
            if (!feof(file)) {
                gagalData++;
            }
        }
    }
    
    fclose(file);
    
    // Proteksi batas maksimal kuota: jika total data sukses menembus target 500, sisanya masuk gagal
    if (jumlahData > 500) {
        gagalData += (jumlahData - 500);
        jumlahData = 500;
    }
    
    printf("Sukses memuat %d data dari Data_Latih.txt\n", jumlahData);
    printf("Gagal memuat %d data (tidak sesuai format/data lebih dari target)\n\n", gagalData);
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
    
    double score = (totalData > 0) ? ((double)indeksTerisi / SIZE) * (1.0 - ((double)abs(totalCollision - (totalData - indeksTerisi)) / totalData)) * 100 : 0;
    
    
    printf("==================================================\n");
    printf("Total Data                   : %d\n", totalData);
    printf("Total Tabrakan (Collision)   : %d Kali\n", totalCollision);
    printf("Jumlah Indeks Laci Terisi    : %d dari 101 Slot\n", indeksTerisi);
    printf("Nilai Score                  : %.2f %%\n", score);
    printf("==================================================\n");
    
    if (totalData == indeksTerisi + totalCollision) {
        printf("informasi: %d data = %d terisi + %d collision\n", totalData, indeksTerisi, totalCollision);
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
    printf("==================================================\n");
    printf("   SISTEM VERIFIKASI e-KTM PERPUSTAKAAN KAMPUS\n");
    printf("==================================================\n");
    printf("Kapasitas Array : %d slot\n", SIZE);
    printf("Metode Handling : Separate Chaining (Linked List)\n");
    printf("==================================================\n\n");
    
    inisialisasiTable();
    bacaFile();
    hitungMetrik();
    bebaskanMemori();
    
    return 0;
}
