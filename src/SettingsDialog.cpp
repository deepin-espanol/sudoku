#include "SettingsDialog.h"
#include "SoundManager.h"

#include <DSwitchButton>
#include <DSlider>
#include <DLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

SettingsDialog::SettingsDialog(SoundManager *soundManager, QWidget *parent)
    : DDialog(parent), m_soundManager(soundManager)
{
    setTitle(tr("Preferencias"));
    setFixedWidth(360);
    setupUI();
}

void SettingsDialog::setupUI()
{
    auto *mainWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(mainWidget);
    layout->setSpacing(16);
    layout->setContentsMargins(20, 20, 20, 20);

    auto *audioGroup = new QVBoxLayout();
    auto *audioLabel = new DLabel(tr("Audio"));
    QFont audioFont = audioLabel->font();
    audioFont.setBold(true);
    audioLabel->setFont(audioFont);
    audioGroup->addWidget(audioLabel);

    auto *muteRow = new QHBoxLayout();
    auto *muteLabel = new DLabel(tr("Silenciar sonido"));
    auto *muteSwitch = new DSwitchButton();
    muteSwitch->setChecked(m_soundManager->isMuted());
    connect(muteSwitch, &DSwitchButton::checkedChanged, this, [this](bool checked) {
        m_soundManager->setMuted(checked);
    });
    muteRow->addWidget(muteLabel);
    muteRow->addStretch();
    muteRow->addWidget(muteSwitch);
    audioGroup->addLayout(muteRow);

    auto *volRow = new QHBoxLayout();
    auto *volLabel = new DLabel(tr("Volumen"));
    auto *volSlider = new DSlider(Qt::Horizontal);
    volSlider->setMinimum(0);
    volSlider->setMaximum(100);
    volSlider->setValue(static_cast<int>(m_soundManager->volume() * 100));
    connect(volSlider, &DSlider::valueChanged, this, [this](int value) {
        m_soundManager->setVolume(value / 100.0);
    });
    volRow->addWidget(volLabel);
    volRow->addWidget(volSlider);
    audioGroup->addLayout(volRow);

    layout->addLayout(audioGroup);
    layout->addStretch();

    addContent(mainWidget);

    addButton(tr("Cerrar"));
    connect(this, &DDialog::accepted, this, &QDialog::accept);
}
