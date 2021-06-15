#include "disker/widgets/disker.h"

#include "disker/logging.h"
#include "disker/widgets/analyser.h"
#include "disker/widgets/selection.h"
#include "ui_disker.h"

namespace disker {
DiskerWidget::DiskerWidget(QWidget* parent) : QWidget(parent), ui_(std::make_unique<Ui::disker>()) {
    ui_->setupUi(this);

    selector_ = new SelectorWidget(this);
    ui_->tabWidget->setTabsClosable(true);
    ui_->tabWidget->addTab(selector_, "Overview");

    const auto tab_bar = ui_->tabWidget->tabBar();
    tab_bar->setTabButton(0, QTabBar::RightSide, nullptr);
    tab_bar->setTabButton(0, QTabBar::LeftSide, nullptr);

    connect(selector_, &SelectorWidget::addAnalyser, this, &DiskerWidget::addAnalyser);
    connect(ui_->tabWidget, &QTabWidget::tabCloseRequested, this, &DiskerWidget::closeTab);
}

DiskerWidget::~DiskerWidget() = default;

void DiskerWidget::addAnalyser(const std::filesystem::path& path) {
    const auto name = QString::fromStdString(path.filename().string());
    ui_->tabWidget->addTab(new AnalyserWidget(path, ui_->tabWidget), name);
    ui_->tabWidget->setCurrentIndex(ui_->tabWidget->count() - 1);
}

void DiskerWidget::closeTab(int tab_idx) const {
    if (tab_idx == 0) {
        return;
    }

    auto widget = ui_->tabWidget->widget(tab_idx);
    ui_->tabWidget->removeTab(tab_idx);
    delete widget;
}

}  // namespace disker