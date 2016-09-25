#ifndef OVERVIEWDELEGATE_H
#define OVERVIEWDELEGATE_H

#include <QObject>
#include <QPainter>
#include <QStyledItemDelegate>

class OverviewDelegate : public QStyledItemDelegate {
    Q_OBJECT
  public:
    explicit OverviewDelegate(QObject* parent = nullptr);
    virtual ~OverviewDelegate();

    /*void paint(QPainer* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) override;*/
};

#endif // OVERVIEWDELEGATE_H
