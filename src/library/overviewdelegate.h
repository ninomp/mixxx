#ifndef OVERVIEWDELEGATE_H
#define OVERVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QTableView>

class OverviewDelegate : public QStyledItemDelegate {
    Q_OBJECT
  public:
    explicit OverviewDelegate(QObject* parent = nullptr);
    virtual ~OverviewDelegate();

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const;

  private:
    QTableView* m_pTableView;
    int m_iIdColumn;
};

#endif // OVERVIEWDELEGATE_H
