#include "state_server.h"

#include <memory>
#include <algorithm>

#include "frame/gui/draw_gui_interface.h"
#include "state_context.h"
#include "state_disconnected.h"
#include "state_title.h"
#include "state_ping.h"
#include "overlay_state.h"

namespace darwin::state {

    void StateServer::Enter(const proto::ClientParameter& client_parameter) {
        logger_->info("Entering server state");
        audio_system_.PlayMusic(proto::AUDIO_MUSIC_MENU);
        client_parameter_ = client_parameter;
        modal_server_params_.server_name = client_parameter_.server_name();
        for (auto* plugin : app_.GetWindow().GetDevice().GetPluginPtrs()) {
            logger_->info(
                "\tPlugin: [{}] {}", 
                (std::uint64_t)plugin, 
                plugin->GetName().c_str());
            if (!draw_gui_interface_) {
                draw_gui_interface_ =
                    dynamic_cast<frame::gui::DrawGuiInterface*>(
                        plugin);
            }
        }
        if (!draw_gui_interface_) {
            throw std::runtime_error("No draw gui interface plugin found?");
        }
        auto overlay_state = std::make_unique<overlay::OverlayState>(
            "overlay_state",
            client_parameter_,
            client_parameter_.overlay_state(),
            audio_system_);
        overlay_state->SetStateName("state server");
        draw_gui_interface_->AddOverlayWindow(
            glm::vec2(0.0f, 0.0f),
            app_.GetWindow().GetDevice().GetSize(),
            std::move(overlay_state));
        draw_gui_interface_->AddModalWindow(
            std::make_unique<modal::ModalServer>(
                "Select Server",
                modal_server_params_));
    }

    void StateServer::Update(StateContext& state_context) {
        if (modal_server_params_.button_result != 
            modal::ModalServerButton::None) {
            switch (modal_server_params_.button_result) {
            case modal::ModalServerButton::Connect:
                client_parameter_.set_is_ssl_enable(
                    modal_server_params_.ssl_enable);
                darwin_client_ = 
                    std::make_unique<DarwinClient>(
                        modal_server_params_.server_name,
                        client_parameter_);
                state_context.ChangeState(
                    std::make_unique<StatePing>(
                        app_,
                        audio_system_,
                        std::move(darwin_client_)));
                break;
            case modal::ModalServerButton::Cancel:
                state_context.ChangeState(
                    std::make_unique<StateTitle>(app_, audio_system_));
                break;
            }
        }
    }

    void StateServer::Exit() {
        logger_->info("Exit server state");
        draw_gui_interface_->DeleteWindow("overlay_state");
    }

} // namespace darwin::state.
