#include "../Source/Core.h"
#include "../Source/Mastering.h"
#include <chrono>
#include <iostream>
#include <cstdlib>
#include <memory>
int main(){
 int count=0;
 for(float sr:{44100.f,48000.f,96000.f,192000.f}){
  for(int e=0;e<10;++e){for(int mode=0;mode<3;++mode){auto dsp=std::make_unique<ud::Engine>();dsp->reset(sr);ud::Params p;p.engine=e;p.drive=.8f;p.bands=mode?5:3;p.tubeType=mode;p.tapeSpeed=mode;p.wow=mode*.25f;p.flutter=mode*.2f;p.consoleType=mode;p.fuzzType=mode%2;p.starve=mode*.25f;p.diodeType=mode;p.foldType=mode;p.rateReduction=mode*.25f;p.parallel=mode==1;p.reverseStages=mode==2;dsp->configure(p);double energy=0;
   for(int i=0;i<int(sr*.1f);++i){float x=.3f*std::sin(2*ud::pi*440*i/sr);float y=dsp->sample(0,x,p);if(!std::isfinite(y)||std::abs(y)>32.01f)return 1;energy+=y*y;}if(energy<.001)return 2;++count;
  }}
  for(int n:{3,5}){auto state=std::make_unique<ud::State>();ud::Params p;p.bands=n;for(int i=0;i<4096;++i){float x=std::sin(i*.13f);auto out=ud::splitBands(*state,x,p,sr);float sum=0;for(int j=0;j<n;++j)sum+=out[j];if(std::abs(sum-x)>1e-6f)return 5;}++count;}
 }
 auto dsp=std::make_unique<ud::Engine>();ud::Params p;p.bypass=true;if(dsp->sample(0,.123f,p)!=.123f)return 3;p.bypass=false;p.mix=0;p.output=0;if(dsp->sample(0,.123f,p)!=.123f)return 4;
 for(int e:{0,3,4,5,7}){ud::Params a,b;b.tubeType=1;b.consoleType=1;b.fuzzType=1;b.diodeType=1;b.foldType=1;double diff=0;for(int i=-100;i<=100;++i){float x=i*.01f;diff+=std::abs(ud::curve(e,x,.6f,.15f,.4f,a)-ud::curve(e,x,.6f,.15f,.4f,b));}if(diff<.001)return 6;++count;}
 for(float sr:{44100.f,48000.f,96000.f,192000.f}){ud::RmsMatch match;match.prepare(sr);double a=0,b=0;for(int i=0;i<int(sr*3.f);++i){float x=.2f*std::sin(2*ud::pi*440*i/sr);float y=match.process(x,x*3.f,true);if(!std::isfinite(y))return 7;if(i>int(sr*2.5f)){a+=x*x;b+=y*y;}}if(std::abs(10*std::log10(b/a))>.5)return 8;for(int i=0;i<int(sr);++i)if(!std::isfinite(match.process(0.f,0.f,true)))return 9;++count;}
 for(int factor:{1,2,4,8,16})for(int bands:{3,5}){ud::LinearSplit fir;fir.configure({180.f,1200.f,4000.f,8000.f},48000.f,factor);std::array<float,ud::LinearSplit::size> hist{},ref{};unsigned pos=0,rp=0;for(int i=0;i<ud::LinearSplit::size;++i){float x=i==0?1.f:.2f*std::sin(i*.071f);ref[rp]=x;float expected=ref[(rp+ud::LinearSplit::size-unsigned(ud::LinearSplit::half*factor))&ud::LinearSplit::mask];rp=(rp+1)&ud::LinearSplit::mask;auto v=fir.process(hist,pos,x,bands);float sum=0;for(int j=0;j<bands;++j)sum+=v[j];if(std::abs(sum-expected)>2e-6f)return 10;}++count;}
 {ud::Loudness meter;meter.prepare(48000);for(int i=0;i<192000;++i){float x=.1f*std::sin(2*ud::pi*1000*i/48000.f);meter.push(x,x,2);}if(std::abs(meter.momentary()+20.f)>.5f||std::abs(meter.shortTerm()+20.f)>.5f)return 11;++count;}
 for(int engine=0;engine<10;++engine){auto e=std::make_unique<ud::Engine>();e->reset(48000);ud::Params p;p.engine=engine;p.drive=.7f;p.bands=5;e->configure(p);auto start=std::chrono::steady_clock::now();double energy=0;for(int i=0;i<48000*60;++i){float x=.23f*std::sin(i*.057f)+.1f*std::sin(i*.79f);float y=e->sample(0,x,p);if(!std::isfinite(y))return 12;energy+=y*y;}if(energy<.001)return 13;std::cout<<"Engine "<<engine<<" 60s mono core wall ms="<<std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()<<"\n";++count;}
 {ud::Loudness meter;meter.prepare(48000);for(int i=0;i<48000*8;++i){float x=.1f*std::sin(2*ud::pi*1000*i/48000.f);meter.push(x,x,2);}if(std::abs(meter.integratedValue+20.f)>.5f||meter.rangeValue<0.f||meter.rangeValue>.3f)return 14;meter.resetSession();if(meter.integratedValue>-119.f||meter.rangeValue>=0.f)return 15;++count;}
 {ud::LinearSplit fir;fir.configure({180.f,1200.f,4000.f,8000.f},48000.f,1);std::array<float,ud::LinearSplit::size> h{};for(size_t i=0;i<h.size();++i)h[i]=std::sin(float(i)*.017f)+.1f*std::cos(float(i)*.77f);for(unsigned c:{0u,255u,256u,2048u,8192u,16127u,16383u})for(int b=0;b<4;++b)if(std::abs(fir.convolve(h,c,b,true)-fir.convolve(h,c,b,false))>1e-5f)return 16;++count;}
 {ud::KWeight filter;filter.prepare(48000);double a=0,b=0;for(int i=0;i<48000;++i){float x=.1f*std::sin(2*ud::pi*1000*i/48000.f);float y=filter.run(x);if(i>24000){a+=x*x;b+=y*y;}}if(!std::isfinite(b)||b<=a)return 17;++count;}
 std::cout<<count<<" DSP cases plus dry and bypass passed\n";
}
