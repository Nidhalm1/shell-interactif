#ifndef PROMPT_H
#define PROMPT_H

#include <stddef.h>

// Récupère le répertoire courant
void get_current_directory(char *buffer, size_t size);

// Tronque le répertoire pour l'affichage
void troncate(char *dir, int c);

// Affiche le prompt et récupère la commande saisie
char *prompt(int last_return_code);

#endif // PROMPT_H
