/**
 * @file translation_view.h
 * @brief Header file for the translation view widget
 */

#pragma once

#include <QWidget>

class QTextEdit;
class QLabel;

/**
 * @class TranslationView
 * @brief Widget for displaying and editing Japanese and English text
 *
 * This class provides a user interface for displaying and editing Japanese
 * source text and its English translation. It includes text editors for both
 * languages and labels for clear identification.
 */
class TranslationView : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for TranslationView
     * @param parent Parent widget (default: nullptr)
     */
    TranslationView(QWidget *parent = nullptr);

public slots:
    /**
     * @brief Update the Japanese text display
     * @param text New Japanese text to display
     */
    void updateJapaneseText(const QString& text);

    /**
     * @brief Update the English text display
     * @param text New English text to display
     */
    void updateEnglishText(const QString& text);

private:
    QLabel* japaneseLabel;     ///< Label for Japanese text section
    QTextEdit* japaneseText;   ///< Editor for Japanese text
    QLabel* englishLabel;      ///< Label for English text section
    QTextEdit* englishText;    ///< Editor for English text
};
