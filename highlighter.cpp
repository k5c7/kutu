#include "highlighter.h"
#include <QRegExpValidator>

Highlighter::Highlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{

}

void Highlighter::highlightBlock(const QString& text)
{
    // TODO: Create static constant variables

    const auto assignment_color = Qt::darkMagenta;
    const QRegularExpression assignment_regex("MOV|ADD|SUB|MUL|DIV|NOP");
    generic_highlight(assignment_color, assignment_regex, text);

    const auto jump_color = Qt::darkRed;
    const QRegularExpression jump_regex("JMPSE|JMPBE|JMPB|JMPS|JMPNE|JMPE|JMP");
    generic_highlight(jump_color, jump_regex, text);

    const auto memory_color = Qt::darkCyan;
    const QRegularExpression memory_regex("\\$[0-9]+");
    generic_highlight(memory_color, memory_regex, text);

    const auto label_color = Qt::darkRed;
    const QRegularExpression label_regex("\\_\\w+");
    generic_highlight(label_color, label_regex, text);

    const auto string_color = Qt::darkGreen;
    const QRegularExpression string_regex("\"(.*?)\"");
    generic_highlight(string_color, string_regex, text);

    const auto console_color = Qt::darkBlue;
    const QRegularExpression console_regex("PRINTNL|PRINT|CLEAR");
    generic_highlight(console_color, console_regex, text);

    const auto comment_color = Qt::lightGray;
    const QRegularExpression comment_regex(R"lit(^#.*)lit");
    generic_highlight(comment_color, comment_regex, text);
}

void Highlighter::generic_highlight(Qt::GlobalColor color, const QRegularExpression& regex, const QString& text)
{
    // FIXME: Change Qt::GlobalColor to QBrush
    QTextCharFormat format;
    format.setForeground(color);
    // format.setFontWeight(QFont::Bold);
    QRegularExpressionMatchIterator iter = regex.globalMatch(text);

    while (iter.hasNext())
    {
        QRegularExpressionMatch match = iter.next();
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }
}
