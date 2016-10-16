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
        m_iOverviewColumn = pTrackModel->fieldIndex(LIBRARYTABLE_WAVESUMMARYHEX);
    }

    OverviewCache* pCache = OverviewCache::instance();
    if (pCache != nullptr) {
        connect(pCache, SIGNAL(overviewReady(const QObject*,TrackId,QPixmap,QSize)),
                this, SLOT(slotOverviewReady(const QObject*,TrackId,QPixmap,QSize)));
    }
}

OverviewDelegate::~OverviewDelegate() {
}

void OverviewDelegate::slotOverviewReady(const QObject *pRequestor, TrackId trackId, QPixmap pixmap, QSize resizedToSize) {
    if (pRequestor == this && !pixmap.isNull()) {
        int row = m_trackIdToRow.take(trackId);
        emit overviewReadyForCell(row, m_iOverviewColumn);
    }
}

void OverviewDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const {
    OverviewCache* pCache = OverviewCache::instance();
    if (pCache == nullptr) {
        return;
    }

    TrackId trackId(index.sibling(index.row(), m_iIdColumn).data().toInt());

    QPixmap pixmap = pCache->requestOverview(trackId, this, option.rect.size());
    if (!pixmap.isNull()) {
        //int width = math_min(pixmap.width(), option.rect.width());
        //int height = math_min(pixmap.height(), option.rect.height());
        /*QRect target(option.rect.x(), option.rect.y(),
                     width, height);*/
        //QRect source(0, 0, target.width(), target.height());
        //painter->drawPixmap(target, pixmap, source);
        //QRect source(0, 0, pixmap.width(), pixmap.height());
        painter->drawPixmap(option.rect, pixmap);
    } else {
        m_trackIdToRow[trackId] = index.row();
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
