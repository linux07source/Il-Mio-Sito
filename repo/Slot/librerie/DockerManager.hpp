#ifndef DOCKER_MANAGER_HPP
#define DOCKER_MANAGER_HPP

#include <string>
#include <vector>

class DockerManager
{
private:
    std::string nomeContainer;
    std::string immagine;

    // Metodi interni di controllo e autoinstallazione
    bool verificaOEInstallaDocker();
    bool installaConApt();
    bool installaConPacman();

public:
    // Costruttori
    DockerManager(const std::string &nome, const std::string &img = "python:alpine");

    // Metodi principali di gestione container
    bool avvia();
    bool avviaSpecifico(const std::string &nome);

    // Esecuzione comandi all'interno del container
    int eseguiComando(const std::string &comando);
    int eseguiComandoSu(const std::string &nome, const std::string &comando);

    // Chiusura e pulizia
    void chiudi();
    void chiudiSpecifico(const std::string &nome);

    // Utility per elencare i container
    static std::vector<std::string> elencaContainerPythonAlpine();
};

#endif