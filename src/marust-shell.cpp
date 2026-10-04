#include "pch.h"

#include <QAction>
#include <QDockWidget>

#include "marust-shell.h"
#include "output-config.h"

namespace {

QAction* s_showListAction = nullptr;
QDockWidget* s_emergencyDockFrame = nullptr;
bool s_syncingUi = false;

void OnShowListToggled(bool checked)
{
    if (s_syncingUi)
        return;
    // checked = show destination list = not headless
    const bool hideDock = !checked;
    GlobalMultiOutputConfig().hideDock = hideDock;
    SaveMultiOutputConfig();
    ApplyDockVisibility(hideDock);
}

} // namespace

void RegisterMarustShell(QDockWidget* emergencyDockFrame)
{
    s_emergencyDockFrame = emergencyDockFrame;

    auto* action = static_cast<QAction*>(
        obs_frontend_add_tools_menu_qaction(obs_module_text("Marust.ShowDestinationDock")));
    if (action) {
        action->setCheckable(true);
        action->setChecked(!GlobalMultiOutputConfig().hideDock);
        QObject::connect(action, &QAction::toggled, OnShowListToggled);
        s_showListAction = action;
    }

    SyncMarustShellUi(GlobalMultiOutputConfig().hideDock);
}

void SyncMarustShellUi(bool hideDock)
{
    s_syncingUi = true;
    if (s_showListAction)
        s_showListAction->setChecked(!hideDock);
    s_syncingUi = false;

    if (s_emergencyDockFrame) {
        // Headless: the only remaining dock is labeled as the marust engine.
        // Non-headless: restore the emergency-stop title (dock is hidden).
        s_emergencyDockFrame->setWindowTitle(
            hideDock ? QString::fromUtf8(obs_module_text("Marust.EngineDockTitle"))
                     : QString::fromUtf8(obs_module_text("EmergencyStop.Title")));
    }
}