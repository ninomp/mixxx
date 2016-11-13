#include "analyzersilence.h"

//const int kBlockLength = 1024;

AnalyzerSilence::AnalyzerSilence(UserSettingsPointer pConfig)
    : m_pConfig(pConfig),
      m_pVamp(nullptr),
      m_fThreshold(-80.0f),
      m_bPrevSilence(true),
      m_iSampleRate(0),
      m_iSamplesProcessed(0) {
}

AnalyzerSilence::~AnalyzerSilence() {
    delete m_pVamp;
}

bool AnalyzerSilence::initialize(TrackPointer tio, int sampleRate, int totalSamples) {
    QString library = "libmixxxminimal";
    QString pluginID = "aubiosilence:1";

    m_iSampleRate = sampleRate;

    m_pVamp = new VampAnalyzer();
    bool success = m_pVamp->Init(library, pluginID, sampleRate, totalSamples, false);

    if (success) {
        qDebug() << "Silence detection started with plugin" << pluginID;
    } else {
        qDebug() << "Silence detection will not start";
    }

    return success;
    //return true;
}

bool AnalyzerSilence::isDisabledOrLoadStoredSuccess(TrackPointer tio) const {
    return false;
}

float computeRMS(const CSAMPLE* pIn, const int iLen) {
    CSAMPLE energy = 0.0f;
    for (int i = 0; i < iLen; ++i) {
        energy += pIn[i] * pIn[i];
    }
    return energy / iLen;
}

void AnalyzerSilence::process(const CSAMPLE *pIn, const int iLen) {
    float rms = computeRMS(pIn, iLen);
    float dB = 10.0f * log10(rms);
    //qDebug() << "Silence detection level" << rms << "=" << dB << "dB";
    bool silence = dB < m_fThreshold;
    if (m_bPrevSilence && !silence) {
        m_NonSilentBegin.push_back(m_iSamplesProcessed / 2);
    } else if (!m_bPrevSilence && silence) {
        m_NonSilentEnd.push_back(m_iSamplesProcessed / 2);
    }

    m_bPrevSilence = silence;
    m_iSamplesProcessed += iLen;

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
    QStringList sl1;
    foreach (int value, m_NonSilentBegin) {
        sl1 << QString::number(value / m_iSampleRate);
    }
    qDebug() << "Non-silence begins:" << sl1.join(" ");

    QStringList sl2;
    foreach (int value, m_NonSilentEnd) {
        sl2 << QString::number(value / m_iSampleRate);
    }
    qDebug() << "Non-silence ends:" << sl2.join(" ");

    if (m_pVamp == nullptr) {
        return;
    }

    bool success = m_pVamp->End();
    qDebug() << "Silence Detection" << (success ? "complete" : "failed");

    QVector<double> datavec_init = m_pVamp->GetInitFramesVector();
    QVector<double> datavec_end = m_pVamp->GetEndFramesVector();
    delete m_pVamp;
    m_pVamp = nullptr;

    QStringList sl3;
    foreach (double value, datavec_init) {
        sl3 << QString::number(value / m_iSampleRate);
    }
    qDebug() << "VAMP Non-silence begins:" << sl3.join(" ");

    QStringList sl4;
    foreach (double value, datavec_end) {
        sl4 << QString::number(value / m_iSampleRate);
    }
    qDebug() << "VAMP Non-silence ends:" << sl4.join(" ");
}
