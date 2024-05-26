#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace ping_pong
{

    struct GameManager{
        Entity player;
        Entity eye;
        Entity text;
        Entity scoreText;
        Entity bg;
        Entity bossBar;
        int rounds {0};
        int basePlayerHP {5};
        int baseEyeHP {100};
        int score {0};
        bool playerDead{false};
        bool eyeDead{false};
        bool playerClickedPlayAgain{false};
        bool playerClickedNextRound{false};
    };

    class GameManagerSystem
            : public System
    {
    public:

        inline String SystemName() { return "GameManager System"; }

        void Run() override;
    };
}