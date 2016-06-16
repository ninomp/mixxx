#ifndef DLGPREFTIMBRE_H
#define DLGPREFTIMBRE_H

#include <QWidget>
#include <QList>

#include "preferences/dialog/ui_dlgpreftimbredlg.h"
#include "preferences/dlgpreferencepage.h"
#include "preferences/usersettings.h"

class DlgPrefTimbre : public DlgPreferencePage, public Ui::DlgPrefTimbreDlg {
    Q_OBJECT
  public:
    DlgPrefTimbre(QWidget *parent, UserSettingsPointer pConfig);
    virtual ~DlgPrefTimbre();

  public slots:
    void slotApply();
    void slotUpdate();

  private slots:
    void pluginSelected(int i);
    void analyserEnabled(int i);
    void reanalyzeEnabled(int i);
    void setDefaults();

  signals:
    void apply(const QString &);

  private:
    void populate();
    void loadSettings();

    UserSettingsPointer m_pConfig;
    QList<QString> m_listName;
    QList<QString> m_listLibrary, m_listIdentifier;
    QString m_selectedAnalyser;
    bool m_bAnalyserEnabled;
    bool m_bReanalyzeEnabled;
};

#endif // DLGPREFTIMBRE_H
