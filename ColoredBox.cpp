// Copyright (c) 2017-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <Pothos/Framework.hpp>
#include <QGroupBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QStaticText>

/*!
 * The text on the colored box
 */
class ColoredBoxGraphicsText : public QGraphicsObject
{
    Q_OBJECT
public:
    ColoredBoxGraphicsText(void):
        _size(12)
    {
        QTextOption to;
        to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        _st.setTextOption(to);
    }

    QRectF boundingRect(void) const
    {
        return QRectF(QPointF(), _st.size());
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        const auto w = this->scene()->width();
        const auto h = this->scene()->height();
        _st.setTextWidth(w);
        const auto size = _st.size();
        painter->drawStaticText(QPointF(
            (w-size.width())/2,
            (h-size.height())/2), _st);
    }

public slots:
    void setText(const QString &text)
    {
        _text = text;
        this->updateStaticText();
    }

    void setFontSize(const int &size)
    {
        _size = size;
        this->updateStaticText();
    }

    void setColor(const QColor &color)
    {
        _color = color;
        this->updateStaticText();
    }

private:
    void updateStaticText(void)
    {
        _st.setText(QString("<span style='color:%1;font-size:%2pt;'>%3</span>")
            .arg((_color.lightnessF() > 0.5)?"black":"white")
            .arg(_size)
            .arg(_text.toHtmlEscaped()));
    }

    QString _text;
    int _size;
    QColor _color;
    QStaticText _st;
};

/*!
 * Custom graphics view to handle resizing.
 */
class ColoredBoxGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    ColoredBoxGraphicsView(QWidget *parent):
        QGraphicsView(parent)
    {
        this->setScene(new QGraphicsScene(this));
    }

protected:
    void resizeEvent(QResizeEvent *event)
    {
        QGraphicsView::resizeEvent(event);
        this->scene()->setSceneRect(QRectF(QPointF(), event->size()));
    }
};

/***********************************************************************
 * |PothosDoc Colored Box
 *
 * A re-sizable box with configurable title and color.
 * Its intended use is simple status visualization.
 * Example: PASS = red, FAIL = green.
 *
 * |category /Widgets
 * |keywords color status pass fail
 *
 * |param title The name of the value displayed by this widget
 * |default "My Status"
 * |widget StringEntry()
 *
 * |param fontSize[Font Size] The font size for the digits in the widget.
 * |default 12
 * |widget SpinBox(minimum=8)
 * |preview disable
 *
 * |param text The text to embed within the colored rectangle.
 * |default ""
 * |widget StringEntry()
 * |preview valid
 *
 * |param color The fill color of the status box in this widget.
 * |widget ColorPicker(mode=pastel)
 * |default "#77dd77"
 * |preview disable
 *
 * |mode graphWidget
 * |factory /widgets/colored_box()
 * |setter setTitle(title)
 * |setter setFontSize(fontSize)
 * |setter setText(text)
 * |setter setColor(color)
 **********************************************************************/
class ColoredBox : public QGroupBox, public Pothos::Block
{
    Q_OBJECT
public:

    static Block *make(void)
    {
        return new ColoredBox();
    }

    ColoredBox(void):
        _view(new ColoredBoxGraphicsView(this)),
        _text(new ColoredBoxGraphicsText()),
        _layout(new QHBoxLayout(this))
    {
        _view->scene()->addItem(_text);

        this->setStyleSheet("QGroupBox {font-weight: bold;}");
        this->registerCall(this, POTHOS_FCN_TUPLE(ColoredBox, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(ColoredBox, setTitle));
        this->registerCall(this, POTHOS_FCN_TUPLE(ColoredBox, setFontSize));
        this->registerCall(this, POTHOS_FCN_TUPLE(ColoredBox, setText));
        this->registerCall(this, POTHOS_FCN_TUPLE(ColoredBox, setColor));

        _layout->addWidget(_view);
        _layout->setContentsMargins(QMargins());
        _layout->setSpacing(0);
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
        QMetaObject::invokeMethod(_text, "setFontSize", Qt::QueuedConnection, Q_ARG(int, size));
        QMetaObject::invokeMethod(_view, "repaint", Qt::QueuedConnection);
    }

    void setText(const QString &text)
    {
        QMetaObject::invokeMethod(_text, "setText", Qt::QueuedConnection, Q_ARG(QString, text));
        QMetaObject::invokeMethod(_view, "repaint", Qt::QueuedConnection);
    }

    void setColor(const QString &color)
    {
        QMetaObject::invokeMethod(this, "handleSetColor", Qt::QueuedConnection, Q_ARG(QColor, QColor(color)));
        QMetaObject::invokeMethod(_text, "setColor", Qt::QueuedConnection, Q_ARG(QColor, QColor(color)));
        QMetaObject::invokeMethod(_view, "repaint", Qt::QueuedConnection);
    }

private slots:

    void handleSetTitle(const QString &title)
    {
        QGroupBox::setTitle(title);
    }

    void handleSetColor(const QColor &color)
    {
        _view->scene()->setBackgroundBrush(color);
    }

protected:

    void mousePressEvent(QMouseEvent *event)
    {
        QGroupBox::mousePressEvent(event);
        event->ignore(); //allows for dragging from QGroupBox title
    }

    ColoredBoxGraphicsView *_view;
    ColoredBoxGraphicsText *_text;
    QHBoxLayout *_layout;
};

static Pothos::BlockRegistry registerColoredBox(
    "/widgets/colored_box", &ColoredBox::make);

#include "ColoredBox.moc"
