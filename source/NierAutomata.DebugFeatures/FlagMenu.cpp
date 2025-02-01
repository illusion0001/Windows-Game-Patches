#include <stdint.h>
#include "helper.hpp"

#include "SDKEnums.hpp"

#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>

#pragma GCC diagnostic ignored "-Wunused-function"
#include "flags.hpp"
#pragma GCC diagnostic warning "-Wunused-function"

struct FlagEntry
{
    const char* fn;
    size_t un;
};

// expand to FlagEntry list

#define FLAG_RESERVED (1 << 31)

#define xx(n, e) {#e, (size_t) (n##_##e | FLAG_RESERVED)},
#define x(n, e) {#e, (size_t) n##_##e},

#define FLAG_MENU(flag_title, flag_cat, flag_macro, flag_name) \
    { \
        static const FlagEntry list[] = { flag_macro(flag_name) }; \
        RenderFlagMenu(flag_title " (" #flag_cat ")", FLAG_CATEGORY::flag_cat, list, _countof(list)); \
    }

static bool force_enable_reserved = false;

static void RenderFlagMenu(const char* title, FLAG_CATEGORY category, const FlagEntry* list, const size_t list_count)
{
    if (ImGui::CollapsingHeader(title))
    {
        static char hoveredFlag[FLAG_CATEGORY::MAX][128]{};
        ImGui::TextUnformatted(!*hoveredFlag[category] ? " \n " : hoveredFlag[category]);
        const int maxCol = 8;
        // TODO: Split into 8 8 rows
        // [0] [0] [0] [0] [0] [0] [0] [0]    [0] [0] [0] [0] [0] [0] [0] [0]
        // [0] [0] [0] [0] [0] [0] [0] [0]    [0] [0] [0] [0] [0] [0] [0] [0]
        // ...
        ImGui::BeginTable(title, maxCol);
        for (size_t i = 0; i < list_count; i++)
        {
            ImGui::TableNextColumn();
            BOOL disabled = list[i].un & FLAG_RESERVED;
            if (disabled)
            {
                if (force_enable_reserved)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 255, 127));
                }
                else
                {
                    ImGui::BeginDisabled(true);
                }
            }
            uint32_t rawFlag = 0;
            uint32_t* ptr = FlagPtr(category, (list[i].un & ~FLAG_RESERVED), rawFlag);
            const BOOL flagState = (*ptr & rawFlag) != 0;
            ImGui::PushID(i);
            if (ImGui::Button(flagState ? "[1]" : "[0]"))
            {
                *ptr ^= rawFlag;
            }
            ImGui::PopID();
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                _snprintf_s(hoveredFlag[category], _countof(hoveredFlag[category]), _TRUNCATE,
                            "0x%02x %03lld\n"
                            "%s%s (0x%x) (0x%p)", i, i,
                            disabled && !force_enable_reserved ? "Unused: " : "", list[i].fn, rawFlag, ptr);
                ImGui::SetTooltip("%s", hoveredFlag[category]);
            }
            if (disabled)
            {
                if (force_enable_reserved)
                {
                    ImGui::PopStyleColor(1);
                }
                else
                {
                    ImGui::EndDisabled();
                }
            }
        }
        ImGui::EndTable();
    }
}

void FlagMenu()
{
    {
        ImGui::Checkbox("Enable unknown flags", &force_enable_reserved);
        FLAG_MENU("Debug", DBG, dbg_flag_x, dbg_flag);
        FLAG_MENU("Debug Step", DBSTP, dbg_step_x, dbg_step);
        FLAG_MENU("Debug Display", DBDISP, dbgdisp_flag_x, dbgdisp);
        FLAG_MENU("Debug Graphics", DBGRAPHIC, dbg_DBGRAPHIC_flag_x, dbg_DBGRAPHIC);
        FLAG_MENU("Stats", STA, dbg_sta_flag_x, dbg_sta);
        FLAG_MENU("Stop", STOP, dbg_stp_flag_x, dbg_stp);
        FLAG_MENU("Graphics", GRAPHIC, dbg_graphic_flag_x, dbg_graphic);
        FLAG_MENU("Display", DISP, dbg_disp_flag_x, dbg_disp);
        FLAG_MENU("Gameplay", GAME, dbg_game_flag_x, dbg_game);
    }
}

#undef FLAG_RESERVED
#undef FLAG_MENU
#undef x
#undef xx
