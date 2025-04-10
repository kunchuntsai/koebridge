#include "settings_dialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("KoeBridge Settings"));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    audioDeviceCombo = new QComboBox(this);
    audioDeviceCombo->addItem("Default Audio Device");

    whisperModelPath = new QLineEdit(this);
    whisperModelPath->setText("_dataset/models/whisper-tiny.bin");

    translationModelPath = new QLineEdit(this);
    translationModelPath->setText("_dataset/models/nllb-small.bin");

    formLayout->addRow(tr("Audio Device:"), audioDeviceCombo);
    formLayout->addRow(tr("Whisper Model:"), whisperModelPath);
    formLayout->addRow(tr("Translation Model:"), translationModelPath);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* saveButton = new QPushButton(tr("Save"), this);
    QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);

    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    resize(400, 200);
}

void SettingsDialog::saveSettings() {
    // Save settings logic
    accept();
}
