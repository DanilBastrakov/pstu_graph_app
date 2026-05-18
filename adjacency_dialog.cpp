#include "adjacency_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QHeaderView>

adjacency_dialog::adjacency_dialog(const QVector<QVector<int>>& matrix, QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Edit Adjacency Matrix");
    resize(500, 400);

    int n = matrix.size();

    auto* layout = new QVBoxLayout(this);

    table = new QTableWidget(n, n, this);
    table->setHorizontalHeaderLabels(QStringList());
    table->setVerticalHeaderLabels(QStringList());

    QStringList headers;
    for (int i = 0; i < n; ++i) {
        headers << QString::number(i);
    }
    table->setHorizontalHeaderLabels(headers);
    table->setVerticalHeaderLabels(headers);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            auto* spin = new QSpinBox(this);
            spin->setRange(0, 9999);
            spin->setValue(matrix[i][j]);
            spin->setMinimumWidth(60);
            spin->setAlignment(Qt::AlignCenter);
            if (i == j) {
                spin->setEnabled(false);
            }
            table->setCellWidget(i, j, spin);
        }
    }

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(table);

    auto* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(button_box);
}

QVector<QVector<int>> adjacency_dialog::get_result() const {
    int n = table->rowCount();
    QVector<QVector<int>> result(n, QVector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            auto* spin = qobject_cast<QSpinBox*>(table->cellWidget(i, j));
            if (spin) {
                result[i][j] = spin->value();
            }
        }
    }
    return result;
}
