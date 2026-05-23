#include "floyd_widget.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QCloseEvent>
#include <climits>

floyd_widget::floyd_widget(const QVector<QVector<int>>& matrix,
                           graph_scene* scene,
                           int start_node,
                           int end_node,
                           QWidget* parent)
    : QDialog(parent), scene(scene), start_node(start_node), end_node(end_node) {
    setWindowTitle("Floyd-Warshall Algorithm Visualization");
    setMinimumSize(520, 420);

    int n = matrix.size();
    std::vector<std::vector<int>> mat(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            mat[i][j] = matrix[i][j];
    g.set_matrix(mat);
    steps = g.floyd_warshall_steps(start_node, end_node);

    auto* main_layout = new QVBoxLayout(this);

    auto* index_layout = new QHBoxLayout();
    k_label = new QLabel("k = -");
    i_label = new QLabel("i = -");
    j_label = new QLabel("j = -");
    QFont index_font = k_label->font();
    index_font.setBold(true);
    index_font.setPointSize(12);
    k_label->setFont(index_font);
    i_label->setFont(index_font);
    j_label->setFont(index_font);
    index_layout->addWidget(k_label);
    index_layout->addSpacing(20);
    index_layout->addWidget(i_label);
    index_layout->addSpacing(20);
    index_layout->addWidget(j_label);
    index_layout->addStretch();
    main_layout->addLayout(index_layout);

    status_label = new QLabel("Initial distance matrix");
    status_label->setWordWrap(true);
    main_layout->addWidget(status_label);

    step_label = new QLabel(QString("Step 0 / %1").arg(steps.size() - 1));
    main_layout->addWidget(step_label);

    build_matrix_table(n);
    main_layout->addWidget(matrix_table);

    auto* btn_layout = new QHBoxLayout();
    back_btn = new QPushButton("Back");
    step_btn = new QPushButton("Step");
    step_k_btn = new QPushButton("Step K");
    auto_btn = new QPushButton("Auto");
    close_btn = new QPushButton("Close");

    btn_layout->addWidget(back_btn);
    btn_layout->addWidget(step_btn);
    btn_layout->addWidget(step_k_btn);
    btn_layout->addWidget(auto_btn);
    btn_layout->addWidget(close_btn);
    main_layout->addLayout(btn_layout);

    connect(step_btn, &QPushButton::clicked, this, &floyd_widget::advance_step);
    connect(step_k_btn, &QPushButton::clicked, this, &floyd_widget::advance_to_next_k);
    connect(back_btn, &QPushButton::clicked, this, &floyd_widget::go_back);
    connect(auto_btn, &QPushButton::clicked, this, &floyd_widget::toggle_auto);
    connect(close_btn, &QPushButton::clicked, this, &floyd_widget::close_dialog);

    auto_timer = new QTimer(this);
    auto_timer->setInterval(300);
    connect(auto_timer, &QTimer::timeout, this, &floyd_widget::advance_step);

    back_btn->setEnabled(false);
    update_display();
}

floyd_widget::~floyd_widget() {
    if (auto_timer && auto_timer->isActive())
        auto_timer->stop();
}

void floyd_widget::closeEvent(QCloseEvent* event) {
    if (auto_timer && auto_timer->isActive())
        auto_timer->stop();

    scene->floyd_reset_highlights();

    if (step_index == (int)steps.size() - 1)
        reconstruct_and_show_path();

    QDialog::closeEvent(event);
}

void floyd_widget::build_matrix_table(int n) {
    matrix_table = new QTableWidget(n + 1, n + 1);
    matrix_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    matrix_table->horizontalHeader()->hide();
    matrix_table->verticalHeader()->hide();

    auto* corner = new QTableWidgetItem("");
    corner->setBackground(QColor(220, 220, 220));
    matrix_table->setItem(0, 0, corner);

    for (int j = 0; j < n; ++j) {
        auto* item = new QTableWidgetItem(QString::number(j));
        item->setBackground(QColor(240, 240, 240));
        item->setTextAlignment(Qt::AlignCenter);
        matrix_table->setItem(0, j + 1, item);
    }
    for (int i = 0; i < n; ++i) {
        auto* item = new QTableWidgetItem(QString::number(i));
        item->setBackground(QColor(240, 240, 240));
        item->setTextAlignment(Qt::AlignCenter);
        matrix_table->setItem(i + 1, 0, item);
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            auto* item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignCenter);
            matrix_table->setItem(i + 1, j + 1, item);
        }
    }

    matrix_table->resizeColumnsToContents();
    matrix_table->resizeRowsToContents();
}

void floyd_widget::update_display() {
    auto& step = steps[step_index];
    int n = step.dist.size();

    k_label->setText(QString("k = %1").arg(
        step.k >= 0 ? QString::number(step.k) : "-"));
    i_label->setText(QString("i = %1").arg(
        step.i >= 0 ? QString::number(step.i) : "-"));
    j_label->setText(QString("j = %1").arg(
        step.j >= 0 ? QString::number(step.j) : "-"));

    if (step.k == -1) {
        status_label->setText("Initial distance matrix");
    } else if (step.updated) {
        status_label->setText(
            QString("dist[%1][%2]: %3 \u2192 %4  (via node %5, improved!)")
                .arg(step.i).arg(step.j)
                .arg(step.old_value).arg(step.new_value)
                .arg(step.k));
    } else {
        status_label->setText(
            QString("dist[%1][%2]: %3  (via node %4, no improvement)")
                .arg(step.i).arg(step.j)
                .arg(step.new_value).arg(step.k));
    }

    step_label->setText(QString("Step %1 / %2").arg(step_index).arg(steps.size() - 1));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            auto* item = matrix_table->item(i + 1, j + 1);
            if (!item) continue;

            if (step.dist[i][j] >= INT_MAX / 2) {
                item->setText("\u221E");
            } else {
                item->setText(QString::number(step.dist[i][j]));
            }

            item->setBackground(Qt::white);

            bool is_current = (step.k >= 0 && i == step.i && j == step.j);
            bool is_k_rowcol = (step.k >= 0 && (i == step.k || j == step.k));

            if (is_current) {
                item->setBackground(step.updated
                    ? QColor(144, 238, 144)
                    : QColor(255, 255, 150));
            } else if (is_k_rowcol) {
                item->setBackground(QColor(230, 230, 250));
            }
        }
    }

    scene->floyd_highlight_step(step.k, step.i, step.j, step.dist, start_node);

    back_btn->setEnabled(step_index > 0);
    step_btn->setEnabled(step_index < (int)steps.size() - 1);
    step_k_btn->setEnabled(step_index < (int)steps.size() - 1);
}

void floyd_widget::advance_step() {
    if (step_index < (int)steps.size() - 1) {
        step_index++;
        update_display();
        if (step_index >= (int)steps.size() - 1 && auto_playing)
            toggle_auto();
    }
}

void floyd_widget::advance_to_next_k() {
    if (step_index >= (int)steps.size() - 1) return;
    int current_k = steps[step_index].k;
    do {
        step_index++;
    } while (step_index < (int)steps.size() - 1 && steps[step_index].k == current_k);
    update_display();
}

void floyd_widget::go_back() {
    if (step_index > 0) {
        step_index--;
        update_display();
    }
}

void floyd_widget::toggle_auto() {
    auto_playing = !auto_playing;
    if (auto_playing) {
        auto_btn->setText("Stop");
        auto_timer->start();
    } else {
        auto_btn->setText("Auto");
        auto_timer->stop();
    }
}

void floyd_widget::reconstruct_and_show_path() {
    auto& final_step = steps.back();
    auto& next = final_step.next;
    int n = final_step.dist.size();

    if (next[start_node][end_node] == -1) return;

    QVector<int> path;
    for (int v = start_node; v != end_node; v = next[v][end_node])
        path.push_back(v);
    path.push_back(end_node);

    std::vector<int> final_dist(n);
    for (int i = 0; i < n; ++i)
        final_dist[i] = final_step.dist[start_node][i];

    std::vector<int> final_parent(n, -1);
    for (int i = 0; i < n; ++i) {
        if (next[start_node][i] == -1) continue;
        int v = start_node;
        while (v != i) {
            int u = next[v][i];
            final_parent[u] = v;
            v = u;
        }
    }

    scene->set_floyd_result(final_dist, final_parent);
    scene->highlight_path(path);
}

void floyd_widget::close_dialog() {
    close();
}
