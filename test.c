#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipefd[2];
    char message[] = "Bonjour depuis le parent";
    char buffer[100];

    // Création du pipe
    if (pipe(pipefd) == -1) {
        perror("Erreur lors de la création du pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Erreur lors de fork");
        return 1;
    }

    if (pid == 0) { // Processus enfant
        close(pipefd[1]); // Ferme l'extrémité d'écriture (inutile ici)

        // Lit le message depuis l'extrémité de lecture du pipe
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Message reçu par l'enfant : %s\n", buffer);

        close(pipefd[0]); // Ferme l'extrémité de lecture du pipe
    } else { // Processus parent
        close(pipefd[0]); // Ferme l'extrémité de lecture (inutile ici)

        // Écrit un message dans l'extrémité d'écriture du pipe
        write(pipefd[1], message, strlen(message) + 1);
        close(pipefd[1]); // Ferme l'extrémité d'écriture après envoi
    }

    return 0;
}
