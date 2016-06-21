// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include "indicator.h"
#include <QVariant>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMouseEvent>

/***********************************************************************
 * |PothosDoc Digit Spinner
 *
 * A numeric entry widget with spin controls for each digit.
 *
 * |category /Widgets
 * |keywords spin frequency
 *
 * |param title The name of the value displayed by this widget
 * |default "Digit Value"
 * |widget StringEntry()
 *
 * |param fontSize The font size for the digits in the widget.
 * |default 12
 * |widget SpinBox(minimum=8)
 *
 * |param size The number of configurable digits in the widget.
 * |default 10
 * |widget SpinBox(minimum=1)
 *
 * |param value The initial value of this digit_spinner.
 * |default 123456789
 *
 * |param minimum The minimum value of this digit_spinner.
 * |default 0.0
 *
 * |param maximum The maximum value of this digit_spinner.
 * |default 1e9
 *
 * |mode graphWidget
 * |factory /widgets/digit_spinner()
 * |setter setTitle(title)
 * |setter setFontSize(fontSize)
 * |setter setSize(size)
 * |setter setValueMin(minimum)
 * |setter setValueMax(maximum)
 * |setter setValue(value)
 **********************************************************************/
class DigitSpinner : public QGroupBox, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new DigitSpinner();
    }

    DigitSpinner(void):
        _spinner(new Indicator())
    {
        _spinner->setFrameShape(QFrame::NoFrame);
        _spinner->setFrameShadow(QFrame::Plain);

        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->addWidget(_spinner);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setFontSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setValueMin));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setValueMax));
        this->registerSignal("valueChanged");
        connect(_spinner, SIGNAL(valueChanged(const qint64)), this, SLOT(handleValueChanged(const qint64)));
    }

    QWidget *widget(void)
    {
        return this;
    }

    void setTitle(const QString &title)
    {
        QMetaObject::invokeMethod(this, "handleSetTitle", Qt::QueuedConnection, Q_ARG(QString, title));
    }

    void setFontSize(const int size)
    {
        QMetaObject::invokeMethod(_spinner, "setFontSize", Qt::QueuedConnection, Q_ARG(int, size));
    }

    void setSize(const int size)
    {
        QMetaObject::invokeMethod(_spinner, "setSize", Qt::QueuedConnection, Q_ARG(int, size));
    }

    double value(void) const
    {
        return _spinner->value();
    }

    void setValue(const double value)
    {
        _spinner->setValue(value);
    }

    void setValueMin(const double value)
    {
        _spinner->setValueMin(value);
    }

    void setValueMax(const double value)
    {
        _spinner->setValueMax(value);
    }

    void activate(void)
    {
        //emit current value when design becomes active
        this->callVoid("valueChanged", this->value());
    }

public slots:

    QVariant saveState(void) const
    {
        return this->value();
    }

    void restoreState(const QVariant &state)
    {
        this->setValue(state.toDouble());
    }

private slots:
    void handleValueChanged(const qint64 value)
    {
        this->callVoid("valueChanged", value);
    }

    void handleSetTitle(const QString &title)
    {
        QGroupBox::setTitle(title);
    }

protected:
    void mousePressEvent(QMouseEvent *event)
    {
        QGroupBox::mousePressEvent(event);
        event->ignore(); //allows for dragging from QGroupBox title
    }

private:
    Indicator *_spinner;
};

static Pothos::BlockRegistry registerDigitSpinner(
    "/widgets/digit_spinner", &DigitSpinner::make);

#include "DigitSpinner.moc"
