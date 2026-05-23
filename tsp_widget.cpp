#include "tsp_widget.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <sstream>

using namespace std;

tsp_widget::tsp_widget(const QVector<QVector<int>>& matrix,
                       graph_scene* scene,
                       int start_node,
                       QWidget* parent)
    : QDialog(parent), scene(scene), start_node(start_node) {
    setWindowTitle("TSP — Branch and Bound");
    setMinimumSize(460, 260);

    int n = matrix.size();
    vector<vector<int>> mat(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            mat[i][j] = matrix[i][j];

    tsp_solver solver(mat);
    steps = solver.solve(start_node);

    auto* main_layout = new QVBoxLayout(this);

    partial_tour_label = new QLabel();
    partial_tour_label->setWordWrap(true);
    QFont f = partial_tour_label->font();
    f.setPointSize(11);
    partial_tour_label->setFont(f);
    main_layout->addWidget(partial_tour_label);

    cost_bound_label = new QLabel();
    main_layout->addWidget(cost_bound_label);

    best_label = new QLabel();
    best_label->setWordWrap(true);
    main_layout->addWidget(best_label);

    status_label = new QLabel();
    status_label->setWordWrap(true);
    main_layout->addWidget(status_label);

    progress_label = new QLabel();
    main_layout->addWidget(progress_label);

    auto* btn_layout = new QHBoxLayout();
    step_btn = new QPushButton("Step");
    auto_btn = new QPushButton("Auto");
    close_btn = new QPushButton("Close");
    step_btn->setMinimumHeight(32);
    auto_btn->setMinimumHeight(32);
    close_btn->setMinimumHeight(32);
    btn_layout->addWidget(step_btn);
    btn_layout->addWidget(auto_btn);
    btn_layout->addWidget(close_btn);
    main_layout->addLayout(btn_layout);

    connect(step_btn, &QPushButton::clicked, this, &tsp_widget::advance_step);
    connect(auto_btn, &QPushButton::clicked, this, &tsp_widget::toggle_auto);
    connect(close_btn, &QPushButton::clicked, this, &tsp_widget::close_dialog);

    auto_timer = new QTimer(this);
    auto_timer->setInterval(400);
    connect(auto_timer, &QTimer::timeout, this, &tsp_widget::advance_step);

    step_btn->setEnabled(steps.size() > 1);
    update_display();
}

tsp_widget::~tsp_widget() {
    if (auto_timer && auto_timer->isActive())
        auto_timer->stop();
}

void tsp_widget::closeEvent(QCloseEvent* event) {
    if (auto_timer && auto_timer->isActive())
        auto_timer->stop();

    scene->tsp_reset_highlights();

    if (step_index == (int)steps.size() - 1)
        show_result();

    QDialog::closeEvent(event);
}

static QString tour_to_string(const vector<int>& tour) {
    QString s;
    for (size_t i = 0; i < tour.size(); ++i) {
        if (i) s += " \u2192 ";
        s += QString::number(tour[i]);
    }
    return s;
}

static QString format_desc(const string& desc) {
    QString s = QString::fromStdString(desc);
    return s;
}

void tsp_widget::update_display() {
    if (step_index >= (int)steps.size()) return;
    auto& step = steps[step_index];

    if (step.type == tsp_step_info::INIT) {
        partial_tour_label->setText(QString("Start:  %1").arg(start_node));
        cost_bound_label->setText("");
        best_label->setText("");
    } else if (step.type == tsp_step_info::EXPLORE ||
               step.type == tsp_step_info::BRANCH ||
               step.type == tsp_step_info::PRUNE) {
        QString tour_str = tour_to_string(step.partial_tour);
        if (step.partial_tour.size() > 1 &&
            step.partial_tour.front() == step.partial_tour.back()) {
            int idx = tour_str.lastIndexOf(" \u2192 ");
            tour_str = tour_str.left(idx);
        }
        if ((int)step.partial_tour.size() < steps[0].partial_tour.size() + 1 ||
            step.partial_tour.empty()) {
            if (!step.partial_tour.empty())
                tour_str += " \u2192 ?";
        }
        partial_tour_label->setText("Tour:  " + tour_str);

        QString cb;
        cb += QString("Cost: %1").arg(step.current_cost);
        if (step.lower_bound > 0 && step.lower_bound < INT_MAX)
            cb += QString("   |   LB: %1").arg(step.lower_bound);
        cost_bound_label->setText(cb);

        if (!step.best_tour.empty() && step.best_cost < INT_MAX) {
            QString best_str = tour_to_string(step.best_tour);
            int idx = best_str.lastIndexOf(" \u2192 ");
            if (idx > 0 && step.best_tour.size() > 1 &&
                step.best_tour.front() == step.best_tour.back()) {
                best_str = best_str.left(idx);
            }
            best_label->setText(QString("Best:  %1   (cost: %2)")
                .arg(best_str).arg(step.best_cost));
        } else {
            best_label->setText("Best:  none");
        }
    } else if (step.type == tsp_step_info::NEW_BEST) {
        QString best_str = tour_to_string(step.best_tour);
        partial_tour_label->setText("Best Tour:  " + best_str);
        cost_bound_label->setText(QString("Cost: %1").arg(step.best_cost));
        best_label->setText(QString("Best:  %1   (cost: %2)")
            .arg(best_str).arg(step.best_cost));
    } else if (step.type == tsp_step_info::DONE) {
        if (!step.best_tour.empty() && step.best_cost < INT_MAX) {
            QString best_str = tour_to_string(step.best_tour);
            partial_tour_label->setText("Best Tour:  " + best_str);
            cost_bound_label->setText(QString("Cost: %1").arg(step.best_cost));
        } else {
            partial_tour_label->setText("No valid tour found.");
            cost_bound_label->setText("");
        }
        best_label->setText("");
    }

    status_label->setText(format_desc(step.description));
    progress_label->setText(QString("Step %1 / %2")
        .arg(step.step_number).arg(steps.back().step_number));

    int last_node = step.partial_tour.empty() ? -1 : step.partial_tour.back();
    scene->tsp_highlight_state(step.partial_tour, step.best_tour, last_node);

    step_btn->setEnabled(step_index < (int)steps.size() - 1);
}

void tsp_widget::advance_step() {
    if (step_index < (int)steps.size() - 1) {
        step_index++;
        update_display();
        if (step_index >= (int)steps.size() - 1 && auto_playing)
            toggle_auto();
    }
}

void tsp_widget::toggle_auto() {
    auto_playing = !auto_playing;
    if (auto_playing) {
        auto_btn->setText("Stop");
        auto_timer->start();
    } else {
        auto_btn->setText("Auto");
        auto_timer->stop();
    }
}

void tsp_widget::show_result() {
    auto& last = steps.back();
    if (last.best_cost >= INT_MAX || last.best_tour.empty()) return;

    QVector<int> path;
    for (int v : last.best_tour)
        path.push_back(v);

    scene->set_floyd_result({}, {});
    scene->highlight_path(path);
}

void tsp_widget::close_dialog() {
    close();
}
