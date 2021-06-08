#pragma once

#include <QString>
#include <QWidget>
#include <filesystem>
#include <memory>
#include <optional>

namespace Ui {
class selection;
}  // namespace Ui

namespace disker {
class SelectionWidget final : public QWidget {
    Q_OBJECT

  public:
    using Path = std::filesystem::path;

    SelectionWidget(const QString& name, const QString& icon, std::optional<Path> path, QWidget* parent = nullptr);
    ~SelectionWidget();

  signals:
    void addAnalyser(const Path& path) const;

  private slots:
    void action();

  private:
    std::unique_ptr<Ui::selection> ui_;
    std::optional<Path> path_;
};
}  // namespace disker