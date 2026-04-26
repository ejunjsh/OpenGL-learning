#include "actionbutton.h"

ActionButton::ActionButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    setFixedHeight(30);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setStyleSheet(
        "QPushButton {"
        "  background-color: #3a8fff;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 5px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #5a9fff;"
        "}"
    );
}
