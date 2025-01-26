#include <stdint.h>
#include "helper.hpp"

#include "SDKEnums.hpp"

#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>

#pragma GCC diagnostic ignored "-Wunused-function"
#include "flags.hpp"
#pragma GCC diagnostic warning "-Wunused-function"

#include "FlagMenu.hpp"

bool dont_pause_in_menu = false;

static void set_pause(BOOL v)
{
    constexpr uint32_t STA_PAUSE_flag = GetRawFlag(dbg_sta_STA_PAUSE);
    constexpr uint32_t DBSTP_SCENE_flag = GetRawFlag(dbg_step_DBSTP_SCENE);
    uint32_t* ptr = FlagPtr(STA, dbg_sta_STA_PAUSE);
    uint32_t* ptr2 = FlagPtr(DBSTP, dbg_step_DBSTP_SCENE);
    if (v)
    {
        *ptr |= STA_PAUSE_flag;
        *ptr2 |= DBSTP_SCENE_flag;
    }
    else
    {
        *ptr &= ~STA_PAUSE_flag;
        *ptr2 &= ~DBSTP_SCENE_flag;
    }
}

void set_paused_flag(BOOL v)
{
    if (!dont_pause_in_menu)
    {
        set_pause(v);
    }
}

static void DemoMenu()
{
    ImGui::ShowDemoWindow();
}

void RenderMenu()
{
    if (ImGui::Begin("Menu"))
    {
        ImGui::Checkbox("Pause when opened", &dont_pause_in_menu);
        if (!dont_pause_in_menu)
        {
            ImGui::BeginDisabled(true);
        }
        if (ImGui::Button("Unpause Now"))
        {
            set_pause(false);
        }
        if (ImGui::Button("Pause Now"))
        {
            set_pause(true);
        }
        if (!dont_pause_in_menu)
        {
            ImGui::EndDisabled();
        }
    }
    ImGui::End();
    FlagMenu();
    if (0)
    {
        DemoMenu();
    }
}
