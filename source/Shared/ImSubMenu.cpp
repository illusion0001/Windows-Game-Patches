#ifdef __IM_SUBMENU__

// #include <stdlib.h> // `_countof`
#define _countof(array) (sizeof(array) / sizeof(array[0]))
#include <imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include "ImSubMenu.hpp"

constexpr size_t MAX_SUBMENUS = 64;

static size_t g_submenuCount = 0;

static SubmenuCb g_submenuStack[MAX_SUBMENUS]{};
static char g_subMenutitle[256]{};

static bool wantExit()
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) ||
        ImGui::IsKeyPressed(ImGuiKey_Backspace) ||
        ImGui::IsKeyPressed(ImGuiKey_Escape) ||
        ImGui::IsKeyPressed(ImGuiKey_GamepadFaceRight))
    {
        if (g_submenuCount > 0)
        {
            g_submenuStack[g_submenuCount] = nullptr;
            --g_submenuCount;
        }
        else
        {
            g_submenuCount = 0;
        }
        return true;
    }
    return false;
}

void ShowSubmenu(const char* subtitle, const char* title, SubmenuCb cb)
{
    if (ImGui::Selectable(title) && g_submenuCount < MAX_SUBMENUS)
    {
        memset(g_subMenutitle, 0, sizeof(g_subMenutitle));
        if (subtitle && *subtitle)
        {
            strncpy_s(g_subMenutitle, _countof(g_subMenutitle), subtitle, _TRUNCATE);
        }
        g_submenuStack[g_submenuCount++] = cb;
    }
}

static void RenderMenuCallback()
{
    if (wantExit())
    {
        return;
    }

    if (*g_subMenutitle)
    {
        // show title
        ImGui::TextUnformatted(g_subMenutitle);
    }

    if (g_submenuCount > 0 && g_submenuStack[g_submenuCount - 1])
    {
        // run recent callback
        g_submenuStack[g_submenuCount - 1]();
    }
}

void RenderMenu(const char* menuTitle)
{
    if (ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
    {
#if _DEBUG
        ImGui::Text("g_submenuCount: %lld", g_submenuCount);
#endif
        if (g_submenuCount == 0)
        {
            ImGui::TextUnformatted(menuTitle);
            RenderMenus();
        }
        else
        {
            RenderMenuCallback();
        }
    }
    ImGui::End();
}

#endif
