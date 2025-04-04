#pragma once

#include <QWidget>

class QTextEdit;
class QLabel;

class TranslationView : public QWidget {
    Q_OBJECT
    
public:
    TranslationView(QWidget *parent = nullptr);
    
public slots:
    void updateJapaneseText(const QString& text);
    void updateEnglishText(const QString& text);
    
private:
    QLabel* japaneseLabel;
    QTextEdit* japaneseText;
    QLabel* englishLabel;
    QTextEdit* englishText;
};
