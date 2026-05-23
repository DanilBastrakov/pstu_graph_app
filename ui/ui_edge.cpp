#include "ui_edge.h"
#include "ui_node.h"
#include <QPainter>
#include <QPolygonF>
#include <QtMath>

ui_edge::ui_edge(ui_node* start, ui_node* end, int weight, QGraphicsItem* parent)
    : QGraphicsItem(parent), edge_weight(weight), start_node(start), end_node(end) {
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

void ui_edge::set_highlighted(bool highlighted) {
    if (highlighted) {
        pen_color = Qt::red;
        pen_width = 4;
        setZValue(10);
    } else {
        pen_color = Qt::black;
        pen_width = 2;
        setZValue(0);
    }
    update();
}

void ui_edge::set_path_edge(bool path_edge) {
    if (path_edge) {
        pen_color = Qt::green;
        pen_width = 4;
    } else {
        pen_color = Qt::black;
        pen_width = 2;
    }
    update();
}

void ui_edge::update_line() {
    prepareGeometryChange();
    update();
}

QPointF ui_edge::get_node_border_point(const QPointF& node_center, const QPointF& direction, qreal radius) const {
    qreal len = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (len < 0.001) return node_center;

    qreal nx = direction.x() / len;
    qreal ny = direction.y() / len;

    return QPointF(node_center.x() + nx * radius, node_center.y() + ny * radius);
}

QRectF ui_edge::boundingRect() const {
    if (!start_node || !end_node) {
        return QRectF();
    }
    QPointF start_center = start_node->scenePos();
    QPointF end_center = end_node->scenePos();

    qreal min_x = qMin(start_center.x(), end_center.x()) - 40;
    qreal max_x = qMax(start_center.x(), end_center.x()) + 40;
    qreal min_y = qMin(start_center.y(), end_center.y()) - 40;
    qreal max_y = qMax(start_center.y(), end_center.y()) + 40;

    return QRectF(min_x, min_y, max_x - min_x, max_y - min_y);
}

void ui_edge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!start_node || !end_node) {
        return;
    }

    qreal node_radius = 25;

    QPointF start_center = start_node->scenePos();
    QPointF end_center = end_node->scenePos();

    QPointF direction(end_center.x() - start_center.x(), end_center.y() - start_center.y());

    QPointF start_pos = get_node_border_point(start_center, direction, node_radius);
    QPointF end_pos = get_node_border_point(end_center, -direction, node_radius);

    qreal dx = end_pos.x() - start_pos.x();
    qreal dy = end_pos.y() - start_pos.y();
    qreal length = qSqrt(dx * dx + dy * dy);

    if (length < arrow_size * 2) {
        return;
    }

    qreal angle = qAtan2(dy, dx);
    qreal arrow_angle = M_PI / 6;
    QPointF p1(end_pos.x() - arrow_size * qCos(angle - arrow_angle),
               end_pos.y() - arrow_size * qSin(angle - arrow_angle));
    QPointF p2(end_pos.x() - arrow_size * qCos(angle + arrow_angle),
               end_pos.y() - arrow_size * qSin(angle + arrow_angle));

    QPen pen(pen_color, pen_width);
    painter->setPen(pen);
    painter->drawLine(QLineF(start_pos, end_pos));

    QPolygonF arrow_head;
    arrow_head << end_pos << p1 << p2;
    painter->setBrush(pen_color);
    painter->drawPolygon(arrow_head);

    qreal text_offset = arrow_size + 10;
    QPointF text_pos(end_pos.x() - qCos(angle) * text_offset,
                      end_pos.y() - qSin(angle) * text_offset);
    QFont f = painter->font();
    f.setPointSize(12);
    painter->setFont(f);
    painter->setBackgroundMode(Qt::OpaqueMode);
    painter->setBackground(Qt::white);
    painter->drawText(text_pos, QString::number(edge_weight));
}