#include "actionbutton.h"
#include "panel.h"

ActionButton::ActionButton(const QString &text, QWidget *parent, GLWidget *glWidget)
    : QPushButton(text, parent)
    , m_glWidget(glWidget)
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

    if (m_glWidget) {
        m_glWidget->setName(this->text());
    }

    connect(this, &QPushButton::clicked, [this]() {
        Panel *panel = qobject_cast<Panel*>(this->parent());
        if (panel) {
            panel->triggerSignal(m_glWidget);
        }
    });
}
