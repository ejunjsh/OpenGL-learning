#include "header/glmodelloading.h"
#include <QMatrix4x4>
#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QFileInfo>

GLModelLoading::GLModelLoading(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLModelLoading");
    setupMenu();
}

GLModelLoading::~GLModelLoading()
{
    makeCurrent();
    m_model.reset();
    doneCurrent();
}

void GLModelLoading::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QLabel *authorLabel = new QLabel("Author Enhanced", menu);
    authorLabel->setStyleSheet("color: white; font-weight: bold; margin-top: 8px;");

    m_modelSelector = new QComboBox(menu);
    m_modelSelector->setStyleSheet(
        "QComboBox { color: white; background: #3a3a3a; border: 1px solid #555; "
        "padding: 4px 8px; border-radius: 4px; }"
        "QComboBox QAbstractItemView { color: white; background: #2b2b2b; "
        "selection-background-color: #555; }"
        "QComboBox::drop-down { border: none; }");

    // Scan objects/ directory for model files
    QDir objDir("objects");
    const QStringList filters = {"*.obj", "*.dae", "*.gltf", "*.fbx"};
    QFileInfoList entries = objDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &dir : entries)
    {
        for (const QString &filter : filters)
        {
            QFileInfoList models = QDir(dir.absoluteFilePath()).entryInfoList({filter});
            for (const QFileInfo &modelFile : models)
            {
                QString displayName = dir.fileName() + " (" + modelFile.suffix().toUpper() + ")";
                m_modelSelector->addItem(displayName);
                m_modelPaths.append(modelFile.filePath());
            }
        }
    }

    menuLayout->addWidget(authorLabel);
    menuLayout->addWidget(m_modelSelector);
    menuLayout->addStretch();

    connect(m_modelSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GLModelLoading::onModelSelected);
}

void GLModelLoading::onModelSelected(int index)
{
    if (index < 0 || index >= m_modelPaths.size())
        return;
    makeCurrent();
    loadModelFile(m_modelPaths[index]);
    doneCurrent();
}

void GLModelLoading::loadModelFile(const QString &filePath)
{
    m_model.reset();
    m_model = std::make_unique<Model>(filePath.toStdString());
}

void GLModelLoading::initializeGL()
{
    GLBase::initializeGL();

    // Compile model loading shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/model_loading/model_loading.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/model_loading/model_loading.frag"))
    {
        qFatal("Failed to compile model loading shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link model loading shader program");
    }

    // Load first model (or default if dropdown not yet populated)
    if (!m_modelPaths.isEmpty())
    {
        loadModelFile(m_modelPaths.first());
    }
}

void GLModelLoading::paintGL()
{
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (!m_model)
        return;

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    m_program.bind();
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);

    // Model matrix — center and scale
    QMatrix4x4 model;
    model.translate(QVector3D(0.0f, 0.0f, 0.0f));
    model.scale(1.0f);
    m_program.setUniformValue("model", model);

    // Draw the loaded model (iterates over all meshes)
    m_model->Draw(m_program);
}
