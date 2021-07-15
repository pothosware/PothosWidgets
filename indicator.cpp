// Copyright (C) 2016 Pavel Demin
// Copyright (c) 2021-2021 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <QtCore/qmath.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QMouseEvent>

#include "indicator.h"

//------------------------------------------------------------------------------

static const QString c_StyleSheets[3] =
{
  "color: %1;",
  "color: %1; background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 153, 153, 255), stop:0.49 rgba(255, 153, 153, 255), stop:0.50 rgba(0, 0, 0, 0), stop:1 rgba(0, 0, 0, 0))",
  "color: %1; background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0, 0, 0, 0), stop:0.50 rgba(0, 0, 0, 0), stop:0.51 rgba(153, 204, 255, 255), stop:1 rgba(153, 204, 255, 255))"
};

//------------------------------------------------------------------------------

class CustomDigit: public QLabel
{
public:
  CustomDigit(QWidget *parent = 0): QLabel(parent), m_Active(true), m_State(0), m_Delta(0), m_Color(Qt::black) {}

  void mouseMoveEvent(QMouseEvent *event)
  {
    if(!m_Active) return;
    if(event->pos().y() < height() / 2 && m_State != 1)
    {
      m_State = 1;
      applyStyleSheet();
    }
    if(event->pos().y() > height() / 2 && m_State != 2)
    {
      m_State = 2;
      applyStyleSheet();
    }
  }

  void leaveEvent(QEvent *)
  {
    if(!m_Active) return;
    m_State = 0;
    applyStyleSheet();
  }

  void mousePressEvent(QMouseEvent *event)
  {
    if(!m_Active) return;
    if(event->pos().y() < height() / 2)
    {
      m_Indicator->applyDelta(m_Delta);
    }
    if(event->pos().y() > height() / 2)
    {
      m_Indicator->applyDelta(-m_Delta);
    }
  }

  void wheelEvent(QWheelEvent *event)
  {
    if(!m_Active) return;
    m_Indicator->applyDelta(event->angleDelta().y() / 90 * m_Delta);
    event->accept();
  }

  void applyStyleSheet(void)
  {
    setStyleSheet(c_StyleSheets[m_State].arg(m_Color.name()));
  }

  bool m_Active;
  int m_State;
  qint64 m_Delta;
  QColor m_Color;
  Indicator *m_Indicator;
};

//------------------------------------------------------------------------------

Indicator::Indicator(QWidget *parent):
  QFrame(parent), m_Layout(0), m_Font(), m_Size(0),
  m_Value(0), m_ValueMin(0), m_ValueMax(0)
{
  m_Layout = new QHBoxLayout(this);
  m_Layout->setContentsMargins(1, 1, 1, 1);
  m_Layout->setSpacing(1);
  setLayout(m_Layout);
}

//------------------------------------------------------------------------------

Indicator::~Indicator()
{
}

//------------------------------------------------------------------------------

void Indicator::setFont(QFont font)
{
  m_Font = font;
  foreach(CustomDigit *digit, findChildren<CustomDigit *>())
  {
    digit->setFont(font);
  }
}

//------------------------------------------------------------------------------

void Indicator::setFontSize(int size)
{
  m_Font.setPointSize(size);
  setFont(m_Font);
  setSize(m_Size);
}

//------------------------------------------------------------------------------

void Indicator::setSize(int size)
{
  int i, width;
  CustomDigit *digit;
  QLayoutItem *item;
  QFontMetrics fm(m_Font);

  width = fm.horizontalAdvance('m') / 5 + 1;

  while((digit = findChild<CustomDigit *>()))
  {
    delete digit;
  }
  while((item = m_Layout->takeAt(0)))
  {
    delete item;
  }

  m_Layout->addStretch();
  for(i = size; i > 0; --i)
  {
    digit = new CustomDigit(this);
    digit->setText(QString::number(0));
    digit->setAlignment(Qt::AlignCenter);
    digit->setFont(m_Font);
    m_Layout->addWidget(digit);
    digit->setMouseTracking(true);
    digit->m_Delta = qint64(qPow(10.0, i - 1) + 0.5);
    digit->m_Indicator = this;
    if(i > 1 && i % 3 == 1) m_Layout->addSpacing(width);
  }
  m_Layout->addStretch();
  m_Size = size;
  setValue(m_Value);
}

//------------------------------------------------------------------------------

void Indicator::setValue(qint64 value)
{
  qint64 quotient;
  if(value < m_ValueMin || value > m_ValueMax) return;
  foreach(CustomDigit *digit, findChildren<CustomDigit *>())
  {
    quotient = value / digit->m_Delta;
    digit->m_Color = (quotient == 0)?Qt::lightGray:Qt::black;
    digit->applyStyleSheet();
    digit->setText(QString::number(quotient % 10));
  }
  m_Value = value;
  emit valueChanged(value);
}

//------------------------------------------------------------------------------

void Indicator::setValueMin(qint64 value)
{
  if(value > m_ValueMax) return;
  m_ValueMin = value;
  if(m_Value < value) setValue(value);
}

//------------------------------------------------------------------------------

void Indicator::setValueMax(qint64 value)
{
  if(value < m_ValueMin) return;
  m_ValueMax = value;
  if(m_Value > value) setValue(value);
}

//------------------------------------------------------------------------------

void Indicator::setDeltaMin(qint64 delta)
{
  foreach(CustomDigit *digit, findChildren<CustomDigit *>())
  {
    if(digit->m_Delta < delta)
    {
      digit->setText(QString::number(0));
      digit->setMouseTracking(false);
      digit->m_Active = false;
    }
    else
    {
      digit->setMouseTracking(true);
      digit->m_Active = true;
    }
  }
}

//------------------------------------------------------------------------------

void Indicator::applyDelta(qint64 delta)
{
  qint64 value = m_Value + delta;
  if(delta < 0 && value < m_ValueMin) return;
  if(delta > 0 && value > m_ValueMax) return;
  setValue(value);
}
