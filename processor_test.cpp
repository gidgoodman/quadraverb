#include "PluginProcessor.h"
#include <cstdio>
#include <random>
static QuadraProcessor* mk(){ auto* p=new QuadraProcessor(); p->setPlayConfigDetails(2,2,48000,512); p->prepareToPlay(48000,512); return p; }
static void setp(QuadraProcessor& p,const char* id,float v){ auto* q=p.apvts.getParameter(id); q->setValueNotifyingHost(q->convertTo0to1(v)); }
static float getp(QuadraProcessor& p,const char* id){ return p.apvts.getRawParameterValue(id)->load(); }
// run n blocks of input through the processor; returns the concatenated left output
static std::vector<float> run(QuadraProcessor& p,const std::vector<float>& in,juce::MidiBuffer* firstMidi=nullptr)
{ std::vector<float> out; for(size_t o=0;o<in.size();o+=512){ int n=(int)std::min<size_t>(512,in.size()-o); juce::AudioBuffer<float> b(2,512); b.clear();
    for(int i=0;i<n;i++){ b.setSample(0,i,in[o+i]); b.setSample(1,i,in[o+i]); } juce::MidiBuffer m; if(o==0&&firstMidi) m=*firstMidi; p.processBlock(b,m);
    for(int i=0;i<n;i++) out.push_back(b.getSample(0,i)); } return out; }
static double rms(const std::vector<float>& v,size_t a,size_t b){ double s=0; for(size_t i=a;i<b&&i<v.size();i++) s+=v[i]*v[i]; return std::sqrt(s/std::max<size_t>(1,b-a)); }
static std::vector<float> noise(size_t n,float amp,unsigned seed=1){ std::mt19937 g(seed); std::normal_distribution<float> d(0,amp); std::vector<float> v(n); for(auto& x:v) x=d(g); return v; }
int main()
{
  juce::ScopedJuceInitialiser_GUI init;
  int fails=0; auto check=[&](bool ok,const char* what){ printf("  [%s] %s\n",ok?"ok":"FAIL",what); if(!ok) ++fails; };
  // ---- A. every one of the 100 programs processes audio cleanly
  { std::unique_ptr<QuadraProcessor> p(mk()); bool allFinite=true; float worst=0; auto in=noise(48000,0.25f);
    for(int i=0;i<100;i++){ p->loadPreset(i); auto o=run(*p,in); for(float x:o){ if(!std::isfinite(x)) allFinite=false; worst=std::max(worst,std::fabs(x)); } }
    printf("A. all 100 programs, 1 s of noise each: peak output %.2f\n",worst); check(allFinite,"no NaN/inf in any program"); check(worst<8.5f,"output stays inside the clamp"); }
  // ---- B. sampling: BYPASS records, EQ plays, MIDI triggers
  { std::unique_ptr<QuadraProcessor> p(mk()); setp(*p,"config",7); setp(*p,"smode",1); setp(*p,"sauto",0); setp(*p,"slevel",99); setp(*p,"dlevel",0);
    printf("B. sampling\n"); check(p->sampleState()==0,"idle at start");
    p->requestRecord(); std::vector<float> in(48000*2,0.f); auto n=noise(48000/2,0.3f,7); for(size_t i=0;i<n.size();i++) in[4800+i]=n[i];
    juce::AudioBuffer<float> b(2,512); juce::MidiBuffer m; bool sawRec=false;
    for(size_t o=0;o<in.size();o+=512){ b.clear(); for(int i=0;i<512&&o+i<in.size();i++){ b.setSample(0,i,in[o+i]); b.setSample(1,i,in[o+i]); } p->processBlock(b,m); if(p->sampleState()==2) sawRec=true; }
    check(sawRec,"state goes to RECORDING after requestRecord"); check(p->sampleState()==0,"recording ends by itself after 1.55 s");
    p->requestPlay(); auto silence=std::vector<float>(48000,0.f); auto o=run(*p,silence);
    check(rms(o,4800+2400,4800+9600)>0.1,"EQ (requestPlay) plays back the recorded noise burst"); check(rms(o,40000,48000)<0.001,"one shot stops at the end");
    // MIDI one-shot trigger at the base note
    setp(*p,"smidi",2); juce::MidiBuffer mm; mm.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),0); auto o2=run(*p,silence,&mm);
    check(rms(o2,4800+2400,4800+9600)>0.1,"MIDI note 60 (base) triggers the sample"); }
  // ---- C. MOD: a controller moves a target parameter
  { std::unique_ptr<QuadraProcessor> p(mk()); p->loadPreset(0); setp(*p,"config",0); setp(*p,"dsel",0); setp(*p,"eqlevel",0); setp(*p,"master",99); setp(*p,"dlevel",0); setp(*p,"rlevel",0); setp(*p,"dylevel",0); setp(*p,"plevel",99); setp(*p,"pmode",0); setp(*p,"pdepth",0);
    // target the "PITCH MIX LEVEL" with controller 74 (source index 4+74), amplitude +99: level should follow the controller
    int tgt=-1; for(int i=0;i<kNumModTargets;i++) if(std::string(kModTargets[i].id)=="plevel") tgt=i;
    setp(*p,"plevel",0); setp(*p,"m1s",78); setp(*p,"m1t",(float)tgt); setp(*p,"m1a",99); setp(*p,"pmode",4); setp(*p,"pdet",0); setp(*p,"pin",0);
    auto in=noise(24000,0.3f); printf("C. MIDI modulation\n");
    auto lo=run(*p,in); juce::MidiBuffer cc; cc.addEvent(juce::MidiMessage::controllerEvent(1,74,127),0); auto hi=run(*p,in,&cc);
    // plevel 0 with the pitch block in detune mode: pit output is the shifted copy; controller 127 opens it fully
    printf("   level before CC 74: %.4f  after CC 74 = 127: %.4f\n",rms(lo,4800,24000),rms(hi,4800,24000)); check(rms(hi,4800,24000)>rms(lo,4800,24000)*3.f,"CC 74 raises the modulated level"); }
  // ---- D. multi tap delay through the processor
  { std::unique_ptr<QuadraProcessor> p(mk()); setp(*p,"config",3); setp(*p,"dtype",3); setp(*p,"din1",1); setp(*p,"dinmix",-99); setp(*p,"master",99); setp(*p,"dlevel",0); setp(*p,"plevel",0); setp(*p,"dylevel",99);
    setp(*p,"t1t",250); setp(*p,"t1v",60); setp(*p,"t2t",250); setp(*p,"t2v",60); setp(*p,"t2p",99);
    std::vector<float> imp(48000,0.f); imp[0]=1.f; auto o=run(*p,imp); int e1=-1,e2=-1; for(int i=100;i<48000;i++) if(std::fabs(o[i])>0.1f){ if(e1<0) e1=i; else if(i>e1+300){ e2=i; break; } }
    printf("D. multi tap: echoes at %.0f ms and %.0f ms\n",e1*1000.0/48000,e2<0?-1.0:e2*1000.0/48000); check(std::abs(e1-12000)<20,"tap 1 at 250 ms"); }
  // ---- E. user programs: name, store, save the project, restore it in a new instance
  { std::unique_ptr<QuadraProcessor> p(mk()); p->loadPreset(5); setp(*p,"rdecay",77); p->setCurrentName("MY SOUND"); p->storeProgram(95);
    p->loadPreset(0); check(p->slotName(0)!="MY SOUND","factory program 00 untouched"); p->loadPreset(95); check(p->currentName()=="MY SOUND","slot 95 recalled with its name"); check(std::lround(getp(*p,"rdecay"))==77,"slot 95 recalled its parameters");
    juce::MemoryBlock mb; p->getStateInformation(mb); std::unique_ptr<QuadraProcessor> q(mk()); q->setStateInformation(mb.getData(),(int)mb.getSize());
    printf("E. project save/restore\n"); check(q->slotName(95)=="MY SOUND","stored program survives a project reload"); q->recallFactory(95); check(q->slotName(95)!="MY SOUND","RECALL FACTORY restores the original");
    q->recallAllFactory(); }
  // ---- F. host controls drive a modulation slot (the Ableton route)
  { std::unique_ptr<QuadraProcessor> p(mk()); setp(*p,"config",0); setp(*p,"dsel",0); setp(*p,"eqlevel",0); setp(*p,"master",99); setp(*p,"dlevel",0); setp(*p,"rlevel",0); setp(*p,"dylevel",0); setp(*p,"pmode",4); setp(*p,"pdet",0); setp(*p,"pin",0); setp(*p,"plevel",0);
    int tgt=-1; for(int i=0;i<kNumModTargets;i++) if(std::string(kModTargets[i].id)=="plevel") tgt=i;
    setp(*p,"m1s",132); setp(*p,"m1t",(float)tgt); setp(*p,"m1a",99); auto in=noise(24000,0.3f); setp(*p,"hc1",0); auto a=run(*p,in); setp(*p,"hc1",127); auto b=run(*p,in);
    printf("F. host control 1: level %.4f at 0, %.4f at 127\n",rms(a,4800,24000),rms(b,4800,24000)); check(rms(b,4800,24000)>rms(a,4800,24000)*3.f,"HOST CONTROL 1 acts as a modulation source"); }
  printf("\n%s (%d failures)\n",fails?"SOME CHECKS FAILED":"ALL CHECKS PASSED",fails); return fails;
}
