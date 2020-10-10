#include "MandelbrotSet.h"

#include <QRunnable>

MandelbrotSetRenderer::MandelbrotSetRenderer()
  : mTime{ 0 }
  , mViewArea(QPointF(1, -1), QPointF(-2.5, 1))
  , mProgram{ 0 }
{}

MandelbrotSetRenderer::~MandelbrotSetRenderer()
{
    delete mProgram;
}

void
MandelbrotSetRenderer::setTime(const qreal& time)
{
    mTime = time;
}

void
MandelbrotSetRenderer::setViewArea(const QRectF& viewArea)
{
    mViewArea = viewArea;
}

void
MandelbrotSetRenderer::setViewportSize(const QSize& viewportSize)
{
    mViewportSize = viewportSize;
}

void
MandelbrotSetRenderer::setWindow(QQuickWindow* window)
{
    mWindow = window;
}

void
MandelbrotSetRenderer::init()
{
    if (!mProgram) {
        QSGRendererInterface* rif = mWindow->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL ||
                 rif->graphicsApi() == QSGRendererInterface::OpenGLRhi);

        initializeOpenGLFunctions();

        mProgram = new QOpenGLShaderProgram();
        mProgram->addCacheableShaderFromSourceCode(
          QOpenGLShader::Vertex,
          "attribute highp vec4 vertices;"
          "varying highp vec2 coords;"
          "uniform highp vec2 viewAreaSize;"
          "uniform highp vec2 viewAreaTopLeft;"
          "void main() {"
          "    gl_Position = vertices;"
          "    highp vec2 normDispCoords = 0.5f * vec2(1.0f - vertices.x, vertices.y + 1.0f);"
          "    coords = (normDispCoords * viewAreaSize) + viewAreaTopLeft;"
          "}");
        mProgram->addCacheableShaderFromSourceCode(
          QOpenGLShader::Fragment,
          "uniform lowp float t;"
          "varying highp vec2 coords;"
          "void main() {"
          "    lowp float x = 0;"
          "    lowp float y = 0;"
          "    int iteration = 0;"
          "    int maxIteration = 1000;"
          "    while (x*x + y*y <= 4.0f && iteration < maxIteration) {"
          "        lowp float xTemp = x*x - y*y + coords.x;"
          "        y = 2.0f * x * y + coords.y;"
          "        x = xTemp;"
          "        iteration += 1;"
          "    }"
          "    lowp float iterationFraction = iteration / (maxIteration - 1.0f);"
          "    gl_FragColor = vec4(vec3(iterationFraction), 1.0);"
          "}");

        mProgram->bindAttributeLocation("vertices", 0);
        bool linkOk = mProgram->link();
        if (!linkOk) {
            qDebug() << mProgram->log();
        }
    }
}

void
MandelbrotSetRenderer::paint()
{
    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    mWindow->beginExternalCommands();

    mProgram->bind();

    mProgram->enableAttributeArray(0);

    double vertices[] = {
        -1, -1, // LB
        +1, -1, // RB
        -1, +1, // LT
        +1, +1  // RT
    };

    // This example relies on (deprecated) client-side pointers for the vertex
    // input. Therefore, we have to make sure no vertex buffer is bound.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mProgram->setAttributeArray(0, GL_DOUBLE, vertices, 2);
    mProgram->setUniformValue("t", static_cast<float>(mTime));
    mProgram->setUniformValue("viewAreaSize", mViewArea.size());
    mProgram->setUniformValue("viewAreaTopLeft", mViewArea.topLeft());

    glViewport(0, 0, mViewportSize.width(), mViewportSize.height());

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mProgram->disableAttributeArray(0);
    mProgram->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    mWindow->resetOpenGLState();

    mWindow->endExternalCommands();
}

MandelbrotSet::MandelbrotSet()
  : mViewArea{ QPointF(1, -1), QPointF(-2.5, 1) }
  , mRenderer{ nullptr }
{
    connect(this, &QQuickItem::windowChanged, this, &MandelbrotSet::handleWindowChanged);
}

qreal
MandelbrotSet::time() const
{
    return mTime;
}

void
MandelbrotSet::setTime(qreal time)
{
    if (mTime == time) {
        return;
    }
    mTime = time;
    emit timeChanged();
    if (window()) {
        window()->update();
    }
}

QRectF
MandelbrotSet::viewArea() const
{
    return mViewArea;
}

void
MandelbrotSet::setViewArea(const QRectF& viewArea)
{
    if (mViewArea == viewArea) {
        return;
    }

    mViewArea = viewArea;
    emit viewAreaChanged();
    if (window()) {
        window()->update();
    }
}

void
MandelbrotSet::sync()
{
    if (!mRenderer) {
        mRenderer = new MandelbrotSetRenderer();
        connect(window(),
                &QQuickWindow::beforeRendering,
                mRenderer,
                &MandelbrotSetRenderer::init,
                Qt::DirectConnection);
        connect(window(),
                &QQuickWindow::beforeRenderPassRecording,
                mRenderer,
                &MandelbrotSetRenderer::paint,
                Qt::DirectConnection);
    }
    mRenderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    mRenderer->setViewArea(mViewArea);
    mRenderer->setTime(mTime);
    mRenderer->setWindow(window());
}

void
MandelbrotSet::cleanup()
{
    delete mRenderer;
    mRenderer = nullptr;
}

void
MandelbrotSet::handleWindowChanged(QQuickWindow* window)
{
    if (window) {
        connect(window,
                &QQuickWindow::beforeSynchronizing,
                this,
                &MandelbrotSet::sync,
                Qt::DirectConnection);
        connect(window,
                &QQuickWindow::sceneGraphInvalidated,
                this,
                &MandelbrotSet::cleanup,
                Qt::DirectConnection);
        window->setColor(Qt::black);
    }
}

class CleanupJob : public QRunnable
{
  public:
    CleanupJob(MandelbrotSetRenderer* renderer)
      : mRenderer{ renderer }
    {}
    void run() override { delete mRenderer; }

  private:
    MandelbrotSetRenderer* mRenderer;
};

void
MandelbrotSet::releaseResources()
{
    window()->scheduleRenderJob(new CleanupJob(mRenderer), QQuickWindow::BeforeSynchronizingStage);
    mRenderer = nullptr;
}
