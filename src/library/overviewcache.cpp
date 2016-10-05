#include <QFutureWatcher>
#include <QPixmapCache>
#include <QtConcurrentRun>

#include "library/overviewcache.h"

namespace {
    QString pixmapCacheKey(TrackId trackId, int width) {
        return QString("Overview_%1_%2").arg(trackId.toInt()).arg(width);
    }
}  // anonymous namespace

//OverviewCache::OverviewCache(QObject *parent) : QObject(parent)
OverviewCache::OverviewCache() {
}

OverviewCache::~OverviewCache() {
    //qDebug()
}

QPixmap OverviewCache::requestOverview(const TrackId trackId,
                                       const QObject *pRequestor,
                                       const int desiredWidth) {
    if (!trackId.isValid()) {
        return QPixmap();
    }

    QString cacheKey = pixmapCacheKey(trackId, desiredWidth);

    QPixmap pixmap;
    if (QPixmapCache::find(cacheKey, &pixmap)) {
        return pixmap;
    }

    QFutureWatcher<FutureResult>* watcher = new QFutureWatcher<FutureResult>(this);
    QFuture<FutureResult> future = QtConcurrent::run(
                this, &OverviewCache::prepareOverview, trackId, pRequestor, desiredWidth);
    connect(watcher, SIGNAL(finished()), this, SLOT(overviewPrepared()));
    watcher->setFuture(future);

    return QPixmap();
}

OverviewCache::FutureResult OverviewCache::prepareOverview(
        const TrackId trackId,
        const QObject *pRequestor,
        const int desiredWidth) {

}
