#include "../Source/Core.h"
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
 std::cout<<count<<" engine/profile, crossover and RMS tests plus dry and bypass passed\n";
}
