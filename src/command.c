#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/execute.h"
#include "../include/prompt.h"
#include "../include/command.h"

#include <unistd.h>

/**
 * @brief Fonction qui affiche un message sur la sortie standard
 *
 * @param input le text a afficher
 * @return int 0 si tout c'est bien passé, -1 sinon
 */
int printt(const char *input)
{
    if (write(STDOUT_FILENO, input, strlen(input)) == -1)
    {
        perror("write");
        return -1;
    }
    return 0;
}

/**
 * @brief Fonction qui affiche un message sur la sortie d'erreur
 *
 * @param input le text a afficher
 * @return int 0 si tout c'est bien passé, -1 sinon
 */
int printerr(char *input)
{
    if (write(STDERR_FILENO, input, strlen(input)) == -1)
    {
        perror("write");
        return -1;
    }
    return 0;
}
