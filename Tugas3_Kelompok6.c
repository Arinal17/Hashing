#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 101

// Struktur Node untuk Linked List (Separate Chaining)
typedef struct Node {
    char ktm[50];
    char nama[500];
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

// ==============================================================
// FUNGSI HASH
// ==============================================================
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
        hash = hash ^ (hash >> 8);
        
        posisi++;
        ptr++;
    }
    
    return hash % SIZE;
}
// ==============================================================

// Memasukkan data ke hash table
void insert(char* ktm, char* nama) {
    int indeks = hitungHash(ktm);
    
    // Cek duplikat
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
        printf("[Error] Alokasi memori gagal!\n");
        return;
    }
    
    strncpy(newNode->ktm, ktm, sizeof(newNode->ktm) - 1);
    newNode->ktm[sizeof(newNode->ktm) - 1] = '\0';
    
    strncpy(newNode->nama, nama, sizeof(newNode->nama) - 1);
    newNode->nama[sizeof(newNode->nama) - 1] = '\0';
    
    // Insert di HEAD
    newNode->next = hashTable[indeks];
    hashTable[indeks] = newNode;
}

// ==============================================================
// BACA FILE
// ==============================================================
void bacaFile() {
    FILE* file = fopen("Data_Latih.txt", "r");
    if (file == NULL) {
        printf("[Error] File Data_Latih.txt tidak ditemukan!\n");
        return;
    }
    
    char tempKtm[50];
    char tempNama[500];
    int jumlahData = 0;
    
    char line[600];
    while (fgets(line, sizeof(line), file)) {
        // Hapus newline (support Windows \r\n dan Unix \n)
        line[strcspn(line, "\r\n")] = '\0';
        
        if (line[0] == '\0') continue;
        
        char* comma = strchr(line, ',');
        if (comma == NULL) continue;
        
        int ktmLen = comma - line;
        strncpy(tempKtm, line, ktmLen);
        tempKtm[ktmLen] = '\0';
        
        strcpy(tempNama, comma + 1);
        
        insert(tempKtm, tempNama);
        jumlahData++;
    }
    
    fclose(file);
    printf("[INFO] Sukses memuat %d data dari Data_Latih.txt\n\n", jumlahData);
}
// ==============================================================

// ==============================================================
// MENGHITUNG STATISTIK (DENGAN RUMUS DINAMIS)
// ==============================================================
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
    
    // ==========================================================
    // RUMUS SCORE DINAMIS (Bisa untuk berapa pun jumlah data)
    // ==========================================================
    double score;
    
    if (totalData == 0) {
        score = 0;
    } else {
        // Target collision ideal = totalData - indeksTerisi
        int targetCollision = totalData - indeksTerisi;
        
        // Score = (indeksTerisi/101) × (1 - |collision - target|/totalData) × 100%
        double rasioSebaran = (double)indeksTerisi / 101;
        double rasioTabrakan = 1 - (double)abs(totalCollision - targetCollision) / totalData;
        
        score = rasioSebaran * rasioTabrakan * 100;
        
        if (score < 0) score = 0;
    }
    // ==========================================================
    
    printf("==================================================\n");
    printf("Total Data                   : %d\n", totalData);
    printf("Total Tabrakan (Collision)   : %d Kali\n", totalCollision);
    printf("Jumlah Indeks Laci Terisi    : %d dari 101 Slot\n", indeksTerisi);
    printf("Collision Ideal (Target)     : %d\n", totalData - indeksTerisi);
    printf("Nilai Score                  : %.2f %%\n", score);
    printf("==================================================\n");
    
    // Verifikasi
    if (totalData == indeksTerisi + totalCollision) {
        printf("informasi: %d data = %d terisi + %d collision\n", totalData, indeksTerisi, totalCollision);
    }
}
// ==============================================================

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