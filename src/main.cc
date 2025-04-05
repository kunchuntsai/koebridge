#include <QApplication>
#include <QObject>
#include <iostream>
#include "ui/main_window.h"
#include "audio/audio_capture.h"
#include "audio/audio_processor.h"
#include "translation/translation_service.h"
#include "utils/config.h"

/**
 * @brief Main application class that coordinates the data flow
 */
class Application : public QObject {
    Q_OBJECT
public:
    explicit Application(QObject* parent = nullptr) : QObject(parent) {
        // Initialize components
        initializeComponents();
        // Connect signals and slots
        connectSignals();
    }

    // Add public method to access main window
    MainWindow* getMainWindow() { return mainWindow_.get(); }

private:
    void initializeComponents() {
        // Load configuration
        Config::getInstance().load("config/config.ini");

        // Initialize audio capture
        audioCapture_ = std::make_unique<AudioCapture>();

        // Initialize audio processor
        audioProcessor_ = std::make_unique<AudioProcessor>();

        // Initialize translation service with model manager
        auto modelManager = std::make_shared<ModelManager>();
        translationService_ = std::make_unique<TranslationService>(modelManager);

        // Initialize UI
        mainWindow_ = std::make_unique<MainWindow>();
    }

    void connectSignals() {
        // Audio capture -> Audio processor
        audioCapture_->setAudioCallback([this](const float* data, int size) {
            // Process audio data
            std::vector<float> processedData;
            audioProcessor_->process(std::vector<float>(data, data + size), processedData);

            // Convert to text (to be implemented)
            std::string text = convertAudioToText(processedData);

            // Translate text
            translationService_->translateTextAsync(text, [this](const TranslationResult& result) {
                if (result.success) {
                    // Update UI with translated text
                    mainWindow_->updateTranslatedText(QString::fromStdString(result.translatedText));
                } else {
                    // Handle translation error
                    std::cerr << "Translation failed: " << result.errorMessage << std::endl;
                }
            });
        });
    }

    std::string convertAudioToText(const std::vector<float>& audioData) {
        // TODO: Implement speech-to-text conversion
        return "Placeholder text";
    }

private:
    std::unique_ptr<AudioCapture> audioCapture_;
    std::unique_ptr<AudioProcessor> audioProcessor_;
    std::unique_ptr<TranslationService> translationService_;
    std::unique_ptr<MainWindow> mainWindow_;
};

// Data Flow:
//   Audio Capture -> Audio Processor -> Speech-to-Text -> Translation Service -> UI
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create and initialize application
    Application application;

    // Show main window
    MainWindow* mainWindow = application.getMainWindow();
    mainWindow->resize(800, 600);
    mainWindow->show();

    return app.exec();
}

#include "main.moc"  //Qt's Meta-Object Compilation (MOC) system
