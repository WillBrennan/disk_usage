#include "disker/widgets/disker.h"

#include "disker/logging.h"
#include "disker/widgets/analyser.h"
#include "disker/widgets/selection.h"
#include "ui_disker.h"

namespace disker {
DiskerWidget::DiskerWidget(QWidget* parent) : QWidget(parent), ui_(std::make_unique<Ui::disker>()) {
    ui_->setupUi(this);

    selector_ = new SelectorWidget(this);
    ui_->tabWidget->addTab(selector_, "Overview");

    connect(selector_, &SelectorWidget::addAnalyser, this, &DiskerWidget::addAnalyser);
}

DiskerWidget::~DiskerWidget() = default;

void DiskerWidget::addAnalyser(const std::filesystem::path& path) {
    const auto name = QString::fromStdString(path.filename().string());
    ui_->tabWidget->addTab(new AnalyserWidget(path, ui_->tabWidget), name);
    ui_->tabWidget->setCurrentIndex(ui_->tabWidget->count() - 1);
}

}  // namespace disker