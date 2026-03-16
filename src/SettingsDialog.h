#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <DDialog>

DWIDGET_USE_NAMESPACE

class SoundManager;

class SettingsDialog : public DDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(SoundManager *soundManager, QWidget *parent = nullptr);

private:
    void setupUI();

    SoundManager *m_soundManager = nullptr;
};

#endif
