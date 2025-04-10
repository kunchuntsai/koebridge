#include "translation_view.h"
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>

TranslationView::TranslationView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    japaneseLabel = new QLabel(tr("Japanese:"), this);
    japaneseText = new QTextEdit(this);
    japaneseText->setReadOnly(true);

    englishLabel = new QLabel(tr("English:"), this);
    englishText = new QTextEdit(this);
    englishText->setReadOnly(true);

    layout->addWidget(japaneseLabel);
    layout->addWidget(japaneseText);
    layout->addWidget(englishLabel);
    layout->addWidget(englishText);

    setLayout(layout);
}

void TranslationView::updateJapaneseText(const QString& text) {
    japaneseText->setText(text);
}

void TranslationView::updateEnglishText(const QString& text) {
    englishText->setText(text);
}
