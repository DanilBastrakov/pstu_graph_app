#ifndef GRAPH_APP_ADJACENCY_DIALOG_H
#define GRAPH_APP_ADJACENCY_DIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QVector>

class adjacency_dialog : public QDialog {
    Q_OBJECT
public:
    explicit adjacency_dialog(const QVector<QVector<int>>& matrix, QWidget* parent = nullptr);
    [[nodiscard]] QVector<QVector<int>> get_result() const;

private:
    QTableWidget* table;
};

#endif
