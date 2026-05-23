#ifndef GRAPH_APP_UI_NODE_H
#define GRAPH_APP_UI_NODE_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QPointF>

class ui_edge;

class ui_node : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT
public:
    explicit ui_node(int node_id, int node_weight = 0, QGraphicsItem* parent = nullptr);
    void set_highlighted(bool highlighted);
    void set_visited(bool visited);
    void set_path(bool path);
    void set_current(bool current);
    void set_info_text(const QString& text);
    [[nodiscard]] int get_id() const { return node_id; }
    [[nodiscard]] int get_weight() const { return node_weight; }

private:
    int node_id;
    int node_weight;
    QGraphicsTextItem* weight_text;
};

#endif