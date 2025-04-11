#pragma once

#include <QDialog>
#include <QSettings>

class QComboBox;
class QLineEdit;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    // Getters for settings
    QString getAudioDevice() const;
    QString getWhisperModelPath() const;
    QString getTranslationModelPath() const;

private slots:
    void saveSettings();
    void loadSettings();

private:
    void setupUI();
    void populateAudioDevices();

    QComboBox* audioDeviceCombo;
    QLineEdit* whisperModelPath;
    QLineEdit* translationModelPath;
    QSettings settings;
};
