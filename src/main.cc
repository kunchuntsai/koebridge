#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QThread>
#include <QCoreApplication>
#include <QDebug>

#include "audio/audio_capture.h"
#include "translation/translation_service.h"
//#include "translation/model_manager.h"
#include "translation/data_structures.h"
#include "utils/config.h"
#include "interfaces/i_model_manager.h"

using namespace koebridge::translation;

// Simple mock implementation for testing
class MockModelManager : public IModelManager {
public:
    bool initialize() override { return true; }
    bool loadModel(const std::string&) override { return true; }
    bool unloadModel() override { return true; }
    std::vector<ModelInfo> getAvailableModels() const override { return {}; }
    ModelInfo getActiveModel() const override { return {}; }
    bool isModelLoaded() const override { return true; }
    bool downloadModel(const std::string&, ProgressCallback) override { return true; }
    std::shared_ptr<koebridge::translation::ITranslationModel> getTranslationModel() override {
        return nullptr;
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("KoeBridge");
        resize(800, 600);

        // Create central widget and layout
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        auto* layout = new QVBoxLayout(centralWidget);

        // Create UI elements
        sourceLanguageCombo_ = new QComboBox(this);
        targetLanguageCombo_ = new QComboBox(this);
        sourceTextEdit_ = new QTextEdit(this);
        translatedTextEdit_ = new QTextEdit(this);
        translateButton_ = new QPushButton("Translate", this);
        progressBar_ = new QProgressBar(this);
        progressBar_->setVisible(false);

        // Add languages
        sourceLanguageCombo_->addItems({"Japanese", "English"});
        targetLanguageCombo_->addItems({"English", "Japanese"});

        // Add widgets to layout
        layout->addWidget(sourceLanguageCombo_);
        layout->addWidget(sourceTextEdit_);
        layout->addWidget(translateButton_);
        layout->addWidget(progressBar_);
        layout->addWidget(targetLanguageCombo_);
        layout->addWidget(translatedTextEdit_);

        // Connect signals
        connect(translateButton_, &QPushButton::clicked, this, &MainWindow::onTranslateClicked);
    }

    void updateTranslatedText(const QString& text) {
        translatedTextEdit_->setText(text);
    }

    void updateProgress(int value) {
        progressBar_->setValue(value);
    }

private slots:
    void onTranslateClicked() {
        QString text = sourceTextEdit_->toPlainText();
        if (text.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please enter text to translate");
            return;
        }

        emit translateRequested(text);
    }

signals:
    void translateRequested(const QString& text);

private:
    QComboBox* sourceLanguageCombo_;
    QComboBox* targetLanguageCombo_;
    QTextEdit* sourceTextEdit_;
    QTextEdit* translatedTextEdit_;
    QPushButton* translateButton_;
    QProgressBar* progressBar_;
};

class Application : public QObject {
    Q_OBJECT
public:
    explicit Application(QObject* parent = nullptr) : QObject(parent) {
        try {
            // Initialize model manager (using mock for now)
            auto modelManager = std::make_shared<MockModelManager>();
            
            // Initialize translation service
            translationService_ = std::make_unique<TranslationService>(modelManager);
            if (!translationService_->initialize()) {
                throw std::runtime_error("Failed to initialize translation service");
            }

            // Create and show main window
            mainWindow_ = std::make_unique<MainWindow>();
            mainWindow_->show();

            // Connect signals
            connect(mainWindow_.get(), &MainWindow::translateRequested,
                    this, &Application::onTranslateRequested);
            
            // Connect translation service signals to main window
            connect(translationService_.get(), &TranslationService::progressUpdated,
                    mainWindow_.get(), &MainWindow::updateProgress);
            connect(translationService_.get(), &TranslationService::error,
                    [this](const QString& message) {
                        QMessageBox::warning(mainWindow_.get(), "Error", message);
                    });
        } catch (const std::exception& e) {
            QMessageBox::critical(nullptr, "Error", 
                QString("Failed to initialize application: %1").arg(e.what()));
            QCoreApplication::quit();
        }
    }

private slots:
    void onTranslateRequested(const QString& text) {
        // Use the standard interface method for translation
        std::string output;
        if (translationService_->translate(text.toStdString(), output)) {
            mainWindow_->updateTranslatedText(QString::fromStdString(output));
        }
    }

private:
    std::unique_ptr<MainWindow> mainWindow_;
    std::unique_ptr<TranslationService> translationService_;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    try {
        Application application;
        return app.exec();
    } catch (const std::exception& e) {
        qCritical() << "Fatal error:" << e.what();
        return 1;
    }
}

#include "main.moc"
