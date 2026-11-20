#include "Engine.h"
#include <cstdio>
#include <random>
using namespace qv;
static double rms(const std::vector<float>& v,int a,int b){double s=0;for(int i=a;i<b;i++)s+=v[i]*v[i];return std::sqrt(s/(b-a));}
int main(){
  const double sr=48000; const int N=48000*4, B=512;
  std::mt19937 rng(1); std::normal_distribution<float> nd(0.f,0.1f);
  std::vector<float> inL(N),inR(N);
  for(int i=0;i<N;i++){ float burst=(i<48000)?1.f:0.f; inL[i]=burst*nd(rng); inR[i]=burst*nd(rng);} 
  auto run=[&](const char* name,EP p){
    Engine e; e.prepare(sr,B); std::vector<float> L=inL,R=inR; bool bad=false;
    for(int o=0;o<N;o+=B){ int n=std::min(B,N-o); e.process(L.data()+o,R.data()+o,n,p);} 
    for(int i=0;i<N;i++) if(!std::isfinite(L[i])||!std::isfinite(R[i])||std::fabs(L[i])>7.9f) bad=true;
    printf("%-34s burst rms %.3f  tail(1.5-2s) %.4f  tail(3-4s) %.5f %s\n",name,rms(L,0,48000),rms(L,72000,96000),rms(L,144000,192000),bad?"**BAD**":"ok");
  };
  EP base; base.master=99; base.dlevel=0; base.rlevel=99; base.plevel=99; base.dylevel=99;
  { EP p=base; p.rtype=1; p.rdecay=40; run("reverb PLATE decay40",p);} 
  for(int t=0;t<5;t++){ EP p=base; p.rtype=t; p.rdecay=60; char n[40]; sprintf(n,"reverb type %d decay60",t); run(n,p);} 
  { EP p=base; p.rdecay=99; run("reverb decay 99 (long)",p);} 
  { EP p=base; p.rgate=true; p.rghold=100; p.rgrel=40; p.rdecay=70; run("reverb gated",p);} 
  { EP p=base; p.rpre=120; p.rpremix=99; run("reverb predelay 120",p);} 
  for(int m=0;m<6;m++){ EP p=base; p.rlevel=0; p.pmode=m; p.pdepth=60; p.pfb=50; char n[40]; sprintf(n,"pitch mode %d",m); run(n,p);} 
  { EP p=base; p.rlevel=0; p.pmode=3; p.ptrig=true; run("trigger flange",p);} 
  for(int d=0;d<3;d++){ EP p=base; p.rlevel=0; p.plevel=0; p.dtype=d; p.dfl=60; p.dfr=60; p.dtl=300; p.dtr=210; char n[40]; sprintf(n,"delay type %d fb60",d); run(n,p);} 
  { EP p=base; p.config=1; p.rin1=4; p.lzspeed=2; p.lzout=99; run("lezlie fast",p);} 
  { EP p=base; p.config=2; p.dsel=1; p.eqlevel=99; for(int i=0;i<11;i++)p.g[i]=(i%2?6:-6); p.dtl=1400; p.dtype=2; p.dfl=50; run("graphic eq>delay 1400ms",p);} 
  { EP p=base; p.config=3; p.dsel=1; p.eqlevel=99; p.ela=8; p.elma=-6; p.ehma=6; p.pmode=0; run("5band>pch>dly",p);} 
  { EP p=base; p.config=4; p.pdepth=50; p.ela=6; p.eha=-6; run("eq>reverb + chorus",p);} 
  { EP p=base; p.config=0; p.pmode=3; p.dtype=0; p.dfl=70; p.rdecay=80; p.rin1=0; p.rinmix=0; run("everything on, ping-pong",p);} 
  return 0;}
