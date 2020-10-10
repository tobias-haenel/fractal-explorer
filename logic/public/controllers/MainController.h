#pragma once

#include "logic_global.h"

#include <QObject>

class LOGIC_EXPORT MainController : public QObject
{
    Q_OBJECT

  public:
    explicit MainController(QObject* parent);
};
