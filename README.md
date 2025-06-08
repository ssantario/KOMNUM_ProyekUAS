# Proyek UAS Komputasi Numerik – Analisis Diode Circuit dengan Metode Golden-Section Search

Rivi Yasha Hafizhan      2306250535


## Deskripsi

Proyek ini merupakan implementasi analisis rangkaian dioda menggunakan **Metode Golden-Section Search** (Bab 4, Chapra) untuk menyelesaikan persamaan nonlinear pada rangkaian dioda dan dioda dengan resistor. Program membaca data permasalahan dari file CSV, melakukan optimisasi untuk mencari solusi tegangan dioda, dan menuliskan hasilnya ke file CSV.

Metode Golden-Section Search digunakan untuk mencari minimum dari fungsi objektif kuadrat `(f(v))^2`, sehingga solusi minimum tersebut adalah akar dari persamaan nonlinear rangkaian dioda.

Implementasi ini menganalisis **31 kasus uji** dan seluruh hasil simulasi dicatat secara otomatis ke file hasil.

---

## Struktur File

- `proyek_uas_optimisasi.c` : Source code utama program
- `data.csv`                : File input parameter masalah 
- `results.csv`             : File output hasil simulasi

---

## Cara Menjalankan

1. **Edit file `data.csv`** sesuai format yang diharapkan (lihat contoh di bawah)
2. **Kompilasi program**  
   ```bash
   gcc proyek_uas_optimisasi.c -o golden_section_solver -lm
   ```
3. **Jalankan program:**
   ```bash
   ./golden_section_solver
   ```
   atau di Windows:
   ```bash
   golden_section_solver.exe
   ```
4. **Lihat hasil di `results.csv`** dan output terminal

---

## Format File `data.csv`

Baris pertama adalah header. Setiap baris berikutnya berisi satu masalah, dengan format:
```
problem_type,param1_name,param1_value,param2_name,param2_value,...,x0,x1,tolerance,max_iterations
```
Contoh:
```
diode,I_s,1e-12,n,1.5,V_t,0.02585,I,0.001,NA,0,0.5,0.8,1e-6,100
```
- **problem_type**: `diode` atau `diode_resistor`
- **paramX_name/paramX_value**: Nama dan nilai parameter 
- **x0, x1**: Batas bawah dan atas pencarian (interval Golden-Section)
- **tolerance**: Toleransi error
- **max_iterations**: Iterasi maksimum

---

## Alur Program dan Metodologi

1. **Baca Data Input**  
   Fungsi `readCsvFile()` membaca seluruh kasus dari `data.csv` dan menyimpan parameter ke dalam array struct.

2. **Analisis Setiap Kasus**  
   Untuk setiap kasus, program memilih model fisika yang sesuai:
   - `diodeFunction()` untuk rangkaian dioda sederhana:
     ```
     f(v) = Is * (exp(v/(n*Vt)) - 1) - I
     ```
   - `diodeResistorFunction()` untuk rangkaian dioda dengan resistor:
     ```
     f(v) = Is * (exp(v/(n*Vt)) - 1) - (Vs - v)/R
     ```

3. **Optimisasi Golden-Section Search**  
   Fungsi `goldenSectionSearch()` mencari nilai `v` yang meminimalkan `(f(v))^2` pada interval `[x0, x1]` hingga toleransi atau iterasi maksimum tercapai.

4. **Catat dan Simpan Hasil**  
   Semua hasil (parameter, solusi, status konvergensi, error, waktu eksekusi) disimpan ke `results.csv` melalui `writeCsvFile()`.

---

## Hasil Eksperimen dan Analisis

- Program berhasil menyelesaikan seluruh kasus uji dengan tingkat konvergensi sangat tinggi.
- Tegangan dioda yang dihasilkan sesuai dengan karakteristik fisika dioda silikon pada umumnya (sekitar 0,6–0,8V untuk forward bias).
- Metode Golden-Section Search terbukti efektif untuk mencari solusi akar persamaan nonlinear tanpa memerlukan turunan fungsi.

---

## Kesimpulan

Implementasi metode Golden-Section Search untuk analisis rangkaian dioda terbukti efektif dan stabil. Program ini dapat digunakan sebagai referensi untuk penyelesaian masalah nonlinear lain yang serupa, terutama jika fungsi turunan sulit dihitung.

---

## Contoh Output

Hasil simulasi dapat dilihat pada file `results.csv` yang berisi detail parameter, solusi, status konvergensi, error, dan waktu eksekusi untuk setiap kasus.

---

> **Catatan:**  
> Untuk visualisasi atau analisis lebih lanjut, hasil pada `results.csv` dapat diolah menggunakan Python, Excel, atau perangkat lunak statistik lainnya.