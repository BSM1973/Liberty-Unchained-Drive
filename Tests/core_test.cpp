#include "../Source/Core.h"
#include <iostream>
#include <cstdlib>
int main(){for(float sr:{44100.f,48000.f,96000.f,192000.f}){for(int e=0;e<10;++e){ud::Engine dsp;dsp.reset(sr);ud::Params p;p.engine=e;p.drive=.8f;double energy=0;for(int i=0;i<int(sr);++i){float x=.3f*std::sin(2*ud::pi*440*i/sr);float y=dsp.sample(0,x,p);if(!std::isfinite(y)||std::abs(y)>32.01f)return 1;energy+=y*y;}if(energy<.001)return 2;}}ud::Engine dsp;ud::Params p;p.bypass=true;if(dsp.sample(0,.123f,p)!=.123f)return 3;p.bypass=false;p.mix=0;p.output=0;if(dsp.sample(0,.123f,p)!=.123f)return 4;std::cout<<"40 engine/rate tests plus dry and bypass passed\n";}
