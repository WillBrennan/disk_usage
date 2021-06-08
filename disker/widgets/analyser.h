#pragma once

#include <QString>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QWidget>
#include <filesystem>
#include <memory>
#include <optional>

#include "disker/analyser.h"

namespace Ui {
class analyser;
}  // namespace Ui

namespace disker {
class AnalyserWidget final : public QWidget {
    Q_OBJECT

  public:
    using Path = std::filesystem::path;

    AnalyserWidget(Path path, QWidget* parent = nullptr);
    ~AnalyserWidget();

  private slots:
    void refresh();
    void update();

  private:
    struct Parent {
        QTreeWidgetItem* widget = nullptr;
        std::size_t idx;
    };

    std::unique_ptr<Ui::analyser> ui_;
    QTimer timer_;

    std::unique_ptr<DiskAnalyser> analyser_;
    Path path_;
    DiskAnalyser::State state_;
    bool populated_ = false;
};
}  // namespace disker