#pragma once

class QDockWidget;

// Tools-menu escape hatch from headless, and emergency-dock title when
// headless. Call RegisterMarustShell once after docks exist; call
// SyncMarustShellUi whenever hide_dock changes (from ApplyDockVisibility).

void RegisterMarustShell(QDockWidget* emergencyDockFrame);
void SyncMarustShellUi(bool hideDock);

// Declared here so marust-shell can toggle the same path as the dock checkbox.
void ApplyDockVisibility(bool hideDock);