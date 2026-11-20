#include "Engine.h"
#include <cstdio>
using namespace qv;
int main(){
  const double sr=48000;
  // 1) synced delay: 1/4 note at 120 BPM = 500 ms -> echo at 24000 samples
  { Engine e; e.prepare(sr,512); EP p; p.dsel=0; p.dlevel=0; p.master=99; p.plevel=0; p.rlevel=0; p.dylevel=99; p.dtype=2; p.dfl=0; p.dsync=7; p.bpm=120; p.config=0; p.din1=1; p.dinmix=-99;
    std::vector<float> L(48000,0.f),R(48000,0.f); L[0]=R[0]=1.f;
    for(int o=0;o<48000;o+=512){int n=std::min(512,48000-o); e.process(L.data()+o,R.data()+o,n,p);} 
    int pk=0; float mx=0; for(int i=100;i<48000;i++) if(std::fabs(L[i])>mx){mx=std::fabs(L[i]);pk=i;}
    printf("1/4 @120bpm: echo at %d samples (%.1f ms), expected 24000 (500 ms)\n",pk,pk*1000.0/sr);
    p.bpm=90; p.dsync=10; // 1/8 @ 90 = 333.3 ms
    Engine e2; e2.prepare(sr,512); std::vector<float> L2(48000,0.f),R2(48000,0.f); L2[0]=R2[0]=1.f;
    for(int o=0;o<48000;o+=512){int n=std::min(512,48000-o); e2.process(L2.data()+o,R2.data()+o,n,p);} 
    mx=0; for(int i=100;i<48000;i++) if(std::fabs(L2[i])>mx){mx=std::fabs(L2[i]);pk=i;}
    printf("1/8 @90bpm : echo at %d samples (%.1f ms), expected 16000 (333.3 ms)\n",pk,pk*1000.0/sr);
    // too long for the mono limit: 4 bars @ 60 bpm = 16 s -> halved until <= 800 ms
  }
  // 2) LFO lock: 1 bar (4 beats) at 120 BPM, host at ppq=2 beats -> phase 0.5 at block start
  { Pitch pt; pt.init(sr); EP p; p.pmode=3; p.psync=3; p.bpm=120; p.ppq=2.0; p.ppqValid=true; p.hostPlaying=true;
    std::vector<float> in(512,0.f),o1(512),o2(512); pt.process(in.data(),in.data(),o1.data(),o2.data(),512,p);
    float expect=0.5f+512*(float)(120/60.0/4.0/sr); printf("LFO phase after 512 samples: %.5f, expected %.5f\n",pt.lfo,expect);
    p.ppq=6.0; pt.process(in.data(),in.data(),o1.data(),o2.data(),512,p); printf("same bar position one bar later: %.5f\n",pt.lfo);
    p.hostPlaying=false; p.ppqValid=false; float before=pt.lfo; pt.process(in.data(),in.data(),o1.data(),o2.data(),512,p);
    printf("transport stopped: LFO free-runs from %.5f to %.5f (advance %.5f)\n",before,pt.lfo,pt.lfo-before);
  }
  return 0; }
