#include "disker/widgets/analyser.h"

#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidget>
#include <chrono>
#include <iostream>
#include <iterator>
#include <stack>

#include "disker/analyser.h"
#include "disker/logging.h"
#include "ui_analyser.h"

namespace disker {
namespace detail {
template <class T>
QTreeWidgetItem* makeItem(const DiskAnalyser::File& file, QWidget* widget, T* parent) {
    const auto name = QString::fromStdString(file.name());
    const auto locale = widget->locale();
    const auto size = locale.formattedDataSize(file.size, 2, QLocale::DataSizeTraditionalFormat);

    const auto item = new QTreeWidgetItem(parent);
    item->setText(0, name);
    item->setText(1, size);

    return item;
}
}  // namespace detail
AnalyserWidget::AnalyserWidget(Path path, QWidget* parent)
    : QWidget(parent), ui_(std::make_unique<Ui::analyser>()), path_(path), timer_(this) {
    using namespace std::chrono_literals;
    timer_.start(100ms);

    ui_->setupUi(this);
    ui_->checkBox->setChecked(true);
    ui_->tree->setColumnCount(2);
    ui_->tree->setHeaderLabels({"name", "size"});
    ui_->tree->header()->resizeSection(0, 250);

    {
        series_ = new QPieSeries();
        series_->setHoleSize(0.35);
        series_->setPieSize(0.7);
        series_->setPieStartAngle(-45);
        series_->setPieEndAngle(-45 + 360);

        chart_view_ = new QChartView(this);
        chart_view_->setRenderHint(QPainter::Antialiasing);
        chart_view_->chart()->addSeries(series_);
        chart_view_->chart()->legend()->setAlignment(Qt::AlignRight);

        chart_view_->chart()->setBackgroundVisible(false);

        ui_->chartLayout->addWidget(chart_view_, 0, 0);
    }

    updateChart(FlatTree<File>::NoParent);

    refresh();

    connect(ui_->refreshButton, &QPushButton::clicked, this, &AnalyserWidget::refresh);
    connect(&timer_, &QTimer::timeout, this, &AnalyserWidget::update);
    connect(ui_->checkBox, &QCheckBox::clicked, this, &AnalyserWidget::refresh);
    connect(ui_->tree, &QTreeWidget::itemClicked, this, &AnalyserWidget::updateChartFromItem);
    connect(ui_->tree, &QTreeWidget::currentItemChanged, this, &AnalyserWidget::updateChartFromChange);
    connect(series_, &QPieSeries::clicked, this, &AnalyserWidget::clickedChart);
    connect(ui_->deleteButton, &QPushButton::clicked, this, &AnalyserWidget::deleteItem);
}

AnalyserWidget::~AnalyserWidget() = default;

void AnalyserWidget::refresh() {
    ui_->tree->clear();
    series_->clear();

    state_ = DiskAnalyser::State{};
    analyser_ = std::make_unique<DiskAnalyser>(path_);
    populated_ = false;
}

void AnalyserWidget::update() {
    if (!analyser_) {
        return;
    }

    state_ = analyser_->state([this](const DiskAnalyser::State& state) { return state; });

    if (!state_.completed || populated_) {
        return;
    }

    const auto display_files = ui_->checkBox->isChecked();
    auto& tree = state_.tree;

    const std::string display = display_files ? "true" : "false";
    logger::info("updating UI - display files: " + display);

    populated_ = true;

    widgets_.clear();

    for (auto idx = 0; idx < tree.size(); ++idx) {
        const auto& file = state_.tree.at(idx);

        QTreeWidgetItem* widget = nullptr;

        // we can look up our parent by index...
        const auto parent = tree.parent(idx);
        const auto has_children = tree.hasChildren(idx);
        const auto display = display_files || has_children;

        if (!display) {
            widget = nullptr;
        } else if (parent == FlatTree<File>::NoParent) {
            widget = detail::makeItem(file, this, ui_->tree);
        } else {
            auto parent_widget = widgets_.at(parent);
            logger::check(parent_widget != nullptr, "parent cannot be null!");
            widget = detail::makeItem(file, this, parent_widget);
        }

        widgets_.emplace_back(widget);
    }

    logger::check_eq(tree.size(), widgets_.size(), "failed to populate widgets correctly!");
    updateChart(0);
}

void AnalyserWidget::deleteItem() {
    const auto item = ui_->tree->currentItem();

    const auto iter = std::find(widgets_.begin(), widgets_.end(), item);
    if (iter == widgets_.end()) {
        QMessageBox msg;
        msg.setText("Must select an item to delete");
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }

    const auto idx = std::distance(widgets_.begin(), iter);
    const auto& file = state_.tree.at(idx);

    const auto& path = file.path;
    const auto path_name = QString::fromStdString(file.name());

    QMessageBox msg;
    msg.setText("Do you wish to delete " + path_name + "?");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setDefaultButton(QMessageBox::No);
    const auto choice = QMessageBox::StandardButton(msg.exec());

    if (choice == QMessageBox::No) {
        logger::info("decided to not delete it...");
        return;
    }

    logger::info("deleting file...");
    const auto num_removed = std::filesystem::remove_all(path);
    logger::info("removed " + std::to_string(num_removed) + " items");

    refresh();
}

void AnalyserWidget::updateChartFromItem(QTreeWidgetItem* item, int col) {
    // note(will.brennan) - this could be huge with lots of files... consider using a log-n container...
    const auto iter = std::find(widgets_.begin(), widgets_.end(), item);

    if (iter == widgets_.end()) {
        updateChart(FlatTree<File>::NoParent);
        return;
    }

    const auto idx = std::distance(widgets_.begin(), iter);

    if (!state_.tree.hasChildren(idx)) {
        return;
    }

    updateChart(idx);
}

void AnalyserWidget::updateChartFromChange(QTreeWidgetItem* curr, QTreeWidgetItem* prev) {
    updateChartFromItem(curr, 0);
}

void AnalyserWidget::clickedChart(QPieSlice* slice) {
    auto iter = std::find_if(slices_.begin(), slices_.end(),
                             [slice](const IndexedSlice& i) { return std::get<0>(i) == slice; });

    const auto fn_focus = [this](const Index idx) {
        if (idx < 0 || idx >= widgets_.size()) {
            logger::debug("tried to set focus on an item that doesnt exist");
            return;
        }

        const auto widget = widgets_.at(idx);

        if (widget == nullptr) {
            return;
        }

        ui_->tree->setCurrentItem(widget);
        ui_->tree->expandItem(widget);
        ui_->tree->scrollToItem(widget);
    };

    if (iter == slices_.end()) {
        updateChart(FlatTree<File>::NoParent);
        fn_focus(0);
    } else {
        const auto idx = std::get<1>(*iter);
        updateChart(idx);
        fn_focus(idx);
    }
}

void AnalyserWidget::updateChart(Index file_idx) {
    logger::info("displaying chart - file_idx: " + std::to_string(file_idx));
    struct IndexedFile {
        Index idx;
        File file;
    };

    series_->clear();
    slices_.clear();

    if (file_idx == FlatTree<File>::NoParent) {
        file_idx = 0;
    }

    if (file_idx >= state_.tree.size()) {
        return;
    }

    const auto fn_sum_size = [](File::Size acc, const IndexedFile& i) { return i.file.size + acc; };

    auto& tree = state_.tree;
    const auto& folder = tree.at(file_idx);
    const auto& node = tree.node(file_idx);

    auto fn_file_to_slice = [this, &folder](const IndexedFile& i) {
        const auto name = i.file.path.filename().string();

        const auto file_size = this->locale().formattedDataSize(i.file.size);

        const auto title = QString::fromStdString(name) + " " + file_size;

        const auto slice = series_->append(title, i.file.size);

        const auto is_small = i.file.size < 0.25 * folder.size;
        slice->setExploded(is_small);
        slice->setLabelVisible(!is_small);
        return slice;
    };

    auto fn_folder_to_slice = [this, fn_file_to_slice](const IndexedFile& i) {
        auto slice = fn_file_to_slice(i);
        slices_.emplace_back(slice, i.idx);
    };

    std::vector<IndexedFile> files;
    std::vector<IndexedFile> folders;

    for (auto idx = node.children_start; idx != node.children_end; ++idx) {
        if (tree.hasChildren(idx)) {
            folders.emplace_back(IndexedFile{idx, tree.at(idx)});
        } else {
            files.emplace_back(IndexedFile{idx, tree.at(idx)});
        }
    }

    const auto fraction = 0.05;
    const auto fn_is_small = [&folder, fraction](const IndexedFile& i) {
        return double(i.file.size) < fraction * double(folder.size);
    };

    logger::debug("combining small files together into a single entry");
    auto file_iter = std::partition(files.begin(), files.end(), fn_is_small);
    const auto small_files_size = std::accumulate(files.begin(), file_iter, 0u, fn_sum_size);
    const auto small_files = File{"Small Files", small_files_size};
    fn_file_to_slice(IndexedFile{-1, small_files});

    logger::debug("adding larger files to pie chart");
    std::for_each(file_iter, files.end(), fn_file_to_slice);

    logger::debug("combining small folders together into a single entry");
    auto folder_iter = std::partition(folders.begin(), folders.end(), fn_is_small);
    const auto small_folders_size = std::accumulate(folders.begin(), folder_iter, 0u, fn_sum_size);
    const auto small_folders = File{"Small Folders", small_folders_size};
    fn_file_to_slice(IndexedFile{-1, small_folders});

    logger::debug("adding larger folders to pie chart");
    std::for_each(folder_iter, folders.end(), fn_folder_to_slice);
}
}  // namespace disker