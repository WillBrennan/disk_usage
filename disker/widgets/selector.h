#pragma once

#include <QWidget>
#include <filesystem>
#include <memory>

namespace Ui {
class selector;
}  // namespace Ui

namespace disker {
class SelectorWidget final : public QWidget {
    Q_OBJECT

  public:
    SelectorWidget(QWidget* parent = nullptr);
    ~SelectorWidget();

  signals:
    void addAnalyser(const std::filesystem::path& path) const;

  private:
    std::unique_ptr<Ui::selector> ui_;
};
}  // namespace disker