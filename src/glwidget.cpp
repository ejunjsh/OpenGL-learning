#include "header/glwidget.h"
#include <QMatrix4x4>
#include <QImage>
#include <QtMath>
#include <QDebug>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_menuVisible(false)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::OpenHandCursor);

    m_fpsLabel = new QLabel(this);
    m_fpsLabel->setStyleSheet("color: yellow; background: rgba(0,0,0,100); padding: 4px; min-width: 80px; font-weight: bold;");
    m_fpsLabel->setText("FPS: 0");
    m_fpsLabel->move(10, 0);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setStyleSheet("color: white; background: rgba(0,0,0,150); padding: 4px; font-weight: bold;");

    // 右上角菜单按钮
    m_menuButton = new QLabel(this);
    m_menuButton->setText("☰");
    m_menuButton->setStyleSheet("color: white; background: rgba(0,0,0,150); padding: 4px 8px; font-size: 16px;");
    m_menuButton->setFixedSize(30, 24);
    m_menuButton->setAlignment(Qt::AlignCenter);
    m_menuButton->setCursor(Qt::PointingHandCursor);

    qApp->installEventFilter(this);  // 全局安装，捕获所有事件

    // 透明下拉面板
    m_menuPanel = new QFrame(this);
    m_menuPanel->setStyleSheet("background: rgba(0,0,0,180); border: 1px solid rgba(255,255,255,50);");
    m_menuPanel->setFixedSize(200, 300);
    m_menuPanel->hide();

    // 右下角帮助按钮
    m_helpButton = new QLabel(this);
    m_helpButton->setText("?");
    m_helpButton->setStyleSheet("color: white; background: rgba(0,0,0,150); padding: 2px 6px; font-size: 14px; font-weight: bold;");
    m_helpButton->setFixedSize(20, 20);
    m_helpButton->setAlignment(Qt::AlignCenter);
    m_helpButton->setCursor(Qt::PointingHandCursor);

    // 帮助面板
    m_helpPanel = new QFrame(this);
    m_helpPanel->setStyleSheet("background: rgba(0,0,0,200); border: 1px solid rgba(255,255,255,50);");
    m_helpPanel->setFixedSize(170, 150);
    m_helpPanel->hide();

    QVBoxLayout *helpLayout = new QVBoxLayout(m_helpPanel);
    m_helpLabel = new QLabel(this);
    m_helpLabel->setStyleSheet("QLabel { border: none; color: white; font-size: 12px; background: transparent; }");
    m_helpLabel->setWordWrap(true);
    helpLayout->addWidget(m_helpLabel);

    connect(&m_frameTimer, &QTimer::timeout, this, [this]() {
        const qint64 ms = m_timer.restart();
        const float dt = static_cast<float>(ms) / 1000.0f;
        m_elapsedTime += dt;
        updateCamera(dt);

        m_frameCount++;
        const qint64 elapsed = m_fpsTimer.elapsed();
        if (elapsed >= 1000) {
            const float fps = m_frameCount * 1000.0 / elapsed;
            m_fpsLabel->setText(QString("FPS: %1").arg(fps, 0, 'f', 1));
            m_frameCount = 0;
            m_fpsTimer.restart();
        }

        update();
    });

    setFixedSize(960, 640);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    m_rootObject.reset();
    doneCurrent();
}

void GLWidget::setName(const QString &name)
{
    m_nameLabel->setText(name);
    m_nameLabel->adjustSize();
}

void GLWidget::toggleMenu()
{
    m_menuVisible = !m_menuVisible;
    if (m_menuVisible) {
        m_menuPanel->move(width() - m_menuPanel->width() - 10, 24);
        m_menuPanel->show();
    } else {
        m_menuPanel->hide();
    }
}

void GLWidget::toggleHelp()
{
    m_helpVisible = !m_helpVisible;
    if (m_helpVisible) {
        m_helpLabel->setText(getHelpText());
        m_helpPanel->move(width() - m_helpPanel->width() - 5, height() - m_helpPanel->height() - 5);
        m_helpPanel->show();
    } else {
        m_helpPanel->hide();
    }
}

QString GLWidget::getHelpText() const
{
    return "Basic OpenGL Widget";
}

bool GLWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (obj == m_menuButton) {
            toggleMenu();
            return true;
        } else if (obj == m_helpButton) {
            toggleHelp();
            return true;
        } else if (m_menuVisible) {
            QPoint globalPos = QCursor::pos();
            QPoint localPos = m_menuPanel->mapFromGlobal(globalPos);
            QRect menuRect = m_menuPanel->rect();
            if (menuRect.contains(localPos)) {
                return false;
            }
            toggleMenu();
            return true;
        } else if (m_helpVisible) {
            QPoint globalPos = QCursor::pos();
            QPoint localPos = m_helpPanel->mapFromGlobal(globalPos);
            QRect helpRect = m_helpPanel->rect();
            if (helpRect.contains(localPos)) {
                return false;
            }
            toggleHelp();
            return true;
        }
    } else if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
        if (obj == m_menuButton) {
            if (event->type() == QEvent::Enter) {
                m_menuButton->setStyleSheet("color: white; background: rgba(80,80,80,200); padding: 4px 8px; font-size: 16px;");
            } else {
                m_menuButton->setStyleSheet("color: white; background: rgba(0,0,0,150); padding: 4px 8px; font-size: 16px;");
            }
        } else if (obj == m_helpButton) {
            if (event->type() == QEvent::Enter) {
                m_helpButton->setStyleSheet("color: white; background: rgba(80,80,80,200); padding: 2px 6px; font-size: 14px; font-weight: bold;");
            } else {
                m_helpButton->setStyleSheet("color: white; background: rgba(0,0,0,150); padding: 2px 6px; font-size: 14px; font-weight: bold;");
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

unsigned int GLWidget::loadTexture(const QString &path, bool flipVertically, bool flipHorizontally)
{
    QImage image(path);
    if (image.isNull()) {
        qWarning() << "Failed to load texture:" << path;
        return 0;
    }
    image = image.convertToFormat(QImage::Format_RGBA8888);

    if (flipVertically) {
        image = image.mirrored(false, true);
    }
    if (flipHorizontally) {
        image = image.mirrored(true, false);
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    m_hasMenuContent = m_menuPanel->layout() && m_menuPanel->layout()->count() > 0;
    if (m_hasMenuContent) {
        m_menuButton->show();
    } else {
        m_menuButton->hide();
    }

    if (m_hasMenuContent) {
        m_nameLabel->move(width() - m_nameLabel->width() - m_menuButton->width() - 5, 0);
    } else {
        m_nameLabel->move(width() - m_nameLabel->width() - 10, 0);
    }
    m_menuButton->move(width() - m_menuButton->width() - 5, 0);
    m_helpButton->move(width() - m_helpButton->width() - 5, height() - m_helpButton->height() - 5);
    if (m_menuVisible) {
        m_menuPanel->move(width() - m_menuPanel->width() - 5, 24);
    }
    if (m_helpVisible) {
        m_helpPanel->move(width() - m_helpPanel->width() - 5, height() - m_helpPanel->height() - 5);
    }
    QOpenGLWidget::resizeEvent(event);
}

void GLWidget::hideEvent(QHideEvent *event)
{
    stopRendering();
    QOpenGLWidget::hideEvent(event);
}

void GLWidget::showEvent(QShowEvent *event)
{
    startRendering();
    QOpenGLWidget::showEvent(event);
}

void GLWidget::updateCamera(float dt)
{
    // 空实现，子类 GLCameraWidget 会重写
    Q_UNUSED(dt);
}

void GLWidget::stopRendering()
{
    m_timer.invalidate();
    m_fpsTimer.invalidate();
    m_frameTimer.stop();
}

void GLWidget::startRendering()
{
    if (!m_frameTimer.isActive()) {
        m_timer.restart();
        m_fpsTimer.restart();
        m_frameTimer.start(16);
    }
}
