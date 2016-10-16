#ifndef OVERVIEWDELEGATE_H
#define OVERVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QTableView>

#include "track/trackid.h"

class OverviewDelegate : public QStyledItemDelegate {
    Q_OBJECT
  public:
    explicit OverviewDelegate(QObject* parent = nullptr);
    virtual ~OverviewDelegate();

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const;

  signals:
    void overviewReadyForCell(int row, int column);

  private slots:
    void slotOverviewReady(const QObject* pRequestor,
                           TrackId trackId,
                           QPixmap pixmap,
                           QSize resizedToSize);

  private:
    QTableView* m_pTableView;
    int m_iIdColumn;
    int m_iOverviewColumn;

    mutable QHash<TrackId, int> m_trackIdToRow;
};

#endif // OVERVIEWDELEGATE_H
