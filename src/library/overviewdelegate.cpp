#include "library/overviewdelegate.h"
#include "library/trackmodel.h"
#include "track/track.h"

OverviewDelegate::OverviewDelegate(QObject* parent)
        : QStyledItemDelegate(parent),
          m_pTableView(nullptr) {
    if (QTableView *tableView = qobject_cast<QTableView*>(parent)) {
        m_pTableView = tableView;
    }
}

OverviewDelegate::~OverviewDelegate() {
}

void OverviewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    TrackModel *pTrackModel = dynamic_cast<TrackModel*>(m_pTableView->model());
    if (pTrackModel) {
        TrackPointer pTrack = pTrackModel->getTrack(index);
        if (pTrack) {
            ConstWaveformPointer pWaveform = pTrack->getWaveform();
            if (pWaveform) {
                painter->drawImage(option.rect, pWaveform->renderToImage());
            }
        }
    }
}
