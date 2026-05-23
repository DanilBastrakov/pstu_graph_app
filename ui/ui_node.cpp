#include "../ui_node.h"
#include <QGraphicsScene>
#include <QTextDocument>
#include <QTextOption>

ui_node::ui_node(int id, int weight, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent), node_id(id), node_weight(weight) {
    setRect(-25, -25, 50, 50);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setBrush(Qt::white);
    setPen(QPen(Qt::darkCyan, 2));

    weight_text = new QGraphicsTextItem(this);
    weight_text->setTextWidth(rect().width());
    weight_text->setDefaultTextColor(Qt::black);
    QTextOption option = weight_text->document()->defaultTextOption();
    option.setAlignment(Qt::AlignCenter);
    weight_text->document()->setDefaultTextOption(option);
    weight_text->setPlainText(QString(weight != 0 ? "%1\n%2" : "%1").arg(id).arg(weight));

    QRectF text_rect = weight_text->boundingRect();
    weight_text->setPos(-text_rect.width() / 2, -text_rect.height() / 2);
}

void ui_node::set_current(bool current) {
    if (current)
        setPen(QPen(QColor(255, 165, 0), 4));
    else
        setPen(QPen(Qt::darkCyan, 2));
}

void ui_node::set_highlighted(bool highlighted) {
    if (highlighted) {
        setPen(QPen(Qt::darkYellow, 4));
    } else {
        setPen(QPen(Qt::darkCyan, 2));
    }
}

void ui_node::set_visited(bool visited) {
    setBrush(visited ? QColor(173, 216, 230) : Qt::white);
}

void ui_node::set_path(bool path) {
    setPen(path ? QPen(Qt::green, 4) : QPen(Qt::darkCyan, 2));
}

void ui_node::set_info_text(const QString& text) {
    weight_text->setPlainText(text);
    QRectF r = weight_text->boundingRect();
    weight_text->setPos(-r.width() / 2, -r.height() / 2);
}