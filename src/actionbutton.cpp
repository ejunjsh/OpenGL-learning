#include "header/actionbutton.h"

ActionButton::ActionButton(const QString &text, QWidget *parent, Factory factory)
    : QPushButton(text, parent)
    , m_factory(std::move(factory))
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

    connect(this, &QPushButton::clicked, [this]() {
        if (!m_glWidget && m_factory) {
            m_glWidget = m_factory();
            if (m_glWidget) {
                m_glWidget->setName(this->text());
            }
        }
        emit reloadRequested(m_glWidget);
    });
}
