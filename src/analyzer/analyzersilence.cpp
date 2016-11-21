#include "analyzersilence.h"

//const int kBlockLength = 1024;

AnalyzerSilence::AnalyzerSilence(UserSettingsPointer pConfig)
    : m_pConfig(pConfig),
      m_pVamp(nullptr),
      m_fThreshold(-80.0f),
      m_bPrevSilence(true),
      m_iSampleRate(0),
      m_iTotalSamples(0),
      m_iFramesProcessed(0) {
}

AnalyzerSilence::~AnalyzerSilence() {
    delete m_pVamp;
}

bool AnalyzerSilence::initialize(TrackPointer tio, int sampleRate, int totalSamples) {
    QString library = "libmixxxminimal";
    QString pluginID = "aubiosilence:1";

    m_iSampleRate = sampleRate;
    m_iTotalSamples = totalSamples;

    m_pVamp = new VampAnalyzer();
    bool success = m_pVamp->Init(library, pluginID, sampleRate, totalSamples, false);

    if (success) {
        qDebug() << "Silence detection started with plugin" << pluginID;
        m_pVamp->SetParameter("silencethreshold", m_fThreshold);
    } else {
        qDebug() << "Silence detection will not start";
    }

    return success;
    //return true;
}

bool AnalyzerSilence::isDisabledOrLoadStoredSuccess(TrackPointer tio) const {
    return false;
}

void mixStereoToMono(CSAMPLE* pDest, const CSAMPLE* pSrc,
                     int iNumSamples) {
    const CSAMPLE_GAIN mixScale = CSAMPLE_GAIN_ONE
            / (CSAMPLE_GAIN_ONE + CSAMPLE_GAIN_ONE);
    // note: LOOP VECTORIZED
    for (int i = 0; i < iNumSamples / 2; ++i) {
        pDest[i] = (pSrc[i * 2] + pSrc[i * 2 + 1]) * mixScale;
    }
}

float computeMonoRMS(const CSAMPLE* pIn, const int iLen) {
    CSAMPLE energy = 0.0f;
    for (int i = 0; i < iLen; ++i) {
        energy += pIn[i] * pIn[i];
    }
    return energy / iLen;
}

void computeStereoRMS(CSAMPLE* pfRmsL, CSAMPLE* pfRmsR, const CSAMPLE* pBuffer, const int iNumSamples) {
    CSAMPLE fSumL = CSAMPLE_ZERO;
    CSAMPLE fSumR = CSAMPLE_ZERO;
    const int iNumFrames = iNumSamples / 2;
    for (int i = 0; i < iNumFrames; ++i) {
        CSAMPLE sqrl = pBuffer[i * 2] * pBuffer[i * 2];
        fSumL += sqrl;
        CSAMPLE sqrr = pBuffer[i * 2 + 1] * pBuffer[i * 2 + 1];
        fSumR += sqrr;
    }

    *pfRmsL = fSumL / iNumFrames;
    *pfRmsR = fSumR / iNumFrames;
}

void AnalyzerSilence::process(const CSAMPLE *pIn, const int iLen) {
    const int iFrames = iLen / 2;
    const int iStart = m_iFramesProcessed;
    const int iEnd = m_iFramesProcessed + iFrames;
    //float rms = computeRMS(pIn, iLen);
    CSAMPLE rmsL, rmsR;
    computeStereoRMS(&rmsL, &rmsR, pIn, iLen);
    CSAMPLE rms = math_min(rmsL, rmsR);
    double dB = 10.0f * log10(rms);
    //double dB_l = 10.0f * log10(rmsL);
    //double dB_r = 10.0f * log10(rmsR);
    bool silence = dB < m_fThreshold;
    /*qDebug() << "Silence detect"
             << "start frame" << m_iFramesProcessed
             << "silence" << silence
             << "RMS" << rms << "=" << dB << "dB"
             << "L" << rmsL << "=" << dB_l << "dB"
             << "R" << rmsR << "=" << dB_r << "dB";
    */

    if (m_bPrevSilence && !silence) {
        m_NonSilentBegin.push_back(iStart);
    } else if (!m_bPrevSilence && silence) {
        m_NonSilentEnd.push_back(iEnd);
    }

    m_bPrevSilence = silence;
    m_iFramesProcessed += iFrames;

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
        sl1 << QString::number(0.5 * (double) value / (double) m_iSampleRate);
    }
    qDebug() << "Non-silence begins:" << sl1.join(" ");

    QStringList sl2;
    foreach (int value, m_NonSilentEnd) {
        sl2 << QString::number(0.5 * (double) value / (double) m_iSampleRate);
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

    if (m_NonSilentBegin.isEmpty() || m_NonSilentEnd.isEmpty()) {
        return;
    }

    int beginning = m_NonSilentBegin.first();
    int ending = m_NonSilentEnd.last();
    int beginning_vamp = datavec_init.first();
    int ending_vamp = datavec_end.last();

    //double fBeginning = (double) beginning / (double) m_iTotalSamples;
    //double fEnding = (double) ending / (double) m_iTotalSamples;
    //tio->setCuePoint(beginning);

    bool bBeginPointFoundAndSet = false;
    bool bEndPointFoundAndSet = false;
    bool bHotcue1FoundAndSet = false;
    bool bHotcue2FoundAndSet = false;
    QList<CuePointer> cues = tio->getCuePoints();
    foreach (CuePointer pCue, cues) {
        if (pCue->getType() == Cue::BEGIN) {
            pCue->setPosition(beginning);
            bBeginPointFoundAndSet = true;
        } else if (pCue->getType() == Cue::END) {
            pCue->setPosition(ending);
            bEndPointFoundAndSet = true;
        } else if (pCue->getType() == Cue::CUE) {
            if (pCue->getHotCue() == 1) {
                pCue->setPosition(beginning_vamp);
                bHotcue1FoundAndSet = true;
            } else if (pCue->getHotCue() == 2) {
                pCue->setPosition(ending_vamp);
                bHotcue2FoundAndSet = true;
            }
        }
    }

    if (!bBeginPointFoundAndSet) {
        CuePointer pCue = tio->addCue();
        pCue->setType(Cue::BEGIN);
        pCue->setHotCue(0);
        pCue->setLabel("BEGIN");
        pCue->setLength(0);
        pCue->setPosition(2 * beginning);
    }

    if (!bEndPointFoundAndSet) {
        CuePointer pCue = tio->addCue();
        pCue->setType(Cue::END);
        pCue->setHotCue(0);
        pCue->setLabel("END");
        pCue->setLength(0);
        pCue->setPosition(2 * ending);
    }

    if (!bHotcue1FoundAndSet) {
        CuePointer pCue = tio->addCue();
        pCue->setType(Cue::CUE);
        pCue->setHotCue(0);
        //pCue->setLabel("END");
        pCue->setLength(0);
        pCue->setPosition(2 * beginning_vamp);
    }

    if (!bHotcue2FoundAndSet) {
        CuePointer pCue = tio->addCue();
        pCue->setType(Cue::CUE);
        pCue->setHotCue(1);
        //pCue->setLabel("END");
        pCue->setLength(0);
        pCue->setPosition(2 * ending_vamp);
    }
}
