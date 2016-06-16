// selectorfeature.cpp
// Created 3/17/2012 by Keith Salisbury (keithsalisbury@gmail.com)

#include <QtDebug>

#include "library/dlgselector.h"
#include "library/selector/selectorfeature.h"
#include "library/librarytablemodel.h"
#include "library/trackcollection.h"
#include "controllers/keyboard/keyboardeventfilter.h"
#include "sources/soundsourceproxy.h"
#include "widget/wlibrary.h"
#include "widget/wlibrarysidebar.h"

const QString SelectorFeature::m_sSelectorViewName = QString("Selector");

SelectorFeature::SelectorFeature(QObject* parent,
                                 UserSettingsPointer pConfig,
                                 TrackCollection* pTrackCollection)
        : LibraryFeature(parent),
          m_pConfig(pConfig),
          m_pTrackCollection(pTrackCollection),
          m_pSelectorView(NULL) {
}

SelectorFeature::~SelectorFeature() {
}

QVariant SelectorFeature::title() {
    return tr("Selector");
}

QIcon SelectorFeature::getIcon() {
    return QIcon(":/images/library/ic_library_selector.png");
}

void SelectorFeature::bindWidget(WLibrary* libraryWidget,
                                 KeyboardEventFilter* keyboard) {
    m_pSelectorView = new DlgSelector(libraryWidget, m_pConfig,
        m_pTrackCollection, keyboard);
    libraryWidget->registerView(m_sSelectorViewName, m_pSelectorView);

    connect(m_pSelectorView, SIGNAL(loadTrack(TrackPointer)),
            this, SIGNAL(loadTrack(TrackPointer)));
    connect(m_pSelectorView, SIGNAL(loadTrackToPlayer(TrackPointer, QString)),
            this, SIGNAL(loadTrackToPlayer(TrackPointer, QString)));
}

TreeItemModel* SelectorFeature::getChildModel() {
    return &m_childModel;
}

void SelectorFeature::activate() {
    //qDebug() << "SelectorFeature::activate()";
    emit(switchToView(m_sSelectorViewName));
    emit(restoreSearch(QString()));
    emit(enableCoverArtDisplay(true));
}

void SelectorFeature::setSeedTrack(TrackPointer pTrack) {
    m_pSelectorView->setSeedTrack(pTrack);
}

void SelectorFeature::calculateAllSimilarities(const QString& filename) {
    m_pSelectorView->calculateAllSimilarities(filename);
}

bool SelectorFeature::dropAccept(QList<QUrl> urls, QWidget *pSource) {
    TrackDAO &trackDao = m_pTrackCollection->getTrackDAO();
    // We can only seed one track so don't accept if there are more dropped
    if (urls.size() != 1) {
        return false;
    }
    // If a track is dropped onto the selector name, but the track isn't in the
    // library, then add the track to the library before using it as a seed.
    QList<QFileInfo> files;
    foreach (QUrl url, urls) {
        QFileInfo file = url.toLocalFile();
        if (SoundSourceProxy::isFileNameSupported(file.fileName())) {
            files.append(file);
        }
    }

    QList<TrackId> trackIds;
    if (pSource) {
        trackIds = m_pTrackCollection->getTrackDAO().getTrackIds(files);
    } else {
        trackIds = trackDao.addMultipleTracks(files, true);
    }

    // qDebug() << "Track ID: " << trackIds.first();
    if (trackIds.first().isValid()) {
        m_pSelectorView->setSeedTrack(trackDao.getTrack(trackIds.first()));
    }
    return true;
}

bool SelectorFeature::dragMoveAccept(QUrl url) {
    QFileInfo file(url.toLocalFile());
    return SoundSourceProxy::isFileNameSupported(file.fileName());
}
