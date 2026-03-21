# XDartScan - Worklog

## 2026-03-21 - Performance optimizations and bug fixes

### Obiettivo
Ottimizzazione delle performance di scansione per ridurre tempi di I/O, allocazioni di memoria e chiamate di sistema.

### Modifiche

#### `logic/scan_engine.c`
- **Singola lettura file**: eliminata la doppia lettura (magic number + contenuto intero). Ora un solo `fread` legge tutto il buffer, da cui si estraggono sia i primi 4 byte per il magic number sia i dati per il calcolo dell'entropia.
- **Conversione magic number via bit-shift**: sostituita `strtoul()` (parsing stringa hex) con shift aritmetico diretto dai byte già letti. Evita la conversione stringa per ogni file.
- **Normalizzazione path su stack**: `normalize_path()` allocava con `malloc` ad ogni chiamata ricorsiva. Ora il path è normalizzato su un buffer locale nello stack, eliminando malloc/free nel percorso critico.
- **Riduzione copie stringa**: rimossi `strncpy` + `strcat` concatenati, sostituiti con un singolo `snprintf` per costruire il path.
- **Uso di `d_type`**: quando disponibile (`_DIRENT_HAVE_D_TYPE`), si usa `dp->d_type` per distinguere file da directory senza chiamare `stat()` inutilmente.
- **Skip `.` e `..` efficiente**: controllo diretto sui primi caratteri invece di `strncmp`.
- **Cap lettura entropia**: la lettura per il calcolo dell'entropia è limitata a `MAX_FILE_SIZE` byte, evitando di caricare file enormi interamente in memoria.
- **Rimossa funzione `p_read_magic_number`**: non più necessaria dato che il magic number viene estratto dal buffer già letto.
- **`sprintf` → `snprintf`**: per sicurezza contro buffer overflow nel report line.

#### `logic/config_manager.c`
- **`qsort` al posto di selection sort**: l'ordinamento delle 397 firme passa da O(n²) a O(n log n).
- **`strcmp` al posto di `strncmp`**: per i parametri di configurazione dove la lunghezza massima non è necessaria.

#### `logic/report_manager.c`
- **Buffer I/O 64KB**: aggiunto `setvbuf(*fp, NULL, _IOFBF, 64 * 1024)` sui file di report per ridurre il numero di syscall di scrittura.
- **Header TSV come stringa costante**: sostituita la costruzione dinamica con `sprintf` con una stringa `static const`.
- **Controllo parametri nulli**: aggiunta validazione iniziale in `create_report_file`.

#### `misc/utils.c`
- **`malloc` al posto di `calloc`** in `read_file_content`: il buffer viene subito sovrascritto da `fread`, quindi l'azzeramento con `calloc` era inutile.
- **`format_size`: array `units` reso `static`**: evita la reinizializzazione ad ogni chiamata.
- **`strnstr` ottimizzato**: aggiunto early-return se la stringa da cercare è vuota o più lunga della stringa in cui cercare.
- **Validazione input**: aggiunti controlli nulli in `format_size`, `read_file_content`, `itoa`.

#### `logic/random_test.c`
- Documentazione estesa della funzione `calc_rand_idx`.

#### `.gitignore`
- Aggiunti `.iac-data` e `.DS_Store`.

### Bug fix
- **`ctime()` buffer overwrite**: `ctime()` restituisce un puntatore a un buffer statico condiviso. Le tre chiamate consecutive per mtime/ctime/atime sovrascrivevano lo stesso buffer, risultando in timestamp identici. Corretto usando `ctime_r()` con buffer separati per ciascun timestamp.
