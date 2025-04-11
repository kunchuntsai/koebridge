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
#include <QProgressDialog>

#include "audio/audio_capture.h"
#include "translation/translation_service.h"
#include "translation/model_manager.h"
#include "translation/data_structures.h"
#include "utils/config.h"
#include "interfaces/i_model_manager.h"
#include "utils/logger.h"

using namespace koebridge::translation;
using namespace koebridge::utils;

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

class AppInitializer {
public:
    // Initialization status
    enum class Status {
        NotStarted,
        ConfigLoading,
        ConfigLoaded,
        ModelManagerInit,
        TranslationServiceInit,
        Complete,
        Error
    };

    // Progress callback
    using ProgressCallback = std::function<void(Status, const std::string&)>;

    bool initialize(ProgressCallback progressCb = nullptr) {
        if (progressCb) progressCb(Status::ConfigLoading, "Loading configuration...");

        // 1. Load configuration
        if (!initializeConfig()) {
            if (progressCb) progressCb(Status::Error, "Failed to load configuration");
            return false;
        }
        if (progressCb) progressCb(Status::ConfigLoaded, "Configuration loaded");

        // 2. Initialize model manager
        if (progressCb) progressCb(Status::ModelManagerInit, "Initializing model manager...");
        if (!initializeModelManager()) {
            if (progressCb) progressCb(Status::Error, "Failed to initialize model manager");
            return false;
        }

        // 3. Initialize translation service
        if (progressCb) progressCb(Status::TranslationServiceInit, "Initializing translation service...");
        if (!initializeTranslationService()) {
            if (progressCb) progressCb(Status::Error, "Failed to initialize translation service");
            return false;
        }

        if (progressCb) progressCb(Status::Complete, "Initialization complete");
        return true;
    }

    std::shared_ptr<ModelManager> getModelManager() const { return modelManager_; }
    std::shared_ptr<TranslationService> getTranslationService() const { return translationService_; }

private:
    bool initializeConfig() {
        std::vector<std::string> configPaths = {
            "config/config.ini",
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString() + "/config.ini"
        };

        bool configLoaded = false;
        for (const auto& path : configPaths) {
            if (Config::getInstance().load(path)) {
                qDebug() << "Configuration loaded from:" << QString::fromStdString(path);
                configLoaded = true;
                break;
            }
        }

        if (!configLoaded) {
            qWarning() << "No configuration file found. Please ensure config/config.ini exists.";
            return false;
        }

        // Print configuration details for debugging
        auto& config = Config::getInstance();
        qDebug() << "Model path:" << QString::fromStdString(config.getString("translation.model_path"));
        return true;
    }

    bool initializeModelManager() {
        auto& config = Config::getInstance();
        std::string modelPath = config.getString("translation.model_path");

        modelManager_ = std::make_shared<ModelManager>(modelPath);
        return modelManager_->initialize();
    }

    bool initializeTranslationService() {
        translationService_ = std::make_shared<TranslationService>(modelManager_);
        return translationService_->initialize();
    }

    std::shared_ptr<ModelManager> modelManager_;
    std::shared_ptr<TranslationService> translationService_;
};

class Application : public QObject {
    Q_OBJECT
public:
    explicit Application(QObject* parent = nullptr) : QObject(parent) {
        try {
            // Create progress dialog
            QProgressDialog progress("Initializing...", "Cancel", 0, 100, nullptr);
            progress.setWindowModality(Qt::WindowModal);
            progress.setMinimumDuration(0);
            progress.setValue(0);
            progress.setAutoClose(false);
            progress.setAutoReset(false);

            // Initialize application
            AppInitializer initializer;
            bool success = initializer.initialize([&progress](AppInitializer::Status status, const std::string& message) {
                int value = 0;
                switch (status) {
                    case AppInitializer::Status::ConfigLoading: value = 0; break;
                    case AppInitializer::Status::ConfigLoaded: value = 10; break;
                    case AppInitializer::Status::ModelManagerInit: value = 20; break;
                    case AppInitializer::Status::TranslationServiceInit: value = 90; break;
                    case AppInitializer::Status::Complete: value = 100; break;
                    case AppInitializer::Status::Error: value = 100; break;
                    default: break;
                }
                progress.setValue(value);
                progress.setLabelText(QString::fromStdString(message));
                QCoreApplication::processEvents();
            });

            // Connect to logger to update progress dialog
            connect(&koebridge::utils::Logger::getInstance(), &koebridge::utils::Logger::logMessage,
                    [&progress](koebridge::utils::LogLevel level, const std::string& message) {
                if (level == koebridge::utils::LogLevel::INFO) {
                    progress.setLabelText(QString::fromStdString(message));
                    QCoreApplication::processEvents();
                }
            });

            if (!success) {
                throw std::runtime_error("Failed to initialize application");
            }

            // Get initialized services
            translationService_ = initializer.getTranslationService();

            // Create and show main window
            mainWindow_ = std::make_unique<MainWindow>();
            mainWindow_->show();

            // Hide progress dialog
            progress.close();

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
    std::shared_ptr<TranslationService> translationService_;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Application application;
    return app.exec();
}

#include "main.moc"
