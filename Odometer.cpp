// Copyright (c) 2016-2021 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include "indicator.h"
#include <QVariant>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMouseEvent>

/***********************************************************************
 * |PothosDoc Odometer
 *
 * The odometer is a numeric entry widget with spin controls for each digit.
 * Click on the top half of a digit to increase its value by one,
 * and clock on the bottom half of a digit to decrease its value by one.
 * The odometer can also be used for display via the setValue slot.
 *
 * |category /Widgets
 * |keywords spin frequency
 *
 * |param title The name of the value displayed by this widget
 * |default "Digit Value"
 * |widget StringEntry()
 *
 * |param fontSize[Font Size] The font size for the digits in the widget.
 * |default 12
 * |widget SpinBox(minimum=8)
 * |preview disable
 *
 * |param size The number of configurable digits in the widget.
 * |default 10
 * |widget SpinBox(minimum=1)
 * |preview disable
 *
 * |param value The initial value of this odometer.
 * |default 123456789
 *
 * |param minimum The minimum value of this odometer.
 * |default 0.0
 *
 * |param maximum The maximum value of this odometer.
 * |default 1e9
 *
 * |mode graphWidget
 * |factory /widgets/odometer()
 * |setter setTitle(title)
 * |setter setFontSize(fontSize)
 * |setter setSize(size)
 * |setter setValueMin(minimum)
 * |setter setValueMax(maximum)
 * |setter setValue(value)
 **********************************************************************/
class Odometer : public QGroupBox, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new Odometer();
    }

    Odometer(void):
        _indicator(new Indicator())
    {
        _indicator->setFrameShape(QFrame::NoFrame);
        _indicator->setFrameShadow(QFrame::Plain);

        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->addWidget(_indicator);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        this->registerCall(this, POTHOS_FCN_TUPLE(Odometer, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(Odometer, setFontSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(Odometer, setSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(Odometer, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(Odometer, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(Odometer, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(Odometer, setValueMin));
        this->registerCall(this, POTHOS_FCN_TUPLE(Odometer, setValueMax));
        this->registerSignal("valueChanged");
        connect(_indicator, &Indicator::valueChanged, this, &Odometer::handleValueChanged);
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
        QMetaObject::invokeMethod(_indicator, "setFontSize", Qt::QueuedConnection, Q_ARG(int, size));
    }

    void setSize(const int size)
    {
        QMetaObject::invokeMethod(_indicator, "setSize", Qt::QueuedConnection, Q_ARG(int, size));
    }

    qint64 value(void) const
    {
        return _indicator->value();
    }

    void setValue(const qint64 value)
    {
        _indicator->setValue(value);
    }

    void setValueMin(const qint64 value)
    {
        _indicator->setValueMin(value);
    }

    void setValueMax(const qint64 value)
    {
        _indicator->setValueMax(value);
    }

    void activate(void)
    {
        //emit current value when design becomes active
        this->emitSignal("valueChanged", this->value());
    }

public slots:

    QVariant saveState(void) const
    {
        return qlonglong(this->value());
    }

    void restoreState(const QVariant &state)
    {
        this->setValue(state.toLongLong());
    }

private slots:
    void handleValueChanged(const qint64 value)
    {
        this->emitSignal("valueChanged", value);
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
    Indicator *_indicator;
};

static Pothos::BlockRegistry registerOdometer(
    "/widgets/odometer", &Odometer::make);

#include "Odometer.moc"
