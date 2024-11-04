#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *line = NULL;

    while (1)
    {
        // Affiche le prompt
        print_prompt();

        // Lecture de la ligne de commande
        line = readline("fsh> ");
        if (!line)
            break; // Gestion de fin de saisie (Ctrl+D)
        add_history(line);

        // Analyse et exécution de la commande
        process_command(line);

        // Libère la mémoire pour la ligne de commande
        free(line);
    }

    printf("Bye!\n");
    return 0;
}
