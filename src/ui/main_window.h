/**
 * @file main_window.h
 * @brief Header file for the main application window
 */

#pragma once

#include <QMainWindow>

class TranslationView;
class QAction;
class QMenu;

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
    
private:
    /**
     * @brief Create application actions
     */
    void createActions();
    
    /**
     * @brief Create application menus
     */
    void createMenus();
    
    TranslationView* translationView; ///< Main translation view widget
    
    QMenu* fileMenu;    ///< File menu
    QMenu* toolsMenu;   ///< Tools menu
    QMenu* helpMenu;    ///< Help menu
    
    QAction* startAct;      ///< Start translation action
    QAction* stopAct;       ///< Stop translation action
    QAction* settingsAct;   ///< Show settings action
    QAction* exitAct;       ///< Exit application action
};
