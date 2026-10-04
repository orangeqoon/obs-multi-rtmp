#include "pch.h"

#include <QVBoxLayout>
#include <QCursor>

#include <chrono>
#include <ctime>
#include <vector>

#include "emergency-stop-widget.h"
#include "dock-registry.h"
#include "websocket-api.h"
#include "output-config.h"

namespace {

bool TargetIsActive(PushWidget* t)
{
    return t->IsRunning() || t->IsConnecting() || t->IsReconnecting();
}

std::string NowLocalIso8601()
{
    using clock = std::chrono::system_clock;
    auto now = clock::now();
    std::time_t tt = clock::to_time_t(now);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &tt);
#else
    localtime_r(&tt, &local);
#endif
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &local);

    char off[8];
    std::strftime(off, sizeof(off), "%z", &local);
    std::string offset = off;
    if (offset.size() == 5)
        offset.insert(3, ":");

    return std::string(buf) + offset;
}

} // namespace

EmergencyStopWidget::EmergencyStopWidget(QWidget* parent, bool showHideDockCheckbox)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);

    button_ = new QPushButton(this);
    button_->setMinimumHeight(40);
    button_->setCursor(Qt::PointingHandCursor);
    layout->addWidget(button_);

    if (showHideDockCheckbox) {
        hideDockCheck_ = new QCheckBox(obs_module_text("Setting.HideDock"), this);
        hideDockCheck_->setChecked(GlobalMultiOutputConfig().hideDock);
        layout->addWidget(hideDockCheck_);
        QObject::connect(hideDockCheck_, &QCheckBox::toggled, this, [this](bool checked) {
            GlobalMultiOutputConfig().hideDock = checked;
            SaveMultiOutputConfig();
            if (hideDockChanged_)
                hideDockChanged_(checked);
        });
    }

    longPressTimer_ = new QTimer(this);
    longPressTimer_->setSingleShot(true);
    longPressTimer_->setInterval(1000);
    QObject::connect(longPressTimer_, &QTimer::timeout, this, [this]() { onLongPressFired(); });

    refreshTimer_ = new QTimer(this);
    refreshTimer_->setInterval(250);
    QObject::connect(refreshTimer_, &QTimer::timeout, this, [this]() { refresh(); });
    refreshTimer_->start();

    QObject::connect(button_, &QPushButton::pressed, this, [this]() { onPressed(); });
    QObject::connect(button_, &QPushButton::released, this, [this]() { onReleased(); });

    refresh();
}

void EmergencyStopWidget::setHideDockChecked(bool hide)
{
    if (hideDockCheck_ && hideDockCheck_->isChecked() != hide) {
        hideDockCheck_->blockSignals(true);
        hideDockCheck_->setChecked(hide);
        hideDockCheck_->blockSignals(false);
    }
}

void EmergencyStopWidget::applyAppearance(bool anyActive, int activeCount)
{
    if (anyActive) {
        button_->setText(QString(obs_module_text("EmergencyStop.Streaming")).arg(activeCount));
        button_->setStyleSheet(
            "QPushButton { background-color: #1e6cff; color: white; font-weight: bold; border: none; border-radius: 4px; padding: 8px; }"
            "QPushButton:pressed { background-color: #1554cc; }");
        button_->setToolTip(obs_module_text("EmergencyStop.HoldHint"));
    } else {
        button_->setText(obs_module_text("EmergencyStop.AllStopped"));
        button_->setStyleSheet(
            "QPushButton { background-color: #c62828; color: white; font-weight: bold; border: none; border-radius: 4px; padding: 8px; }"
            "QPushButton:disabled { background-color: #c62828; color: white; }");
        button_->setToolTip(obs_module_text("EmergencyStop.IdleHint"));
    }
    button_->setEnabled(true);
}

void EmergencyStopWidget::refresh()
{
    int active = 0;
    for (auto* t : GetAllStreamTargets()) {
        if (TargetIsActive(t))
            ++active;
    }
    applyAppearance(active > 0, active);
}

void EmergencyStopWidget::onPressed()
{
    int active = 0;
    for (auto* t : GetAllStreamTargets()) {
        if (TargetIsActive(t))
            ++active;
    }
    pressArmed_ = active > 0;
    if (pressArmed_)
        longPressTimer_->start();
}

void EmergencyStopWidget::onReleased()
{
    longPressTimer_->stop();
    pressArmed_ = false;
}

void EmergencyStopWidget::onLongPressFired()
{
    if (!pressArmed_)
        return;
    pressArmed_ = false;

    std::vector<std::string> stoppedIds;
    for (auto* t : GetAllStreamTargets()) {
        if (TargetIsActive(t))
            stoppedIds.push_back(t->GetTargetId());
        t->ForceStopStreaming();
    }

    nlohmann::json ids = nlohmann::json::array();
    for (auto& id : stoppedIds)
        ids.push_back(id);

    NotifyEmergencyStop(NowLocalIso8601(), ids, static_cast<int>(stoppedIds.size()));
    refresh();
}