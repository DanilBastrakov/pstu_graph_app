#ifndef GRAPH_APP_FLOYD_WIDGET_H
#define GRAPH_APP_FLOYD_WIDGET_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVector>

#include "data_graph.h"

class graph_scene;

class floyd_widget : public QDialog {
    Q_OBJECT
public:
    explicit floyd_widget(const QVector<QVector<int>>& matrix,
                          graph_scene* scene,
                          int start_node,
                          int end_node,
                          QWidget* parent = nullptr);
    ~floyd_widget() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void advance_step();
    void advance_to_next_k();
    void go_back();
    void toggle_auto();
    void close_dialog();

private:
    void build_matrix_table(int n);
    void update_display();
    void reconstruct_and_show_path();

    data_graph g;
    std::vector<floyd_step_info> steps;
    int step_index = 0;
    graph_scene* scene;
    int start_node;
    int end_node;

    QTableWidget* matrix_table;
    QLabel* k_label;
    QLabel* i_label;
    QLabel* j_label;
    QLabel* status_label;
    QLabel* step_label;
    QPushButton* back_btn;
    QPushButton* step_btn;
    QPushButton* step_k_btn;
    QPushButton* auto_btn;
    QPushButton* close_btn;
    QTimer* auto_timer;

    bool auto_playing = false;
};

#endif
