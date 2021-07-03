#include "base.h"
#include "interface.h"

void
loadConfig(const std::string& configPath, Config *config)
{
    config->defaultScene = "MainMenu";
}

void
call(GameState *state, Storage *storage, const System f, const Entity id)
{
    Signature entSig = storage->entitySignatures[id];
    Signature sysSig = storage->systemSignature[f];
    if ((entSig & sysSig) == sysSig)
    {
        f(state, storage, id);
    }
}

void
updateState(GameState *state, Storage *storage)
{
    for (Entity id : storage->usedIds)
    {
        for (System system : storage->systemsArray)
        {
            call(state, storage, system, id);
        }
    }
}

int
main()
{
    Config config;
    loadConfig("data/config", &config);

    Storage storage;
    GameState state {};
    initializeEngine(&state, &storage);
    loadScene(&config, config.defaultScene, &state, &storage);

    while (state.running)
    {
        updateState(&state, &storage);
    }

    return 0;
}