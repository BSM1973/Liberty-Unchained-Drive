#include "../Source/Plugin.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <iostream>
int main(int argc,char** argv){
 juce::ScopedJuceInitialiser_GUI init;auto p=std::make_unique<DriveProcessor>();p->setRateAndBufferSizeDetails(48000,128);p->prepareToPlay(48000,128);
 if(argc>1&&juce::String(argv[1])=="--ui"){if(!p->renderQcPages(juce::File::getCurrentWorkingDirectory().getChildFile("ui-qc")))return 20;std::cout<<"16 rendered FR/EN UI pages and layout bounds checks passed\n";return 0;}
 if(DriveProcessor::presets().size()!=100)return 1;juce::MidiBuffer midi;juce::AudioBuffer<float> audio(2,257);
 juce::StringArray names;for(int index=0;index<100;++index){auto& preset=DriveProcessor::presets()[size_t(index)];if(names.contains(preset.name))return 2;names.add(preset.name);p->loadPreset(index);p->set("os",0);p->prepareToPlay(48000,128);double energy=0;
  for(int b=0;b<16;++b){for(int i=0;i<257;++i)for(int c=0;c<2;++c)audio.setSample(c,i,.2f*std::sin(float(b*257+i)*.057f+c*.2f));p->processBlock(audio,midi);for(int c=0;c<2;++c)for(int i=0;i<257;++i){float y=audio.getSample(c,i);if(!std::isfinite(y))return 3;energy+=y*y;}}if(energy<1.e-6||p->selectedPreset.load()!=index)return 4;
 }
 p->learn("drive");midi.addEvent(juce::MidiMessage::controllerEvent(3,74,127),0);audio.clear();p->processBlock(audio,midi);p->flushMidiChanges();if(p->mapping("drive")!=2*128+74||std::abs(p->val("drive")-100.f)>.1f)return 5;
 juce::MemoryBlock state;p->language.store(1);p->getStateInformation(state);p->set("drive",0);p->unlearn("drive");p->language.store(0);p->setStateInformation(state.getData(),int(state.getSize()));if(p->mapping("drive")!=2*128+74||p->language.load()!=1||std::abs(p->val("drive")-100.f)>.1f)return 6;
 p->initPreset();p->set("bypass",1);p->prepareToPlay(48000,128);int latency=p->getLatencySamples(),peakIndex=-1;float peak=0;for(int b=0;b<8;++b){audio.clear();if(b==0)audio.setSample(0,0,1.f);p->processBlock(audio,midi);for(int i=0;i<257;++i){float x=std::abs(audio.getSample(0,i));if(x>peak){peak=x;peakIndex=b*257+i;}}}if(peakIndex!=latency||std::abs(peak-1.f)>1.e-5f)return 7;
 p->initPreset();p->set("mix",0);p->set("delta",1);p->set("os",2);p->prepareToPlay(48000,128);double residual=0,reference=0;for(int b=0;b<120;++b){for(int i=0;i<257;++i)for(int c=0;c<2;++c)audio.setSample(c,i,.2f*std::sin(float(b*257+i)*.057f));p->processBlock(audio,midi);if(b>100)for(int i=0;i<257;++i){double x=audio.getSample(0,i);residual+=x*x;reference+=.02;}}std::cout<<"Delta dry residual dB="<<10*std::log10(std::max(1.e-15,residual)/reference)<<"\n";if(!std::isfinite(residual)||residual/reference>.001)return 8;
 p->initPreset();p->set("os",1);p->prepareToPlay(48000,128);
 // Simulate normalized host parameter writes, not actual external DAW automation.
 for(int b=0;b<64;++b){for(const auto* id:{"drive","mix","bias","tone","output"}){auto* q=p->ap.getParameter(id);if(!q)return 9;float n=float(b%32)/31.f;q->setValueNotifyingHost(n);if(std::abs(q->getValue()-n)>.015f)return 10;}
  for(int c=0;c<2;++c)for(int i=0;i<257;++i)audio.setSample(c,i,.2f*std::sin(float(b*257+i)*.043f+c*.1f));p->processBlock(audio,midi);for(int c=0;c<2;++c)for(int i=0;i<257;++i)if(!std::isfinite(audio.getSample(c,i)))return 11;}
 std::cout<<"Normalized parameter automation simulation passed\n";
 std::cout<<"100 preset numerical checks, MIDI CC and state recall, oversized buffers, bypass impulse and Delta passed\n";
}
