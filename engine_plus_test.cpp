#include "Engine.h"
#include <cstdio>
#include <complex>
#include <random>
using namespace qv;
static const double SR=48000;
typedef std::vector<float> V;
static void fft(std::vector<std::complex<double>>& a){int n=a.size();for(int i=1,j=0;i<n;i++){int bit=n>>1;for(;j&bit;bit>>=1)j^=bit;j^=bit;if(i<j)std::swap(a[i],a[j]);}
 for(int len=2;len<=n;len<<=1){double ang=-2*M_PI/len;std::complex<double> wl(cos(ang),sin(ang));for(int i=0;i<n;i+=len){std::complex<double> w(1);for(int j=0;j<len/2;j++){auto u=a[i+j],v=a[i+j+len/2]*w;a[i+j]=u+v;a[i+j+len/2]=u-v;w*=wl;}}}}
// strongest spectral peaks (Hz) of a signal segment
static std::vector<double> peaks(const V& x,int start,int N,int count){std::vector<std::complex<double>> a(N);for(int i=0;i<N;i++){double w=0.5-0.5*cos(2*M_PI*i/(N-1));a[i]=x[start+i]*w;}fft(a);
 std::vector<double> mag(N/2);for(int i=0;i<N/2;i++)mag[i]=std::abs(a[i]);std::vector<double> out;
 for(int c=0;c<count;c++){int bi=(int)(40*N/SR);for(int i=2;i<N/2-1;i++)if(mag[i]>mag[bi]&&i*SR/N>35&&i*SR/N<3000)bi=i; double f=bi*SR/N; out.push_back(f); for(int i=std::max(1,bi-12);i<std::min(N/2,bi+13);i++)mag[i]=0;} return out;}
static double fundamental(const V& x,int start,int N){std::vector<std::complex<double>> a(N);for(int i=0;i<N;i++){double w=0.5-0.5*cos(2*M_PI*i/(N-1));a[i]=x[start+i]*w;}fft(a);
 std::vector<double> m(N/2);double mx=0;for(int i=0;i<N/2;i++){m[i]=std::abs(a[i]);if(i*SR/N>40)mx=std::max(mx,m[i]);}
 for(int i=(int)(40*N/SR)+1;i<N/2-1;i++)if(m[i]>0.45*mx&&m[i]>=m[i-1]&&m[i]>=m[i+1])return i*SR/N;return 0;}
static double rms(const V& v,int a,int b){double s=0;for(int i=a;i<b;i++)s+=v[i]*v[i];return std::sqrt(s/std::max(1,b-a));}
static void process(Engine& e,V& L,V& R,const EP& p,int B=512){for(int o=0;o<(int)L.size();o+=B){int n=std::min(B,(int)L.size()-o);e.process(L.data()+o,R.data()+o,n,p);}}
static EP fx(){EP p;p.master=99;p.dlevel=0;p.plevel=99;p.dylevel=0;p.rlevel=0;p.dsel=0;return p;}
int main(){
  // ---- 1. ring modulator: 500 Hz shifted by 200 Hz (manual figure 1): up 700, down 300
  for(int mode: {+99,-99}){ Engine e;e.prepare(SR,512);EP p=fx();p.config=5;p.rmshift=200;p.rmmix=mode;
    int N=48000;V L(N),R(N);for(int i=0;i<N;i++)L[i]=R[i]=0.5f*std::sin(2*M_PI*500*i/SR);process(e,L,R,p);
    auto pk=peaks(L,8192,16384,2); {std::vector<std::complex<double>> a(16384);for(int i=0;i<16384;i++){double w=0.5-0.5*cos(2*M_PI*i/16383);a[i]=L[8192+i]*w;}fft(a);double m7=std::abs(a[(int)(700*16384/SR+0.5)]),m3=std::abs(a[(int)(300*16384/SR+0.5)]);
  printf("ring mod, shift 200Hz, mix %+d: strongest %.0f Hz; 700 Hz vs 300 Hz = %+.1f dB  (expect %s wins)\n",mode,pk[0],20*log10(m7/m3),mode>0?"700 (up)":"300 (down)");}}
  // ---- 2. resonators: one voice at a time (MIDI gated, one key held) so the fundamental is unambiguous
  { struct C{int note,tune;double expect;} cs[]={{60,0,261.6},{69,0,440.0},{60,12,523.3},{60,7,392.0},{48,-12,65.4},{60,-12,130.8}};
    for(auto c:cs){Engine e;e.prepare(SR,512);EP p=fx();p.config=6;p.resdecay=70;p.resgate=1;p.res[0]=c.tune;
      int N=96000;V L(N),R(N);std::mt19937 g(3);std::normal_distribution<float> nd(0,0.2f);for(int i=0;i<30000;i++)L[i]=R[i]=nd(g);
      for(int o=0;o<N;o+=512){int n=std::min(512,N-o);EP q=p;if(o==0){q.numNotes=1;q.notes[0]={1,c.note,100};}e.process(L.data()+o,R.data()+o,n,q);} 
      printf("resonator: key %d, tune %+d semitones -> %.1f Hz (expect %.1f)\n",c.note,c.tune,fundamental(L,12000,32768),c.expect);}
    // MIDI gated: after the key is released the resonator decays away even with input active
    Engine e;e.prepare(SR,512);EP p=fx();p.config=6;p.resdecay=60;p.resgate=1;int N=96000;V L(N),R(N);std::mt19937 g(5);std::normal_distribution<float> nd(0,0.2f);for(int i=0;i<N;i++)L[i]=R[i]=nd(g);
    for(int o=0;o<N;o+=512){int n=std::min(512,N-o);EP q=p;if(o==0){q.numNotes=1;q.notes[0]={1,60,100};}if(o==512*90){q.numNotes=1;q.notes[0]={0,60,0};}e.process(L.data()+o,R.data()+o,n,q);} 
    printf("resonator MIDI gate: level while key held %.3f, 1 s after release %.4f (input noise continues)\n",rms(L,40000,46000),rms(L,N-6000,N));}
  // ---- 3. multi tap delay: relative times 250,250,500 ms with pans; impulse response
  { Engine e;e.prepare(SR,512);EP p=fx();p.config=3;p.dtype=3;p.dinmix=-99;p.din1=1;p.dylevel=99;p.plevel=0;
    p.tapt[0]=250;p.tapt[1]=250;p.tapt[2]=500;p.tapt[3]=125;for(int k=0;k<4;k++){p.tapv[k]=60;p.tapp[k]=(k==0?-99:(k==1?99:0));}
    int N=96000;V L(N,0.f),R(N,0.f);L[0]=R[0]=1.f;process(e,L,R,p);
    printf("multi tap echoes (time ms / L / R):");for(int i=100;i<N;i++){if(std::fabs(L[i])>0.05f||std::fabs(R[i])>0.05f){printf(" [%.0f %.2f %.2f]",i*1000.0/SR,L[i],R[i]);i+=200;}}printf("\n   expect taps at 250 (hard L), 500 (hard R), 1000, 1125 ms\n");
    // cumulative limit and feedback stability
    EP q=p;q.tapf[0]=99;q.tapf[1]=99;q.mtmaster=99;Engine e2;e2.prepare(SR,512);V L2(N*2,0.f),R2(N*2,0.f);L2[0]=R2[0]=1.f;process(e2,L2,R2,q);
    printf("multi tap max feedback: peak %.2f after 4 s (stable if < ~8)\n",*std::max_element(L2.begin()+N,L2.end()));}
  // ---- 4. sampler
  { Engine e;e.prepare(SR,512);EP p=fx();p.config=7;p.dlevel=0;p.slevel=99;p.smode=1;p.sauto=false;
    int N=SR*4;V L(N,0.f),R(N,0.f);std::mt19937 g(5);std::normal_distribution<float> nd(0,0.3f);
    // record (front panel): press BYPASS at t=0, feed a 0.3 s noise burst starting at 0.2 s
    V src(N,0.f);for(int i=SR*0.2;i<SR*0.5;i++)src[i]=nd(g);
    L=src;R=src; EP q=p;q.recCmd=1; Engine* ep=&e; 
    for(int o=0;o<N;o+=512){int n=std::min(512,N-o);EP u=p;if(o==0)u.recCmd=1;if(o==96256)u.playCmd=1; 
        // after recording ended (1.55 s) trigger playback at ~2.0s with silent input
        if(o>=SR*1.7){for(int i=0;i<n;i++){L[o+i]=R[o+i]=0.f;}}
        ep->process(L.data()+o,R.data()+o,n,u);} 
    double played=rms(L,96256+(int)(SR*0.25),96256+(int)(SR*0.45)), before=rms(L,SR*1.7,SR*1.95);
    printf("sampler: state after record %d, playback rms %.3f over the burst (source rms 0.300), silence before %.4f\n",ep->sampState,played,before);
    // the played material should match the recorded input shifted: correlation with src[0..]
    double c=0,a2=0,b2=0;for(int i=0;i<SR*0.5;i++){double s=src[i],pl=L[96256+i];c+=s*pl;a2+=s*s;b2+=pl*pl;} printf("   correlation of playback with recorded input: %.3f (1.0 = identical)\n",c/std::sqrt(a2*b2+1e-12));
  }
  { // audio trigger recording: wait for threshold, then record; MIDI trigger with pitch fold
    Engine e;e.prepare(SR,512);EP p=fx();p.config=7;p.slevel=99;p.dlevel=0;p.sauto=true;int N=SR*3;V L(N,0.f),R(N,0.f);
    for(int i=SR*0.5;i<SR*0.6;i++)L[i]=R[i]=0.5f*std::sin(2*M_PI*1000*i/SR);
    for(int o=0;o<N;o+=512){int n=std::min(512,N-o);EP u=p;if(o==0)u.recCmd=1;e.process(L.data()+o,R.data()+o,n,u);if(o==0)printf("sampler armed (waiting for audio): state=%d\n",e.sampState);}
    printf("   after audio arrived and 1.55 s elapsed: state=%d (0 = finished)\n",e.sampState);
    // MIDI: note 84 with base 60 should fold to +12 semitones = 2000 Hz
    EP q=p;q.sauto=false;q.smidi=2;q.sbase=60;q.numNotes=1;q.notes[0]={1,84,100};V L3(SR,0.f),R3(SR,0.f);
    for(int o=0;o<(int)L3.size();o+=512){int n=std::min(512,(int)L3.size()-o);EP u=q;if(o>0)u.numNotes=0;e.process(L3.data()+o,R3.data()+o,n,u);} 
    auto pk=peaks(L3,0,16384,1);printf("   MIDI note 84, base 60 -> playback pitch %.0f Hz (1 kHz sample folded up one octave: expect ~2000)\n",pk[0]);
  }
  // ---- 5. panning and tremolo on the EQ output
  { for(int mode: {2,3}){Engine e;e.prepare(SR,512);EP p=fx();p.config=0;p.dsel=mode;p.eqlevel=99;p.ptdepth=99;p.ptspeed=40;p.master=0;p.rlevel=0;
      int N=SR*3;V L(N),R(N);for(int i=0;i<N;i++)L[i]=R[i]=0.5f*std::sin(2*M_PI*220*i/SR);process(e,L,R,p);
      double mnL=9,mxL=0,mnR=9,mxR=0;for(int i=0;i<N;i+=480){double a=rms(L,i,std::min(N,i+480)),b=rms(R,i,std::min(N,i+480));mnL=std::min(mnL,a);mxL=std::max(mxL,a);mnR=std::min(mnR,b);mxR=std::max(mxR,b);}
      printf("%s depth 99: L level %.3f..%.3f, R level %.3f..%.3f\n",mode==2?"auto panning":"tremolo     ",mnL,mxL,mnR,mxR);} }
  // ---- 6. stability sweep over every configuration, with extreme settings
  { std::mt19937 g(9);int bad=0,runs=0,clamped=0;for(int cfg=0;cfg<8;cfg++)for(int t=0;t<6;t++){Engine e;e.prepare(SR,512);EP p;p.config=cfg;std::uniform_int_distribution<int> d99(0,99);
      p.master=99;p.plevel=99;p.dylevel=99;p.rlevel=99;p.dlevel=99;p.lzout=99;p.slevel=99;p.dtype=t%4;p.dfl=p.dfr=99;p.dtl=p.dtr=d99(g)*8+1;p.rdecay=99;p.rtype=t%5;p.pmode=t%6;p.pdepth=99;p.pfb=99;
      p.rmshift=1+d99(g)*3;p.rmmix=d99(g)*2-99;p.resdecay=99;for(int j=0;j<5;j++)p.res[j]=d99(g)%60-24;for(int k=0;k<8;k++){p.tapf[k]=99;p.tapv[k]=99;p.tapt[k]=d99(g)*3;}p.mtmaster=99;p.dsel=t%4;p.smode=t%3;p.sauto=(t%2);
      for(int k=0;k<11;k++)p.g[k]=14.f*(k%2?1:-1);p.ela=14;p.eha=14;p.ema=14;
      int N=SR*2;V L(N),R(N);std::uniform_real_distribution<float> u(-1,1);for(int i=0;i<N;i++){L[i]=0.6f*u(g);R[i]=0.6f*u(g);}
      for(int o=0;o<N;o+=512){int n=std::min(512,N-o);EP q=p;if(o==0){q.recCmd=1;q.playCmd=1;q.numNotes=1;q.notes[0]={1,60,100};}e.process(L.data()+o,R.data()+o,n,q);} 
      bool b=false;for(int i=0;i<N;i++){if(!std::isfinite(L[i])||!std::isfinite(R[i])){bad++;break;}if(std::fabs(L[i])>7.9f)b=true;}runs++;if(b)clamped++; }
    printf("stability sweep: %d runs over all 8 configurations at extreme settings: %d produced NaN/inf, %d hit the +/-8 output clamp (everything at maximum with 99%% feedback)\n",runs,bad,clamped);}
  return 0;}
