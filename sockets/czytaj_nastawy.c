#include <stdio.h>
#include <string.h>

void czytaj_nastawy(char *magazyn);

int main(void)
{
        char magazyn[40];

        czytaj_nastawy(magazyn);

        return 0;
}



// Funkcja zwraca c-string z nastawami rozdzielonymi przecinkami.
// (Dla regulatora PI beda to 2 liczby float - parsowanie zrobi regulator w Matlabie)

void czytaj_nastawy(char *magazyn)
{
        FILE *nastawy;
        char bufor[40];

        nastawy = fopen("nastawy.txt", "r");
        
        fscanf(nastawy, "%s", bufor);
        printf("Bufor: %s\n", bufor);

        strcpy(magazyn, bufor);
        printf("Magazyn: %s\n", magazyn);

        // Zapis
        // fprintf(nastawy, "Test fprintf: \n");

        fclose(nastawy);
}

