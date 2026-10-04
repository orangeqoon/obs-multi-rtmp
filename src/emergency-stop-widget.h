#pragma once

#include <functional>

#include <QWidget>

class QPushButton;
class QCheckBox;
class QTimer;

// Small panel with the D7 emergency-stop button (1-second long press while
// any target is live/connecting/reconnecting). Optionally shows the
// "hide destination dock" checkbox used for headless/marust-driven mode.
class EmergencyStopWidget : public QWidget {
public:
    explicit EmergencyStopWidget(QWidget* parent = nullptr, bool showHideDockCheckbox = true);

    void setHideDockChangedCallback(std::function<void(bool)> cb) { hideDockChanged_ = std::move(cb); }
    void refresh();
    void setHideDockChecked(bool hide);

private:
    void onPressed();
    void onReleased();
    void onLongPressFired();
    void applyAppearance(bool anyActive, int activeCount);

    QPushButton* button_ = nullptr;
    QCheckBox* hideDockCheck_ = nullptr;
    QTimer* longPressTimer_ = nullptr;
    QTimer* refreshTimer_ = nullptr;
    bool pressArmed_ = false;
    std::function<void(bool)> hideDockChanged_;
};