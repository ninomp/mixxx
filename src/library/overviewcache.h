#ifndef OVERVIEWCACHE_H
#define OVERVIEWCACHE_H


class OverviewCache : public QObject, public Singleton<CoverArtCache> {
    Q_OBJECT
public:
    void requestOverview(const Track* pTrack);

public slots:

signals:
    void overviewGenerated(QImage);

protected:
    OverviewCache();
    virtual ~OverviewCache();
    friend class Singleton<OverviewCache>;
};

#endif // OVERVIEWCACHE_H
