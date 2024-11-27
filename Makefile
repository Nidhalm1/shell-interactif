# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude #pour le dosssier include
LDFLAGS = -lreadline

# Nom de l'exécutable
TARGET = fsh

# Dossier contenant les fichiers sources
SRC_DIR = src

# Liste des fichiers sources
SRCS = $(SRC_DIR)/fsh.c $(SRC_DIR)/execute.c $(SRC_DIR)/parser.c $(SRC_DIR)/prompt.c $(SRC_DIR)/command.c $(SRC_DIR)/builtin.c $(SRC_DIR)/simple_commands.c $(SRC_DIR)/piped_commands.c $(SRC_DIR)/loop.c $(SRC_DIR)/if_function.c $(SRC_DIR)/structured_command.c

# Règle par défaut pour compiler l'exécutable
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Règle pour nettoyer les fichiers compilés
clean:
	rm -f $(TARGET)
