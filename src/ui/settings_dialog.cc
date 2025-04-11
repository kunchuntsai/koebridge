#include "settings_dialog.h"
#include "utils/config.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QAudioInput>
#include <QMediaDevices>
#include <QStandardPaths>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , settings("KoeBridge", "KoeBridge")
{
    setupUI();
    loadSettings();
}

SettingsDialog::~SettingsDialog() {
    // Settings are automatically saved when the dialog is closed
}

void SettingsDialog::setupUI() {
    setWindowTitle(tr("KoeBridge Settings"));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    // Audio device selection
    audioDeviceCombo = new QComboBox(this);
    populateAudioDevices();

    // Model paths
    whisperModelPath = new QLineEdit(this);
    translationModelPath = new QLineEdit(this);

    // Add browse buttons for model paths
    QHBoxLayout* whisperLayout = new QHBoxLayout();
    QPushButton* whisperBrowseBtn = new QPushButton(tr("Browse"), this);
    connect(whisperBrowseBtn, &QPushButton::clicked, [this]() {
        QString path = QFileDialog::getOpenFileName(this, tr("Select Whisper Model"),
            whisperModelPath->text(), tr("Model Files (*.bin)"));
        if (!path.isEmpty()) {
            whisperModelPath->setText(path);
        }
    });
    whisperLayout->addWidget(whisperModelPath);
    whisperLayout->addWidget(whisperBrowseBtn);

    QHBoxLayout* translationLayout = new QHBoxLayout();
    QPushButton* translationBrowseBtn = new QPushButton(tr("Browse"), this);
    connect(translationBrowseBtn, &QPushButton::clicked, [this]() {
        QString path = QFileDialog::getOpenFileName(this, tr("Select Translation Model"),
            translationModelPath->text(), tr("Model Files (*.bin)"));
        if (!path.isEmpty()) {
            translationModelPath->setText(path);
        }
    });
    translationLayout->addWidget(translationModelPath);
    translationLayout->addWidget(translationBrowseBtn);

    // Add form fields
    formLayout->addRow(tr("Audio Device:"), audioDeviceCombo);
    formLayout->addRow(tr("Whisper Model:"), whisperLayout);
    formLayout->addRow(tr("Translation Model:"), translationLayout);

    // Buttons
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
    resize(500, 200);
}

void SettingsDialog::populateAudioDevices() {
    audioDeviceCombo->clear();
    audioDeviceCombo->addItem(tr("Default Audio Device"));

    const QList<QAudioDevice> inputDevices = QMediaDevices::audioInputs();
    for (const QAudioDevice& device : inputDevices) {
        audioDeviceCombo->addItem(device.description());
    }
}

void SettingsDialog::loadSettings() {
    // Load audio device from QSettings
    QString savedDevice = settings.value("audio/device", "Default Audio Device").toString();
    int index = audioDeviceCombo->findText(savedDevice);
    if (index >= 0) {
        audioDeviceCombo->setCurrentIndex(index);
    }

    // Load model paths from Config singleton
    auto& config = koebridge::utils::Config::getInstance();
    whisperModelPath->setText(QString::fromStdString(
        config.getString("models.whisper_path", "_dataset/models/whisper-tiny.bin")));
    translationModelPath->setText(QString::fromStdString(
        config.getString("models.translation_path", "_dataset/models/nllb-small.bin")));
}

void SettingsDialog::saveSettings() {
    // Save audio device to QSettings
    settings.setValue("audio/device", audioDeviceCombo->currentText());

    // Save model paths to Config singleton
    auto& config = koebridge::utils::Config::getInstance();
    config.setString("models.whisper_path", whisperModelPath->text().toStdString());
    config.setString("models.translation_path", translationModelPath->text().toStdString());

    // Try to save config to file
    std::string configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/config.ini";
    if (!config.save(configPath)) {
        QMessageBox::warning(this, tr("Settings"),
            tr("Failed to save configuration to file. Settings will be lost when the application closes."));
    }

    // Sync QSettings to disk
    settings.sync();

    accept();
}

QString SettingsDialog::getAudioDevice() const {
    return audioDeviceCombo->currentText();
}

QString SettingsDialog::getWhisperModelPath() const {
    return whisperModelPath->text();
}

QString SettingsDialog::getTranslationModelPath() const {
    return translationModelPath->text();
}
