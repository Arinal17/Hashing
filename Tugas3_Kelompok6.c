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

// ==============================================================
// FUNGSI HASH YANG DIPERBAIKI (UNTUK MENCAPAI TARGET 100%)
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
        
        // Menggunakan posisi pangkat 3 untuk variasi maksimal
        // dan dikalikan dengan bilangan prima 131
        hash = hash * 131 + (ascii * posisi * posisi * posisi);
        
        // Operasi XOR untuk menambah keacakan
        hash = hash ^ (hash >> 8);
        
        posisi++;
        ptr++;
    }
    
    // Wajib modulo dengan ukuran array (101)
    return hash % SIZE;
}
// ==============================================================

// Memasukkan data ke hash table (Separate Chaining - Insert di Head)
void insert(char* ktm, char* nama) {
    int indeks = hitungHash(ktm);
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("[Error] Alokasi memori gagal!\n");
        return;
    }
    
    strcpy(newNode->ktm, ktm);
    strcpy(newNode->nama, nama);
    
    // Insert di HEAD (lebih efisien)
    newNode->next = hashTable[indeks];
    hashTable[indeks] = newNode;
}

// Membaca file data latih
void bacaFile() {
    FILE* file = fopen("Data_Latih.txt", "r");
    if (file == NULL) {
        printf("[Error] File Data_Latih.txt tidak ditemukan!\n");
        return;
    }
    
    char tempKtm[15];
    char tempNama[100];
    int jumlahData = 0;
    
    while (fscanf(file, " %[^,],%[^\n]\n", tempKtm, tempNama) != EOF) {
        insert(tempKtm, tempNama);
        jumlahData++;
    }
    
    fclose(file);
    printf("[INFO] Sukses memuat %d data dari Data_Latih.txt\n\n", jumlahData);
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
    
    // Rumus score sesuai soal
    double score = ((double)indeksTerisi / 101) * 
                   (1 - (double)abs(totalCollision - 399) / 500) * 100;
    
    if (score < 0) score = 0;
    
    // OUTPUT SESUAI YANG DIMINTA SOAL
    printf("==================================================\n");
    printf("Total Tabrakan (Collision)   : %d Kali\n", totalCollision);
    printf("Jumlah Indeks Laci Terisi    : %d dari 101 Slot\n", indeksTerisi);
    printf("Nilai Score                  : %.2f %%\n", score);
    printf("==================================================\n");
    
    // Verifikasi
    if (totalData == indeksTerisi + totalCollision) {
        printf("[VALID] %d data = %d terisi + %d collision\n", totalData, indeksTerisi, totalCollision);
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