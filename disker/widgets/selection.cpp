#include "disker/widgets/selection.h"

#include <QFileDialog>
#include <QPushButton>

#include "disker/logging.h"
#include "ui_selection.h"

namespace disker {
SelectionWidget::SelectionWidget(const QString& name, const QString& icon, std::optional<Path> path, QWidget* parent)
    : QWidget(parent), ui_(std::make_unique<Ui::selection>()), path_(path) {
    ui_->setupUi(this);
    ui_->text->setText(name);

    const auto actionText = path_.has_value() ? "Analyse" : "Pick Folder";
    ui_->button->setText(actionText);

    connect(ui_->button, &QPushButton::pressed, this, &SelectionWidget::action);
}

void SelectionWidget::action() {
    if (path_.has_value()) {
        emit addAnalyser(path_.value());
    } else {
        const auto directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"));

        if (directory.isEmpty()) {
            return;
        }

        const auto fs_directory = Path(directory.toStdWString());
        emit addAnalyser(fs_directory);
    }
}

SelectionWidget::~SelectionWidget() = default;

}  // namespace disker