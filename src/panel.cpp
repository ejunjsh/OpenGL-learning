#include "header/panel.h"
#include "header/actionbutton.h"
#include "header/gltriangle.h"
#include "header/gltexture.h"
#include "header/gltransform.h"
#include "header/glcamera.h"
#include "header/glcoordinate.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>
#include <QScrollArea>

Panel::Panel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void Panel::triggerSignal(GLBase *widget)
{
    emit reloadRequested(widget);
}

void Panel::setupUI()
{
    setStyleSheet("background-color: #2b2b2b;");

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // ---- 滚动区域 ----
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { background: #1e1e1e; width: 8px; margin: 0; }"
        "QScrollBar::handle:vertical { background: #555; border-radius: 4px; min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background: #777; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }");
    outerLayout->addWidget(scrollArea);

    // ---- 滚动内容容器 ----
    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(4);
    scrollArea->setWidget(scrollContent);

    // ---- 通用样式 ----
    const QString headerStyle =
        "QPushButton { text-align: left; padding: 8px 12px; color: #ccc; "
        "background: rgba(255,255,255,8); border: none; border-radius: 4px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background: rgba(255,255,255,18); }";

    // ---- 创建手风琴分组的辅助 lambda ----
    auto addSection = [&](const QString &title, std::initializer_list<ActionButton*> buttons, bool expanded = false) {
        QPushButton *header = new QPushButton((expanded ? "▼ " : "▶ ") + title, this);
        header->setStyleSheet(headerStyle);
        header->setCursor(Qt::PointingHandCursor);
        header->setFixedHeight(34);
        layout->addWidget(header);

        QFrame *content = new QFrame(this);
        content->setStyleSheet("background: transparent; border: none;");
        QVBoxLayout *contentLayout = new QVBoxLayout(content);
        contentLayout->setContentsMargins(8, 4, 8, 4);
        contentLayout->setSpacing(4);
        for (auto *btn : buttons) {
            contentLayout->addWidget(btn);
            connect(btn, &ActionButton::reloadRequested, this, &Panel::triggerSignal);
        }
        content->setVisible(expanded);
        layout->addWidget(content);

        connect(header, &QPushButton::clicked, this, [header, content, title]() {
            bool expanding = !content->isVisible();
            header->setText((expanding ? "▼ " : "▶ ") + title);

            // 停止正在运行的动画
            auto *oldAnim = content->findChild<QPropertyAnimation*>(QString(), Qt::FindDirectChildrenOnly);
            if (oldAnim) oldAnim->stop();

            if (expanding) {
                content->setVisible(true);
                int targetHeight = content->sizeHint().height();
                content->setMaximumHeight(0);

                auto *anim = new QPropertyAnimation(content, "maximumHeight", content);
                anim->setDuration(250);
                anim->setEasingCurve(QEasingCurve::OutCubic);
                anim->setStartValue(0);
                anim->setEndValue(targetHeight);
                anim->start(QAbstractAnimation::DeleteWhenStopped);
            } else {
                int startHeight = content->height();
                content->setMaximumHeight(startHeight);

                auto *anim = new QPropertyAnimation(content, "maximumHeight", content);
                anim->setDuration(250);
                anim->setEasingCurve(QEasingCurve::OutCubic);
                anim->setStartValue(startHeight);
                anim->setEndValue(0);
                connect(anim, &QPropertyAnimation::finished, content, [content]() {
                    content->setVisible(false);
                });
                anim->start(QAbstractAnimation::DeleteWhenStopped);
            }
        });
    };

    // ---- Getting Started ----
    addSection("Getting Started", {
        new ActionButton("Hello Triangle", this, []() { return new GLTriangle(); }),
        new ActionButton("Texture",  this, []() { return new GLTexture(); }),
        new ActionButton("Transform", this, []() { return new GLTransform(); }),
        new ActionButton("Coordinate Systems", this, []() { return new GLCoordinate(); }),
        new ActionButton("Camera", this, []() { return new GLCamera(); })
    }, true);
    addSection("Lighting", {}, false);
    addSection("Model Loading", {}, false);
    addSection("Advanced OpenGL", {}, false);
    addSection("Advanced Lighting", {}, false);
    addSection("PBR", {}, false);
     addSection("In Practice", {}, false);

    layout->addStretch();

    // 默认加载第一个
    QTimer::singleShot(0, this, [this]() {
        auto *btn = findChild<ActionButton*>();
        if (btn) btn->click();
    });
}
