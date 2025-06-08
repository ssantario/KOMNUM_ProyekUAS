/*
Analisis Rangkaian Dioda dengan Metode Optimisasi Golden-Section Search

Mata Kuliah    : Komputasi Numerik (ENCE604015)
Tugas         : Proyek UAS
Semester      : Genap 2024/2025

Rivi Yasha Hafizhan 2306250535

Input         : File CSV berisi parameter rangkaian (data.csv)
Output        : File CSV berisi hasil analisis (results.csv)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Konstanta untuk ukuran max data input dan parameter
#define MAX_LINE_LENGTH 1024
#define MAX_PARAMS 10

// Struct untuk menyimpan informasi masalah yang akan diselesaikan
typedef struct {
    char problemType[50];             // "diode" atau "diode_resistor"
    char paramNames[MAX_PARAMS][50];  // Nama-nama parameter
    double paramValues[MAX_PARAMS];   // Nilai-nilai parameter
    int paramCount;                   // Jumlah parameter yang dipakai
    double xl_init;                   // Batas bawah awal untuk optimisasi
    double xu_init;                   // Batas atas awal untuk optimisasi
    double tolerance;                 // Toleransi error untuk konvergensi
    int maxIterations;                // Max iterasi
} Problem;

// --- DEKLARASI FUNGSI (PROTOTYPES) ---
double diodeFunction(double v, double* params);
double diodeResistorFunction(double v, double* params);
double goldenSectionSearch(double (*f)(double, double*), double xl, double xu, double* params, double tol, int maxIter, int* iterations, double* error, int* converged);
void readCsvFile(const char* filename, Problem* problems, int* problemCount);
void writeCsvFile(const char* filename, Problem* problems, int problemCount, double* solutions, int* iterations, int* converged, double* errors, double* times);

// Fungsi utama
int main() {
    Problem problems[100];
    int problemCount = 0;
    double solutions[100];
    int iterations[100];
    int converged[100];
    double errors[100];
    double times[100];
    
    // Baca data input dari file csv
    readCsvFile("data.csv", problems, &problemCount);
    
    // Loop untuk menyelesaikan setiap masalah
    for (int i = 0; i < problemCount; i++) {
        clock_t start = clock(); 
        
        printf("\n>> Solving Problem #%d: %s with Golden-Section Search\n", i + 1, problems[i].problemType);
        
        // Pilih fungsi yang tepat berdasarkan tipe masalah
        if (strcmp(problems[i].problemType, "diode") == 0) {
            solutions[i] = goldenSectionSearch(diodeFunction, problems[i].xl_init, problems[i].xu_init, 
                                               problems[i].paramValues, problems[i].tolerance, 
                                               problems[i].maxIterations, &iterations[i], 
                                               &errors[i], &converged[i]);
        } 
        else if (strcmp(problems[i].problemType, "diode_resistor") == 0) {
            solutions[i] = goldenSectionSearch(diodeResistorFunction, problems[i].xl_init, problems[i].xu_init, 
                                               problems[i].paramValues, problems[i].tolerance, 
                                               problems[i].maxIterations, &iterations[i], 
                                               &errors[i], &converged[i]);
        } 
        else {
            printf("[!] Unknown problem type: %s\n", problems[i].problemType);
            solutions[i] = 0; iterations[i] = 0; converged[i] = 0; errors[i] = 0;
        }
        
        clock_t end = clock();
        times[i] = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC; 
        
        // Tampilkan hasil
        if (converged[i]) {
            printf("Solution found: %lf after %d iterations\n", solutions[i], iterations[i]);
            printf("Final objective function value (error^2): %e\n", errors[i]);
            printf("Execution time: %.2f ms\n", times[i]);
        } else {
            printf("Method failed to converge after %d iterations\n", iterations[i]);
            printf("Last value: %lf, final objective function value: %e\n", solutions[i], errors[i]);
        }
    }
    
    // Simpan hasil ke CSV
    writeCsvFile("results.csv", problems, problemCount, solutions, iterations, converged, errors, times);
    
    printf("\n[v] Simulation done. Results saved to results.csv\n");
    
    return 0;
}


// --- FUNGSI MODEL FISIK ---
// f(v) = Is * (exp(v/(n*Vt)) - 1) - I = 0
double diodeFunction(double v, double* params) {
    double Is = params[0];  // Arus saturasi
    double n = params[1];   // Faktor idealitas
    double Vt = params[2];  // Tegangan thermal
    double I = params[3];   // Arus input
    
    double exponent_arg = v / (n * Vt);
    // Perlindungan overflow
    if (exponent_arg > 70.0) { exponent_arg = 70.0; }
    
    double exp_term = exp(exponent_arg);
    double diode_current = Is * (exp_term - 1);
    
    return diode_current - I;
}

// f(v) = Is * (exp(v/(n*Vt)) - 1) - (Vs - v) / R = 0  
double diodeResistorFunction(double v, double* params) {
    double Is = params[0];  // Arus saturasi dioda
    double n = params[1];   // Faktor idealitas
    double Vt = params[2];  // Tegangan thermal 
    double R = params[3];   // Nilai resistansi
    double Vs = params[4];  // Tegangan sumber
    
    double exponent_arg = v / (n * Vt);
    // Perlindungan overflow
    if (exponent_arg > 70.0) { exponent_arg = 70.0; }
    
    return Is * (exp(exponent_arg) - 1) - (Vs - v) / R;
}


// --- FUNGSI SOLVER OPTIMISASI ---
double goldenSectionSearch(double (*f)(double, double*), double xl, double xu, double* params, 
                           double tol, int maxIter, int* iterations, double* error, int* converged) {
    
    double phi = (1.0 + sqrt(5.0)) / 2.0; // Golden ratio
    double d, x1, x2, fx1, fx2;
    double res = 1.0 / (phi * phi); // 1/phi^2

    *iterations = 0;
    *converged = 0;

    // Hitung titik internal awal
    d = (phi - 1) * (xu - xl);
    x1 = xl + d;
    x2 = xu - d;

    // Evaluasi fungsi objektif (f(x)^2) pada titik internal
    fx1 = pow(f(x1, params), 2);
    fx2 = pow(f(x2, params), 2);

    for (int i = 0; i < maxIter; i++) {
        (*iterations)++;

        if (fx1 < fx2) { // Minimum berada di interval [xl, x2]
            xl = x2;
            x2 = x1;
            fx2 = fx1;
            x1 = xl + res * (xu - xl); // Hitung titik x1 yang baru
            fx1 = pow(f(x1, params), 2);
        } else { // Minimum berada di interval [x1, xu]
            xu = x1;
            x1 = x2;
            fx1 = fx2;
            x2 = xu - res * (xu - xl); // Hitung titik x2 yang baru
            fx2 = pow(f(x2, params), 2);
        }

        // Cek konvergensi
        if (fabs(xu - xl) < tol) {
            *converged = 1;
            *error = (fx1 < fx2) ? fx1 : fx2; // Simpan nilai error kuadrat terakhir
            return (xl + xu) / 2.0; // Kembalikan titik tengah sebagai solusi
        }
    }

    // Jika gagal konvergen
    *error = (fx1 < fx2) ? fx1 : fx2;
    return (xl + xu) / 2.0;
}


// --- FUNGSI I/O (Sama seperti program Secant) ---
// Fungsi untuk membaca data dari file CSV
void readCsvFile(const char* filename, Problem* problems, int* problemCount) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error: Cannot open input file");
        exit(1);
    }
    
    char line[MAX_LINE_LENGTH];
    int lineNum = 0;
    
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (lineNum == 0 || line[0] == '/' || strlen(line) <= 1) { 
            lineNum++;
            continue; 
        }
        
        Problem* prob = &problems[*problemCount];
        char* token;
        int paramIndex = 0;
        int tokenIndex = 0;
        
        token = strtok(line, ",");
        while (token != NULL) {
            // Logika parsing disesuaikan dengan nama kolom x0 dan x1 di CSV
            if (tokenIndex == 0) {
                strcpy(prob->problemType, token); 
            } else if (tokenIndex % 2 == 1 && paramIndex < 5) {
                strcpy(prob->paramNames[paramIndex], token); 
            } else if (tokenIndex % 2 == 0 && paramIndex < 5 && tokenIndex > 0) {
                prob->paramValues[paramIndex] = atof(token); 
                paramIndex++;
            } else if (tokenIndex == 11) { // Kolom 'x0' dibaca sebagai xl_init
                prob->xl_init = atof(token); 
            } else if (tokenIndex == 12) { // Kolom 'x1' dibaca sebagai xu_init
                prob->xu_init = atof(token); 
            } else if (tokenIndex == 13) {
                prob->tolerance = atof(token); 
            } else if (tokenIndex == 14) {
                prob->maxIterations = atoi(token);  
            }
            
            token = strtok(NULL, ",");
            tokenIndex++;
        }
        
        prob->paramCount = paramIndex;
        (*problemCount)++;
        lineNum++;
    }
    
    fclose(file);
}

// Fungsi untuk menulis hasil ke file CSV
void writeCsvFile(const char* filename, Problem* problems, int problemCount, 
                  double* solutions, int* iterations, int* converged, double* errors, double* times) {
    FILE* file = fopen(filename, "w");
     if (file == NULL) {
        perror("Error: Cannot open output file for writing");
        exit(1);
    }

    // Header CSV
    fprintf(file, "problem_type,");
    for (int j = 0; j < MAX_PARAMS; j++) {
        fprintf(file, "param%d_name,param%d_value,", j+1, j+1);
    }
    fprintf(file, "xl_init,xu_init,tolerance,max_iterations,solution,iterations,converged,error_squared,time_ms\n");
    
    for (int i = 0; i < problemCount; i++) {
        fprintf(file, "%s,", problems[i].problemType);
        
        for (int j = 0; j < MAX_PARAMS; j++) {
            if (j < problems[i].paramCount) {
                fprintf(file, "%s,%g,", problems[i].paramNames[j], problems[i].paramValues[j]);
            } else {
                fprintf(file, ",,"); 
            }
        }
        
        fprintf(file, "%g,%g,%g,%d,%g,%d,%s,%g,%g\n",
                problems[i].xl_init, problems[i].xu_init, problems[i].tolerance, problems[i].maxIterations,
                solutions[i], iterations[i], converged[i] ? "yes" : "no", errors[i], times[i]);
    }
    
    fclose(file);
}