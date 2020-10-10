#pragma once

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuickItem>
#include <QQuickWindow>

class MandelbrotSetRenderer
  : public QObject
  , protected QOpenGLFunctions
{
    Q_OBJECT

  public:
    MandelbrotSetRenderer();
    ~MandelbrotSetRenderer();

    void setTime(const qreal& time);

    void setViewArea(const QRectF& viewArea);

    void setViewportSize(const QSize& viewportSize);

    void setWindow(QQuickWindow* window);

  public slots:
    void init();
    void paint();

  private:
    qreal mTime;
    QRectF mViewArea;
    QSize mViewportSize;
    QQuickWindow* mWindow;
    QOpenGLShaderProgram* mProgram;
};

class MandelbrotSet : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal time READ time WRITE setTime NOTIFY timeChanged)

    Q_PROPERTY(QRectF viewArea READ viewArea WRITE setViewArea NOTIFY viewAreaChanged)

    QML_ELEMENT

  public:
    MandelbrotSet();

    qreal time() const;
    void setTime(qreal time);

    QRectF viewArea() const;
    void setViewArea(const QRectF& viewArea);

  signals:
    void timeChanged();
    void viewAreaChanged();

  public slots:
    void sync();
    void cleanup();

  private slots:
    void handleWindowChanged(QQuickWindow* window);

  private:
    void releaseResources() override;

    qreal mTime;
    QRectF mViewArea;
    MandelbrotSetRenderer* mRenderer;
};
