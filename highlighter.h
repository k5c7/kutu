#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H
#include <QSyntaxHighlighter>

class Highlighter : QSyntaxHighlighter
{
public:
    Highlighter(QTextDocument* parent = nullptr);
    void highlightBlock(const QString& text) override;

private:
    void generic_highlight(Qt::GlobalColor color,
                           const QRegularExpression& regex,
                           const QString& text);
};

#endif // HIGHLIGHTER_H
