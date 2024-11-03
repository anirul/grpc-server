#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <mutex>
#include "darwin_service.pb.h"

namespace darwin {

    struct UniformEnum {
        std::vector<glm::vec4> spheres;
        std::vector<glm::vec4> colors;
    };

    enum class SoundEffectEnum {
        SOUND_EFFECT_NONE,  
        SOUND_EFFECT_BAD,
        SOUND_EFFECT_GOOD,
    };

    class WorldSimulator {
    public:
        void SetUserName(const std::string& name);
        std::string GetUserName() const;
        void UpdateData(
            const std::vector<proto::Element>& elements,
            const std::vector<proto::Character>& characters,
            double time);
        void UpdateTime();
        UniformEnum GetUniforms() const;
        UniformEnum GetCloseUniforms(
            const proto::Vector3& normal, 
            double delta_time) const;
        proto::Character GetCharacterByName(const std::string& name) const;
        void SetCharacter(const proto::Character& character);
        void RemoveCharacter(const std::string& name);
        std::string GetPotentialHit(const proto::Character& character) const;
        proto::Physic GetPlanet() const;
        bool HasCharacter(const std::string& name) const;
        SoundEffectEnum GetSoundEffect(
            const std::string& player_character_name) const;

    public:
        double GetLastServerUpdateTime() const {
            std::lock_guard l(mutex_);
            return last_server_update_time_;
        }
        void SetPlayerParameter(const proto::PlayerParameter& parameter) {
            std::lock_guard l(mutex_);
            player_parameter_ = parameter;
        }
        std::vector<proto::Element> GetElements() const {
            std::lock_guard l(mutex_);
            return elements_;
        }
        std::size_t GetElementsSize() const {
            std::lock_guard l(mutex_);
            return elements_.size();
        }
        std::vector<proto::Character> GetCharacters() const {
            std::lock_guard l(mutex_);
            return characters_;
        }
        std::size_t GetCharactersSize() const {
            std::lock_guard l(mutex_);
            return characters_.size();
        }
        proto::PlayerParameter GetPlayerParameter() const {
            std::lock_guard l(mutex_);
            return player_parameter_;
        }
        void Clear() {
            std::lock_guard l(mutex_);
            elements_.clear();
            characters_.clear();
        }

    protected:
        bool IsClose(
            const proto::Vector3& normal, 
            const proto::Vector3& position) const;
        glm::vec4 GetSphere(const proto::Physic& physic) const;
        glm::vec4 GetSphereUdpate(
            const proto::Physic& physic, 
            double delta_time) const;
        glm::vec4 GetColor(const proto::Element& element) const;
        glm::vec4 GetColor(const proto::Character& character) const;
        std::vector<proto::Element> GetGForceElementsLocked();
        void ApplyGForceAndSpeedToCharacterLocked(
            const std::vector<proto::Element>& static_elements,
            double delta_time);

    private:
        std::string name_;
        bool started_ = false;
        mutable std::mutex mutex_;
        std::vector<proto::Element> elements_;
        std::vector<proto::Character> characters_;
        proto::Character player_character_;
        double time_;
        double last_server_update_time_;
        std::chrono::time_point<std::chrono::high_resolution_clock> last_time_;
        proto::PlayerParameter player_parameter_;
        SoundEffectEnum sound_effect_ = SoundEffectEnum::SOUND_EFFECT_NONE;
    };

} // End namespace darwin.
