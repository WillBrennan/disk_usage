#include <QApplication>
#include <QMessageBox>
#include <cstdlib>

#include "disker/widgets/disker.h"

int main(int argc, char** argv) {
    auto app = QApplication(argc, argv);

    disker::DiskerWidget disker;
    disker.show();

    return app.exec();
}