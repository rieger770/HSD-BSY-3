#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

typedef struct SRange
{ 
    long from; 
    long to; 
} TRange; 

int ParseToken(const char *tok, TRange *r);
void PrintRanges(TRange *ranges, int count, const char* filename);

int main(int argc, const char *argv[])
{
    // Unterstützt argc == 2 (nur Dateiname) und argc == 3 (Intervalle + Dateiname)
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "Nutzung: %s [intervalle] <datei>\n", argv[0]);
        return 1;
    }

    TRange ranges[10]; // Speicher für maximal 10 Zeilenabschnitte
    int range_count = 0;
    const char *filename;

    if (argc == 2)
    {
        // Fall: Nur Dateiname (z.B. "lineout maerchen") -> Gesamte Datei ausgeben
        ranges[0].from = 1;
        ranges[0].to = LONG_MAX;
        range_count = 1;
        filename = argv[1];
    }
    else
    {
        // Fall: Intervalle und Dateiname
        filename = argv[2];
        
        char intervals[256];
        strncpy(intervals, argv[1], sizeof(intervals)); // tatsächlichen inhalt kopieren
        intervals[sizeof(intervals) - 1] = '\0'; // nullterminieren

        char *token = strtok(intervals, ",");
        while (token != NULL && range_count < 10)
        {
            if (ParseToken(token, &ranges[range_count]))
            {
                range_count++;
            }
            else
            {
                fprintf(stderr, "Ungültiges Intervall: %s\n", token);
                return 1;
            }
            token = strtok(NULL, ",");
        }
           if (token != NULL) // mehr als 10 intervalle?
        {
            fprintf(stderr, "Zu viele Zeilenabschnitte (max. 10)\n");
            return 1;
        }
    }

    // Datei öffnen und alle Bereiche verarbeiten
    PrintRanges(ranges, range_count, filename);

    return 0;
}

void PrintRanges(TRange *ranges, int count, const char* filename)
{
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        perror("Error in fopen.");
        return;
    }
    
    char lineBuff[1025]; // Max 1024 Bytes pro Zeile laut Angabe
    long current = 1;

    while(fgets(lineBuff, sizeof(lineBuff), file) != NULL)
    {
        // Prüfen, ob die aktuelle Zeile in einem der gespeicherten Bereiche liegt
        for (int i = 0; i < count; i++)
        {
            if(current >= ranges[i].from && current <= ranges[i].to)
            {
                printf("%ld   %s", current, lineBuff);
                break; // Verhindert doppelte Ausgabe bei sich überschneidenden Bereichen
            }
        }
        current++;
    }
    fclose(file);
}

// liefert 1 bei Erfolg, 0 bei Syntaxfehler
int ParseToken(const char *tok, TRange *r)
{
    char *end;
    if (tok[0] == '-') 
    {               
        r->from = 1;  // "-20"
        r->to = strtol(tok + 1, &end, 10);
    } 
    else 
    {
        r->from = strtol(tok, &end, 10);
        if (*end == '\0')
        {
            r->to = r->from;    // "3"
        } 
        else if (*end == '-') 
        {
            if (end[1] == '\0')
            { 
                r->to = LONG_MAX;   // "14-"
                end++;
            }
            else              
            { 
                r->to = strtol(end + 1, &end, 10); // "4-9"
            }
        }
    }
    return (*end == '\0' && r->from >= 1 && r->from <= r->to) ? 1 : 0;
}