#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 101

// ==============================================================
// STRUKTUR NODE (UKURAN EFISIEN)
// ==============================================================
typedef struct Node {
    char ktm[30];       // 30 byte (cukup untuk KTM-XXXx)
    char nama[100];    
    struct Node* next;
} Node;
// ==============================================================

Node* hashTable[SIZE];

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

void insert(char* ktm, char* nama) {
    int indeks = hitungHash(ktm);
    
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
    
    newNode->next = hashTable[indeks];
    hashTable[indeks] = newNode;
}

void bacaFile() {
    FILE* file = fopen("Data_Latih.txt", "r");
    if (file == NULL) {
        printf("File Data_Latih.txt tidak ditemukan!\n");
        return;
    }
    
    char tempKtm[30];
    char tempNama[200];
    int jumlahData = 0;
    
    char line[250];
    while (fgets(line, sizeof(line), file)) {
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
    printf("Sukses memuat %d data dari Data_Latih.txt\n\n", jumlahData);
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
    
    double score;
    if (totalData == 0) {
        score = 0;
    } else {
        int targetCollision = totalData - indeksTerisi;
        double rasioSebaran = (double)indeksTerisi / 101;
        double rasioTabrakan = 1 - (double)abs(totalCollision - targetCollision) / totalData;
        score = rasioSebaran * rasioTabrakan * 100;
        if (score < 0) score = 0;
    }
    
    printf("==================================================\n");
    printf("Total Data                   : %d\n", totalData);
    printf("Total Tabrakan (Collision)   : %d Kali\n", totalCollision);
    printf("Jumlah Indeks Laci Terisi    : %d dari 101 Slot\n", indeksTerisi);
    printf("Collision Ideal (Target)     : %d\n", totalData - indeksTerisi);
    printf("Nilai Score                  : %.2f %%\n", score);
    printf("==================================================\n");
    
    if (totalData == indeksTerisi + totalCollision) {
        printf("info %d data = %d terisi + %d collision\n", totalData, indeksTerisi, totalCollision);
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