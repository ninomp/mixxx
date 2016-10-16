#include <QFutureWatcher>
#include <QPixmapCache>
#include <QtConcurrentRun>
#include <QtSql>
#include <QSqlDatabase>

#include "library/dao/analysisdao.h"
#include "library/overviewcache.h"
#include "waveform/waveformfactory.h"

namespace {
    QString pixmapCacheKey(TrackId trackId, QSize size) {
        //return QString("Overview_%1").arg(trackId.toInt());
        //return QString("Overview_%1_%2").arg(trackId.toInt()).arg(width);
        return QString("Overview_%1_%2_%3").arg(trackId.toInt()).arg(size.width()).arg(size.height());
    }

    // The transformation mode when scaling images
    const Qt::TransformationMode kTransformationMode = Qt::SmoothTransformation;

    // Resizes the image (preserving aspect ratio) to width.
    inline QImage resizeImageWidth(const QImage& image, int width) {
        return image.scaledToWidth(width, kTransformationMode);
    }

    inline QImage resizeImageSize(const QImage& image, QSize size) {
        return image.scaled(size, Qt::IgnoreAspectRatio, kTransformationMode);
    }
}  // anonymous namespace

//OverviewCache::OverviewCache(QObject *parent) : QObject(parent)
OverviewCache::OverviewCache() {
}

OverviewCache::~OverviewCache() {
    //qDebug()
}

void OverviewCache::initialize(UserSettingsPointer pConfig) {
    m_database = QSqlDatabase::addDatabase("QSQLITE", "OVERVIEW_CACHE");
    if (!m_database.isOpen()) {
        m_database.setHostName("localhost");
        m_database.setDatabaseName(QDir(pConfig->getSettingsPath()).filePath("mixxxdb.sqlite"));
        m_database.setUserName("mixxx");
        m_database.setPassword("mixxx");

        //Open the database connection in this thread.
        if (!m_database.open()) {
            qDebug() << "Failed to open database from overview cacher thread."
                     << m_database.lastError();
        }
    }

    m_pAnalysisDao = std::make_unique<AnalysisDao>(m_database, pConfig);
}

QPixmap OverviewCache::requestOverview(const TrackId trackId,
                                       const QObject *pRequestor,
                                       const QSize desiredSize) {
    if (!trackId.isValid()) {
        return QPixmap();
    }

    QString cacheKey = pixmapCacheKey(trackId, desiredSize);

    QPixmap pixmap;
    if (QPixmapCache::find(cacheKey, &pixmap)) {
        return pixmap;
    }

    QFutureWatcher<FutureResult>* watcher = new QFutureWatcher<FutureResult>(this);
    QFuture<FutureResult> future = QtConcurrent::run(
                this, &OverviewCache::prepareOverview, trackId, pRequestor, desiredSize);
    connect(watcher, SIGNAL(finished()), this, SLOT(overviewPrepared()));
    watcher->setFuture(future);

    return QPixmap();

    /*
    ConstWaveformPointer pLoadedTrackWaveformSummary;
    QList<AnalysisDao::AnalysisInfo> analyses =
            m_pAnalysisDao->getAnalysesForTrackByType(trackId, AnalysisDao::AnalysisType::TYPE_WAVESUMMARY);

    if (analyses.size() != 1) {
        return QPixmap();
    }
    */

    /*
    QListIterator<AnalysisDao::AnalysisInfo> it(analyses);
    while (it.hasNext()) {
        const AnalysisDao::AnalysisInfo& analysis = it.next();
        pLoadedTrackWaveformSummary = ConstWaveformPointer(
                WaveformFactory::loadWaveformFromAnalysis(analysis));
    }
    */
    /*
    pLoadedTrackWaveformSummary = ConstWaveformPointer(
                WaveformFactory::loadWaveformFromAnalysis(analyses[0]));

    if (!pLoadedTrackWaveformSummary.isNull() && !pLoadedTrackWaveformSummary->isValid()) {
        return QPixmap();
    }

    QImage image = pLoadedTrackWaveformSummary->renderToImage();

    if (!image.isNull() && desiredWidth > 0) {
        image = resizeImageWidth(image, desiredWidth);
    }

    // Create pixmap, GUI thread only
    /*QPixmap*/ /*pixmap = QPixmap::fromImage(image);
    if (!pixmap.isNull() && desiredWidth != 0) {
        // we have to be sure that res.cover.hash is unique
        // because insert replaces the images with the same key
        QString cacheKey = pixmapCacheKey(
                trackId, desiredWidth);
        QPixmapCache::insert(cacheKey, pixmap);
    }

    return pixmap;*/
}

OverviewCache::FutureResult OverviewCache::prepareOverview(
        const TrackId trackId,
        const QObject *pRequestor,
        const QSize desiredSize) {
    ConstWaveformPointer pLoadedTrackWaveformSummary;
    QList<AnalysisDao::AnalysisInfo> analyses =
            m_pAnalysisDao->getAnalysesForTrackByType(trackId, AnalysisDao::AnalysisType::TYPE_WAVESUMMARY);

    QListIterator<AnalysisDao::AnalysisInfo> it(analyses);
    while (it.hasNext()) {
        const AnalysisDao::AnalysisInfo& analysis = it.next();
        pLoadedTrackWaveformSummary = ConstWaveformPointer(
                WaveformFactory::loadWaveformFromAnalysis(analysis));
    }

    QImage image;

    if (!pLoadedTrackWaveformSummary.isNull() && pLoadedTrackWaveformSummary->isValid()) {
        image = pLoadedTrackWaveformSummary->renderToImage();

        if (!image.isNull() && !desiredSize.isEmpty()) {
            //image = resizeImageWidth(image, desiredWidth);
            image = resizeImageSize(image, desiredSize);
        }
    }

    FutureResult result;
    result.trackId = trackId;
    result.requestor = pRequestor;
    result.image = image;
    result.resizedToSize = desiredSize;
    return result;
}

void OverviewCache::overviewPrepared() {
    QFutureWatcher<FutureResult>* watcher;
    watcher = reinterpret_cast<QFutureWatcher<FutureResult>*>(sender());
    FutureResult res = watcher->result();

    // Create pixmap, GUI thread only
    QPixmap pixmap = QPixmap::fromImage(res.image);
    if (!pixmap.isNull() && !res.resizedToSize.isEmpty()) {
        // we have to be sure that res.cover.hash is unique
        // because insert replaces the images with the same key
        QString cacheKey = pixmapCacheKey(
                res.trackId, res.resizedToSize);
        QPixmapCache::insert(cacheKey, pixmap);
    }

    emit(overviewReady(res.requestor, res.trackId, pixmap, res.resizedToSize));
}
