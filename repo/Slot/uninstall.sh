#!/bin/bash

echo "[INFO] Rimozione di slot in corso..."

# 1. Rimuove i comandi globali dal sistema
if [ -f "/usr/local/bin/slot" ]; then
    sudo rm -f /usr/local/bin/slot
    echo "[OK] Rimosso il comando globale 'slot'."
fi

if [ -f "/usr/local/bin/aethercli" ]; then
    sudo rm -f /usr/local/bin/aethercli
    echo "[OK] Rimosso il comando globale 'aethercli'."
fi

# 2. (Opzionale) Chiede se rimuovere anche il container Docker 'Slot' associato
read -p "Vuoi eliminare anche il container Docker 'Slot' e i suoi dati? (y/N): " scelta
if [[ "$scelta" =~ ^[Yy]$ ]]; then
    echo "[INFO] Arresto e rimozione del container Docker 'Slot'..."
    sudo docker stop Slot > /dev/null 2>&1
    sudo docker rm Slot > /dev/null 2>&1
    echo "[OK] Container Docker rimosso con successo."
else
    echo "[INFO] Il container Docker è stato mantenuto intatto."
fi

echo "[SUCCESSO] Disinstallazione completata con successo!"