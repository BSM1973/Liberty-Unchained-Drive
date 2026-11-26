#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Core.h"
#include "Mastering.h"
#include <atomic>
#include <vector>
#include <memory>
class DriveProcessor final : public juce::AudioProcessor, private juce::Timer {
public:
 using AP=juce::AudioProcessorValueTreeState;
 struct Preset {juce::String name,category; int engine;float drive,color,bias,mix,tone,dynamics;};
 DriveProcessor(); ~DriveProcessor() override {stopTimer();}
 juce::UndoManager undo;
 static AP::ParameterLayout layout(); AP ap;
 static const juce::StringArray& engines(); static const juce::StringArray& categories(); static const std::vector<Preset>& presets();
 void prepareToPlay(double,int) override; void releaseResources() override{};
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 juce::AudioProcessorEditor* createEditor() override; bool hasEditor() const override{return true;}
 const juce::String getName() const override{return "Unchained Drive";}
 bool acceptsMidi() const override{return true;} bool producesMidi() const override{return false;} bool isMidiEffect() const override{return false;} double getTailLengthSeconds()const override{return 0;}
 int getNumPrograms()override{return 1;} int getCurrentProgram()override{return 0;} void setCurrentProgram(int)override{} const juce::String getProgramName(int)override{return{};} void changeProgramName(int,const juce::String&)override{}
 void getStateInformation(juce::MemoryBlock&)override;void setStateInformation(const void*,int)override;
 float val(const juce::String& id)const; ud::Params circuitParams() const { return snapshot(); } void set(const juce::String&,float); void loadPreset(int); void initPreset();
 void learn(const juce::String&); void unlearn(const juce::String&); int mapping(const juce::String&)const;
 std::atomic<int> language{0},selectedPreset{-1},learnTarget{-1};
 std::array<std::atomic<float>,4096> wave{},inputWave{}; std::atomic<unsigned> wavePos{0};std::array<std::atomic<float>,2> peak{},rms{},inputPeak{},inputRms{};
 std::atomic<float> lufsM{-120.f},lufsS{-120.f},truePeak{0.f};
 std::atomic<float> cpuAverage{0.f},cpuPeak{0.f};std::atomic<int> activeQuality{1};
 std::array<juce::ValueTree,2> compare; int compareIndex=0;
 void switchCompare(int);void copyCompare();
private:
 struct Mapping{juce::RangedAudioParameter* param=nullptr; std::atomic<int> cc{-1};std::atomic<float> pending{-1};};
 std::array<Mapping,256> map{};int mapCount=0;void timerCallback()override;
 enum ParamIndex { kamt1, kamt2, kamt3, kamt4, kamt5, kattack, kattack2, kautoGain, kbc1, kbc2, kbc3, kbd1, kbd2, kbd3, kbe1, kbe2, kbe3, kbi1, kbi2, kbi3, kbias, kblend, kbm1, kbm2, kbm3, kbo1, kbo2, kbo3, kbody, kbs1, kbs2, kbs3, kbt1, kbt2, kbt3, kbu1, kbu2, kbu3, kby1, kby2, kby3, kbypass, kcolor, kcross1, kcross2, kdepth, kdest1, kdest2, kdest3, kdest4, kdest5, kdivision1, kdivision2, kdrive, kdriveB, kdynamics, kef1, kef2, kef3, kef4, keg1, keg2, keg3, keg4, kengine, keo1, keo2, keo3, keo4, keq1, keq2, keq3, keq4, khp, kinput, klp, kmix, kmode, kos, koutput, kparallel, kpf1, kpf2, kpg1, kpg2, kpost, kpq1, kpq2, kpre, kpresence, krate1, krate2, krelease, krelease2, ksensitivity, ksensitivity2, kshape1, kshape2, kshelfH, kshelfL, ksrc1, ksrc2, ksrc3, ksrc4, ksrc5, kstageA, kstageB, ksync1, ksync2, kthreshold, ktone, ktransient, klink, kbandCount, kcross3, kcross4, ktubeType, ktubeSag, ktapeSpeed, ktapeMemory, kwow, kflutter, kedge, kconsoleType, kheadroom, kfuzzType, kstarve, kfuzzGate, kdiodeType, kdiodeThreshold, kdiodeHard, kbitDepth, krateReduction, kfoldType, kfoldDepth, kreverseStages, kbe4, kbd4, kbm4, kbi4, kbo4, kbt4, kbc4, kby4, kbs4, kbu4, kbe5, kbd5, kbm5, kbi5, kbo5, kbt5, kbc5, kby5, kbs5, kbu5, kofflineOS, klinearPhase, kdelta, kCount };
 std::array<std::atomic<float>*,kCount> cached{};
 float pv(ParamIndex i) const noexcept {return cached[size_t(i)]->load(std::memory_order_relaxed);}
 ud::Engine core;std::unique_ptr<ud::Engine> alternate;ud::Engine* activeCore=nullptr;ud::Engine* fadingCore=nullptr;
 ud::Params smoothParams{},previousParams{},lastParams{};bool smoothReady=false;int fadeRemaining=0,fadeLength=1,qualityFade=0,qualityFadeLength=1;float bypassWet=1.f;
 ud::Params snapshot()const;double baseRate=48000;int lastOs=-1,maxLatency=0;std::array<std::unique_ptr<juce::dsp::Oversampling<float>>,4> overs;
 std::array<std::vector<float>,2> delay,dryDelay;size_t delayPos=0,dryPos=0;juce::AudioBuffer<float> alignedDry;std::unique_ptr<juce::dsp::Oversampling<float>> peakOvers;ud::Loudness loudness;float deltaWet=0.f;std::array<float,2> phase{},env{};uint32_t rng=1;float heldRandom=0;int step=0;
};
