#!/bin/bash
# Skript um das Programm ./append source target zu testen

# Zähler für fehlgeschlagene Tests (für die Zusammenfassung am Ende)
FAILED=0

# Funktion zur Überprüfung des Exit-Status
# Parameter:
#   $1 ... tatsächlicher Exit-Status
#   $2 ... erwartetes Ergebnis (0 = Erfolg, 1 = Fehler)
check_status()
{
    local status=$1
    local expected=$2

    echo "Exit-Status: $status"

    if [ "$expected" -eq 0 ]; then
        # Erfolg wird erwartet
        if [ "$status" -eq 0 ]; then
            echo "Test -> OK"
        else
            echo "Test -> NOK"
            FAILED=$((FAILED + 1))
        fi
    else
        # Fehler wird erwartet
        if [ "$status" -ne 0 ]; then
            echo "Test -> OK"
        else
            echo "Test -> NOK"
            FAILED=$((FAILED + 1))
        fi
    fi
}

# Funktion zur Überprüfung der ausgegebenen Bytezahl
# Parameter:
#   $1 ... tatsächliche Ausgabe des Programms
#   $2 ... erwartete Bytezahl
check_output()
{
    local actual=$1
    local expected=$2

    echo "Ausgabe: '$actual' (erwartet: '$expected')"

    if [ "$actual" = "$expected" ]; then
        echo "Test -> OK"
    else
        echo "Test -> NOK"
        FAILED=$((FAILED + 1))
    fi
}

# Funktion zur Überprüfung der Dateigröße
# Parameter:
#   $1 ... Dateiname
#   $2 ... erwartete Größe in Bytes
check_size()
{
    local file=$1
    local expected=$2
    local actual

    actual=$(wc -c < "$file")
    echo "Größe von $file: $actual Bytes (erwartet: $expected)"

    if [ "$actual" -eq "$expected" ]; then
        echo "Test -> OK"
    else
        echo "Test -> NOK"
        FAILED=$((FAILED + 1))
    fi
}

# Funktion zur Überprüfung, dass eine Datei NICHT angelegt wurde
# Parameter:
#   $1 ... Dateiname
check_not_exists()
{
    if [ -e "$1" ]; then
        echo "Datei $1 existiert (nicht erwartet)"
        echo "Test -> NOK"
        FAILED=$((FAILED + 1))
    else
        echo "Datei $1 existiert nicht (erwartet)"
        echo "Test -> OK"
    fi
}

# Loeschen der generierten Dateien
# rm -rf -> force...ignoriere Warnungen beim Loeschen
rm -rf append a.txt b.txt big.bin leer.txt ziel.txt neu.txt zielordner geheim.txt

# Erzeugen des Programms
gcc -o append append.c -Wall

if [ $? -ne 0 ]; then
    echo "ERROR: Compilation failed!"
    exit 1
fi

# Testdateien anlegen
printf 'Hallo\n' > a.txt        # 6 Bytes
truncate -s 1000 b.txt          # 1000 Bytes
truncate -s 1M big.bin          # 1048576 Bytes
: > leer.txt                    # 0 Bytes

# ---------------------------------------------------------------
# Fehlerfaelle bei der Kommandozeile
# ---------------------------------------------------------------
echo "create an error in the command line (no arguments) -> ./append"
./append
check_status $? 1

echo
echo "create an error in the command line (one argument) -> ./append a.txt"
./append a.txt
check_status $? 1

echo
echo "create an error in the command line (too many arguments) -> ./append a.txt ziel.txt neu.txt"
./append a.txt ziel.txt neu.txt
check_status $? 1

# ---------------------------------------------------------------
# Fehlerfaelle bei den Dateien
# ---------------------------------------------------------------
echo
echo "source file does not exist (-> error expected, target must not be created):"
./append gibtsnicht.txt neu.txt
check_status $? 1
check_not_exists neu.txt

echo
echo "source and target are the same file (-> error expected, no endless loop):"
./append a.txt a.txt
check_status $? 1
check_size a.txt 6

echo
echo "target is a directory (-> error open target expected):"
mkdir zielordner
./append a.txt zielordner
check_status $? 1

echo
echo "source file is not readable (-> error open source expected):"
if [ "$(id -u)" -eq 0 ]; then
    echo "skipped: as root every file is readable"
else
    echo "geheim" > geheim.txt
    chmod 000 geheim.txt
    ./append geheim.txt neu.txt
    check_status $? 1
    check_not_exists neu.txt
    chmod 644 geheim.txt
fi

# ---------------------------------------------------------------
# Erfolgsfaelle
# ---------------------------------------------------------------
echo
echo "append a.txt (6 Bytes) to new file ziel.txt (file must be created):"
out=$(./append a.txt ziel.txt)
check_status $? 0
check_output "$out" 6
check_size ziel.txt 6

echo
echo "append a.txt again (ziel.txt must grow, not be overwritten):"
out=$(./append a.txt ziel.txt)
check_status $? 0
check_output "$out" 6
check_size ziel.txt 12

echo
echo "append b.txt (1000 Bytes, two full buffers minus remainder):"
out=$(./append b.txt ziel.txt)
check_status $? 0
check_output "$out" 1000
check_size ziel.txt 1012

echo
echo "append big.bin (1 MByte):"
out=$(./append big.bin ziel.txt)
check_status $? 0
check_output "$out" 1048576
check_size ziel.txt 1049588

echo
echo "append empty file (0 Bytes, ziel.txt must stay unchanged):"
out=$(./append leer.txt ziel.txt)
check_status $? 0
check_output "$out" 0
check_size ziel.txt 1049588

# ---------------------------------------------------------------
# Ergebnisse auflisten
# ---------------------------------------------------------------
echo
echo "list all generated files:"
ls -l a.txt b.txt big.bin leer.txt ziel.txt

echo
echo "first 12 bytes of ziel.txt (must be the text of a.txt twice):"
head -c 12 ziel.txt

# ---------------------------------------------------------------
# Zusammenfassung
# ---------------------------------------------------------------
echo
if [ "$FAILED" -eq 0 ]; then
    echo "ALL TESTS OK"
    exit 0
else
    echo "$FAILED TEST(S) FAILED"
    exit 1
fi
