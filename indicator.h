#ifndef Indicator_h
#define Indicator_h

#include <QtWidgets/QFrame>

class QHBoxLayout;

class Indicator: public QFrame
{
  Q_OBJECT

public:
  Indicator(QWidget *parent = 0);
  virtual ~Indicator();

public slots:
  qint64 value() {return m_Value;};
  void setFont(QFont font);
  void setSize(int size);
  void setValue(qint64 value);
  void setValueMin(qint64 value);
  void setValueMax(qint64 value);
  void setDeltaMin(qint64 delta);
  void applyDelta(qint64 delta);

signals:
  void valueChanged(qint64 value);

private:
  QHBoxLayout *m_Layout;
  QFont m_Font;
  qint64 m_Value;
  qint64 m_ValueMin;
  qint64 m_ValueMax;
};

#endif
