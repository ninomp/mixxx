#ifndef OVERVIEWCACHE_H
#define OVERVIEWCACHE_H

#include <QSqlDatabase>

#include "preferences/usersettings.h"
#include "util/singleton.h"
#include "track/track.h"

class AnalysisDao;

class OverviewCache : public QObject, public Singleton<OverviewCache> {
    Q_OBJECT
  public:
    void initialize(UserSettingsPointer pConfig);

    QPixmap requestOverview(const TrackId trackId,
                            const QObject* pRequestor,
                            const QSize desiredSize);

    struct FutureResult {
        FutureResult()
            : /*resizedToWidth(0),*/
              requestor(nullptr) {
        }

        TrackId trackId;
        QImage image;
        QSize resizedToSize;
        const QObject* requestor;
    };

  public slots:
    void overviewPrepared();

  signals:
    void overviewReady(const QObject* pRequestor,
                       TrackId trackId,
                       QPixmap pixmap,
                       QSize resizedToSize);

  protected:
    OverviewCache();
    virtual ~OverviewCache();
    friend class Singleton<OverviewCache>;

    FutureResult prepareOverview(const TrackId trackId,
                                 const QObject* pRequestor,
                                 const QSize desiredSize);

  private:
    QSqlDatabase m_database;
    std::unique_ptr<AnalysisDao> m_pAnalysisDao;
};

#endif // OVERVIEWCACHE_H
