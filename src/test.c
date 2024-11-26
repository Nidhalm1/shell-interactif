#include <stdio.h>
#include <unistd.h>

int main() {
    // Tableau d'arguments pour la commande `ls`
    char *args[] = {"ls", "-l",">" , "fihce.txt", NULL};  // Le premier élément est généralement le nom du programme
 int len =sizeof(args);

    printf("%d",len);

    // Ce code ne sera pas exécuté si execv réussit
    printf("       Ce message ne s'affichera pas si execv réussit.\n");

    return 0;
}
