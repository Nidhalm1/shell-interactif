#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Déclarez les fonctions argc et argv pour qu'elles soient visibles ici
int argc(char *input)
{
    int count = 0;
    int len = strlen(input);
    for (int i = 0; i < len; i++)
    {
        if (input[i] == ' ')
        {
            count++;
        }
    }
    return count + 1;
}

char **argv(char *input)
{
    int arg_count = argc(input);
    char **args = malloc((arg_count + 1) * sizeof(char *));
    if (args == NULL)
    {
        perror("malloc");
        return NULL;
    }
    char *input_copy = strdup(input);
    if (input_copy == NULL)
    {
        perror("strdup");
        free(args); // Libérer args si strdup échoue
        return NULL;
    }
    char *arg = strtok(input_copy, " ");
    int i = 0;
    while (arg != NULL)
    {
        args[i] = strdup(arg);
        if (args[i] == NULL)
        {
            goto error;
        }
        arg = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
    free(input_copy);
    return args;

error: // free tout en cas d'erreur
    perror("strdup");
    free(input_copy);
    for (int index = 0; index < i; index++)
    {
        free(args[index]);
    }
    free(args);
    return NULL;
}

int main() {
    char input[256];
    printf("Entrez une commande pour tester : ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; // Retirer le caractère de nouvelle ligne

    // Calculer le nombre d'arguments
    int count = argc(input);
    printf("Nombre d'arguments : %d\n", count);

    // Récupérer les arguments sous forme de tableau
    char **args = argv(input);
    if (args == NULL) {
        perror("argv");
        return 1;
    }

    // Afficher chaque argument trouvé
    printf("Arguments :\n");
    for (int i = 0; i < count; i++) {
        printf("args[%d] = '%s'\n", i, args[i]);
    }

    // Libérer la mémoire allouée pour les arguments
    for (int i = 0; i < count; i++) {
        free(args[i]);
    }
    free(args);

    return 0;
}
