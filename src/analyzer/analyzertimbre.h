#ifndef ANALYZERTIMBRE_H
#define ANALYZERTIMBRE_H

#include "analyzer/analyzer.h"
#include "analyzer/vamp/vampanalyzer.h"
#include "preferences/usersettings.h"

class AnalyzerTimbre : public Analyzer {
  public:
    AnalyzerTimbre(UserSettingsPointer pConfig);
    virtual ~AnalyzerTimbre();

    bool initialize(TrackPointer tio, int sampleRate, int totalSamples) override;
    bool isDisabledOrLoadStoredSuccess(TrackPointer tio) const override;
    void process(const CSAMPLE *pIn, const int iLen) override;
    void finalize(TrackPointer tio) override;
    void cleanup(TrackPointer tio) override;

  private:
    static QHash<QString, QString> getExtraVersionInfo(
        QString pluginId, bool bPreferencesFastAnalysis);
    bool checkStoredTimbre(TrackPointer tio) const;

    UserSettingsPointer m_pConfig;
    VampAnalyzer* m_pVamp;
    QString m_pluginId;
    int m_iSampleRate;
    int m_iTotalSamples;
    bool m_bShouldAnalyze;
    bool m_bPreferencesFastAnalysis;
    bool m_bPreferencesReanalyzeEnabled;
};

#endif // ANALYZERTIMBRE_H
