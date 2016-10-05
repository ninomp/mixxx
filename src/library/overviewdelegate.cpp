#include "library/overviewdelegate.h"
#include "library/overviewcache.h"
#include "library/dao/trackdao.h"
#include "library/trackmodel.h"
//#include "track/track.h"

OverviewDelegate::OverviewDelegate(QObject* parent)
        : QStyledItemDelegate(parent),
          m_pTableView(nullptr) {
    TrackModel* pTrackModel = nullptr;
    if (QTableView* pTableView = qobject_cast<QTableView*>(parent)) {
        m_pTableView = pTableView;
        pTrackModel = dynamic_cast<TrackModel*>(pTableView->model());
    }

    if (pTrackModel) {
        m_iIdColumn = pTrackModel->fieldIndex(LIBRARYTABLE_ID);
    }
}

OverviewDelegate::~OverviewDelegate() {
}

void OverviewDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const {
    OverviewCache* pCache = OverviewCache::instance();
    if (pCache == nullptr) {
        return;
    }

    TrackId trackId(index.sibling(index.row(), m_iIdColumn).data().toInt());

    QPixmap pixmap = pCache->requestOverview(trackId, this, option.rect.width());
    if (!pixmap.isNull()) {
        int width = math_min(pixmap.width(), option.rect.width());
        int height = math_min(pixmap.height(), option.rect.height());
        QRect target(option.rect.x(), option.rect.y(),
                     width, height);
        QRect source(0, 0, target.width(), target.height());
        painter->drawPixmap(target, pixmap, source);
    }

/*
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
*/
}
