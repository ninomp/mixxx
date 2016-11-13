#ifndef ANALYZER_ANALYZERSILENCE_H
#define ANALYZER_ANALYZERSILENCE_H

#include "analyzer/analyzer.h"
#include "analyzer/vamp/vampanalyzer.h"
#include "preferences/usersettings.h"

class AnalyzerSilence : public Analyzer {
  public:
    AnalyzerSilence(UserSettingsPointer pConfig);
    virtual ~AnalyzerSilence();

    bool initialize(TrackPointer tio, int sampleRate, int totalSamples) override;
    bool isDisabledOrLoadStoredSuccess(TrackPointer tio) const override;
    void process(const CSAMPLE *pIn, const int iLen) override;
    void finalize(TrackPointer tio) override;
    void cleanup(TrackPointer tio) override;

  private:
    UserSettingsPointer m_pConfig;
    VampAnalyzer* m_pVamp;
    //QString m_pluginId;
    QList<int> m_NonSilentBegin;
    QList<int> m_NonSilentEnd;
    float m_fThreshold;
    bool m_bPrevSilence;
    int m_iSampleRate;
    int m_iSamplesProcessed;
};

#endif // ANALYZER_ANALYZERSILENCE_H
