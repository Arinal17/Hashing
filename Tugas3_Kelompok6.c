#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 101

typedef struct Node {
    char ktm[15];
    char nama[100];
    struct Node* next;
} Node;

Node* hashTable[SIZE];

void inisialisasiTable() {
    for (int i = 0; i < SIZE; i++) {
        hashTable[i] = NULL;
    }
}

// ========== FUNGSI HASH YANG DIPERKUAT ==========
int hitungHash(char* ktm) {
    unsigned long hash1 = 5381;  // Nilai awal (DJB2)
    unsigned long hash2 = 0;
    int posisi = 1;
    
    // Abaikan "KTM-" karena seragam
    char* ptr = ktm;
    if (strncmp(ktm, "KTM-", 4) == 0) {
        ptr = ktm + 4;
    }
    
    while (*ptr != '\0') {
        int ascii = *ptr;
        
        // Teknik 1: DJB2 (hash * 33 + ascii) dengan XOR
        hash1 = ((hash1 << 5) + hash1) ^ ascii;
        
        // Teknik 2: Bobot posisi kuadrat
        hash2 += (ascii * posisi * posisi);
        
        // Teknik 3: XOR dengan shifting berdasarkan posisi
        if (posisi % 2 == 0) {
            hash1 ^= (ascii << 2);
        } else {
            hash1 ^= (ascii >> 1);
        }
        
        // Teknik 4: Kombinasi hash2 setiap 2 karakter
        if (posisi % 2 == 0) {
            hash1 ^= hash2;
        }
        
        ptr++;
        posisi++;
    }
    
    // Kombinasi final
    unsigned long final_hash = hash1 ^ (hash2 << 4);
    
    return final_hash % SIZE;
}
// =================================================

void insert(char* ktm, char* nama) {
    int indeks = hitungHash(ktm);

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("[Error] Alokasi memori gagal!\n");
        return;
    }
    strcpy(newNode->ktm, ktm);
    strcpy(newNode->nama, nama);
    newNode->next = NULL;

    if (hashTable[indeks] == NULL) {
        hashTable[indeks] = newNode;
    } else {
        // Insert di HEAD (lebih cepat O(1))
        newNode->next = hashTable[indeks];
        hashTable[indeks] = newNode;
    }
}

void bacaFile() {
    FILE* file = fopen("Data_Latih.txt", "r");
    if (file == NULL) {
        printf("[Error] File Data_Latih.txt tidak ditemukan!\n");
        printf("Pastikan file berada di direktori yang sama.\n");
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
    printf("==================================================\n");
    printf("[INFO] Sukses memuat %d data dari Data_Latih.txt\n", jumlahData);
    printf("==================================================\n\n");
}

// ========== FUNGSI HITUNG METRIK YANG DIPERBAIKI ==========
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

    // RUMUS YANG BENAR (sesuai soal)
    double skorAkhir = ((double)indeksTerisi / 101) * 
                        (1 - (double)abs(totalCollision - 399) / 500) * 100;

    if (skorAkhir < 0) skorAkhir = 0.0;

    printf("--- LAPORAN METRIK HASIL DISTRIBUSI HASH TABLE ---\n");
    printf("1. Jumlah Indeks Terisi : %d / 101 slot (Target: 101)\n", indeksTerisi);
    printf("2. Total Data Tabrakan  : %d kali  (Target: 399)\n", totalCollision);
    printf("--------------------------------------------------\n");
    printf(">> SKOR AKHIR PERFORMA  : %.2f %% (Target: 100.00%%)\n", skorAkhir);
    printf("--------------------------------------------------\n");
    
    // Verifikasi
    if (totalData == indeksTerisi + totalCollision) {
        printf("[VALID] %d data = %d terisi + %d collision \n", totalData, indeksTerisi, totalCollision);
    }
}

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