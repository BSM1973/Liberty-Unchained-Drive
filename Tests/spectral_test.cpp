#include "../Source/Core.h"
#include "../Source/SpectralLab.h"
#include <juce_dsp/juce_dsp.h>
#include <iostream>
#include <iomanip>
#include <chrono>
int main(){
 constexpr int n=4800;constexpr double sr=48000.,freq=7000.;ud::Params p;p.drive=.8f;p.bias=.1f;
 std::cout<<"engine,factor,THD_percent,THDN_percent,nonharmonic_residual_dB,core_ms,os_latency_samples\n";
 for(int engine=0;engine<8;++engine)for(int k=0;k<5;++k){
  juce::AudioBuffer<float> buf(1,n);juce::dsp::AudioBlock<float> block(buf);
  std::unique_ptr<juce::dsp::Oversampling<float>> os;if(k){os=std::make_unique<juce::dsp::Oversampling<float>>(1,size_t(k),juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,true,true);os->initProcessing(n);}
  auto start=std::chrono::steady_clock::now();for(int pass=0;pass<4;++pass){for(int i=0;i<n;++i)buf.setSample(0,i,.3f*std::sin(2*ud::pi*freq*i/sr));auto work=os?os->processSamplesUp(block):block;for(size_t i=0;i<work.getNumSamples();++i)work.setSample(0,int(i),ud::curve(engine,work.getSample(0,int(i)),p.drive,p.bias,p.color,p));if(os)os->processSamplesDown(block);}
  double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count(),mean=0,total=0,energy[4]={};
  for(int i=0;i<n;++i){double x=buf.getSample(0,i);if(!std::isfinite(x))return 1;mean+=x;total+=x*x;}mean/=n;total=total/n-mean*mean;
  for(int h=1;h<=3;++h){double a=0,b=0;for(int i=0;i<n;++i){double phi=2*3.141592653589793*freq*h*i/sr,x=buf.getSample(0,i);a+=x*std::cos(phi);b+=x*std::sin(phi);}energy[h]=2*(a*a+b*b)/(double(n)*n);}
  double residual=std::max(1.e-15,total-energy[1]-energy[2]-energy[3]);double thd=100*std::sqrt((energy[2]+energy[3])/std::max(1.e-15,energy[1]));double db=10*std::log10(residual/std::max(1.e-15,total));double thdn=100*std::sqrt(std::max(0.,total-energy[1])/std::max(1.e-15,energy[1]));
  std::cout<<engine<<","<<(1<<k)<<","<<thd<<","<<thdn<<","<<db<<","<<ms<<","<<(os?os->getLatencyInSamples():0.f)<<"\n";
 }
 std::cout<<"v0.7 corner smoothing: engine,factor,legacy_residual_dB,smooth_residual_dB,legacy_ms,smooth_ms\n";
 for(int engine:{4,7})for(int k=0;k<5;++k){p.engine=engine;p.antiAlias=false;auto a=std::chrono::steady_clock::now();auto legacy=ud::measureCurve(p,k,7000.);auto b=std::chrono::steady_clock::now();p.antiAlias=true;auto smooth=ud::measureCurve(p,k,7000.);auto c=std::chrono::steady_clock::now();if(!legacy.valid||!smooth.valid)return 3;std::cout<<engine<<","<<(1<<k)<<","<<legacy.residualDb<<","<<smooth.residualDb<<","<<std::chrono::duration<double,std::milli>(b-a).count()<<","<<std::chrono::duration<double,std::milli>(c-b).count()<<"\n";}
 // Known intersample peak: fs/4 sine at pi/4 phase, sample peak sqrt(0.5), true peak 1.
 for(double srTest:{44100.,48000.,96000.,192000.}){
  juce::AudioBuffer<float> b(1,1024);juce::dsp::AudioBlock<float> block(b);juce::dsp::Oversampling<float> up(1,2,juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple,true,true);up.initProcessing(1024);
  float maxPeak=0;for(int pass=0;pass<4;++pass){for(int i=0;i<1024;++i)b.setSample(0,i,float(std::sin(3.141592653589793*(.5*i+.25))));auto u=up.processSamplesUp(block);if(pass==3)for(size_t i=0;i<u.getNumSamples();++i)maxPeak=std::max(maxPeak,std::abs(u.getSample(0,int(i))));}
  double db=20*std::log10(maxPeak);std::cout<<"True peak reference "<<srTest<<" Hz: "<<db<<" dBTP\n";if(!std::isfinite(db)||std::abs(db)>.3)return 2;
 }
}
