// Copyright (c) 2014-2021 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include "MyDoubleSlider.hpp"
#include <QVariant>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMouseEvent>

/***********************************************************************
 * |PothosDoc Slider
 *
 * A slider widget for graphical control of an integer value.
 *
 * |category /Widgets
 * |keywords slider
 *
 * |param title The name of the value displayed by this widget
 * |default "My Slider Value"
 * |widget StringEntry()
 *
 * |param orientation The slider orientation (horizontal or vertical).
 * |default "Horizontal"
 * |option [Horizontal] "Horizontal"
 * |option [Vertical] "Vertical"
 * |preview disable
 *
 * |param value The initial value of this slider.
 * |default 0.0
 *
 * |param minimum The minimum value of this slider.
 * |default -1.0
 *
 * |param maximum The maximum value of this slider.
 * |default 1.0
 *
 * |param step [Step Size] The increment between discrete values.
 * |default 0.01
 *
 * |mode graphWidget
 * |factory /widgets/slider(orientation)
 * |setter setTitle(title)
 * |setter setMinimum(minimum)
 * |setter setMaximum(maximum)
 * |setter setSingleStep(step)
 * |setter setValue(value)
 **********************************************************************/
class Slider : public QGroupBox, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(const std::string &orientation)
    {
        return new Slider(orientation);
    }

    Slider(const std::string &orientation):
        _slider(new MyDoubleSlider((orientation == "Horizontal")? Qt::Horizontal : Qt::Vertical))
    {
        auto layout = new QVBoxLayout(this);
        layout->setContentsMargins(QMargins());
        layout->addWidget(_slider);
        this->setStyleSheet("QGroupBox {font-weight: bold;}");

        this->registerCall(this, POTHOS_FCN_TUPLE(Slider, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(Slider, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(Slider, value));
        this->registerCall(this, POTHOS_FCN_TUPLE(Slider, setValue));
        this->registerCall(this, POTHOS_FCN_TUPLE(Slider, setMinimum));
        this->registerCall(this, POTHOS_FCN_TUPLE(Slider, setMaximum));
        this->registerCall(this, POTHOS_FCN_TUPLE(Slider, setSingleStep));
        this->registerSignal("valueChanged");
        connect(_slider, &MyDoubleSlider::valueChanged, this, &Slider::handleValueChanged);
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
        return _slider->value();
    }

    void setValue(const double value)
    {
        _slider->setValue(value);
    }

    void setMinimum(const double value)
    {
        _slider->setMinimum(value);
    }

    void setMaximum(const double value)
    {
        _slider->setMaximum(value);
    }

    void setSingleStep(const double value)
    {
        _slider->setSingleStep(value);
    }

    void activate(void)
    {
        //emit current value when design becomes active
        this->emitSignal("valueChanged", this->value());
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
    void handleValueChanged(const double value)
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
    MyDoubleSlider *_slider;
};

static Pothos::BlockRegistry registerSlider(
    "/widgets/slider", &Slider::make);

#include "Slider.moc"
