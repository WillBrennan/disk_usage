#include "disker/widgets/selector.h"

#include <QFrame>
#include <QLine>
#include <QPushButton>

#include "disker/analyser.h"
#include "disker/widgets/selection.h"
#include "ui_selector.h"

namespace disker {
SelectorWidget::SelectorWidget(QWidget* parent) : QWidget(parent), ui_(std::make_unique<Ui::selector>()) {
    ui_->setupUi(this);

    std::vector<SelectionWidget*> selection_widgets;
    selection_widgets.emplace_back(new SelectionWidget("Analyse Directory", "", {}, this));

    for (auto& favourite : platform().favourites()) {
        const auto name = QString(favourite.filename().c_str());
        selection_widgets.emplace_back(new SelectionWidget(name, "", favourite, this));
    }

    for (auto idx = 0; idx < selection_widgets.size(); ++idx) {
        auto widget = selection_widgets[idx];
        ui_->gridLayout->addWidget(widget);

        connect(widget, &SelectionWidget::addAnalyser, this, &SelectorWidget::addAnalyser);
    }
}

SelectorWidget::~SelectorWidget() = default;

}  // namespace disker