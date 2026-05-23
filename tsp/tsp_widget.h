#ifndef GRAPH_APP_TSP_WIDGET_H
#define GRAPH_APP_TSP_WIDGET_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVector>

#include "tsp_solver.h"

class graph_scene;

class tsp_widget : public QDialog {
    Q_OBJECT
public:
    explicit tsp_widget(const QVector<QVector<int>>& matrix,
                        graph_scene* scene,
                        int start_node,
                        QWidget* parent = nullptr);
    ~tsp_widget() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void advance_step();
    void toggle_auto();
    void close_dialog();

private:
    void update_display();
    void show_result();

    std::vector<tsp_step_info> steps;
    int step_index = 0;
    graph_scene* scene;
    int start_node;

    QLabel* partial_tour_label;
    QLabel* cost_bound_label;
    QLabel* best_label;
    QLabel* status_label;
    QLabel* progress_label;
    QPushButton* step_btn;
    QPushButton* auto_btn;
    QPushButton* close_btn;
    QTimer* auto_timer;
    bool auto_playing = false;
};

#endif
