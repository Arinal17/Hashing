#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 101

typedef struct Node { // Struktur Node untuk Linked List 
    char ktm[15];
    char nama[100];
    struct Node* next;
} Node;

Node* hashTable[SIZE]; // Array Hash Table

void inisialisasiTable() {  // Inisialisasi hash table
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
    
        hash = hash ^ (hash >> 8); // Operasi XOR untuk menambah keacakan
        posisi++;
        ptr++;
    }
    
    return hash % SIZE;     // modulo dengan ukuran array (101)
}

int insert(char* ktm, char* nama) {
    int indeks = hitungHash(ktm);
    
    Node* curr = hashTable[indeks];
    while (curr != NULL) {
        if (strcmp(curr->ktm, ktm) == 0) {
            strncpy(curr->nama, nama, sizeof(curr->nama) - 1);
            curr->nama[sizeof(curr->nama) - 1] = '\0';
            return 0;
        }
        curr = curr->next;
    }
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Alokasi memori gagal!\n");
        return 0;
    }
    
    strncpy(newNode->ktm, ktm, sizeof(newNode->ktm) - 1);
    newNode->ktm[sizeof(newNode->ktm) - 1] = '\0';
    
    strncpy(newNode->nama, nama, sizeof(newNode->nama) - 1);
    newNode->nama[sizeof(newNode->nama) - 1] = '\0';
    
    newNode->next = hashTable[indeks];
    hashTable[indeks] = newNode;
    
    return 1;
}

void bacaFile() {
    FILE* file = fopen("Data_Latih.txt", "r");
    if (file == NULL) {
        printf("File Data_Latih.txt tidak ditemukan!\n");
        return;
    }
    
    char tempKtm[15];   
    char tempNama[100]; 
    int jumlahData = 0;
    int gagalData = 0;
    
    while (!feof(file)) {
        int hasilKecocokan = fscanf(file, " %14[^,],%99[^\r\n]", tempKtm, tempNama);
        
        int c;
        int karakterTersisa = 0;
        while ((c = fgetc(file)) != '\n' && c != EOF) {
            karakterTersisa++; 
        }
        
        if (hasilKecocokan == 2) {
            if (karakterTersisa > 0) {
                gagalData++;
                continue; 
            }
            
            // Menggunakan hasil kembalian insert()
            if (insert(tempKtm, tempNama) == 1) {
                jumlahData++; //jumlahData hanya bertambah jika data tersebut bukan duplikat
            } else {
                gagalData++; // Duplikasi KTM dihitung sebagai gagal/ditolak masuk data baru
            }
        } 
        else {
            if (!feof(file)) {
                gagalData++;
            }
        }
    }
    
    fclose(file);
    
    if (jumlahData > 500) {
        gagalData += (jumlahData - 500);
        jumlahData = 500;
    }
    
    printf("Sukses memuat %d data dari Data_Latih.txt\n", jumlahData);
    printf("Gagal memuat %d data (tidak sesuai format/data lebih dari target)\n\n", gagalData);
}

void hitungMetrik() { // Menghitung statistik
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
    printf("Jumlah Indeks Laci Terisi    : %d dari %d Slot\n", indeksTerisi, SIZE);
    printf("Nilai Score                  : %.2f %%\n", score);
    printf("==================================================\n");
    
    if (totalData == indeksTerisi + totalCollision) {
        printf("informasi: %d data = %d terisi + %d collision\n", totalData, indeksTerisi, totalCollision);
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
