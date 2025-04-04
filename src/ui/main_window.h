#pragma once

#include <QMainWindow>

class TranslationView;
class QAction;
class QMenu;

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void startTranslation();
    void stopTranslation();
    void showSettings();
    
private:
    void createActions();
    void createMenus();
    
    TranslationView* translationView;
    
    QMenu* fileMenu;
    QMenu* toolsMenu;
    QMenu* helpMenu;
    
    QAction* startAct;
    QAction* stopAct;
    QAction* settingsAct;
    QAction* exitAct;
};
