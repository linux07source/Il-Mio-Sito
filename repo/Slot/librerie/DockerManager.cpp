#include "DockerManager.hpp"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <vector>

DockerManager::DockerManager(const std::string &nome, const std::string &img)
    : nomeContainer(nome), immagine(img) {}

bool DockerManager::installaConApt()
{
    std::cout << "[INFO] Rilevato sistema APT (ARM64/x86_64). Installazione Docker in corso..." << std::endl;
    std::string cmd = "sudo apt-get update && sudo apt-get install -y docker.io curl";
    return system(cmd.c_str()) == 0;
}

bool DockerManager::installaConPacman()
{
    std::cout << "[INFO] Rilevato sistema Pacman (ARM64/x86_64). Installazione Docker in corso..." << std::endl;
    std::string cmd = "sudo pacman -Sy --noconfirm docker curl";
    return system(cmd.c_str()) == 0;
}

bool DockerManager::verificaOEInstallaDocker()
{
    int testDocker = system("docker --version > /dev/null 2>&1");
    if (testDocker != 0)
    {
        std::cout << "[ATTENZIONE] Docker non è installato." << std::endl;
        bool usaApt = (system("command -v apt-get > /dev/null 2>&1") == 0);
        bool usaPacman = (system("command -v pacman > /dev/null 2>&1") == 0);

        if (usaApt && !installaConApt())
            return false;
        else if (usaPacman && !installaConPacman())
            return false;
        else if (!usaApt && !usaPacman)
        {
            std::cerr << "[ERRORE] Nessun package manager supportato (apt/pacman) trovato." << std::endl;
            return false;
        }
    }

    system("sudo systemctl start docker > /dev/null 2>&1");
    system("sudo systemctl enable docker > /dev/null 2>&1");
    return true;
}

std::vector<std::string> DockerManager::elencaContainerPythonAlpine()
{
    std::vector<std::string> containers;
    std::string cmd = "docker ps -a --filter \"ancestor=python:alpine\" --format \"{{.Names}}\"";

    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        return containers;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        std::string nome(buffer);
        if (!nome.empty() && nome.back() == '\n')
        {
            nome.pop_back();
        }
        if (!nome.empty())
        {
            containers.push_back(nome);
        }
    }
    pclose(pipe);
    return containers;
}

bool DockerManager::avvia()
{
    return avviaSpecifico(nomeContainer);
}

bool DockerManager::avviaSpecifico(const std::string &nome)
{
    if (!verificaOEInstallaDocker())
        return false;

    // Controlla se il container esiste già (anche fermo)
    std::string checkExists = "docker ps -a -q -f name=^" + nome + "$ | grep -q .";
    bool esiste = (system(checkExists.c_str()) == 0);

    if (esiste)
    {
        std::cout << "[INFO] Il container '" << nome << "' esiste già. Avvio in corso..." << std::endl;
        std::string comandoStart = "docker start " + nome + " > /dev/null 2>&1";
        system(comandoStart.c_str());
        return true; // Ritorna true e prosegue subito senza chiedere nulla!
    }

    // Se non esiste, lo crea e lo avvia da zero
    std::cout << "[INFO] Creazione e avvio del nuovo container '" << nome << "'..." << std::endl;
    std::string comandoAvvio = "docker run -d --name " + nome + " -v \"$(pwd)\":/app -w /app " + immagine + " sleep infinity";

    if (system(comandoAvvio.c_str()) == 0)
    {
        std::cout << "[SUCCESSO] Container '" << nome << "' creato e avviato!" << std::endl;
        return true;
    }

    std::cerr << "[ERRORE] Impossibile avviare il container " << nome << std::endl;
    return false;
}

int DockerManager::eseguiComando(const std::string &comando)
{
    return eseguiComandoSu(nomeContainer, comando);
}

int DockerManager::eseguiComandoSu(const std::string &nome, const std::string &comando)
{
    std::string comandoCompleto = "docker exec " + nome + " " + comando;
    return system(comandoCompleto.c_str());
}

void DockerManager::chiudi()
{
    chiudiSpecifico(nomeContainer);
}

void DockerManager::chiudiSpecifico(const std::string &nome)
{
    std::cout << "\n[INFO] Chiusura container '" << nome << "'..." << std::endl;
    std::string comandoStop = "docker stop " + nome + " > /dev/null 2>&1";
    std::string comandoRm = "docker rm " + nome + " > /dev/null 2>&1";
    system(comandoStop.c_str());
    system(comandoRm.c_str());
    std::cout << "[OK] Container rimosso." << std::endl;
}