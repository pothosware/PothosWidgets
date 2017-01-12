// Copyright (c) 2017-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>

/***********************************************************************
 * |PothosDoc Text Entry
 *
 * The text entry widget allows for entry of a single line of text.
 * When the user presses the return key, the value of the entry
 * will be emitted to the valueChanged() signal as a string.
 *
 * This block can be paired with the the Evaluator block to emit data types
 * other than the string type by connecting to the setExpression() slot.
 *
 * |category /Widgets
 * |keywords text entry edit
 *
 * |param title The name of the value displayed by this widget
 * |default "My Text Value"
 * |widget StringEntry()
 *
 * |param value The initial value for the text entry.
 * |default "Text here"
 * |widget StringEntry()
 *
 * |param mode The mode controls how valueChanged() signal reacts to changes.
 * Use the return mode to emit only when the return button is pressed.
 * Or use the on-edit mode to emit valueChanged() on every text change.
 * |default "RETURN"
 * |option [Return] "RETURN"
 * |option [On-Edit] "ONEDIT"
 * |preview disable
 *
 * |mode graphWidget
 * |factory /widgets/text_entry()
 * |setter setTitle(title)
 * |setter setValue(value)
 * |setter setMode(mode)
 **********************************************************************/
class TextEntry : public QWidget, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new TextEntry();
    }

    TextEntry(void):
        _emitOnChange(false),
        _layout(new QHBoxLayout(this)),
        _label(new QLabel(this)),
        _lineEdit(new QLineEdit(this))
    {
        _layout->setContentsMargins(QMargins());
        _layout->addWidget(_label);
        _layout->addWidget(_lineEdit);
        this->registerCall(this, POTHOS_FCN_TUPLE(TextEntry, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(TextEntry, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(TextEntry, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(TextEntry, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(TextEntry, setMode));
        this->registerSignal("valueChanged");
        connect(_lineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(handleTextEdited(const QString &)));
        connect(_lineEdit, SIGNAL(returnPressed(void)), this, SLOT(handleReturnPressed(void)));
    }

    QWidget *widget(void)
    {
        return this;
    }

    void setValue(const QString &value)
    {
        QMetaObject::invokeMethod(this, "handleSetValue", Qt::QueuedConnection, Q_ARG(QString, value));
    }

    QString value(void) const
    {
        return _lineEdit->text();
    }

    void setTitle(const QString &title)
    {
        QMetaObject::invokeMethod(this, "handleSetTitle", Qt::QueuedConnection, Q_ARG(QString, title));
    }

    void setMode(const QString &mode)
    {
        _emitOnChange = (mode == "ONEDIT");
    }

    void activate(void)
    {
        QMetaObject::invokeMethod(this, "handleReturnPressed", Qt::QueuedConnection);
    }

public slots:

    QVariant saveState(void) const
    {
        return this->value();
    }

    void restoreState(const QVariant &state)
    {
        this->setValue(state.toString());
    }

private slots:

    void handleSetValue(const QString &value)
    {
        _lineEdit->setText(value);
        _commitedText = value;
        this->update(value);
    }

    void handleSetTitle(const QString &title)
    {
        _title = title;
        this->update(this->value());
    }

    void handleTextEdited(const QString &text)
    {
        if (_emitOnChange)
        {
            this->emitSignal("valueChanged", text);
            _commitedText = text;
        }
        this->update(text);
    }

    void handleReturnPressed(void)
    {
        const auto value = this->value();
        this->emitSignal("valueChanged", value);
        _commitedText = value;
        this->update(value);
    }

private:

    void update(const QString &newValue)
    {
        const auto title = QString("<b>%1:</b>").arg(_title.toHtmlEscaped());
        if (_commitedText != newValue)
        {
            _label->setText(title+"*");
            _lineEdit->setStyleSheet("QLineEdit {background-color: pink;}");
            _layout->setSpacing(0);
        }
        else
        {
            _label->setText(title);
            _lineEdit->setStyleSheet("QLineEdit {}");
            QFontMetrics fm(_label->font());
            _layout->setSpacing(fm.width("*"));
        }
    }

    bool _emitOnChange;
    QString _title;
    QHBoxLayout *_layout;
    QLabel *_label;
    QLineEdit *_lineEdit;
    QString _commitedText;
};

static Pothos::BlockRegistry registerTextEntry(
    "/widgets/text_entry", &TextEntry::make);

#include "TextEntry.moc"
