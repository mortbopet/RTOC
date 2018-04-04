#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QWidget>
#include <string>

namespace Ui {
class Configurator;
}

class Configurator : public QWidget {
    Q_OBJECT

public:
    explicit Configurator(QWidget* parent = 0);
    ~Configurator();

    void setOptions(const std::vector<std::string>& options);

private:
    Ui::Configurator* ui;
};

#endif  // CONFIGURATOR_H
