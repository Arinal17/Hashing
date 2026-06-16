#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 101

typedef struct Node {
    char ktm[20];
    char nama[100];
    struct Node* next;
} Node;

Node* hashTable[SIZE];

void inisialisasiTable() {
    for (int i = 0; i < SIZE; i++) {
        hashTable[i] = NULL;
    }
}

int hitungHash(char* ktm) {
    unsigned long hash = 0;
    int posisi = 1;
    
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

void insert(char* ktm, char* nama) {
    int indeks = hitungHash(ktm);
    
    Node* curr = hashTable[indeks];
    while (curr != NULL) {
        if (strcmp(curr->ktm, ktm) == 0) {
            strcpy(curr->nama, nama);
            return;
        }
        curr = curr->next;
    }
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("[Error] Alokasi memori gagal!\n");
        return;
    }
    
    strcpy(newNode->ktm, ktm);
    strcpy(newNode->nama, nama);
    
    newNode->next = hashTable[indeks];
    hashTable[indeks] = newNode;
}

void bacaFile() {
    FILE* file = fopen("Data_Latih.txt", "r");
    if (file == NULL) {
        printf("[Error] File Data_Latih.txt tidak ditemukan!\n");
        return;
    }
    
    char tempKtm[20];
    char tempNama[100];
    int jumlahData = 0;
    
    // Baca per baris dengan fgets
    char line[200];
    while (fgets(line, sizeof(line), file)) {
        // Hapus newline di akhir
        line[strcspn(line, "\n")] = '\0';
        
        // Cari koma
        char* comma = strchr(line, ',');
        if (comma == NULL) continue;
        
        // Ambil KTM (sebelum koma)
        int i = 0;
        while (line + i < comma && i < 19) {
            tempKtm[i] = line[i];
            i++;
        }
        tempKtm[i] = '\0';
        
        // Ambil Nama (setelah koma)
        int j = 0;
        char* namaPtr = comma + 1;
        while (*namaPtr != '\0' && j < 99) {
            tempNama[j] = *namaPtr;
            j++;
            namaPtr++;
        }
        tempNama[j] = '\0';
        
        insert(tempKtm, tempNama);
        jumlahData++;
    }
    
    fclose(file);
    printf(" Sukses memuat %d data dari Data_Latih.txt\n\n", jumlahData);
}

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
    printf("Total Data                   : %d\n", totalData);
    printf("Total Tabrakan (Collision)   : %d Kali\n", totalCollision);
    printf("Jumlah Indeks Laci Terisi    : %d dari 101 Slot\n", indeksTerisi);
    printf("Nilai Score                  : %.2f %%\n", score);
    printf("==================================================\n");
    
    if (totalData == indeksTerisi + totalCollision) {
        printf("[VALID] %d data = %d terisi + %d collision\n", totalData, indeksTerisi, totalCollision);
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
