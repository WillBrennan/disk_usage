#pragma once

#include <QPieSeries>
#include <QString>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QtCharts>
#include <filesystem>
#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "disker/analyser.h"
#include "disker/helpers/flat_tree.h"

namespace Ui {
class analyser;
}  // namespace Ui

namespace disker {
class AnalyserWidget final : public QWidget {
    Q_OBJECT

  public:
    using Path = std::filesystem::path;
    using File = DiskAnalyser::File;
    using Index = FlatTree<File>::Index;

    AnalyserWidget(Path path, QWidget* parent = nullptr);
    ~AnalyserWidget();

  private slots:
    void refresh();
    void update();

    void updateChartFromItem(QTreeWidgetItem* item, int col);
    void updateChartFromChange(QTreeWidgetItem* curr, QTreeWidgetItem* prev);
    void updateChart(Index file_idx);

    void clickedChart(QPieSlice* slice);

  private:
    using IndexedSlice = std::tuple<QPieSlice*, Index>;

    std::unique_ptr<Ui::analyser> ui_;
    QTimer timer_;

    std::unique_ptr<DiskAnalyser> analyser_;
    Path path_;
    DiskAnalyser::State state_;
    bool populated_ = false;

    QPieSeries* series_ = nullptr;
    QChartView* chart_view_ = nullptr;

    Index selected_chart_ = FlatTree<File>::NoParent;
    std::vector<QTreeWidgetItem*> widgets_;
    std::vector<IndexedSlice> slices_;
};
}  // namespace disker