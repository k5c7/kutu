#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H
#include <QSyntaxHighlighter>

class Highlighter : QSyntaxHighlighter
{
public:
    Highlighter(QTextDocument* parent = nullptr);
    void highlightBlock(const QString& text) override;
};

#endif // HIGHLIGHTER_H
