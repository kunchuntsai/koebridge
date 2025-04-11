/**
 * @file main_window.h
 * @brief Header file for the main application window
 */

#pragma once

#include <QMainWindow>
#include <QString>
#include <QSettings>

class TranslationView;
class QAction;
class QMenu;
class SettingsDialog;

/**
 * @class MainWindow
 * @brief Main window class for the application
 *
 * This class represents the main application window, providing the primary
 * user interface for the translation application. It includes menus, actions,
 * and the main translation view.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor for MainWindow
     * @param parent Parent widget (default: nullptr)
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor for MainWindow
     */
    ~MainWindow();

    /**
     * @brief Update the translated text in the UI
     * @param text The translated text to display
     */
    void updateTranslatedText(const QString& text);

private slots:
    /**
     * @brief Start the translation process
     */
    void startTranslation();

    /**
     * @brief Stop the translation process
     */
    void stopTranslation();

    /**
     * @brief Show the settings dialog
     */
    void showSettings();

    /**
     * @brief Apply the settings
     */
    void applySettings();

private:
    /**
     * @brief Create application actions
     */
    void createActions();

    /**
     * @brief Create application menus
     */
    void createMenus();

    /**
     * @brief Load the settings
     */
    void loadSettings();

    /**
     * @brief Save the settings
     */
    void saveSettings();

    void applyConfigSettings(const koebridge::utils::Config& config);

    TranslationView* translationView; ///< Main translation view widget
    SettingsDialog* settingsDialog;    ///< Settings dialog widget
    QSettings settings;                ///< Application settings

    QMenu* fileMenu;    ///< File menu
    QMenu* toolsMenu;   ///< Tools menu
    QMenu* helpMenu;    ///< Help menu

    QAction* startAct;      ///< Start translation action
    QAction* stopAct;       ///< Stop translation action
    QAction* settingsAct;   ///< Show settings action
    QAction* exitAct;       ///< Exit application action

    void updateTranslatedText(const QString& text);
    void setupConnections();
    void setupMenuBar();
    void setupStatusBar();
    void setupCentralWidget();
    void setupTranslationView();
    void setupSettingsDialog();
    void setupAudioCapture();
    void setupTranslationService();
    void setupModelManager();
    void setupRealtimeTranscriber();
    void setupWhisperWrapper();
    void setupLLMManager();
    void setupLLMModel();
    void setupNLLBModel();
    void setupGGMLModel();
    void setupInferenceEngine();
    void setupTransformer();
    void setupLogger();
    void setupConfig();
    void setupUtils();
    void setupInterfaces();
    void setupModels();
    void setupTranslation();
    void setupAudio();
    void setupSTT();
    void setupLLM();
    void setupInference();
    void setupUI();
    void setupCore();
    void setupAll();
};
