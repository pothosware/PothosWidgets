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
 * |default "My Spinner Value"
 * |widget StringEntry()
 *
 * |param value The initial value of this digit_spinner.
 * |default 0.0
 *
 * |param minimum The minimum value of this digit_spinner.
 * |default -1.0
 *
 * |param maximum The maximum value of this digit_spinner.
 * |default 1.0
 *
 * |param step [Step Size] The increment between discrete values.
 * |default 0.01
 *
 * |mode graphWidget
 * |factory /widgets/digit_spinner()
 * |setter setTitle(title)
 * |setter setValueMin(minimum)
 * |setter setValueMax(maximum)
 * |setter setDeltaMin(step)
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
        _spinner->setSize(8);
        _spinner->setFrameShape(QFrame::Panel);
        _spinner->setFrameShadow(QFrame::Sunken);

        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->addWidget(_spinner);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setValueMin));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setValueMax));
        this->registerCall(this, POTHOS_FCN_TUPLE(DigitSpinner, setDeltaMin));
        this->registerSignal("valueChanged");
        connect(_spinner, SIGNAL(valueChanged(const int)), this, SLOT(handleValueChanged(const int)));
    }

    QWidget *widget(void)
    {
        return this;
    }

    void setTitle(const QString &title)
    {
        QMetaObject::invokeMethod(this, "handleSetTitle", Qt::QueuedConnection, Q_ARG(QString, title));
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

    void setDeltaMin(const double value)
    {
        //_spinner->setDeltaMin(value);
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
    void handleValueChanged(const int value)
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
