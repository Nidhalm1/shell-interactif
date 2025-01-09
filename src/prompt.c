#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "../include/command.h"

/**
 * @brief Récupère le répertoire courant
 *
 * @param buffer Buffer pour stocker le répertoire
 * @param size Taille du buffer
 */
void get_current_directory(char *buffer, size_t size)
{
    if (getcwd(buffer, size) == NULL)
    {
        printerr("Error: Unable to get current directory\n");
    }
}

/**
 * @brief Tronque le répertoire pour l'affichage
 *
 * @param dir Répertoire à tronquer
 */
void troncate(char *dir, int c)
{
    int len = strlen(dir);
    int enlever = len - c;
    strcpy(dir, dir + enlever);
    dir[c] = '\0';
    for (size_t i = 0; i < 3; i++)
    {
        dir[i] = '.';
    }
}
int longueur_int(int n)
{
    int longueur = 0;
    if (n == 0)
    {
        return 1;
    }
    while (n != 0)
    {
        n /= 10;
        longueur++;
    }
    return longueur;
}
/**
 * @brief Affiche le prompt
 *
 * @param last_return_code Dernier code de retour
 * @return char* Commande saisie
 */
char *prompt(int last_return_code)
{
    char color[10];
    char reset_color[10];
    char color_dir[10];
    char dir[1024];
    get_current_directory(dir, sizeof(dir));
    int len = longueur_int(last_return_code);
    int longeur = strlen(dir);
    if (longeur > 25)
    {
        troncate(dir, 25 - len + 1);
    }
    if (last_return_code == 0)
    {
        strcpy(color, "\001\033[32m\002"); // Green
    }
    else
    {
        strcpy(color, "\001\033[91m\002"); // Red
    }

    // Couleur pour le répertoire
    strcpy(color_dir, "\001\033[34m\002");   // bleu pour le répertoire
    strcpy(reset_color, "\001\033[00m\002"); // coleur de base normal

    char prompt_string[1070];
    if (last_return_code != 2)
    {
        snprintf(prompt_string, sizeof(prompt_string), "%s[%d]%s%s%s$ ", color, last_return_code, color_dir, dir, reset_color);
    }
    else
    {
        memmove(&dir[3], &dir[5], strlen(dir) - 4);
        snprintf(prompt_string, sizeof(prompt_string), "%s[%s]%s%s%s$ ", color, "SIG", color_dir, dir, reset_color);
    }
    rl_outstream = stderr;
    char *input = readline(prompt_string);
    if (input == NULL)
    {
        return NULL;
    }

    add_history(input);
    return input;
}
