#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <FractalExplorerApplication.h>

int
main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    FractalExplorerApplication app(argc, argv);

    return app.exec();
}
