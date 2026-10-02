#!/bin/bash

echo "[INFO] Installazione di slot in corso..."

# 1. Controlla e installa Docker se manca (APT o Pacman)
if ! command -v docker &> /dev/null; then
    echo "[ATTENZIONE] Docker non è trovato. Installazione automatica..."
    if command -v apt-get &> /dev/null; then
        sudo apt-get update && sudo apt-get install -y docker.io curl
    elif command -v pacman &> /dev/null; then
        sudo pacman -Sy --noconfirm docker curl
    else
        echo "[ERRORE] Nessun package manager supportato trovato!"
        exit 1
    fi
fi

sudo systemctl start docker
sudo systemctl enable docker

# 2. Verifica che la cartella 'librerie' e i file al suo interno esistano
if [ ! -d "librerie" ]; then
    echo "[ERRORE] Impossibile trovare la cartella 'librerie/'."
    exit 1
fi

if [ ! -f "librerie/DockerManager.cpp" ] || [ ! -f "librerie/DockerManager.hpp" ] || [ ! -f "librerie/logica_segreta.o" ]; then
    echo "[ERRORE] Mancano file essenziali dentro la cartella 'librerie/'."
    exit 1
fi

echo "[INFO] Compilazione ed assemblaggio di slot..."

# 3. Compila usando i file protetti e chiama l'eseguibile temporaneo 'slot_bin'
g++ librerie/DockerManager.cpp librerie/logica_segreta.o -I"librerie" -o slot_bin

if [ $? -ne 0 ]; then
    echo "[ERRORE] Compilazione fallita."
    exit 1
fi

# 4. Sposta l'eseguibile direttamente nel percorso globale come comando 'slot'
echo "[INFO] Installazione del comando globale 'slot'..."
sudo cp slot_bin /usr/local/bin/slot
sudo chmod +x /usr/local/bin/slot

# Facoltativo: se vuoi mantenere anche 'aethercli' come alias/symlink di riserva
sudo ln -sf /usr/local/bin/slot /usr/local/bin/aethercli

# Pulizia locale del binario temporaneo
rm -f slot_bin

echo "[SUCCESSO] Installazione completata! Ora puoi digitare semplicemente 'slot' da qualsiasi terminale."