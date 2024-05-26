#include "ping_pong_main.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"
#include "core/graphics/gui.h"
#include "core/audio.h"
#include "core/graphics/text.h"
#include "tools/diagnostics.h"

#include "gameplay/common/simple_collisions.h"
#include "gameplay/ping_pong/pingpong_playerinput.h"
#include "gameplay/ping_pong/bullethell_eye.h"
#include "gameplay/ping_pong/bullethell_bullet.h"
#include "gameplay/ping_pong/bullethell_bar.h"
#include "gameplay/ping_pong/bullethell_dash.h"
#include "gameplay/ping_pong/bullethell_gamemanager.h"


using namespace dagger;
using namespace ping_pong;


void PingPongGame::CoreSystemsSetup()
{
    auto& engine = Engine::Instance();

    engine.AddSystem<WindowSystem>();
    engine.AddSystem<AudioSystem>();
    engine.AddSystem<InputSystem>();
    engine.AddSystem<ShaderSystem>();
    engine.AddSystem<TextureSystem>();
    engine.AddSystem<SpriteRenderSystem>();
    engine.AddPausableSystem<TransformSystem>();
    engine.AddPausableSystem<AnimationSystem>();

#if !defined(NDEBUG)
    engine.AddSystem<DiagnosticSystem>();
    engine.AddSystem<GUISystem>();
    engine.AddSystem<ToolMenuSystem>();
#endif //!defined(NDEBUG)
}

void PingPongGame::GameplaySystemsSetup()
{
    auto& engine = Engine::Instance();

    engine.AddPausableSystem<SimpleCollisionsSystem>();
    engine.AddSystem<PingPongPlayerInputSystem>();
    engine.AddPausableSystem<EyeSystem>();
    engine.AddPausableSystem<DamageSystem>();
    engine.AddPausableSystem<BulletSystem>();
    engine.AddPausableSystem<BarSystem>();
    engine.AddPausableSystem<DashSystem>();
    engine.AddSystem<GameManagerSystem>();

#if defined(DAGGER_DEBUG)

#endif //defined(DAGGER_DEBUG)
}

void PingPongGame::WorldSetup()
{
    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 800, 600 };
    camera->zoom = 1;
    camera->position = { 0, 0, 0 };
    camera->Update();

    SetupWorld();
    Engine::GetDefaultResource<Audio>()->PlayLoop("battle_music2");
}

void ping_pong::SetupWorld()
{
    Vector2 scale(1, 1);

    auto& engine = Engine::Instance();
    auto& reg = engine.Registry();

    // field
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;

    constexpr int height = 20;
    constexpr int width = 26;
    constexpr float tileSize = 20.f;// / static_cast<float>(Width);
    constexpr float player_width = 32.f;
    constexpr float player_height = 48.f;

    float zPos = 1.f;

    constexpr float Space = 0.1f;

//    float zPos = 1.f;

    auto gameManagerEnt = reg.create();
    auto& game = reg.emplace<GameManager>(gameManagerEnt);
    auto ui = reg.create();
    auto& text = reg.emplace<Text>(ui);
    text.spacing = 0.6f;
    text.Set("pixel-font", "");
    game.text=ui;

    auto scoreUi = reg.create();
    auto& text2 = reg.emplace<Text>(scoreUi);
    text2.spacing = 0.6f;
    text2.Set("pixel-font", "");
    game.scoreText=scoreUi;

    auto bg = reg.create();
    auto& spr = reg.emplace<Sprite>(bg);
    game.bg = bg;
    AssignSprite(spr,"EmptyWhitePixel");
    spr.scale = {1000,1000};
    spr.color = {0.f,0.f,0.f,1.f};
    spr.position ={0,0,3};

    // player controller setup
    const Float32 playerSize = tileSize * ((height - 2) * (1 + Space) * 0.33f);
    PingPongPlayerInputSystem::SetupPlayerBoarders(playerSize, -playerSize);
//    PingPongPlayerInputSystem::s_PlayerSpeed = tileSize * 14.f;
    //1st player
    {
        auto entity = reg.create();
        game.player = entity;
        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size.x = 20;
        col.size.y = 20;

        auto& transform = reg.emplace<Transform>(entity);
        transform.position.x = 0;
        transform.position.y = 0;
        transform.position.z = zPos;

        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "spritesheets:touhou:reimu_idle_anim:1");
        sprite.size.x = player_width;
        sprite.size.y = player_height;

        auto &anim = reg.emplace<Animator>(entity);
        AnimatorPlay(anim, "bullet_hell:player:IDLE");
//        sprite.scale = {10, 1};
        reg.emplace<EyeTarget>(entity);

        auto& controller = reg.emplace<ControllerMapping>(entity);

        auto& mov = reg.emplace<MovementData>(entity);

        auto& dash = reg.emplace<DashData>(entity);
        dash.dashCooldown = 3.f;
        dash.dashTimer = 3.f;
        dash.canDash = true;

        createBar(&dash.dashTimer, {-250, -180, 0}, 200, 50, {0,0,1,1}, dash.dashCooldown);

        auto& stats = reg.emplace<StatsData>(entity);
        stats.hp = 5;

        createBar(&stats.hp, {-250, -250, 0}, 200, 50, {1,0,0,1}, stats.hp);

        reg.emplace<Damageable>(entity);

        PingPongPlayerInputSystem::SetupPlayerOneInput(controller);
    }
    //Eye
    {
        auto entity = reg.create();
        game.eye = entity;
        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size.x = tileSize;
        col.size.y = tileSize;

        reg.emplace<Eye>(entity);

        auto& transform = reg.emplace<Transform>(entity);
        transform.position.x = 0;
        transform.position.y = 0;
        transform.position.z = zPos;

        auto& stats = reg.emplace<StatsData>(entity);
        stats.hp = 1e2;

        game.bossBar = createBar(&stats.hp, {0, 250, 0},400, 50, {1,0,0,1},stats.hp);

        auto& dmg = reg.emplace<Damageable>(entity);
        dmg.hurtDuration = .2f;
        dmg.hurtSound = "eye_hurt";

        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "bullet_hell:eye:IDLE:idle1");
        sprite.size.x = tileSize;
        sprite.size.y = tileSize;
        sprite.scale = {5, 5};

        auto &anim = reg.emplace<Animator>(entity);
        AnimatorPlay(anim, "bullet_hell:eye:IDLE");

    }



}


