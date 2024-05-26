//
// Created by igork on 26/05/2024.
//

#include "bullethell_gamemanager.h"
#include "core/engine.h"
#include "bullethell_eye.h"
#include "pingpong_playerinput.h"
#include "core/graphics/text.h"
#include "core/graphics/sprite.h"
#include "bullethell_bullet.h"
#include "bullethell_bar.h"

using namespace ping_pong;


void GameManagerSystem::Run() {
    auto view = Engine::Registry().view<GameManager>();
    for(auto& mgr : view){
        auto& manager = Engine::Registry().get<GameManager>(mgr);
        auto& playerStats = Engine::Registry().get<StatsData>(manager.player);
        auto& eyeStats = Engine::Registry().get<StatsData>(manager.eye);
        if(playerStats.isDead){
            manager.playerDead = true;
            dagger::Engine::ToggleSystemsPause(true);
            auto& text = Engine::Registry().get<Text>(manager.text);
            text.Set("pixel-font", "You Died. Press p to play again.");

            auto& scoreText = Engine::Registry().get<Text>(manager.scoreText);
            scoreText.Set("pixel-font", "Score: " + std::to_string(manager.score),{0,-50,0});
            auto& bg = Engine::Registry().get<Sprite>(manager.bg);
            bg.position.z = 0;
        }
        if(eyeStats.isDead){
            eyeStats.isDead = false;
            manager.eyeDead = true;
            dagger::Engine::ToggleSystemsPause(true);
            auto& text = Engine::Registry().get<Text>(manager.text);
            text.Set("pixel-font", "You beat round " +std::to_string(manager.rounds));
            manager.rounds++;

            auto& scoreText = Engine::Registry().get<Text>(manager.scoreText);
            scoreText.Set("pixel-font", "Press n for next round.",{0,-50,0});
            auto& bg = Engine::Registry().get<Sprite>(manager.bg);
            bg.position.z = 0;
        }
        if(manager.playerClickedPlayAgain){
            manager.playerClickedPlayAgain = false;
            playerStats.hp = 5;
            playerStats.isDead = false;
            manager.playerDead = false;
            manager.rounds = 0;
            eyeStats.hp = 100;
            eyeStats.maxhp = eyeStats.hp;
            auto& bar = Engine::Registry().get<Bar>(manager.bossBar);
            bar.max = eyeStats.hp;
            eyeStats.isDead = false;
            manager.eyeDead = false;
            manager.score = 0;
            auto bulletView = Engine::Registry().view<Bullet>();
            for(auto& bullet : bulletView){
                Engine::Registry().destroy(bullet);
            }
            dagger::Engine::ToggleSystemsPause(false);
            auto& text = Engine::Registry().get<Text>(manager.text);
            text.Set("pixel-font", "");

            auto& scoreText = Engine::Registry().get<Text>(manager.scoreText);
            scoreText.Set("pixel-font", "",{0,-50,0});
            auto& bg = Engine::Registry().get<Sprite>(manager.bg);
            bg.position.z = 3;
        }
        if(manager.playerClickedNextRound){
            manager.playerClickedNextRound = false;
            playerStats.hp = 5;
            playerStats.isDead = false;
            manager.playerDead = false;
            eyeStats.hp = 100.f + 50.f*manager.rounds;
            eyeStats.maxhp = eyeStats.hp;
            auto& bar = Engine::Registry().get<Bar>(manager.bossBar);
            bar.max = eyeStats.hp;
            eyeStats.isDead = false;
            manager.eyeDead = false;
            auto bulletView = Engine::Registry().view<Bullet>();
            for(auto& bullet : bulletView){
                Engine::Registry().destroy(bullet);
            }
            dagger::Engine::ToggleSystemsPause(false);
            auto& text = Engine::Registry().get<Text>(manager.text);
            text.Set("pixel-font", "");

            auto& scoreText = Engine::Registry().get<Text>(manager.scoreText);
            scoreText.Set("pixel-font", "",{0,-50,0});
            auto& bg = Engine::Registry().get<Sprite>(manager.bg);
            bg.position.z = 3;
        }
    }
}
