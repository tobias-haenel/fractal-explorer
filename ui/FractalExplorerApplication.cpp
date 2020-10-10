#include "FractalExplorerApplication.h"

#include <QQmlContext>

FractalExplorerApplication::FractalExplorerApplication(int& argc, char** argv)
  : QGuiApplication(argc, argv)
  , mMainController{ new MainController(this) }
{
    QQmlContext* context = mView.rootContext();
    context->setContextProperty("MainController", mMainController);

    const QUrl url(QStringLiteral("qrc:/MainView.qml"));
    mView.setResizeMode(QQuickView::SizeRootObjectToView);
    mView.setSource(url);

    QObject::connect(
      &mView,
      &QQuickView::statusChanged,
      this,
      [url](QQuickView::Status status) {
          if (status == QQuickView::Status::Error)
              QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);

    mView.show();
}
