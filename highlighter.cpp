#include "highlighter.h"
#include <QRegExpValidator>

Highlighter::Highlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{

}

void Highlighter::highlightBlock(const QString& text)
{
    QTextCharFormat myClassFormat;

    myClassFormat.setForeground(Qt::darkMagenta);

    QRegularExpression expression("MOV");
    QRegularExpressionMatchIterator i = expression.globalMatch(text);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), myClassFormat);
    }


    QTextCharFormat myClassFormat2;

    myClassFormat2.setForeground(Qt::darkGreen);

    QRegularExpression expression2("ADD");
    QRegularExpressionMatchIterator ii = expression2.globalMatch(text);
    while (ii.hasNext())
    {
        QRegularExpressionMatch match = ii.next();
        setFormat(match.capturedStart(), match.capturedLength(), myClassFormat2);
    }
}
