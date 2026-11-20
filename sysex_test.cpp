#include "PluginProcessor.h"
#include <cstdio>
using SX = QuadraVerbSysEx;
static std::vector<juce::uint8> bytes(const juce::MidiMessage& m){ return std::vector<juce::uint8>(m.getRawData(), m.getRawData()+m.getRawDataSize()); }
static int fails=0; static void check(bool ok,const char* w){ printf("  [%s] %s\n",ok?"ok":"FAIL",w); if(!ok)++fails; }
// everything that needs the message thread (timers) runs as steps of one dispatch loop
struct Script : juce::Timer {
  struct Step { int delayMs; std::function<void()> fn; }; std::vector<Step> steps; size_t i=0;
  void add(int d,std::function<void()> f){ steps.push_back({d,std::move(f)}); }
  void run(){ next(); juce::MessageManager::getInstance()->runDispatchLoop(); }
  void next(){ if(i>=steps.size()){ juce::MessageManager::getInstance()->stopDispatchLoop(); return; } startTimer(steps[i].delayMs>0?steps[i].delayMs:1); }
  void timerCallback() override { stopTimer(); auto f=steps[i++].fn; f(); next(); } };
static bool sameDump(const juce::Array<juce::MidiMessage>& a,const juce::Array<juce::MidiMessage>& b){ if(a.size()!=b.size()) return false; for(int i=0;i<a.size();i++) if(bytes(a[i])!=bytes(b[i])) return false; return true; }
int main(){
  juce::ScopedJuceInitialiser_GUI init; Script s;
  printf("1. 16-bit value in three 7-bit bytes\n");
  { bool rt=true,seven=true; for(int v=0;v<65536;v++){ juce::uint8 b[3]; SX::pack16((juce::uint16)v,b); if(b[0]>0x03||b[1]>0x7F||b[2]>0x7F) seven=false; if(SX::unpack16(b)!=v) rt=false; }
    check(rt,"all 65536 values survive pack -> unpack"); check(seven,"every byte stays below 0x80 (v1 only carries bits 15-14)");
    auto m=SX::makeMessage(0x05,0x12,0x1234); check(bytes(m)==std::vector<juce::uint8>({0xF0,0x00,0x00,0x0E,0x02,0x01,0x05,0x12,0x00,0x24,0x34,0xF7}),"0x1234 -> F0 00 00 0E 02 01 05 12 00 24 34 F7");
    check(bytes(SX::makeMessage(0x7F,0x7F,0xFFFF))==std::vector<juce::uint8>({0xF0,0,0,0x0E,2,1,0x7F,0x7F,0x03,0x7F,0x7F,0xF7}),"0xFFFF -> 03 7F 7F");
    auto m3=SX::makeMessage(0xFF,0x93,1); check(bytes(m3)[6]==0x7F && bytes(m3)[7]==0x13,"func/page are masked to 7 bits"); }
  printf("2. parsing\n");
  { SX::Packet p; check(SX::parseMessage(SX::makeMessage(3,9,40000),p)&&p.func==3&&p.page==9&&p.value==40000&&p.deviceId==1,"own message parses back to func/page/value");
    juce::uint8 w1[]={0x00,0x00,0x0F,2,1,1,1,0,0,0}; check(!SX::parseMessage(juce::MidiMessage::createSysExMessage(w1,10),p),"wrong manufacturer rejected");
    juce::uint8 w2[]={0,0,0x0E,2,1,1,1,0,0}; check(!SX::parseMessage(juce::MidiMessage::createSysExMessage(w2,9),p),"wrong length rejected");
    check(!SX::parseMessage(juce::MidiMessage::noteOn(1,60,(juce::uint8)100),p),"non-SysEx rejected"); }
  // ---- 3. throttle
  auto sx3=std::make_shared<SX>(); auto ts=std::make_shared<std::vector<double>>(); auto ord=std::make_shared<std::vector<int>>();
  s.add(0,[=]{ printf("3. 15 ms throttle (timestamps taken at the output)\n"); sx3->testSink=[=](const juce::MidiMessage& m){ ts->push_back(juce::Time::getMillisecondCounterHiRes()); ord->push_back(m.getRawData()[7]); };
    for(int i=0;i<8;i++) sx3->enqueueParam(1,(juce::uint8)i,(juce::uint16)(i*100)); check(sx3->queueSize()==8,"8 messages queued, none sent yet"); });
  s.add(300,[=]{ check(ts->size()==8,"all 8 delivered"); bool o=ord->size()==8; for(int i=0;o&&i<8;i++) o=(*ord)[(size_t)i]==i; check(o,"order preserved");
    double mn=1e9,mx=0; for(size_t i=1;i<ts->size();i++){ mn=std::min(mn,(*ts)[i]-(*ts)[i-1]); mx=std::max(mx,(*ts)[i]-(*ts)[i-1]); }
    printf("   gaps between messages: min %.1f ms, max %.1f ms (target 15)\n",mn,mx); check(mn>=12.0,"never faster than ~15 ms apart"); check(ts->back()-ts->front()>=100.0,"the 8 messages span >= 105 ms, not a burst");
    ts->clear(); std::vector<juce::uint8> big(600,0x11); sx3->enqueue(juce::MidiMessage::createSysExMessage(big.data(),(int)big.size())); sx3->enqueue(SX::makeMessage(1,1,1)); });
  s.add(500,[=]{ printf("   after a 602-byte message the next one follows %.0f ms later (wire time ~193 ms)\n",ts->size()>1?(*ts)[1]-(*ts)[0]:-1.0); check(ts->size()==2&&(*ts)[1]-(*ts)[0]>=170.0,"large dump is followed by a wire-time pause"); });
  // ---- 4. plugin MIDI output
  auto sx4=std::make_shared<SX>(); auto got4=std::make_shared<std::vector<int>>();
  s.add(0,[=]{ printf("4. plugin MIDI output (no hardware port)\n"); sx4->setUseHardwareOutput(false); for(int i=0;i<3;i++) sx4->enqueueParam(1,(juce::uint8)(i+1),(juce::uint16)(i*7)); });
  for(int k=0;k<8;k++) s.add(20,[=]{ juce::MidiBuffer b; sx4->drainOutputTo(b); for(const auto m:b) got4->push_back(m.getMessage().getRawData()[7]); });
  s.add(0,[=]{ check(*got4==std::vector<int>({1,2,3}),"3 messages came out of the plugin MIDI buffer, in order"); });
  // ---- 5. dumps
  auto a5=std::make_shared<SX>();
  s.add(0,[=]{ printf("5. dump capture and .syx import / export\n"); a5->setCapture(true); std::vector<juce::uint8> body(300); for(size_t i=0;i<body.size();i++) body[i]=(juce::uint8)(i&0x7F);
    a5->receive(SX::makeMessage(2,1,5)); a5->receive(juce::MidiMessage::createSysExMessage(body.data(),(int)body.size())); a5->receive(SX::makeMessage(2,2,9)); a5->receive(juce::MidiMessage::noteOn(1,60,(juce::uint8)90)); });
  s.add(60,[=]{ check(a5->dumpMessageCount()==3,"3 SysEx messages captured (the note-on ignored)"); check(a5->dumpByteCount()==12+302+12,"byte count is right");
    auto f=juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("qv_test.syx"); f.deleteFile(); check(a5->exportSyx(f),"exported"); check(f.getSize()==a5->dumpByteCount(),"file is the raw messages back to back");
    SX b; check(b.importSyx(f)==3,"imported 3 messages"); check(sameDump(a5->getDump(),b.getDump()),"imported dump equals the exported one byte for byte");
    std::vector<juce::uint8> raw={0x01,0x02,0xF0,0x00,0x00,0x0E,0x02,0x01,0x01,0x01,0x00,0x00,0x05,0xF7,0xAA,0xF0,0x10,0x20,0xF0,0x00,0x00,0x0E,0x02,0x01,0x01,0x02,0x00,0x00,0x06,0xF7};
    check(SX::splitSyx(raw.data(),raw.size()).size()==2,"splitSyx skips junk and the truncated message, keeps the 2 complete ones");
    auto g=juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("qv_empty.syx"); g.replaceWithText("not a syx file"); check(b.importSyx(g)==-1,"a file without SysEx is rejected"); f.deleteFile(); g.deleteFile(); });
  // ---- 6. incoming parameter packets
  auto sx6=std::make_shared<SX>(); auto pk6=std::make_shared<SX::Packet>(); auto n6=std::make_shared<int>(0);
  s.add(0,[=]{ printf("6. incoming packets\n"); sx6->onPacket=[=](const SX::Packet& p){ *pk6=p; ++*n6; }; sx6->receive(SX::makeMessage(1,0x21,12345)); });
  s.add(60,[=]{ check(*n6==1&&pk6->func==1&&pk6->page==0x21&&pk6->value==12345,"packet decoded and delivered"); check(sx6->lastRx().startsWith("f0 00 00 0e 02 01"),"last received message is shown as hex"); });
  // ---- 7. state
  auto a7=std::make_shared<SX>();
  s.add(0,[=]{ printf("7. state (ValueTree)\n"); a7->setCapture(true); a7->receive(SX::makeMessage(2,1,5)); a7->receive(SX::makeMessage(2,2,6)); });
  s.add(60,[=]{ a7->setUsePluginOutput(false); a7->setMirrorEnabled(true); juce::ValueTree t("STATE"); a7->saveState(t); SX b; b.loadState(t);
    check(sameDump(a7->getDump(),b.getDump())&&b.getDump().size()==2,"raw dump restored"); check(!b.usesPluginOutput()&&b.mirrorEnabled()&&b.usesHardwareOutput(),"settings restored"); });
  // ---- 8. through the real processor
  auto p8=std::make_shared<QuadraProcessor>(); auto buf8=std::make_shared<juce::AudioBuffer<float>>(2,512);
  s.add(0,[=]{ printf("8. processor: plugin MIDI out, host SysEx in, project state\n"); p8->setPlayConfigDetails(2,2,48000,512); p8->prepareToPlay(48000,512); p8->sysex.setUseHardwareOutput(false); p8->sysex.enqueueParam(1,0x30,777); });
  s.add(60,[=]{ buf8->clear(); juce::MidiBuffer midi; p8->processBlock(*buf8,midi); bool found=false; for(const auto m:midi){ SX::Packet pk; if(SX::parseMessage(m.getMessage(),pk)&&pk.page==0x30&&pk.value==777) found=true; }
    check(found,"queued SysEx appears in the MIDI buffer returned by processBlock"); check(p8->producesMidi(),"plugin declares MIDI output");
    p8->sysex.setCapture(true); juce::MidiBuffer in; in.addEvent(SX::makeMessage(2,5,99),0); buf8->clear(); p8->processBlock(*buf8,in); });
  s.add(60,[=]{ check(p8->sysex.dumpMessageCount()==1,"SysEx arriving from the host is captured");
    juce::MemoryBlock mb; p8->getStateInformation(mb); QuadraProcessor q; q.setPlayConfigDetails(2,2,48000,512); q.prepareToPlay(48000,512); q.setStateInformation(mb.getData(),(int)mb.getSize());
    check(q.sysex.dumpMessageCount()==1,"raw dump restored by setStateInformation"); auto d=q.sysex.getDump(); check(d.size()==1&&bytes(d[0])==bytes(SX::makeMessage(2,5,99)),"and it is the same bytes");
    p8->apvts.getParameter("bypass")->setValueNotifyingHost(1.f); p8->sysex.enqueueParam(1,0x31,5); });
  s.add(60,[=]{ juce::MidiBuffer m2; buf8->clear(); p8->processBlock(*buf8,m2); bool f2=false; for(const auto m:m2){ SX::Packet pk; if(SX::parseMessage(m.getMessage(),pk)&&pk.page==0x31) f2=true; } check(f2,"SysEx still leaves while the plugin is bypassed"); });
  s.run();
  printf("\n%s (%d failures)\n",fails?"SOME CHECKS FAILED":"ALL CHECKS PASSED",fails); return fails; }
