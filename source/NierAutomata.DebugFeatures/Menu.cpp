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
    ImGui::ShowStyleEditor();
}

int g_DamageMode = 0;
int g_DamageMul = 4;
int g_DamageConst = 1;

void RenderMenu()
{
    if (ImGui::Begin("Menu"))
    {
        ImGui::DragFloat("global scale", &ImGui::GetIO().FontGlobalScale, 0.005f, 0.1f, 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything

        uint32_t* ptr = 0;
        bool cheat_enabled = CheckFlag(DBG, dbg_flag_DBG_EM_MANY_DAMAGE, &ptr);
        if (ptr)
        {
            ImGui::CheckboxFlags("DBG_EM_MANY_DAMAGE", ptr, GetRawFlag(dbg_flag_DBG_EM_MANY_DAMAGE));
            ImGui::CheckboxFlags("DBG_PLAYER_INVINCIBLE", FlagPtr(DBG, dbg_flag_DBG_PLAYER_INVINCIBLE), GetRawFlag(dbg_flag_DBG_PLAYER_INVINCIBLE));
            if (!cheat_enabled)
            {
                ImGui::BeginDisabled(true);
            }
            ImGui::RadioButton("Normal", &g_DamageMode, 0);
            ImGui::RadioButton("Multiply", &g_DamageMode, 1);
            ImGui::RadioButton("Constant", &g_DamageMode, 2);
            ImGui::Text("g_DamageMode: %d", g_DamageMode);
            ImGui::SliderInt("g_DamageMul", &g_DamageMul, 1, 10000);
            ImGui::SliderInt("g_DamageConst", &g_DamageConst, 1, 1024 * 1024);
            if (!cheat_enabled)
            {
                ImGui::EndDisabled();
            }
        }
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
