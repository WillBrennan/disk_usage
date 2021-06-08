#include "disker/widgets/analyser.h"

#include <QList>
#include <QPushButton>
#include <QTreeWidget>
#include <QtCharts>
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

template <class T>
void makeFolder(const DiskAnalyser::Folder& folder, QWidget* widget, T* parent, const bool display_files) {
    auto item = makeItem(folder, widget, parent);

    for (auto& subfolder : folder.folders) {
        makeFolder(subfolder, widget, item, display_files);
    }

    if (display_files) {
        for (auto& file : folder.files) {
            makeItem(file, widget, item);
        }
    }
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

    refresh();

    connect(ui_->refreshButton, &QPushButton::clicked, this, &AnalyserWidget::refresh);
    connect(&timer_, &QTimer::timeout, this, &AnalyserWidget::update);
    connect(ui_->checkBox, &QCheckBox::clicked, this, &AnalyserWidget::refresh);
}

AnalyserWidget::~AnalyserWidget() = default;

void AnalyserWidget::refresh() {
    ui_->tree->clear();
    ui_->chart->clear();

    state_ = DiskAnalyser::State{};
    analyser_ = std::make_unique<DiskAnalyser>(path_);
    populated_ = false;
}

void AnalyserWidget::update() {
    if (!analyser_) {
        return;
    }

    state_ = analyser_->state([this](const DiskAnalyser::State& state) { return state; });

    if (!state_.folder || populated_) {
        return;
    }

    const auto display_files = ui_->checkBox->isChecked();

    populated_ = true;

    const auto& folder = state_.folder.value();

    detail::makeFolder(folder, this, ui_->tree, display_files);
}
}  // namespace disker