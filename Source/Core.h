#pragma once
#include <algorithm>
#include <array>
#include <cmath>
namespace ud {
constexpr float pi=3.14159265358979323846f;
inline float clip(float x,float a,float b){return std::max(a,std::min(b,x));}
inline float gain(float db){return std::pow(10.f,db/20.f);}
inline float coef(float hz,float sr){return 1.f-std::exp(-2.f*pi*clip(hz,1.f,sr*.45f)/sr);}
inline float fold(float x){float t=std::fmod(x+1.f,4.f);if(t<0)t+=4.f;return 1.f-std::abs(t-2.f);}
struct Params {
 int engine=0, bands=3, mode=0, stageA=0,stageB=2; bool parallel=false,link=true,autoGain=false,bypass=false,pre=true,post=true;
 float drive=.3f,input=0,output=0,mix=1,bias=0,tone=0,color=.4f,dynamics=.35f,presence=0,body=0;
 float attack=10,release=120,threshold=-24,sensitivity=.5f,depth=0,transient=0;
 float cross1=180,cross2=1200,hp=20,lp=20000,low=0,high=0;
 float blend=.5f,driveB=.3f,cross3=4000,cross4=8000;
 int tubeType=0,tapeSpeed=1,consoleType=0,fuzzType=0,diodeType=0,foldType=0;
 float tubeSag=.35f,tapeMemory=.25f,wow=0,flutter=0,edge=.5f,headroom=0,starve=0,fuzzGate=.1f,diodeThreshold=.7f,diodeHard=.35f,bitDepth=12,rateReduction=0,foldDepth=.5f;
 bool reverseStages=false;
 std::array<float,4> eqF{100,400,1600,6400},eqG{},eqQ{1,1,1,1}; std::array<bool,4> eqOn{true,true,true,true};
 std::array<float,2> preF{400,2000},preG{},preQ{1,1}; float preShelfL=0,preShelfH=0;
 std::array<int,5> bandEngine{}; std::array<float,5> bandDrive{.25f,.25f,.25f,.25f,.25f},bandMix{1,1,1,1,1},bandIn{},bandOut{},bandTone{},bandColor{.4f,.4f,.4f,.4f,.4f},bandDyn{.35f,.35f,.35f,.35f,.35f};
 std::array<bool,5> solo{},mute{};
};
struct RmsMatch {
 float reference=0.f,wet=0.f,correction=1.f;float energyAlpha=0.f,gainAlpha=0.f;
 void prepare(float sr){reference=wet=0.f;correction=1.f;energyAlpha=1.f-std::exp(-1.f/(.30f*sr));gainAlpha=1.f-std::exp(-1.f/(.18f*sr));}
 float process(float dry,float processed,bool enabled){reference+=energyAlpha*(dry*dry-reference);wet+=energyAlpha*(processed*processed-wet);float target=1.f;if(enabled&&reference>1.e-8f&&wet>1.e-8f)target=clip(std::sqrt(reference/wet),.12589254f,3.9810717f);correction+=gainAlpha*(target-correction);return processed*correction;}
};
struct State {float env=0, slow=0, tone=0, body=0, preLow=0,preHigh=0,shelfL=0,shelfH=0,c1=0,c2=0,dc=0,tape=0,hold=0;int counter=0; float circuitEnv=0,magnet=0,tapePhase=0,flutterPhase=0;
 std::array<float,4096> tapeDelay{}; unsigned tapeWrite=0; std::array<float,4> split{};
};
inline float curve(int engine,float x,float d,float bias,float color,const Params& p=Params{}){
 const float a=1.f+18.f*d,v=x*a,b=bias*.55f;
 switch(engine){
 case 0:{float asym=(p.tubeType==0?.16f:p.tubeType==1?.32f:.07f)*(0.3f+color*.7f)+b;return (std::tanh(v+asym)-std::tanh(asym))/(1.f+.15f*d);}
 case 1:return v/std::sqrt(1.f+v*v);
 case 2:{float asym=.12f+.28f*p.edge;return v>=0?std::tanh(v*(1.f+color*p.edge)+b)-std::tanh(b):(1.f-asym)*(std::tanh(v*(1.f+color*.3f)/(1.f-asym)+b)-std::tanh(b));}
 case 3:{float h=gain(-p.headroom);float q=v*h;switch(p.consoleType){case 1:return std::tanh(q*.55f)*1.5f;case 2:return (q/std::sqrt(1+q*q))*.9f+std::tanh(q*.11f)*.25f;default:return std::tanh(q*.45f)*1.53f+std::tanh(q*.12f)*.15f;}}
 case 4:{float lim=p.fuzzType==0?.65f:.9f;float gate=.001f+p.fuzzGate*.045f+p.starve*.025f;if(std::abs(x)<gate)return 0;float off=b+p.starve*.4f;float q=v*(2.f+color);return clip(q+off,-lim*(1.f-.35f*p.starve),lim)-clip(off,-lim*(1.f-.35f*p.starve),lim);}
 case 5:{float threshold=std::max(.05f,p.diodeThreshold)*(p.diodeType==0?1.f:p.diodeType==1?.65f:1.4f);float t=v+b;auto diode=[&](float q){float lim=threshold*(q<0?1.f-.35f*bias:1.f+.35f*bias);return (1.f-p.diodeHard)*lim*std::tanh(q/lim)+p.diodeHard*clip(q,-lim,lim);};return diode(t)-diode(b);}
 case 6:{float q=std::pow(2.f,clip(p.bitDepth,2.f,16.f)-1.f);return std::round(clip(v,-1.f,1.f)*q)/q;}
 case 7:{float q=v*(.5f+3.f*p.foldDepth+color);if(p.foldType==1)return std::sin(q+b)-std::sin(b);if(p.foldType==2)return fold(q+b+.3f)-fold(b+.3f);return fold(q+b)-fold(b);}
 default:return std::tanh(v);
 }
}
inline std::array<float,4> crossoverFrequencies(const Params& p,float sr){
 const float hi=std::min(20000.f,sr*.44f);
 float a=clip(p.cross1,20.f,hi-80.f),b=clip(p.cross2,a+20.f,hi-60.f),c=clip(p.cross3,b+20.f,hi-40.f),d=clip(p.cross4,c+20.f,hi-20.f);
 return {a,b,c,d};
}
inline std::array<float,5> splitBands(State& st,float x,const Params& p,float sr){
 auto freq=crossoverFrequencies(p,sr);int n=p.bands==5?5:3;std::array<float,5> out{};float last=0;
 for(int i=0;i<n-1;++i){st.split[i]+=coef(freq[i],sr)*(x-st.split[i]);out[i]=st.split[i]-last;last=st.split[i];}out[n-1]=x-last;return out;
}
struct Biquad {float b0=1,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;
 void peak(float f,float g,float q,float sr){float w=2*pi*clip(f,10,sr*.45f)/sr,a=std::pow(10.f,g/40.f),al=std::sin(w)/(2*std::max(.1f,q)),c=std::cos(w),d=1+al/a;b0=(1+al*a)/d;b1=-2*c/d;b2=(1-al*a)/d;a1=b1;a2=(1-al/a)/d;}
 float run(float x){float y=b0*x+z1;z1=b1*x-a1*y+z2;z2=b2*x-a2*y;return y;}
};
struct Engine {
 float sr=48000; float sagAlpha=0,slowAlpha=0,toneAlpha=0,bodyAlpha=0,dcAlpha=0,bandToneAlpha=0; std::array<RmsMatch,2> rmsMatch{}; std::array<State,2> s{};std::array<std::array<State,5>,2> bs{};std::array<State,2> hybrid{}; std::array<std::array<Biquad,4>,2> eq{};std::array<std::array<Biquad,2>,2> preEq{};
 void reset(float rate){sr=rate;s={};bs={};hybrid={};eq={};preEq={};sagAlpha=coef(12,sr);slowAlpha=coef(8,sr);toneAlpha=coef(2200,sr);bodyAlpha=coef(180,sr);dcAlpha=coef(5,sr);bandToneAlpha=coef(1500,sr);for(auto& a:rmsMatch)a.prepare(sr);}
 void configure(const Params& p){for(int ch=0;ch<2;++ch){for(int j=0;j<4;++j)eq[ch][j].peak(p.eqF[j],p.eqOn[j]?p.eqG[j]:0,p.eqQ[j],sr);for(int j=0;j<2;++j)preEq[ch][j].peak(p.preF[j],p.preG[j],p.preQ[j],sr);}}
 float nonlinear(State& st,int eng,float x,float d,float bias,float color,float dyn,const Params& p){
 if(eng==0){st.circuitEnv+=sagAlpha*(std::abs(x)-st.circuitEnv);d=clip(d/(1.f+p.tubeSag*st.circuitEnv*2.f),0,1);}
 if(eng==1){
  float speed=p.tapeSpeed==0?.65f:p.tapeSpeed==1?1.f:1.35f;st.tape+=coef((6500.f+color*9000.f)*speed,sr)*(x-st.tape);x=.65f*x+.35f*st.tape;
  st.magnet+=coef(30.f+speed*120.f,sr)*(std::tanh(x*(1.f+4.f*d)+p.tapeMemory*st.magnet)-st.magnet);x+=p.tapeMemory*.18f*st.magnet;
  if(p.wow>0||p.flutter>0){st.tapePhase+=2*pi*.45f/sr;st.flutterPhase+=2*pi*6.2f/sr;if(st.tapePhase>2*pi)st.tapePhase-=2*pi;if(st.flutterPhase>2*pi)st.flutterPhase-=2*pi;
   float depth=sr*(.0012f*p.wow+.00018f*p.flutter);float delay=clip(depth+sr*(.0012f*p.wow*std::sin(st.tapePhase)+.00018f*p.flutter*std::sin(st.flutterPhase)),0.f,4093.f);
   auto i=unsigned(delay);float f=delay-float(i);st.tapeDelay[st.tapeWrite]=x;auto idx=(st.tapeWrite+4096u-i)&4095u;x=st.tapeDelay[idx]*(1.f-f)+st.tapeDelay[(idx+4095u)&4095u]*f;st.tapeWrite=(st.tapeWrite+1u)&4095u;
  }
 }
 if(eng==6){if(st.counter<=0){st.hold=x;st.counter=1+int(p.rateReduction*std::max(1.f,sr/1000.f));}--st.counter;x=st.hold;}
 float y=curve(eng,x,d,bias,color,p);return y/(1.f+dyn*std::abs(y)*.35f);
 }
 float sample(int ch,float x,const Params& p,float linkedLevel=-1.f){auto& st=s[size_t(ch)]; if(!std::isfinite(x))x=0; const float dry=x;if(p.bypass)return dry;
 x*=gain(p.input);float level=linkedLevel>=0?linkedLevel:std::abs(x);float k=1.f-std::exp(-1000.f/(sr*std::max(0.1f,level>st.env?p.attack:p.release)));st.env+=k*(level-st.env);st.slow+=slowAlpha*(level-st.slow);
 float env=clip((st.env-gain(p.threshold))*p.sensitivity*3.f,0,1);float d=clip(p.drive+p.depth*env+p.transient*(st.env-st.slow),0,1);
 if(p.pre){st.preLow+=coef(p.hp,sr)*(x-st.preLow);x-=st.preLow;st.preHigh+=coef(p.lp,sr)*(x-st.preHigh);x=st.preHigh;for(auto& f:preEq[ch])x=f.run(x);st.shelfL+=coef(180,sr)*(x-st.shelfL);st.shelfH+=coef(4000,sr)*(x-st.shelfH);x+=(gain(p.preShelfL)-1.f)*st.shelfL+(gain(p.preShelfH)-1.f)*(x-st.shelfH); }
 float y=0;
 if(p.engine==8){auto v=splitBands(st,x,p,sr);int n=p.bands==5?5:3;bool solo=false;for(int i=0;i<n;++i)solo=solo||p.solo[i];for(int i=0;i<n;++i){if(p.mute[i]||(solo&&!p.solo[i]))continue;auto& b=bs[ch][i];float a=v[i]*gain(p.bandIn[i]);float w=nonlinear(b,p.bandEngine[i],a,p.bandDrive[i],p.bias,p.bandColor[i],p.bandDyn[i],p);b.tone+=bandToneAlpha*(w-b.tone);w+=p.bandTone[i]*(w-b.tone)*.5f;y+=(a+(w-a)*p.bandMix[i])*gain(p.bandOut[i]);}}
 else if(p.engine==9){if(p.parallel){float a=nonlinear(st,p.stageA,x,d,p.bias,p.color,p.dynamics,p);float b=nonlinear(hybrid[ch],p.stageB,x,p.driveB,p.bias,p.color,p.dynamics,p);y=a+(b-a)*p.blend;}else if(p.reverseStages){float b=nonlinear(hybrid[ch],p.stageB,x,p.driveB,p.bias,p.color,p.dynamics,p);float a=nonlinear(st,p.stageA,b,d,p.bias,p.color,p.dynamics,p);y=b+(a-b)*p.blend;}else{float a=nonlinear(st,p.stageA,x,d,p.bias,p.color,p.dynamics,p);float b=nonlinear(hybrid[ch],p.stageB,a,p.driveB,p.bias,p.color,p.dynamics,p);y=a+(b-a)*p.blend;}}
 else y=nonlinear(st,p.engine,x,d,p.bias,p.color,p.dynamics,p);
 st.tone+=toneAlpha*(y-st.tone);st.body+=bodyAlpha*(y-st.body);y+=(p.tone*.65f+(gain(p.presence)-1.f))*(y-st.tone)+(gain(p.body)-1.f)*st.body;
 if(p.post){for(auto& f:eq[ch])y=f.run(y);y+=(gain(p.low)-1.f)*st.body+(gain(p.high)-1.f)*(y-st.tone);}
 st.dc+=dcAlpha*(y-st.dc);y-=st.dc;
 y=rmsMatch[size_t(ch)].process(dry,y,p.autoGain);
 y=(dry+(y-dry)*p.mix)*gain(p.output);return std::isfinite(y)?clip(y,-32,32):0;
 }
};
}
