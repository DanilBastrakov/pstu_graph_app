#ifndef GRAPH_APP_UI_EDGE_H
#define GRAPH_APP_UI_EDGE_H

#include <QGraphicsItem>
#include <QPen>
#include <QPointF>
#include <QString>
#include <QRectF>
#include <QColor>

class ui_node;

class ui_edge : public QGraphicsItem {
public:
    explicit ui_edge(ui_node* start_node, ui_node* end_node, int edge_weight = 0,
                     QGraphicsItem* parent = nullptr);
    void set_highlighted(bool highlighted);
    void set_path_edge(bool path_edge);
    [[nodiscard]] int get_weight() const { return edge_weight; }
    [[nodiscard]] ui_node* get_start_node() const { return start_node; }
    [[nodiscard]] ui_node* get_end_node() const { return end_node; }
    void update_line();

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QPointF get_node_border_point(const QPointF& node_center, const QPointF& direction, qreal radius) const;

    int edge_weight;
    ui_node* start_node;
    ui_node* end_node;
    qreal arrow_size = 10;
    QColor pen_color = Qt::black;
    qreal pen_width = 2;
};

#endif