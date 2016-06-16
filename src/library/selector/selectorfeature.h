// selectorfeature.h
// Created 8/23/2012 by Keith Salisbury (keith@globalkeith.com)
#ifndef SELECTORFEATURE_H
#define SELECTORFEATURE_H

#include <QStringListModel>

#include "library/libraryfeature.h"
#include "library/treeitemmodel.h"
#include "library/dlgselector.h"
#include "preferences/usersettings.h"

class LibraryTableModel;
class TrackCollection;

class SelectorFeature : public LibraryFeature {
    Q_OBJECT
  public:
    SelectorFeature(QObject* parent,
                    UserSettingsPointer pConfig,
                    TrackCollection* pTrackCollection);
    virtual ~SelectorFeature();

    QVariant title();
    QIcon getIcon();

    bool dropAccept(QList<QUrl> urls,QWidget *pSource);
    bool dragMoveAccept(QUrl url);
    void bindWidget(WLibrary* libraryWidget,
                    KeyboardEventFilter* keyboard);
    TreeItemModel* getChildModel();

  public slots:
    void activate();
    void setSeedTrack(TrackPointer pTrack);
    void calculateAllSimilarities(const QString& filename);

  private:
    UserSettingsPointer m_pConfig;
    TrackCollection* m_pTrackCollection;
    const static QString m_sSelectorViewName;
    TreeItemModel m_childModel;
    DlgSelector* m_pSelectorView;
};
#endif /* SELECTORFEATURE_H */
