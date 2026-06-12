#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 101 // Kapasitas maksimum Hash Table sesuai ketentuan tugas

// Struktur Node untuk Linked List (Separate Chaining)
typedef struct Node {
    char ktm[15];       // Menyimpan token unik e-KTM (Contoh: KTM-001A)
    char nama[100];     // Menyimpan nama lengkap mahasiswa
    struct Node* next;  // Pointer ke node berikutnya jika terjadi collision
} Node;

// Array global dari pointer Node yang bertindak sebagai Hash Table
Node* hashTable[SIZE];

// Fungsi untuk menginisialisasi seluruh slot Hash Table menjadi kosong (NULL)
void inisialisasiTable() {
    for (int i = 0; i < SIZE; i++) {
        hashTable[i] = NULL;
    }
}

// FUNGSI HASH OPTIMAL (Mendapatkan sebaran sempurna 101 indeks terisi)
int hitungHash(char* ktm) {
    unsigned long hash = 0;
    int p = 31; // Konstanta bilangan prima untuk polynomial rolling hash
    
    // Mengabaikan teks "KTM-" di depan karena seragam, fokus pada keunikan di belakang
    char* ptr = ktm;
    if (strncmp(ktm, "KTM-", 4) == 0) {
        ptr = ktm + 4; 
    }
    
    while (*ptr != '\0') {
        hash = (hash * p + *ptr) % SIZE;
        ptr++;
    }
    
    return hash;
}

// Fungsi untuk memasukkan data ke Hash Table menggunakan Separate Chaining
void insert(char* ktm, char* nama) {
    int indeks = hitungHash(ktm);

    // Alokasi memori dinamis untuk node baru
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("[Eror] Alokasi memori gagal!\n");
        return;
    }
    strcpy(newNode->ktm, ktm);
    strcpy(newNode->nama, nama);
    newNode->next = NULL;

    // Jika slot indeks masih kosong, pasang sebagai kepala (Head)
    if (hashTable[indeks] == NULL) {
        hashTable[indeks] = newNode;
    } 
    // Jika sudah terisi (Collision), sambungkan ke bagian paling ujung (Tail)
    else {
        Node* curr = hashTable[indeks];
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = newNode;
    }
}

// Fungsi untuk membaca berkas eksternal dan mem-parsing data baris per baris
void bacaFile() {
    FILE* file = fopen("Data_Latih.txt", "r");
    if (file == NULL) {
        printf("[Eror] File Data_Latih.txt tidak ditemukan!\n");
        printf("Pastikan file berada di direktori yang sama dengan berkas C ini.\n");
        return;
    }

    char tempKtm[15];
    char tempNama[100];
    int jumlahData = 0;

    // Parsing string terformat dipisahkan oleh karakter koma
    while (fscanf(file, " %[^,],%[^\n]\n", tempKtm, tempNama) != EOF) {
        insert(tempKtm, tempNama);
        jumlahData++;
    }

    fclose(file);
    printf("==================================================\n");
    printf("[INFO] Sukses memuat %d data dari Data_Latih.txt\n", jumlahData);
    printf("==================================================\n\n");
}

// Fungsi untuk menghitung statistik distribusi dan akurasi skor performa
void hitungMetrik() {
    int indeksTerisi = 0;
    int totalCollision = 0;

    // Lakukan iterasi menyeluruh ke 101 slot laci
    for (int i = 0; i < SIZE; i++) {
        if (hashTable[i] != NULL) {
            indeksTerisi++; // Indeks terisi karena Head tidak kosong

            // Hitung sisa elemen di dalam rantai laci tersebut sebagai collision
            Node* curr = hashTable[i]->next;
            while (curr != NULL) {
                totalCollision++;
                curr = curr->next;
            }
        }
    }

    // Kalkulasi Skor Akhir menggunakan rumus objektivitas penilaian di panduan
    double rasioSebaran = (double)indeksTerisi / 101.0;
    double rasioTabrakan = 1.0 - (abs(totalCollision - 399) / 399.0);
    double skorAkhir = (rasioSebaran * 100.0) * rasioTabrakan;

    if (skorAkhir < 0) skorAkhir = 0.0;

    // Tampilkan rangkuman output laporan metrik ke terminal
    printf("--- LAPORAN METRIK HASIL DISTRIBUSI HASH TABLE ---\n");
    printf("1. Jumlah Indeks Terisi : %d / 101 slot (Target: 101)\n", indeksTerisi);
    printf("2. Total Data Tabrakan  : %d kali  (Target: 399)\n", totalCollision);
    printf("--------------------------------------------------\n");
    printf(">> SKOR AKHIR PERFORMA  : %.2f %% (Target: 100.00%%)\n", skorAkhir);
    printf("--------------------------------------------------\n");
}

// Fungsi untuk mengosongkan memori (Mencegah Memory Leak)
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
    // 1. Mengosongkan penunjuk wadah tabel
    inisialisasiTable();

    // 2. Membaca file Data_Latih.txt dan mengisi struktur data
    bacaFile();

    // 3. Menghitung dan mencetak statistik performa tim
    hitungMetrik();

    // 4. Bersihkan alokasi memori RAM sebelum aplikasi dihentikan
    bebaskanMemori();

    return 0;
}