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
 * |param color The fill color of the status box in this widget.
 * |widget ColorPicker(mode=pastel)
 * |default "#77dd77"
 * |preview disable
 *
 * |mode graphWidget
 * |factory /widgets/colored_box()
 * |setter setTitle(title)
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
        _layout(new QHBoxLayout(this))
    {
        this->setStyleSheet("QGroupBox {font-weight: bold;}");
        this->registerCall(this, POTHOS_FCN_TUPLE(ColoredBox, widget));
        this->registerCall(this, POTHOS_FCN_TUPLE(ColoredBox, setTitle));
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

    void setColor(const QString &color)
    {
        QMetaObject::invokeMethod(this, "handleSetColor", Qt::QueuedConnection, Q_ARG(QColor, QColor(color)));
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
    QHBoxLayout *_layout;
};

static Pothos::BlockRegistry registerColoredBox(
    "/widgets/colored_box", &ColoredBox::make);

#include "ColoredBox.moc"
