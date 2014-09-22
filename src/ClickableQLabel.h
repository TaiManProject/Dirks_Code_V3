#ifndef CLICKABLEQLABEL_H
#define CLICKABLEQLABEL_H
#include "commonTool.h"
#include <QWidget>

class ClickableQLabel:public QLabel {
Q_OBJECT

public:
    //explicit ClickableQLabel( const QString& text ="", QWidget * parent = 0 ) {
    //    this->setText(text);
    //}
    explicit ClickableQLabel( QWidget * parent = 0, Qt::WindowFlags f = 0 )
        : QLabel () {
    }


signals:
    void clicked(ClickableQLabel* label);

protected:
    void mousePressEvent (QMouseEvent* event) {
        emit clicked(this);
    }
};

#endif // CLICKABLEQLABEL_H
