#pragma once
#include <cstring>

enum PKind { KF, KI, KC, KB };
struct PDef { const char* id; const char* name; PKind kind; float mn, mx, def, step; const char* unit; const char* labels; };

static constexpr int kNumPresets = 100;

static const PDef kParams[] = {
    { "config",  "Configuration",       KC, 0, 7, 0, 1, "", "1: EQ>PCH>DL>REV|2: LEZLIE>DL>REV|3: GRAPHIC EQ>DL|4: 5BAND>PCH>MTAP|5: EQ>REVERB 2|6: RING>DL>REV|7: RESONATOR>DL>REV|8: SAMPLING" },
    { "in",      "Input Gain",          KF, -24, 12, 0, 0.1f, "dB", "" },
    { "out",     "Output Gain",         KF, -24, 12, 0, 0.1f, "dB", "" },
    { "bypass",  "Bypass",              KB, 0, 1, 0, 1, "", "" },

    // Reverb
    { "rtype",   "Reverb Type",         KC, 0, 4, 1, 1, "", "HALL 1|PLATE 1|ROOM 1|CHAMBER 1|REVERSE 1" },
    { "rinmix",  "Reverb Input Mix",    KI, -99, 99, 0, 1, "", "" },
    { "rpre",    "Reverb Pre-Delay",    KI, 0, 200, 15, 1, "ms", "" },
    { "rpremix", "Pre-Delay Mix",       KI, 0, 99, 50, 1, "%", "" },
    { "rdecay",  "Reverb Decay",        KI, 1, 99, 45, 1, "", "" },
    { "rdiff",   "Reverb Diffusion",    KI, 1, 9, 6, 1, "", "" },
    { "rdens",   "Reverb Density",      KI, 1, 9, 6, 1, "", "" },
    { "rlf",     "Low Freq Decay",      KI, -99, 0, -10, 1, "", "" },
    { "rhf",     "High Freq Decay",     KI, -99, 0, -35, 1, "", "" },
    { "rgate",   "Reverb Gate",         KB, 0, 1, 0, 1, "", "" },
    { "rghold",  "Gate Hold Time",      KI, 0, 500, 100, 1, "ms", "" },
    { "rgrel",   "Gate Release",        KI, 0, 500, 50, 1, "ms", "" },

    // Delay & Multi-Tap
    { "dtype",   "Delay Type",          KC, 0, 3, 1, 1, "", "PING PONG|STEREO|MONO|MULTI TAP" },
    { "dinmix",  "Delay Input Mix",     KI, -99, 99, 0, 1, "", "" },
    { "dtl",     "Left Delay Time",     KI, 1, 1500, 250, 1, "ms", "" },
    { "dfl",     "Left Feedback",       KI, 0, 99, 30, 1, "%", "" },
    { "dtr",     "Right Delay Time",    KI, 1, 1500, 375, 1, "ms", "" },
    { "dfr",     "Right Feedback",      KI, 0, 99, 30, 1, "%", "" },
    { "mtmaster","Multi-Tap Master FB", KI, 0, 99, 20, 1, "%", "" },
    { "t1t", "Tap 1 Time", KI, 0, 1500, 120, 1, "ms", "" }, { "t1v", "Tap 1 Vol", KI, 0, 99, 80, 1, "", "" }, { "t1p", "Tap 1 Pan", KI, -99, 99, -60, 1, "", "" },
    { "t2t", "Tap 2 Time", KI, 0, 1500, 240, 1, "ms", "" }, { "t2v", "Tap 2 Vol", KI, 0, 99, 70, 1, "", "" }, { "t2p", "Tap 2 Pan", KI, -99, 99,  60, 1, "", "" },
    { "t3t", "Tap 3 Time", KI, 0, 1500, 360, 1, "ms", "" }, { "t3v", "Tap 3 Vol", KI, 0, 99, 60, 1, "", "" }, { "t3p", "Tap 3 Pan", KI, -99, 99, -35, 1, "", "" },
    { "t4t", "Tap 4 Time", KI, 0, 1500, 480, 1, "ms", "" }, { "t4v", "Tap 4 Vol", KI, 0, 99, 50, 1, "", "" }, { "t4p", "Tap 4 Pan", KI, -99, 99,  35, 1, "", "" },
    { "t5t", "Tap 5 Time", KI, 0, 1500, 600, 1, "ms", "" }, { "t5v", "Tap 5 Vol", KI, 0, 99, 40, 1, "", "" }, { "t5p", "Tap 5 Pan", KI, -99, 99, -80, 1, "", "" },
    { "t6t", "Tap 6 Time", KI, 0, 1500, 720, 1, "ms", "" }, { "t6v", "Tap 6 Vol", KI, 0, 99, 35, 1, "", "" }, { "t6p", "Tap 6 Pan", KI, -99, 99,  80, 1, "", "" },
    { "t7t", "Tap 7 Time", KI, 0, 1500, 840, 1, "ms", "" }, { "t7v", "Tap 7 Vol", KI, 0, 99, 25, 1, "", "" }, { "t7p", "Tap 7 Pan", KI, -99, 99,   0, 1, "", "" },
    { "t8t", "Tap 8 Time", KI, 0, 1500, 960, 1, "ms", "" }, { "t8v", "Tap 8 Vol", KI, 0, 99, 20, 1, "", "" }, { "t8p", "Tap 8 Pan", KI, -99, 99,   0, 1, "", "" },

    // Pitch / Chorus / Flange / Ring / Resonator
    { "pmode",   "Pitch Mode",          KC, 0, 5, 0, 1, "", "STEREO CHORUS|MONO CHORUS|MONO FLANGE|STEREO FLANGE|DETUNE|PHASER" },
    { "pspeed",  "LFO Speed",           KI, 0, 99, 25, 1, "", "" },
    { "pdepth",  "LFO Depth",           KI, 0, 99, 35, 1, "", "" },
    { "pfb",     "Pitch Feedback",      KI, 0, 99, 20, 1, "%", "" },
    { "pdet",    "Detune Amount",       KI, -50, 50, 8, 1, "ct", "" },
    { "rmshift", "Ring Freq Shift",     KI, 1, 300, 100, 1, "Hz", "" },
    { "resdecay","Resonator Decay",     KI, 0, 99, 50, 1, "", "" },

    // Parametric EQ (3-Band & 5-Band)
    { "elf",     "Low EQ Freq",         KI, 20, 1000, 100, 1, "Hz", "" },
    { "ela",     "Low EQ Gain",         KF, -14, 14, 0, 0.5f, "dB", "" },
    { "elmf",    "Low-Mid EQ Freq",     KI, 20, 500, 250, 1, "Hz", "" },
    { "elmbw",   "Low-Mid BW",          KF, 0.2f, 2.55f, 1.0f, 0.05f, "OCT", "" },
    { "elma",    "Low-Mid EQ Gain",     KF, -14, 14, 0, 0.5f, "dB", "" },
    { "emf",     "Mid EQ Freq",         KI, 200, 10000, 1000, 1, "Hz", "" },
    { "embw",    "Mid EQ BW",           KF, 0.2f, 2.55f, 1.0f, 0.05f, "OCT", "" },
    { "ema",     "Mid EQ Gain",         KF, -14, 14, 0, 0.5f, "dB", "" },
    { "ehmf",    "High-Mid EQ Freq",    KI, 2000, 10000, 4000, 10, "Hz", "" },
    { "ehmbw",   "High-Mid BW",         KF, 0.2f, 2.55f, 1.0f, 0.05f, "OCT", "" },
    { "ehma",    "High-Mid EQ Gain",    KF, -14, 14, 0, 0.5f, "dB", "" },
    { "ehf",     "High EQ Freq",        KI, 2000, 18000, 6000, 10, "Hz", "" },
    { "eha",     "High EQ Gain",        KF, -14, 14, 0, 0.5f, "dB", "" },

    // 11-Band Graphic EQ
    { "g0", "16Hz", KF, -14, 14, 0, 0.5f, "dB", "" },  { "g1", "32Hz", KF, -14, 14, 0, 0.5f, "dB", "" },
    { "g2", "62Hz", KF, -14, 14, 0, 0.5f, "dB", "" },  { "g3", "126Hz", KF, -14, 14, 0, 0.5f, "dB", "" },
    { "g4", "250Hz", KF, -14, 14, 0, 0.5f, "dB", "" }, { "g5", "500Hz", KF, -14, 14, 0, 0.5f, "dB", "" },
    { "g6", "1kHz", KF, -14, 14, 0, 0.5f, "dB", "" },  { "g7", "2kHz", KF, -14, 14, 0, 0.5f, "dB", "" },
    { "g8", "4kHz", KF, -14, 14, 0, 0.5f, "dB", "" },  { "g9", "8kHz", KF, -14, 14, 0, 0.5f, "dB", "" },
    { "g10","16kHz", KF, -14, 14, 0, 0.5f, "dB", "" },

    // Mix Levels
    { "dlevel",  "Direct Level",        KI, 0, 99, 65, 1, "", "" },
    { "master",  "Master FX Level",     KI, 0, 99, 60, 1, "", "" },
    { "eqlevel", "EQ Output Level",     KI, 0, 99, 99, 1, "", "" },
    { "plevel",  "Pitch Out Level",     KI, 0, 99, 50, 1, "", "" },
    { "dylevel", "Delay Out Level",     KI, 0, 99, 50, 1, "", "" },
    { "rlevel",  "Reverb Out Level",    KI, 0, 99, 80, 1, "", "" }
};

static constexpr int kNumParams = (int) (sizeof (kParams) / sizeof (kParams));

inline const PDef* findParamDef (const char* id)
{
    if (id == nullptr) return nullptr;
    for (int i = 0; i < kNumParams; ++i)
        if (std::strcmp (kParams[i].id, id) == 0)
            return &kParams[i];
    return nullptr;
}

