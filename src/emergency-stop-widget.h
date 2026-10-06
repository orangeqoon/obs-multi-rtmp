#pragma once

#include <QWidget>

class QPushButton;
class QTimer;

// Small panel with the D7 emergency-stop button (1-second long press while
// any target is live/connecting/reconnecting). This is the only UI of the
// plugin (the destination list dock is gone; marust drives the engine).
class EmergencyStopWidget : public QWidget {
public:
    explicit EmergencyStopWidget(QWidget* parent = nullptr);

    void refresh();

private:
    void onPressed();
    void onReleased();
    void onLongPressFired();
    void applyAppearance(bool anyActive, int activeCount);

    QPushButton* button_ = nullptr;
    QTimer* longPressTimer_ = nullptr;
    QTimer* refreshTimer_ = nullptr;
    bool pressArmed_ = false;
};