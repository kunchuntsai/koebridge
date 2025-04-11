#include "main_window.h"
#include "translation_view.h"
#include "settings_dialog.h"
#include "utils/config.h"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , settings("KoeBridge", "KoeBridge")
{
    translationView = new TranslationView(this);
    setCentralWidget(translationView);

    createActions();
    createMenus();
    loadSettings();

    setWindowTitle(tr("KoeBridge"));
}

MainWindow::~MainWindow() {
    saveSettings();
    delete settingsDialog;
}

void MainWindow::createActions() {
    startAct = new QAction(tr("&Start"), this);
    startAct->setShortcut(tr("Ctrl+S"));
    connect(startAct, &QAction::triggered, this, &MainWindow::startTranslation);

    stopAct = new QAction(tr("S&top"), this);
    stopAct->setShortcut(tr("Ctrl+T"));
    stopAct->setEnabled(false);
    connect(stopAct, &QAction::triggered, this, &MainWindow::stopTranslation);

    settingsAct = new QAction(tr("Se&ttings"), this);
    connect(settingsAct, &QAction::triggered, this, &MainWindow::showSettings);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, &QAction::triggered, this, &QApplication::quit);
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(startAct);
    fileMenu->addAction(stopAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(settingsAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
}

void MainWindow::startTranslation() {
    startAct->setEnabled(false);
    stopAct->setEnabled(true);
    // Start translation logic
}

void MainWindow::stopTranslation() {
    stopAct->setEnabled(false);
    startAct->setEnabled(true);
    // Stop translation logic
}

void MainWindow::showSettings() {
    if (!settingsDialog) {
        settingsDialog = new SettingsDialog(this);
    }

    if (settingsDialog->exec() == QDialog::Accepted) {
        applySettings();
    }
}

void MainWindow::applySettings() {
    auto& config = koebridge::utils::Config::getInstance();

    // Apply audio device settings
    QString audioDevice = settingsDialog->getAudioDevice();
    settings.setValue("audio/device", audioDevice);

    // Apply model paths
    QString whisperModelPath = settingsDialog->getWhisperModelPath();
    QString translationModelPath = settingsDialog->getTranslationModelPath();
    config.setString("models.whisper_path", whisperModelPath.toStdString());
    config.setString("models.translation_path", translationModelPath.toStdString());

    // Save config to file
    std::string configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/config.ini";
    if (!config.save(configPath)) {
        QMessageBox::warning(this, tr("Settings"),
            tr("Failed to save configuration to file. Some settings may be lost when the application closes."));
    }

    // Sync QSettings to disk
    settings.sync();

    // TODO: Apply settings to audio capture and model manager
    // This will be implemented when we connect these components
}

void MainWindow::loadSettings() {
    // Load window geometry from QSettings
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/state").toByteArray());

    // Load other settings from Config singleton
    auto& config = koebridge::utils::Config::getInstance();
    if (!config.load(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/config.ini")) {
        // If no config file exists, try to load from default locations
        std::vector<std::string> configPaths = {
            "config/config.ini",
            "config/default_settings.json"
        };
        for (const auto& path : configPaths) {
            if (config.load(path)) {
                break;
            }
        }
    }
}

void MainWindow::saveSettings() {
    // Save window geometry to QSettings
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());

    // Save config to file
    auto& config = koebridge::utils::Config::getInstance();
    std::string configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/config.ini";
    if (!config.save(configPath)) {
        QMessageBox::warning(this, tr("Settings"),
            tr("Failed to save configuration to file. Some settings may be lost when the application closes."));
    }

    // Sync QSettings to disk
    settings.sync();
}

void MainWindow::updateTranslatedText(const QString& text) {
    if (translationView) {
        translationView->updateEnglishText(text);
    }
}
