#pragma once

#include <QWidget>
#include <filesystem>
#include <memory>

#include "disker/widgets/selector.h"

namespace Ui {
class disker;
}  // namespace Ui

namespace disker {
class DiskerWidget final : public QWidget {
    Q_OBJECT

  public:
    DiskerWidget(QWidget* parent = nullptr);
    ~DiskerWidget();

  private slots:
    void addAnalyser(const std::filesystem::path& path);
    void closeTab(int tab_idx) const;

  private:
    std::unique_ptr<Ui::disker> ui_;
    SelectorWidget* selector_ = nullptr;
};
}  // namespace disker