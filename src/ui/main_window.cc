#include "main_window.h"
#include "translation_view.h"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    translationView = new TranslationView(this);
    setCentralWidget(translationView);
    
    createActions();
    createMenus();
    
    setWindowTitle(tr("KoeBridge"));
}

MainWindow::~MainWindow() {
    // Clean up resources
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
    // Show settings dialog
    QMessageBox::information(this, tr("Settings"), tr("Settings dialog would appear here."));
}

void MainWindow::updateTranslatedText(const QString& text) {
    if (translationView) {
        translationView->updateEnglishText(text);
    }
}
