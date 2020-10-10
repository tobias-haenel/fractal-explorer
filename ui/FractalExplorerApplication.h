#pragma once

#include <QGuiApplication>
#include <QQuickView>

#include <controllers/MainController.h>

class FractalExplorerApplication : public QGuiApplication
{
    Q_OBJECT
  public:
    explicit FractalExplorerApplication(int& argc, char** argv);

  private:
    MainController* mMainController;
    QQuickView mView;
};
