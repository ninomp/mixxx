#include "analyzersilence.h"

AnalyzerSilence::AnalyzerSilence(UserSettingsPointer pConfig)
    : m_pConfig(pConfig),
      m_pVamp(nullptr) {
}

AnalyzerSilence::~AnalyzerSilence() {
    delete m_pVamp;
}

bool AnalyzerSilence::initialize(TrackPointer tio, int sampleRate, int totalSamples) {
    QString library = "libmixxxminimal";
    QString pluginID = "aubiosilence:0";

    m_pVamp = new VampAnalyzer();
    bool success = m_pVamp->Init(library, pluginID, sampleRate, totalSamples, false);

    if (success) {
        qDebug() << "Silence detection started with plugin" << pluginID;
    } else {
        qDebug() << "Silence detection will not start";
    }

    return success;
}

bool AnalyzerSilence::isDisabledOrLoadStoredSuccess(TrackPointer tio) const {
    return false;
}

void AnalyzerSilence::process(const CSAMPLE *pIn, const int iLen) {
    if (m_pVamp == nullptr) {
        return;
    }

    bool success = m_pVamp->Process(pIn, iLen);
    if (!success) {
        delete m_pVamp;
        m_pVamp = nullptr;
    }
}

void AnalyzerSilence::cleanup(TrackPointer tio) {
    Q_UNUSED(tio);
    delete m_pVamp;
    m_pVamp = nullptr;
}

void AnalyzerSilence::finalize(TrackPointer tio) {
    if (m_pVamp == nullptr) {
        return;
    }

    bool success = m_pVamp->End();
    qDebug() << "Silence Detection" << (success ? "complete" : "failed");

    QVector<double> datavec = m_pVamp->GetInitFramesVector();
    delete m_pVamp;
    m_pVamp = nullptr;

    QStringList sl;
    foreach (double value, datavec) {
        sl << QString::number(value);
    }
    qDebug() << "Non-silence begins:" << sl.join(" ");
}
