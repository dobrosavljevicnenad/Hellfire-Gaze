#include "bullethell_dash.h"
#include "core/engine.h"
#include "core/audio.h"

#include "gameplay/ping_pong/bullethell_dash.h"
#include "gameplay/ping_pong/pingpong_playerinput.h"
#include "core/game/transforms.h"

void ping_pong::DashSystem::Run() {
    auto view = Engine::Registry().view<DashData, MovementData, ControllerMapping>();

    for(auto& entity : view){
        auto& dash = Engine::Registry().get<DashData>(entity);
        auto& mov = Engine::Registry().get<MovementData>(entity);
        auto& ctrl = Engine::Registry().get<ControllerMapping>(entity);

        float boost = ctrl.doubleTap.x * dash.boost;
//        if(dash.canDash && std::abs(ctrl.doubleTap.x) < 1e-4)
//            dash.canDash = false;

        if(dash.dashTimer < dash.dashCooldown)
            dash.dashTimer += Engine::DeltaTime();
        if(dash.dashTimer > dash.dashCooldown){
            dash.dashTimer = dash.dashCooldown;
            Engine::GetDefaultResource<Audio>()->Play("dash_reload");
        }

        if(!dash.canDash && std::abs(dash.dashTimer - dash.dashCooldown) < 1e-4){
            dash.canDash = true;
        }

        if(!dash.canDash){
            ctrl.doubleTap.x = 0;
        }

        if (dash.canDash && std::abs(ctrl.doubleTap.x) < 1e-4){
            dash.canDash = false;
        }
        if(std::abs(ctrl.doubleTap.x) > 1e-4){
            mov.angle += boost;
            dash.dashTimer = 0.f;
        }
        if(std::abs(ctrl.doubleTap.x) == 1.f){
            Engine::GetDefaultResource<Audio>()->Play("dash");
        }
    }
}
