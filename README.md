# Sistem Kehadiran RFID Client–Server

Sistem ini dirancang untuk mencatat dan memantau kehadiran mahasiswa dan staf melalui pemindaian RFID secara **real-time**. Setiap reader RFID bertindak sebagai **client** yang mengirimkan data ke server pusat. Server menerima, menyimpan, dan menyediakan database untuk keperluan edit dan administrasi.

---

## Fitur Utama

1. **Client–Server Architecture**  
   Reader RFID (client) mengirim ID dan timestamp ke server pusat melalui koneksi TCP.
   
2. **Pencarian & Pengurutan Data**  
   Mencari log berdasarkan ID mahasiswa/staf dan mengurutkan kunjungan berdasarkan waktu scan.

3. **Persistensi Data**  
   - Menyimpan log harian ke file **biner** untuk efisiensi dan keamanan.  
   - Mengekspor log harian ke **JSON** untuk integrasi dengan dashboard administrasi.

4. **Pemrosesan Paralel & Sinkronisasi**  
   Mampu menangani banyak pemindaian secara bersamaan tanpa korupsi data menggunakan **threading** dan **lock/sinkronisasi** file.

---
## Struktur Program 
server.cpp: Kode utama untuk server yang menerima data dari client dan memprosesnya.

server.hpp: Header file untuk deklarasi fungsi-fungsi di server.

client.cpp: Kode untuk client yang mengirimkan ID ke server.

client.hpp: Header file untuk deklarasi fungsi-fungsi di client.

studentdatabase.csv: Database berisi daftar ID mahasiswa yang valid.

file student 1-6 : memuat data tiap mahasiswa

attendance.csv: File log kehadiran yang mencatat ID dan timestamp.

## Cara Menjalankan Program
1. Persiapan
Pastikan Anda memiliki compiler C++ dan sistem operasi yang mendukung socket programming (seperti Linux atau macOS).

2. Kompilasi
Kompilasi server dan client menggunakan perintah berikut:
```
./make client
```

3. Menjalankan Server
Jalankan server dengan perintah:

```
./server
```
Server akan berjalan dan mendengarkan koneksi pada port 8080.

4. Menjalankan Client
Jalankan client dengan perintah:
```
./client
```

Masukkan ID mahasiswa saat diminta. Client akan mengirimkan ID tersebut ke server untuk diproses.

## Format File
- studentdatabase.csv: Berisi daftar ID mahasiswa yang valid, satu ID per baris.
- attendance.csv: Mencatat ID dan timestamp setiap pemindaian, dengan format ID,YYYY-MM-DD HH:MM:SS.
