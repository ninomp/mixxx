#ifndef OVERVIEWCACHE_H
#define OVERVIEWCACHE_H

#include "util/singleton.h"
#include "track/track.h"

class OverviewCache : public QObject, public Singleton<OverviewCache> {
    Q_OBJECT
  public:
    QPixmap requestOverview(const TrackId trackId,
                            const QObject* pRequestor,
                            const int desiredWidth);

    struct FutureResult {
        FutureResult()
            : requestor(nullptr) {
        }

        TrackId trackId;
        QImage image;
        const QObject* requestor;
    };

  public slots:
    void overviewPrepared();

  signals:
    void overviewReady(const QObject* pRequestor,
                       TrackId trackId,
                       QPixmap pixmap);

  protected:
    OverviewCache();
    virtual ~OverviewCache();
    friend class Singleton<OverviewCache>;

    FutureResult prepareOverview(const TrackId trackId,
                                 const QObject* pRequestor,
                                 const int desiredWidth);
};

#endif // OVERVIEWCACHE_H
