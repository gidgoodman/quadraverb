// QuadraVerb-style effects engine (plain C++, no JUCE dependency).
// Signal flow and parameter ranges follow the Alesis QuadraVerb reference manual.
#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

namespace qv
{
constexpr float kPi = 3.14159265358979f;
inline float clampf (float x, float a, float b) { return x < a ? a : (x > b ? b : x); }
inline float lvl (int v) { return (float) v / 99.f; }
inline float sat (float x) { return x / std::sqrt (1.f + 0.04f * x * x); }   // unity gain for small signals, limits runaway feedback at about +/-5

struct EP   // engine parameters, in the units shown on the unit's display
{
    int config = 0;   // 0 EQ>PCH>DL>REVERB (QuadMode)  1 LEZLIE>DL>REVERB  2 GRAPHIC EQ>DELAY  3 5BAND EQ>PCH>DLY  4 EQ>REVERB
    // reverb
    int rtype = 1, rin1 = 0, rin2 = 0, rinmix = 0, rpre = 0, rpremix = 50, rdecay = 30, rdiff = 6, rdens = 6, rlf = 0, rhf = -40;
    bool rgate = false; int rghold = 100, rgrel = 50, rglevel = 0;
    // delay
    int dtype = 2, din1 = 0, dinmix = -99, dtl = 250, dfl = 25, dtr = 250, dfr = 25;
    // pitch
    int pmode = 0, pin = 1, pwave = 0, pspeed = 20, pdepth = 30, pfb = 20, pdet = 5; bool ptrig = false;
    // eq
    float elf = 100, ela = 0, emf = 1000, embw = 1, ema = 0, ehf = 6000, eha = 0;
    float elmf = 100, elmbw = 1, elma = 0, ehmf = 6000, ehmbw = 1, ehma = 0;
    float g[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    // lezlie
    int lzsep = 50, lzspeed = 1, lzhigh = 0, lzout = 50; bool lzmotor = true;
    // mix
    int dsel = 0, dlevel = 50, master = 50, eqlevel = 99, plevel = 50, dylevel = 50, rlevel = 99;
    // tempo sync (0 = off) and host transport info
    int psync = 0, dsync = 0, dsyncr = 0;
    double bpm = 120.0, ppq = 0.0; bool ppqValid = false, hostPlaying = false;
    // ---- QuadraVerb Plus additions: config 5 ring modulator, 6 resonators, 7 sampling; multi tap delay; panning / tremolo
    int rmshift = 100, rmmix = 0, rmdr = 0;                        // ring mod: spectrum shift (Hz), output mix and delay/reverb mix (down <-> up)
    int resgate = 0, resdecay = 49, res[5] = { 0, 0, 0, 0, 0 };    // resonators: gate mode, decay, tuning in semitones (0 = C3)
    int tapt[8] = { 125, 125, 125, 125, 125, 125, 125, 125 }, tapv[8] = { 50, 0, 0, 0, 0, 0, 0, 0 };
    int tapp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }, tapf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }, mtmaster = 0;   // multi tap: time (relative), volume, pan, feedback
    int ptdepth = 99, ptspeed = 30;                                // auto panning / tremolo on the EQ output (dsel 2 = panning, 3 = tremolo)
    int smode = 1, sstart = 0, slen = 155, smidi = 1, slow = 0, sbase = 60, shigh = 127, slevel = 50; bool sauto = false;   // sampling
    int recCmd = 0, playCmd = 0;                                   // front-panel commands for this block
    struct NoteEv { int on, note, vel; };
    NoteEv notes[16]; int numNotes = 0;                            // MIDI notes received during this block
};

// note lengths in beats for the sync choices: OFF, 4 BARS, 2 BARS, 1 BAR, 1/2, 1/2 D, 1/2 T, 1/4, 1/4 D, 1/4 T, 1/8, 1/8 D, 1/8 T, 1/16, 1/16 D, 1/16 T
static const double kSyncBeats[16] = { 0.0, 16.0, 8.0, 4.0, 2.0, 3.0, 4.0 / 3.0, 1.0, 1.5, 2.0 / 3.0, 0.5, 0.75, 1.0 / 3.0, 0.25, 0.375, 1.0 / 6.0 };

struct Biquad
{
    float b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;
    inline float tick (float x) { const float y = b0 * x + z1; z1 = b1 * x - a1 * y + z2; z2 = b2 * x - a2 * y; return y; }
    void copy (const Biquad& o) { b0 = o.b0; b1 = o.b1; b2 = o.b2; a1 = o.a1; a2 = o.a2; }
    void ident() { b0 = 1; b1 = b2 = a1 = a2 = 0; }
    void set (float B0, float B1, float B2, float A0, float A1, float A2)
    { const float i = 1.f / A0; b0 = B0 * i; b1 = B1 * i; b2 = B2 * i; a1 = A1 * i; a2 = A2 * i; }
    static float w0 (double sr, float f) { return 2.f * kPi * clampf (f, 10.f, (float) sr * 0.45f) / (float) sr; }
    void lowShelf (double sr, float f, float dB)
    {
        if (std::fabs (dB) < 0.01f) { ident(); return; }
        const float A = std::pow (10.f, dB / 40.f), w = w0 (sr, f), cs = std::cos (w), sn = std::sin (w);
        const float al = sn / 2.f * std::sqrt (2.f), sq = 2.f * std::sqrt (A) * al;
        set (A * ((A + 1) - (A - 1) * cs + sq), 2 * A * ((A - 1) - (A + 1) * cs), A * ((A + 1) - (A - 1) * cs - sq),
             (A + 1) + (A - 1) * cs + sq, -2 * ((A - 1) + (A + 1) * cs), (A + 1) + (A - 1) * cs - sq);
    }
    void highShelf (double sr, float f, float dB)
    {
        if (std::fabs (dB) < 0.01f) { ident(); return; }
        const float A = std::pow (10.f, dB / 40.f), w = w0 (sr, f), cs = std::cos (w), sn = std::sin (w);
        const float al = sn / 2.f * std::sqrt (2.f), sq = 2.f * std::sqrt (A) * al;
        set (A * ((A + 1) + (A - 1) * cs + sq), -2 * A * ((A - 1) + (A + 1) * cs), A * ((A + 1) + (A - 1) * cs - sq),
             (A + 1) - (A - 1) * cs + sq, 2 * ((A - 1) - (A + 1) * cs), (A + 1) - (A - 1) * cs - sq);
    }
    void peak (double sr, float f, float bwOct, float dB)
    {
        if (std::fabs (dB) < 0.01f) { ident(); return; }
        const float A = std::pow (10.f, dB / 40.f), w = w0 (sr, f), cs = std::cos (w), sn = std::sin (w);
        const float al = sn * std::sinh (0.34657359f * bwOct * w / sn);
        set (1 + al * A, -2 * cs, 1 - al * A, 1 + al / A, -2 * cs, 1 - al / A);
    }
    void lowpass (double sr, float f, float Q)
    {
        const float w = w0 (sr, f), cs = std::cos (w), al = std::sin (w) / (2 * Q);
        set ((1 - cs) / 2, 1 - cs, (1 - cs) / 2, 1 + al, -2 * cs, 1 - al);
    }
};

struct DelayBuf
{
    std::vector<float> b; int w = 0, size = 1;
    void init (int n) { size = std::max (n, 4); b.assign ((size_t) size, 0.f); w = 0; }
    void clear() { std::fill (b.begin(), b.end(), 0.f); w = 0; }
    inline void push (float x) { b[(size_t) w] = x; if (++w >= size) w = 0; }
    inline float read (float d) const
    {
        d = clampf (d, 1.f, (float) (size - 2));
        float pos = (float) w - d; if (pos < 0) pos += (float) size;
        const int i0 = (int) pos; const float fr = pos - (float) i0;
        int i1 = i0 + 1; if (i1 >= size) i1 = 0;
        return b[(size_t) i0] * (1.f - fr) + b[(size_t) i1] * fr;
    }
};

struct Stereo
{
    std::vector<float> l, r;
    void resize (int n) { l.assign ((size_t) n, 0.f); r.assign ((size_t) n, 0.f); }
};

// ---------------------------------------------------------------- EQ (3-band, 5-band, 11-band graphic)
struct EqBank
{
    Biquad f[2][11]; int nb = 0;
    void clear() { for (auto& ch : f) for (auto& q : ch) q.z1 = q.z2 = 0.f; }
    void configure (double sr, const EP& p)
    {
        nb = 0; Biquad c;
        auto push = [&] { for (int ch = 0; ch < 2; ++ch) f[ch][nb].copy (c); ++nb; };
        if (p.config == 0 || p.config == 4)
        {
            c.lowShelf (sr, p.elf, p.ela); push();
            c.peak (sr, p.emf, p.embw, p.ema); push();
            c.highShelf (sr, p.ehf, p.eha); push();
        }
        else if (p.config == 3)
        {
            c.lowShelf (sr, p.elf, p.ela); push();
            c.peak (sr, p.elmf, p.elmbw, p.elma); push();
            c.peak (sr, p.emf, p.embw, p.ema); push();
            c.peak (sr, p.ehmf, p.ehmbw, p.ehma); push();
            c.highShelf (sr, p.ehf, p.eha); push();
        }
        else if (p.config == 2)
        {
            static const float fr[11] = { 16, 32, 63, 126, 250, 500, 1000, 2000, 4000, 8000, 16000 };
            for (int i = 0; i < 11; ++i) { c.peak (sr, fr[i], 1.f, p.g[i]); push(); }
        }
    }
    void process (float* l, float* r, int n)
    {
        for (int b = 0; b < nb; ++b)
            for (int i = 0; i < n; ++i) { l[i] = f[0][b].tick (l[i]); r[i] = f[1][b].tick (r[i]); }
    }
};

// ---------------------------------------------------------------- PITCH: chorus, flange, detune, phase shifter
struct Pitch
{
    double sr = 44100; DelayBuf dl[2], sh[2];
    float ap[2][6] = {}; float lfo = 0, tph = 1, env = 0, shPh[2] = { 0, 0.37f };
    void clear() { dl[0].clear(); dl[1].clear(); sh[0].clear(); sh[1].clear(); for (auto& a : ap) for (float& z : a) z = 0.f; env = 0; tph = 1; }
    void init (double s)
    {
        sr = s; for (int c = 0; c < 2; ++c) { dl[c].init ((int) (sr * 0.05) + 8); sh[c].init ((int) (sr * 0.1) + 8); }
        for (auto& a : ap) for (float& z : a) z = 0.f;
        lfo = 0; tph = 1; env = 0;
    }
    static float tri (float ph) { return 4.f * std::fabs (ph - std::floor (ph) - 0.5f) - 1.f; }
    void process (const float* iL, const float* iR, float* oL, float* oR, int n, const EP& p)
    {
        const float hz = 0.05f * std::pow (2.f, (float) p.pspeed / 99.f * 7.f);
        float inc = hz / (float) sr;
        if (p.psync > 0 && p.psync < 16 && p.bpm > 1.0)   // locked to host tempo, and to the bar grid while the transport runs
        {
            const double beats = kSyncBeats[p.psync];
            inc = (float) (p.bpm / 60.0 / beats / sr);
            if (p.ppqValid && p.hostPlaying) lfo = (float) std::fmod (std::fmod (p.ppq / beats, 1.0) + 1.0, 1.0);
        }
        const float depth = lvl (p.pdepth), fbk = lvl (p.pfb);
        const bool mono = (p.pmode == 1 || p.pmode == 2);
        const float msr = (float) sr * 0.001f;
        for (int i = 0; i < n; ++i)
        {
            lfo += inc; if (lfo >= 1.f) lfo -= 1.f;
            float xL = iL[i], xR = iR[i];
            if (mono) xL = xR = 0.5f * (xL + xR);
            float yL = 0, yR = 0;
            switch (p.pmode)
            {
                case 0: case 1:   // stereo / mono chorus
                {
                    for (int c = 0; c < 2; ++c)
                    {
                        const float ph = lfo + (c == 1 && p.pmode == 0 ? 0.5f : 0.f);
                        const float w = p.pwave == 0 ? tri (ph) : ((ph - std::floor (ph)) < 0.5f ? 1.f : -1.f);
                        const float d = (12.f + 7.f * depth * w) * msr;
                        const float y = dl[c].read (d);
                        dl[c].push ((c == 0 ? xL : xR) + 0.6f * fbk * y);
                        (c == 0 ? yL : yR) = y;
                    }
                    break;
                }
                case 2: case 3:   // mono / stereo flange
                {
                    float w0 = tri (lfo), w1 = tri (lfo + 0.25f);
                    if (p.ptrig)
                    {
                        const float e = std::max (std::fabs (iL[i]), std::fabs (iR[i]));
                        if (e > 0.02f && e > env * 3.f) tph = 0.f;
                        env = std::max (e, env * 0.9995f);
                        tph = std::min (1.f, tph + inc * 2.f); w0 = w1 = 1.f - 2.f * tph;
                    }
                    for (int c = 0; c < 2; ++c)
                    {
                        const float w = (c == 1 && p.pmode == 3) ? w1 : w0;
                        const float d = (2.4f + 1.9f * depth * w) * msr;
                        const float y = dl[c].read (d);
                        dl[c].push ((c == 0 ? xL : xR) + 0.85f * fbk * y);
                        (c == 0 ? yL : yR) = y;
                    }
                    break;
                }
                case 4:   // pitch detune: +/- cents, opposite on each side
                {
                    const float win = (float) sr * 0.04f;
                    for (int c = 0; c < 2; ++c)
                    {
                        const float cents = (float) p.pdet * (c == 0 ? 1.f : -1.f);
                        const float ratio = std::pow (2.f, cents / 1200.f);
                        shPh[c] += (1.f - ratio) / win; if (shPh[c] >= 1.f) shPh[c] -= 1.f; else if (shPh[c] < 0.f) shPh[c] += 1.f;
                        const float s = std::sin (kPi * shPh[c]), g1 = s * s, g2 = 1.f - g1;
                        float p2 = shPh[c] + 0.5f; if (p2 >= 1.f) p2 -= 1.f;
                        sh[c].push (c == 0 ? xL : xR);
                        (c == 0 ? yL : yR) = g1 * sh[c].read (shPh[c] * win + 2.f) + g2 * sh[c].read (p2 * win + 2.f);
                    }
                    break;
                }
                default:  // phase shifter
                {
                    for (int c = 0; c < 2; ++c)
                    {
                        const float w = tri (lfo + (c == 1 ? 0.25f : 0.f));
                        const float fmax = 200.f * std::pow (15.f, 0.2f + 0.8f * depth);
                        const float f = 200.f * std::pow (fmax / 200.f, 0.5f + 0.5f * w);
                        const float t = std::tan (kPi * clampf (f, 20.f, (float) sr * 0.4f) / (float) sr), a = (t - 1.f) / (t + 1.f);
                        float v = c == 0 ? xL : xR; const float x0 = v;
                        for (int s = 0; s < 6; ++s) { const float y = a * v + ap[c][s]; ap[c][s] = v - a * y; v = y; }
                        (c == 0 ? yL : yR) = 0.5f * (x0 + v);
                    }
                    break;
                }
            }
            oL[i] = yL; oR[i] = yR;
        }
    }
};

// ---------------------------------------------------------------- DELAY: mono / stereo / ping-pong
struct DelayFx
{
    double sr = 44100; DelayBuf a, b; float curMono = -1.f, curL = -1.f, curR = -1.f;   // smoothed read positions (a delay time change glides, it never jumps)
    void clear() { a.clear(); b.clear(); curMono = curL = curR = -1.f; }
    void init (double s) { sr = s; a.init ((int) (sr * 1.6) + 4); b.init ((int) (sr * 1.6) + 4); }
    void process (const float* iL, const float* iR, float* oL, float* oR, int n, const EP& p)
    {
        const bool big = (p.config == 2 || p.config == 3);
        const float maxMono = big ? 1500.f : (p.config == 6 ? 720.f : 800.f), maxSt = big ? 750.f : (p.config == 6 ? 320.f : 400.f), k = (float) sr * 0.001f;
        const float fbL = std::min (lvl (p.dfl), 0.97f), fbR = std::min (lvl (p.dfr), 0.97f);
        auto timeMs = [&] (int ms, int sync, float mx)   // synced times follow the host tempo, halved until they fit
        {
            if (sync > 0 && sync < 16 && p.bpm > 1.0)
            { double t = kSyncBeats[sync] * 60000.0 / p.bpm; while (t > (double) mx) t *= 0.5; while (t < 1.0) t *= 2.0; return (float) t; }
            return clampf ((float) ms, 1.f, mx);
        };
        const float tMono = timeMs (p.dtl, p.dsync, maxMono) * k, tSt = timeMs (p.dtl, p.dsync, maxSt) * k, tStR = timeMs (p.dtr, p.dsyncr, maxSt) * k;
        if (curMono < 0.f) { curMono = tMono; curL = tSt; curR = tStR; }
        const float glide = 1.f - std::exp (-1.f / (0.03f * (float) sr));   // ~30 ms, like a tape/BBD delay being retimed
        for (int i = 0; i < n; ++i)
        {
            const float m = 0.5f * (iL[i] + iR[i]);
            if (p.dtype == 2)        // mono
            {
                curMono += (tMono - curMono) * glide;
                const float y = a.read (curMono);
                a.push (sat (m + fbL * y)); oL[i] = oR[i] = y;
            }
            else if (p.dtype == 1)   // stereo: two independent delays
            {
                curL += (tSt - curL) * glide; curR += (tStR - curR) * glide;
                const float yA = a.read (curL), yB = b.read (curR);
                a.push (sat (iL[i] + fbL * yA)); b.push (sat (iR[i] + fbR * yB)); oL[i] = yA; oR[i] = yB;
            }
            else                     // ping-pong
            {
                curL += (tSt - curL) * glide;
                const float t = curL;
                const float yA = a.read (t), yB = b.read (t);
                a.push (sat (m + fbL * yB)); b.push (yA); oL[i] = yA; oR[i] = yB;
            }
        }
    }
};

// ---------------------------------------------------------------- REVERB: five types, pre-delay, decay, damping, gate, reverse
struct Reverb
{
    struct Comb { std::vector<float> b; int len = 1, i = 0; float st = 0; };
    struct AP   { std::vector<float> b; int len = 1, i = 0; };
    double sr = 44100; Comb c[2][8]; AP a[2][4]; DelayBuf pre;
    float env = 0, gain = 1, hold = 0, rev = 0; bool open = false, armed = true; int lastType = -1;
    void clear()
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            for (auto& cb : c[ch]) { std::fill (cb.b.begin(), cb.b.end(), 0.f); cb.i = 0; cb.st = 0.f; }
            for (auto& ar : a[ch]) { std::fill (ar.b.begin(), ar.b.end(), 0.f); ar.i = 0; }
        }
        pre.clear(); env = 0; gain = 1; hold = 0; rev = 0; open = false; armed = true;
    }
    void init (double s)
    {
        sr = s; const double k = sr / 44100.0 * 1.6;
        static const int cl[8] = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 }, al[4] = { 556, 441, 341, 225 };
        for (int ch = 0; ch < 2; ++ch)
        {
            for (int j = 0; j < 8; ++j) { c[ch][j].b.assign ((size_t) ((cl[j] + 23) * k) + 8, 0.f); c[ch][j].st = 0; }
            for (int j = 0; j < 4; ++j) a[ch][j].b.assign ((size_t) ((al[j] + 23) * k) + 8, 0.f);
        }
        pre.init ((int) (sr * 0.45)); env = 0; gain = 1; hold = 0; rev = 0; open = false; armed = true; lastType = -1;
    }
    void process (const float* iL, const float* iR, float* oL, float* oR, int n, const EP& p)
    {
        static const int cl[8] = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 }, al[4] = { 556, 441, 341, 225 };
        static const float tscale[5] = { 1.5f, 0.8f, 0.55f, 1.0f, 1.0f }, tdamp[5] = { 0.30f, 0.10f, 0.40f, 0.25f, 0.20f };
        const int ty = (int) clampf ((float) p.rtype, 0.f, 4.f);
        const double k = sr / 44100.0 * tscale[ty];
        for (int ch = 0; ch < 2; ++ch)
        {
            for (int j = 0; j < 8; ++j) c[ch][j].len = std::min ((int) ((cl[j] + (ch ? 23 : 0)) * k), (int) c[ch][j].b.size() - 1);
            for (int j = 0; j < 4; ++j) a[ch][j].len = std::min ((int) ((al[j] + (ch ? 23 : 0)) * sr / 44100.0), (int) a[ch][j].b.size() - 1);
        }
        const bool isRev = (ty == 4);
        const float revT = 0.1f + lvl (p.rdecay) * 1.4f;
        const float rt60 = isRev ? std::max (revT, 0.2f) : 0.2f * std::pow (60.f, (float) (p.rdecay - 1) / 98.f);
        float g[8];
        for (int j = 0; j < 8; ++j) g[j] = std::min (0.985f, std::pow (10.f, -3.f * (float) c[0][j].len / (rt60 * (float) sr)));
        const float damp = clampf (tdamp[ty] + (float) (-p.rhf) / 99.f * 0.6f, 0.f, 0.95f);
        const float apf = 0.3f + (float) (p.rdiff - 1) / 8.f * 0.3f;
        const float preD = (float) p.rpre * 0.001f * (float) sr, pm = lvl (p.rpremix);
        const float floorG = lvl (p.rglevel), relStep = (1.f - floorG) / std::max (1.f, (float) p.rgrel * 0.001f * (float) sr);
        const int holdN = (int) ((float) p.rghold * 0.001f * (float) sr);
        const float inGain = 0.03f, outScale = 1.5f;
        for (int i = 0; i < n; ++i)
        {
            const float x = 0.5f * (iL[i] + iR[i]);
            pre.push (x);
            const float xin = (p.rpre > 0) ? ((1.f - pm) * x + pm * pre.read (std::max (1.f, preD))) : x;
            // gate / reverse envelope, keyed from the reverb input
            const float e = std::fabs (x);
            env = e > env ? e : env * (1.f - 1.f / ((float) sr * 0.02f));
            float eg = 1.f;
            if (isRev)
            {
                if (env > 0.01f && armed) { open = true; armed = false; rev = 0.f; }
                if (env < 0.005f) armed = true;
                if (open) { rev += 1.f / (revT * (float) sr); if (rev >= 1.f) { open = false; rev = 0.f; gain = 0.f; } }
                eg = open ? rev * rev : 0.f;
            }
            else if (p.rgate)
            {
                if (env > 0.01f) { open = true; hold = (float) holdN; }
                else if (hold > 0.f) hold -= 1.f; else open = false;
                if (open) gain = std::min (1.f, gain + 0.01f); else gain = std::max (floorG, gain - relStep);
                eg = gain;
            }
            float out[2];
            for (int ch = 0; ch < 2; ++ch)
            {
                float sum = 0.f;
                for (int j = 0; j < 8; ++j)
                {
                    Comb& cb = c[ch][j]; const float y = cb.b[(size_t) cb.i];
                    cb.st = y * (1.f - damp) + cb.st * damp;
                    cb.b[(size_t) cb.i] = xin * inGain + cb.st * g[j];
                    if (++cb.i >= cb.len) cb.i = 0;
                    sum += y;
                }
                for (int j = 0; j < 4; ++j)
                {
                    AP& ar = a[ch][j]; const float bo = ar.b[(size_t) ar.i], in = sum;
                    sum = bo - in; ar.b[(size_t) ar.i] = in + bo * apf;
                    if (++ar.i >= ar.len) ar.i = 0;
                }
                out[ch] = sum * outScale * eg;
            }
            oL[i] = out[0]; oR[i] = out[1];
        }
    }
};

// ---------------------------------------------------------------- LEZLIE: rotary speaker simulation
struct Leslie
{
    double sr = 44100; Biquad lp[2], hp[2]; DelayBuf dl[2];
    void clear() { for (int c = 0; c < 2; ++c) { dl[c].clear(); lp[c].z1 = lp[c].z2 = 0.f; } }
    float hornSpd = 0.8f, drumSpd = 0.66f, hornPh = 0, drumPh = 0;
    void init (double s) { sr = s; for (int c = 0; c < 2; ++c) { dl[c].init ((int) (sr * 0.01) + 8); lp[c] = Biquad(); hp[c] = Biquad(); } hornPh = drumPh = 0; }
    void process (const float* iL, const float* iR, float* oL, float* oR, int n, const EP& p)
    {
        Biquad cl; cl.lowpass (sr, 800.f, 0.707f);
        for (int c = 0; c < 2; ++c) lp[c].copy (cl);
        const float hornT = p.lzspeed == 0 ? 0.f : (p.lzspeed == 1 ? 0.8f : 6.6f), drumT = p.lzspeed == 0 ? 0.f : (p.lzspeed == 1 ? 0.66f : 5.7f);
        const float hornA = p.lzmotor ? 1.f / (float) (1.0 * sr) : 1.f, drumA = p.lzmotor ? 1.f / (float) (3.0 * sr) : 1.f;
        const float hg = std::pow (10.f, (float) p.lzhigh / 20.f), sep = lvl (p.lzsep) * 0.25f, k = (float) sr * 0.001f;
        for (int i = 0; i < n; ++i)
        {
            hornSpd += (hornT - hornSpd) * hornA * 4.f; drumSpd += (drumT - drumSpd) * drumA * 4.f;
            hornPh += hornSpd / (float) sr; if (hornPh >= 1.f) hornPh -= 1.f;
            drumPh += drumSpd / (float) sr; if (drumPh >= 1.f) drumPh -= 1.f;
            const float m = 0.5f * (iL[i] + iR[i]);
            for (int c = 0; c < 2; ++c)
            {
                const float lo = lp[c].tick (m), hi = m - lo;
                const float ph = c == 0 ? 0.f : sep;
                const float hp_ = 2.f * kPi * (hornPh + ph), dp = 2.f * kPi * (drumPh + ph);
                dl[c].push (hi);
                const float hornOut = dl[c].read ((0.6f + 0.45f * std::sin (hp_)) * k + 1.f) * (1.f + 0.45f * std::sin (hp_ + 1.2f));
                const float drumOut = lo * (1.f + 0.3f * std::sin (dp));
                (c == 0 ? oL : oR)[i] = drumOut + hornOut * hg;
            }
        }
    }
};

// ---------------------------------------------------------------- MULTI TAP DELAY: 8 taps, each with time (relative to the previous tap), volume, pan, feedback
struct MultiTap
{
    double sr = 44100; DelayBuf buf;
    void init (double s) { sr = s; buf.init ((int) (sr * 1.6) + 4); }
    void clear() { buf.clear(); }
    void process (const float* iL, const float* iR, float* oL, float* oR, int n, const EP& p)
    {
        float pos[8], vl[8], vr[8], fb[8]; float cum = 0.f, fbSum = 0.f;
        for (int k = 0; k < 8; ++k)
        {
            cum = std::min (1500.f, cum + (float) p.tapt[k]);                       // times are cumulative: a tap moves all later taps
            pos[k] = cum * 0.001f * (float) sr;
            const float v = lvl (p.tapv[k]), pn = (float) p.tapp[k] / 99.f, a = (pn + 1.f) * kPi * 0.25f;
            vl[k] = v * std::cos (a) * 1.41421356f; vr[k] = v * std::sin (a) * 1.41421356f;   // equal-power pan, unity at centre
            fb[k] = lvl (p.tapf[k]); fbSum += fb[k];
        }
        float master = lvl (p.mtmaster);
        if (fbSum * master > 0.95f) master = 0.95f / fbSum;                          // keep the feedback loop stable
        for (int i = 0; i < n; ++i)
        {
            const float m = 0.5f * (iL[i] + iR[i]);
            float l = 0.f, r = 0.f, f = 0.f;
            for (int k = 0; k < 8; ++k)
            {
                if (pos[k] < 1.f) continue;
                const float y = buf.read (pos[k]);
                l += y * vl[k]; r += y * vr[k]; f += y * fb[k];
            }
            buf.push (sat (m + master * f));
            oL[i] = l; oR[i] = r;
        }
    }
};

// ---------------------------------------------------------------- RING MODULATOR: spectrum shift up and down (single-sideband via an all-pass Hilbert pair)
struct FreqShifter
{
    struct Chain { float xh[4][2] = {}, yh[4][2] = {}; };
    Chain a, b; float dz = 0, ph = 0; double sr = 44100;
    void clear() { a = Chain(); b = Chain(); dz = 0; }
    static float run (Chain& c, const float* coef, float in)
    {
        float v = in;
        for (int s = 0; s < 4; ++s)
        {
            const float k = coef[s] * coef[s];
            const float y = k * (v + c.yh[s][1]) - c.xh[s][1];
            c.xh[s][1] = c.xh[s][0]; c.xh[s][0] = v; c.yh[s][1] = c.yh[s][0]; c.yh[s][0] = y; v = y;
        }
        return v;
    }
    void init (double s) { sr = s; a = Chain(); b = Chain(); dz = 0; ph = 0; }
    void process (const float* in, float* up, float* down, int n, float shiftHz)
    {
        static const float c1[4] = { 0.6923878f, 0.9360654322959f, 0.9882295226860f, 0.9987488452737f };
        static const float c2[4] = { 0.4021921162426f, 0.8561710882420f, 0.9722909545651f, 0.9952884791278f };
        const float inc = shiftHz / (float) sr;
        for (int i = 0; i < n; ++i)
        {
            const float I = run (a, c1, in[i]);
            const float q0 = run (b, c2, in[i]); const float Q = dz; dz = q0;       // second path is one sample late: 90 degrees apart
            ph += inc; if (ph >= 1.f) ph -= 1.f;
            const float cs = std::cos (2.f * kPi * ph), sn = std::sin (2.f * kPi * ph);
            up[i] = I * cs + Q * sn; down[i] = I * cs - Q * sn;
        }
    }
};

// ---------------------------------------------------------------- RESONATORS: five tuned comb filters that follow MIDI notes
struct Resonators
{
    double sr = 44100; DelayBuf d[5]; float lp[5] = {}, gate[5] = { 1, 1, 1, 1, 1 }, off[5] = {}; int note[5] = { -1, -1, -1, -1, -1 }, rr = 0; bool held[5] = {};
    void clear() { for (auto& b : d) b.clear(); for (int j = 0; j < 5; ++j) { lp[j] = 0; gate[j] = 1; } }
    void init (double s)
    {
        sr = s; for (auto& b : d) b.init ((int) (sr / 15.0) + 8);
        for (int j = 0; j < 5; ++j) { lp[j] = 0; gate[j] = 1; off[j] = 0; note[j] = -1; held[j] = false; }
        rr = 0;
    }
    void noteOn (int n)
    {
        int v = -1; for (int j = 0; j < 5; ++j) if (! held[j]) { v = j; break; }
        if (v < 0) v = rr++ % 5;
        note[v] = n; held[v] = true; off[v] = (float) (n - 60);   // the resonator takes the pitch of the key
    }
    void noteOff (int n) { for (int j = 0; j < 5; ++j) if (held[j] && note[j] == n) held[j] = false; }
    void process (const float* iL, const float* iR, float* oL, float* oR, int n, const EP& p)
    {
        float del[5], g[5], tgt[5];
        const float rt60 = 0.05f * std::pow (100.f, (float) p.resdecay / 99.f);
        for (int j = 0; j < 5; ++j)
        {
            const float f = clampf (440.f * std::pow (2.f, ((float) (60 + p.res[j]) + off[j] - 69.f) / 12.f), 20.f, (float) sr * 0.45f);
            del[j] = (float) sr / f - 1.0f; g[j] = std::min (0.9995f, std::pow (10.f, -3.f * del[j] / (rt60 * (float) sr)));   // minus the loop filter's own delay, so the pitch is true
            tgt[j] = (p.resgate == 1) ? (held[j] ? 1.f : 0.f) : 1.f;
        }
        for (int i = 0; i < n; ++i)
        {
            const float m = 0.5f * (iL[i] + iR[i]); float y = 0.f;
            for (int j = 0; j < 5; ++j)
            {
                gate[j] += (tgt[j] - gate[j]) * 0.004f;
                const float o = d[j].read (del[j]);
                lp[j] += 0.5f * (o - lp[j]);
                d[j].push (m * gate[j] * std::sqrt (1.f - g[j] * g[j]) * 0.4f + g[j] * lp[j]);
                y += o;
            }
            oL[i] = oR[i] = sat (y * 0.8f);
        }
    }
};

// ---------------------------------------------------------------- SAMPLER: 1.55 s record, one-shot / looping / audio / MIDI triggered playback
struct Sampler
{
    double sr = 44100; std::vector<float> bl, br; int cap = 1, len = 0, state = 0, recPos = 0, curNote = -1, quiet = 0;   // state: 0 idle, 1 waiting for audio, 2 recording
    bool playing = false, loopNow = false, armed = true; double pos = 0, rate = 1.0;
    void clear() { playing = false; }   // the recorded sample itself is kept
    void init (double s) { sr = s; cap = (int) (1.55 * sr) + 4; bl.assign ((size_t) cap, 0.f); br = bl; len = 0; state = 0; playing = false; armed = true; quiet = 0; }
    void startRecord (const EP& p) { playing = false; len = 0; recPos = 0; state = p.sauto ? 1 : 2; }
    void trigger (const EP& p, double r) { if (len < 4) return; pos = (double) p.sstart * 0.01 * sr; rate = r; playing = true; loopNow = (p.smode == 0); }
    void panelPlay (const EP& p) { if (p.smode == 0 && playing) playing = false; else trigger (p, 1.0); }   // looping: the button toggles
    void noteOn (const EP& p, int note)
    {
        if (p.smidi == 0 || note < p.slow || note > p.shigh) return;
        int d = note - p.sbase; while (d > 12) d -= 12; while (d < -12) d += 12;   // two-octave range, folded back in (manual p.14)
        curNote = note; trigger (p, std::pow (2.0, (double) d / 12.0));
    }
    void noteOff (const EP& p, int note) { if (p.smidi == 1 && note == curNote) playing = false; }
    void process (const float* iL, const float* iR, float* oL, float* oR, int n, const EP& p)
    {
        const int startF = (int) ((double) p.sstart * 0.01 * sr), lenF = (int) ((double) p.slen * 0.01 * sr);
        for (int i = 0; i < n; ++i)
        {
            const float pk = std::max (std::fabs (iL[i]), std::fabs (iR[i]));
            if (state == 1 && pk > 0.126f) { state = 2; recPos = 0; }               // -18 dB threshold, the first LED
            if (state == 2) { bl[(size_t) recPos] = iL[i]; br[(size_t) recPos] = iR[i]; if (++recPos >= cap) { len = cap; state = 0; } }
            if (p.smode == 2)                                                       // audio trigger: the -18 dB LED must go out before it can retrigger
            {
                if (pk > 0.126f) { if (armed) { armed = false; trigger (p, 1.0); } quiet = 0; }
                else if (++quiet > (int) (0.02 * sr)) armed = true;
            }
            float yl = 0.f, yr = 0.f;
            if (playing)
            {
                const int endF = std::min (len, startF + lenF);
                if (endF - startF < 2) playing = false;
                else
                {
                    if (pos >= (double) endF) { if (loopNow) pos = (double) startF; else playing = false; }
                    if (playing)
                    {
                        const int i0 = (int) pos, i1 = std::min (i0 + 1, len - 1); const float fr = (float) (pos - (double) i0);
                        yl = bl[(size_t) i0] * (1.f - fr) + bl[(size_t) i1] * fr; yr = br[(size_t) i0] * (1.f - fr) + br[(size_t) i1] * fr;
                        pos += rate;
                    }
                }
            }
            oL[i] = yl; oR[i] = yr;
        }
    }
};

// ---------------------------------------------------------------- the whole machine
class Engine
{
public:
    int sampState = 0; bool sampPlaying = false;   // read by the processor for the front-panel display
    // Empties every delay line, reverb tank and filter state. Used while the output is muted around a program change.
    void clear() { eqb.clear(); pitch.clear(); delay.clear(); reverb.clear(); les.clear(); mtap.clear(); shift.clear(); res.clear(); smp.clear(); gInit = false; }
    void prepare (double sampleRate, int maxBlock)
    {
        sr = sampleRate; cap = std::max (maxBlock, 64);
        pitch.init (sr); delay.init (sr); reverb.init (sr); les.init (sr); mtap.init (sr); shift.init (sr); res.init (sr); smp.init (sr);
        ptPh = 0.f;
        for (Stereo* s : { &x, &eq, &pit, &din, &dout, &rin, &rout, &lz, &smpOut, &up, &dn }) s->resize (cap);
    }
    // in-place stereo processing; input gain/output gain are applied by the caller
    void process (float* L, float* R, int n, const EP& p)
    {
        if (p.recCmd && p.config == 7) smp.startRecord (p);          // BYPASS in the sampling configuration
        if (p.playCmd && p.config == 7) smp.panelPlay (p);           // EQ in the sampling configuration
        for (int k = 0; k < p.numNotes; ++k)
        {
            const auto& e = p.notes[k];
            if (p.config == 6) { if (e.on) res.noteOn (e.note); else res.noteOff (e.note); }
            if (p.config == 7) { if (e.on) smp.noteOn (p, e.note); else smp.noteOff (p, e.note); }
        }
        for (int off = 0; off < n; off += cap) run (L + off, R + off, std::min (cap, n - off), p);
        sampState = smp.state; sampPlaying = smp.playing;
    }
private:
    static void zero (Stereo& s, int n) { std::fill (s.l.begin(), s.l.begin() + n, 0.f); std::fill (s.r.begin(), s.r.begin() + n, 0.f); }
    void run (float* L, float* R, int n, const EP& p)
    {
        std::copy (L, L + n, x.l.begin()); std::copy (R, R + n, x.r.begin());
        eq.l = x.l; eq.r = x.r;
        const int cfg = p.config;
        if (cfg == 0 || cfg == 2 || cfg == 3 || cfg == 4) { eqb.configure (sr, p); eqb.process (eq.l.data(), eq.r.data(), n); }
        for (Stereo* s : { &pit, &dout, &rout, &lz, &smpOut }) zero (*s, n);

        if (cfg == 1) les.process (x.l.data(), x.r.data(), lz.l.data(), lz.r.data(), n, p);
        if (cfg == 5)   // ring modulator: shift the spectrum up and down, mix the two for the mixer and (separately) for delay/reverb
        {
            shift.process (x.l.data(), up.l.data(), dn.l.data(), n, (float) p.rmshift);
            std::copy (up.l.begin(), up.l.begin() + n, up.r.begin()); std::copy (dn.l.begin(), dn.l.begin() + n, dn.r.begin());
            const float wo = (float) (p.rmmix + 99) / 198.f, wd = (float) (p.rmdr + 99) / 198.f;
            for (int i = 0; i < n; ++i)
            {
                const size_t k = (size_t) i;
                pit.l[k] = pit.r[k] = dn.l[k] * (1.f - wo) + up.l[k] * wo;
                lz.l[k] = lz.r[k] = dn.l[k] * (1.f - wd) + up.l[k] * wd;
            }
        }
        if (cfg == 6) { res.process (x.l.data(), x.r.data(), pit.l.data(), pit.r.data(), n, p); lz.l = pit.l; lz.r = pit.r; }
        if (cfg == 0 || cfg == 3)
        {
            const Stereo& src = (p.pin == 1) ? eq : x;
            pitch.process (src.l.data(), src.r.data(), pit.l.data(), pit.r.data(), n, p);
        }
        if (cfg != 4 && cfg != 7)
        {
            const bool fxIn = (cfg == 1 || cfg == 5 || cfg == 6);
            const Stereo& a = fxIn ? x : (p.din1 == 0 ? eq : x);
            const Stereo* b = fxIn ? &lz : (cfg == 2 ? nullptr : &pit);
            const float w = b ? ((float) (p.dinmix + 99) / 198.f) : 0.f;
            for (int i = 0; i < n; ++i)
            {
                din.l[(size_t) i] = a.l[(size_t) i] * (1.f - w) + (b ? b->l[(size_t) i] * w : 0.f);
                din.r[(size_t) i] = a.r[(size_t) i] * (1.f - w) + (b ? b->r[(size_t) i] * w : 0.f);
            }
            if (cfg == 3 && p.dtype == 3) mtap.process (din.l.data(), din.r.data(), dout.l.data(), dout.r.data(), n, p);
            else delay.process (din.l.data(), din.r.data(), dout.l.data(), dout.r.data(), n, p);
        }
        if (cfg == 0 || cfg == 1 || cfg == 4 || cfg == 5 || cfg == 6)
        {
            const bool fxIn = (cfg == 1 || cfg == 5 || cfg == 6);
            if (cfg == 4) { rin.l = eq.l; rin.r = eq.r; }
            else
            {
                const float w2 = (float) (p.rinmix + 99) / 198.f;
                for (int i = 0; i < n; ++i)
                    for (int ch = 0; ch < 2; ++ch)
                    {
                        const size_t k = (size_t) i;
                        auto pick = [&] (const Stereo& s) { return ch == 0 ? s.l[k] : s.r[k]; };
                        float s1;
                        switch (p.rin1) { case 0: s1 = pick (din); break; case 1: s1 = pick (pit); break; case 2: s1 = pick (eq); break;
                                          case 3: s1 = pick (x); break; default: s1 = pick (fxIn ? lz : din); break; }
                        const float s2 = p.rin2 == 0 ? pick (dout) : pick (pit);
                        (ch == 0 ? rin.l : rin.r)[k] = s1 * (1.f - w2) + s2 * w2;
                    }
            }
            reverb.process (rin.l.data(), rin.r.data(), rout.l.data(), rout.r.data(), n, p);
            if (cfg == 4 && p.pdepth > 0)   // "reverb chorus"
            {
                EP cp = p; cp.pmode = 0;
                rin.l.assign (rout.l.begin(), rout.l.end()); rin.r.assign (rout.r.begin(), rout.r.end());
                pitch.process (rin.l.data(), rin.r.data(), pit.l.data(), pit.r.data(), n, cp);
                for (int i = 0; i < n; ++i) { rout.l[(size_t) i] = 0.5f * (rout.l[(size_t) i] + pit.l[(size_t) i]); rout.r[(size_t) i] = 0.5f * (rout.r[(size_t) i] + pit.r[(size_t) i]); }
            }
        }
        if (cfg == 7) smp.process (x.l.data(), x.r.data(), smpOut.l.data(), smpOut.r.data(), n, p);

        // ---- mix section
        const bool pre = (p.dsel == 0 || cfg == 1 || cfg >= 5);
        // the mixer gains ramp across the block instead of stepping, so level changes and program changes don't click
        const float cur[7] = { (cfg == 7 || pre) ? lvl (p.dlevel) : lvl (p.eqlevel), lvl (p.master), lvl (p.plevel), lvl (p.dylevel), lvl (p.rlevel), lvl (p.lzout), lvl (p.slevel) };
        if (! gInit) { for (int q = 0; q < 7; ++q) gPrev[q] = cur[q]; gInit = true; }
        const bool panTrem = (cfg == 0 || cfg == 3) && p.dsel >= 2;
        const float lfoInc = 0.1f * std::pow (2.f, (float) p.ptspeed / 99.f * 7.f) / (float) sr, depth = lvl (p.ptdepth);
        for (int i = 0; i < n; ++i)
        {
            const size_t k = (size_t) i;
            const float ramp = (float) (i + 1) / (float) n;
            float gv[7]; for (int q = 0; q < 7; ++q) gv[q] = gPrev[q] + (cur[q] - gPrev[q]) * ramp;
            const float dl = gv[0], ms = gv[1], pl = gv[2], dyl = gv[3], rl = gv[4], zl = gv[5], sl = gv[6];
            float dirL = pre ? x.l[k] : eq.l[k], dirR = pre ? x.r[k] : eq.r[k];
            if (panTrem)
            {
                ptPh += lfoInc; if (ptPh >= 1.f) ptPh -= 1.f;
                const float s = std::sin (2.f * kPi * ptPh);
                if (p.dsel == 2) { const float a = (depth * s + 1.f) * kPi * 0.25f; dirL *= std::cos (a) * 1.41421356f; dirR *= std::sin (a) * 1.41421356f; }
                else { const float g = 1.f - depth * (0.5f + 0.5f * s); dirL *= g; dirR *= g; }
            }
            float oL, oR;
            if (cfg == 7) { oL = dl * dirL + sl * smpOut.l[k]; oR = dl * dirR + sl * smpOut.r[k]; }
            else
            {
                const bool usePit = (cfg == 0 || cfg == 3 || cfg == 5 || cfg == 6);
                oL = dl * dirL + ms * ((usePit ? pl * pit.l[k] : 0.f) + dyl * dout.l[k] + rl * rout.l[k] + (cfg == 1 ? zl * lz.l[k] : 0.f));
                oR = dl * dirR + ms * ((usePit ? pl * pit.r[k] : 0.f) + dyl * dout.r[k] + rl * rout.r[k] + (cfg == 1 ? zl * lz.r[k] : 0.f));
            }
            if (! std::isfinite (oL)) oL = 0.f;
            if (! std::isfinite (oR)) oR = 0.f;
            L[i] = clampf (oL, -8.f, 8.f); R[i] = clampf (oR, -8.f, 8.f);
        }
        for (int q = 0; q < 7; ++q) gPrev[q] = cur[q];
    }
    double sr = 44100; int cap = 512; float ptPh = 0.f, gPrev[7] = {}; bool gInit = false;
    EqBank eqb; Pitch pitch; DelayFx delay; Reverb reverb; Leslie les; MultiTap mtap; FreqShifter shift; Resonators res; Sampler smp;
    Stereo x, eq, pit, din, dout, rin, rout, lz, smpOut, up, dn;
};
} // namespace qv
