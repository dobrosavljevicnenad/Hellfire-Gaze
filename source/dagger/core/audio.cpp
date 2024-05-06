#include "audio.h"

#include <core/core.h>
#include <core/engine.h>
#include <set>
#include <string>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

using namespace dagger;

static ma_engine engine;
static std::unordered_map<std::string, std::set<ma_sound*>> active_sounds;

void Audio::Initialize() {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        Logger::critical("Failed to initialize sound engine");
    }
    Logger::info("Initialized sound engine");
}

void Audio::Load(AssetLoadRequest<Sound> request_)
{

    FilePath path{ request_.path };
    String name = path.stem().string();
    auto& sounds = Engine::Res<Sound>();



    FilePath root{ request_.path };
    root.remove_filename();

    String soundName = "";
    {
        String pathName = root.append(path.stem().string()).string();
        if (pathName.find("sounds") == 0)
            pathName = pathName.substr(7, pathName.length() - 4);

        std::replace(pathName.begin(), pathName.end(), '/', ':');
        std::replace(pathName.begin(), pathName.end(), '\\', ':');
        soundName = pathName;
    }

    if (!sounds.contains(soundName))
        sounds[soundName] = new Sound();
    auto* sound = sounds[soundName];

    sound->name = soundName;
    Logger::info("Loading {}", request_.path.c_str());
    sound->path = request_.path;

    Logger::info("Loaded sound {}", sound->name);
}

// Callback function called by MiniAudio when the sound playback ends
//void onSoundEnded(void* pUserData, ma_sound* sound) {
//
//}

void gc() {
    for (auto& [name, sound_list] : active_sounds) {
        for (auto it = sound_list.begin(); it != sound_list.end();) {
            ma_sound* sound = *it;
            if (!ma_sound_is_playing(sound)) {
                ma_sound_stop(sound);
                ma_sound_uninit(sound);
                it = sound_list.erase(it);
                delete sound;
            } else {
                ++it;
            }
        }
    }
}

void Audio::Play(String name_)
{
    gc();
    auto& sounds = Engine::Res<Sound>();
    assert(sounds.contains(name_));

    ma_result result;
    ma_sound* masound = new ma_sound;
    result = ma_sound_init_from_file(&engine, sounds[name_]->path.c_str(), MA_SOUND_FLAG_ASYNC, NULL, NULL, masound);
    if(result != MA_SUCCESS){
        Logger::critical("Failed to load {} {}", name_, sounds[name_]->path.c_str());
    }
    Logger::info("Loaded {} {}", name_, sounds[name_]->path.c_str());
    active_sounds[name_].insert(masound);
    ma_sound_start(masound);
//    ma_sound_set_end_callback(masound, &onSoundEnded, NULL);
}

void Audio::PlayLoop(String name_)
{
    gc();
    auto& sounds = Engine::Res<Sound>();
    assert(sounds.contains(name_));

    ma_result result;
    ma_sound* masound = new ma_sound;
    result = ma_sound_init_from_file(&engine, sounds[name_]->path.c_str(), MA_SOUND_FLAG_ASYNC, NULL, NULL, masound);
    if(result != MA_SUCCESS){
        Logger::critical("Failed to load {} {}", name_, sounds[name_]->path.c_str());
    }
    Logger::info("Loaded {} {}", name_, sounds[name_]->path.c_str());
    active_sounds[name_].insert(masound);
    ma_sound_set_looping(masound, true);
    ma_sound_set_volume(masound, 0.3f);
    ma_sound_start(masound);
//    ma_sound_set_end_callback(masound, &onSoundEnded, NULL);


}

void Audio::Stop()
{
    for(auto [name, sound_type] : active_sounds){
        for(ma_sound* sound : sound_type){
            ma_sound_stop(sound);
            ma_sound_uninit(sound);
        }
    }

    for(auto [name, sound_type] : active_sounds){
        sound_type.clear();
    }
}
void Audio::Stop(String name_)
{
    for(ma_sound* sound : active_sounds[name_]){
        ma_sound_stop(sound);
        ma_sound_uninit(sound);
    }
    active_sounds[name_].clear();
}

void Audio::Uninitialize() {
    for (auto sound : Engine::Res<Sound>())
    {
        delete sound.second;
    }
    Engine::Res<Sound>().clear();

    ma_engine_uninit(&engine);
}


void AudioSystem::OnLoadAsset(AssetLoadRequest<Sound> request_)
{
    Engine::GetDefaultResource<Audio>()->Load(request_);
}

void AudioSystem::SpinUp()
{
    Engine::PutDefaultResource<Audio>(new Audio());
    auto* audio = Engine::GetDefaultResource<Audio>();
    audio->Initialize();

    Engine::Dispatcher().sink<AssetLoadRequest<Sound>>().connect<&AudioSystem::OnLoadAsset>(this);

    if (Files::exists("sounds") && Files::is_directory("sounds"))
    {
        for (auto& entry : Files::recursive_directory_iterator("sounds"))
        {
            if (entry.path().extension() == ".wav")
                Engine::Dispatcher().trigger<AssetLoadRequest<Sound>>(AssetLoadRequest<Sound>{ entry.path().string() });
        }
    }
}

void AudioSystem::WindDown()
{
    auto* audio = Engine::GetDefaultResource<Audio>();
    delete Engine::GetDefaultResource<Audio>();
}