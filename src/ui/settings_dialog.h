#pragma once

#include <QDialog>

class QComboBox;
class QLineEdit;

class SettingsDialog : public QDialog {
    Q_OBJECT
    
public:
    SettingsDialog(QWidget *parent = nullptr);
    
private slots:
    void saveSettings();
    
private:
    QComboBox* audioDeviceCombo;
    QLineEdit* whisperModelPath;
    QLineEdit* translationModelPath;
};
