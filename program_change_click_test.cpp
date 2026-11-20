#define qv oldqv
#include "old_engine_for_click_test.h"
#undef qv
#include "Source/Engine.h"
#include <cstdio>
#include <random>
static const double SR=48000;
template<class P> void setup(P& p,int which){
  p.config=0; p.master=which?40:60; p.dlevel=which?70:40; p.plevel=99; p.dylevel=99; p.rlevel=99; p.dsel=0; p.pdepth=which?85:55; p.pspeed=which?15:60; p.pin=1; p.pfb=which?50:20;
  p.pmode=which?2:0; p.dtype=which?0:2; p.dtl=which?120:380; p.dfl=which?20:40; p.dinmix=0; p.rin1=0; p.rinmix=0; p.rtype=which?2:0; p.rdecay=which?30:70; p.rhf=which?-15:-45;
  p.rgate=which?true:false; p.rghold=120; p.rgrel=40; p.ela=which?3.f:0.f; p.eha=which?4.f:-2.f; p.rpre=which?0:60; p.rpremix=60; }
static std::vector<float> hpf(const std::vector<float>& x){ std::vector<float> y(x.size()); double z1=0,z2=0; const double w=2*M_PI*4000/SR,cs=std::cos(w),al=std::sin(w)/(2*0.707); const double b0=(1+cs)/2,b1=-(1+cs),b2=(1+cs)/2,a0=1+al,a1=-2*cs,a2=1-al; for(size_t i=0;i<x.size();i++){ double in=x[i],o=(b0*in+z1)/a0; z1=b1*in-a1*o+z2; z2=b2*in-a2*o; y[i]=(float)o; } return y; }
static std::vector<double> env(const std::vector<float>& y){ int hop=(int)(0.005*SR); std::vector<double> e; for(size_t i=0;i+hop<=y.size();i+=hop){ double s=0; for(int k=0;k<hop;k++) s+=y[i+k]*y[i+k]; e.push_back(std::sqrt(s/hop)); } return e; }
static std::vector<float> input(int n){ std::mt19937 g(5); std::uniform_real_distribution<float> u(-1,1); std::vector<float> x(n); for(int i=0;i<n;i++){ double t=i/SR; x[i]=(float)(0.25*std::sin(2*M_PI*220*t)+0.15*std::sin(2*M_PI*330*t)+0.10*std::sin(2*M_PI*880*t))+0.01f*u(g); } return x; }
// mode 0: old engine, abrupt; 1: new engine, abrupt; 2: new engine + fade / swap / clear / fade up (what the processor now does)
static std::vector<float> render(int mode,std::vector<int>& marks){
  const int B=512, total=SR*4; auto in=input(total); std::vector<float> out(total);
  oldqv::Engine eo; qv::Engine en; if(mode==0) eo.prepare(SR,B); else en.prepare(SR,B);
  oldqv::EP po; qv::EP pn; setup(po,0); setup(pn,0); int cur=0; float mg=1.f; const float dn=1.f/(0.005f*SR), up=1.f/(0.020f*SR); int fadeState=0, wait=0;
  for(int o=0;o<total;o+=B){ int n=std::min(B,total-o);
    int sw=(o>(int)(0.5*SR) && (o-(int)(0.5*SR))%((int)(0.4*SR)/B*B)<B && o<(int)(3.5*SR));   // a switch every ~0.4 s
    if(sw && (marks.empty()||o-marks.back()>(int)(0.2*SR))){ marks.push_back(o); if(mode<2){ cur^=1; setup(po,cur); setup(pn,cur); } else { fadeState=1; wait=2; } }
    std::vector<float> L(in.begin()+o,in.begin()+o+n), R=L;
    if(mode==0) eo.process(L.data(),R.data(),n,po); else en.process(L.data(),R.data(),n,pn);
    if(mode==2){ for(int i=0;i<n;i++){ mg = (fadeState==1)? std::max(0.f,mg-dn) : std::min(1.f,mg+up); L[i]*=mg; }
      if(fadeState==1 && --wait<=0){ cur^=1; setup(pn,cur); en.clear(); mg=0.f; fadeState=0; } }
    for(int i=0;i<n;i++) out[(size_t)(o+i)]=L[(size_t)i]; }
  return out; }
int main(){ const char* names[3]={"old engine, parameters jump (the version in your video)","new engine smoothing, parameters jump","new engine + fade / swap / clear / fade up (now)"}; double worst[3];
  printf("two realistic programs alternating on a steady input; HF burst energy in the 80 ms after each switch (x the normal HF level)\n");
  for(int m=0;m<3;m++){ std::vector<int> marks; auto out=render(m,marks); auto e=env(hpf(out)); auto s=e; std::sort(s.begin(),s.end()); double med=s[s.size()/2]+1e-9; double w=0; printf("  %-52s",names[m]);
    for(int mk:marks){ int a=(int)(mk/(0.005*SR)), b=a+20; double mx=0; for(int i=a;i<b&&i<(int)e.size();i++) mx=std::max(mx,e[(size_t)i]/med); printf(" %5.1f",mx); w=std::max(w,mx); } worst[m]=w; printf("   worst %.1f\n",w); }
  printf("\n  worst burst: %.1fx  ->  %.1fx  ->  %.1fx\n",worst[0],worst[1],worst[2]); return 0; }
