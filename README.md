# Sistem Kehadiran RFID Client–Server

Sistem ini dirancang untuk mencatat dan memantau kehadiran mahasiswa dan staf melalui pemindaian kartu RFID secara **real-time**. Setiap reader RFID bertindak sebagai **client** yang mengirimkan data ke server pusat. Server menerima, menyimpan, dan menyediakan log untuk keperluan audit dan administrasi.

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

