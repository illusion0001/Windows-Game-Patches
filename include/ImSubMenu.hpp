#pragma once

#ifdef __IM_SUBMENU__

typedef void(*SubmenuCb)(void);

void RenderMenu(const char* menuTitle);
void ShowSubmenu(const char* subtitle, const char* title, SubmenuCb cb);

void RenderMenus();

#endif
