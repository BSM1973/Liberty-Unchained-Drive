#pragma once
#include "Core.h"
#include <juce_dsp/juce_dsp.h>
#include <memory>
namespace ud {
struct SpectralResult {double thd=0,thdn=0,residualDb=0;bool valid=false;};
inline SpectralResult measureCurve(const Params& p,int quality,double frequency=1000.){
 constexpr int n=4800;constexpr double sr=48000.;if(p.engine<0||p.engine>7)return {};
 juce::AudioBuffer<float> buf(1,n);juce::dsp::AudioBlock<float> block(buf);std::unique_ptr<juce::dsp::Oversampling<float>> os;
 if(quality>0){os=std::make_unique<juce::dsp::Oversampling<float>>(1,size_t(quality),juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,true,true);os->initProcessing(n);}
 for(int pass=0;pass<4;++pass){for(int i=0;i<n;++i)buf.setSample(0,i,float(.3*std::sin(2*3.141592653589793*frequency*i/sr)));auto work=os?os->processSamplesUp(block):block;for(size_t i=0;i<work.getNumSamples();++i)work.setSample(0,int(i),curve(p.engine,work.getSample(0,int(i)),p.drive,p.bias,p.color,p));if(os)os->processSamplesDown(block);}
 double mean=0,total=0;for(int i=0;i<n;++i){double x=buf.getSample(0,i);if(!std::isfinite(x))return {};mean+=x;total+=x*x;}mean/=n;total=std::max(0.,total/n-mean*mean);
 double fundamental=0,harmonics=0;for(int h=1;h*frequency<sr*.5;++h){double a=0,b=0;for(int i=0;i<n;++i){double phi=2*3.141592653589793*frequency*h*i/sr,x=buf.getSample(0,i);a+=x*std::cos(phi);b+=x*std::sin(phi);}double e=2*(a*a+b*b)/(double(n)*n);if(h==1)fundamental=e;else harmonics+=e;}
 if(fundamental<1.e-12)return {};return {100*std::sqrt(harmonics/fundamental),100*std::sqrt(std::max(0.,total-fundamental)/fundamental),10*std::log10(std::max(1.e-15,total-fundamental-harmonics)/std::max(1.e-15,total)),true};
}
}
