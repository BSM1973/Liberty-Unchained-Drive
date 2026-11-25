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
 float blend=.5f,driveB=.3f;
 std::array<float,4> eqF{100,400,1600,6400},eqG{},eqQ{1,1,1,1}; std::array<bool,4> eqOn{true,true,true,true};
 std::array<float,2> preF{400,2000},preG{},preQ{1,1}; float preShelfL=0,preShelfH=0;
 std::array<int,3> bandEngine{0,0,0}; std::array<float,3> bandDrive{.25f,.25f,.25f},bandMix{1,1,1},bandIn{},bandOut{},bandTone{},bandColor{.4f,.4f,.4f},bandDyn{.35f,.35f,.35f};
 std::array<bool,3> solo{},mute{};
};
struct State {float env=0, slow=0, tone=0, body=0, preLow=0,preHigh=0,shelfL=0,shelfH=0,c1=0,c2=0,dc=0,tape=0,hold=0;int counter=0;};
inline float curve(int engine,float x,float d,float bias,float color){
 const float a=1.f+18.f*d;const float v=x*a;const float b=bias*.55f;
 switch(engine){
 case 0:return (std::tanh(v+b)-std::tanh(b))/(1.f+0.15f*d);
 case 1:return v/std::sqrt(1.f+v*v);
 case 2:return v>=0?std::tanh(v*(1.f+color)):0.8f*std::tanh(v*(1.f-color*.5f)/.8f);
 case 3:return std::tanh(v*.45f)*1.8f*.85f+std::tanh(v*.12f)*.15f;
 case 4:return std::abs(x)<.005f+.04f*color?0.f:clip(v*(2.f+color)+b,-.8f,.8f)-clip(b,-.8f,.8f);
 case 5:return clip(v,-(.65f+.3f*color),.65f+.3f*color)*(1.f-.15f*std::abs(bias));
 case 6:{float q=std::pow(2.f,15.f-12.f*d);return std::round(clip(v,-1.f,1.f)*q)/q;}
 case 7:return fold(v*(1.f+color*2.f)+b)-fold(b);
 default:return std::tanh(v);
 }
}
struct Biquad {float b0=1,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;
 void peak(float f,float g,float q,float sr){float w=2*pi*clip(f,10,sr*.45f)/sr,a=std::pow(10.f,g/40.f),al=std::sin(w)/(2*std::max(.1f,q)),c=std::cos(w),d=1+al/a;b0=(1+al*a)/d;b1=-2*c/d;b2=(1-al*a)/d;a1=b1;a2=(1-al/a)/d;}
 float run(float x){float y=b0*x+z1;z1=b1*x-a1*y+z2;z2=b2*x-a2*y;return y;}
};
struct Engine {
 float sr=48000; std::array<State,2> s{};std::array<std::array<State,3>,2> bs{};std::array<State,2> hybrid{}; std::array<std::array<Biquad,4>,2> eq{};std::array<std::array<Biquad,2>,2> preEq{};
 void reset(float rate){sr=rate;s={};bs={};hybrid={};eq={};preEq={};}
 void configure(const Params& p){for(int ch=0;ch<2;++ch){for(int j=0;j<4;++j)eq[ch][j].peak(p.eqF[j],p.eqOn[j]?p.eqG[j]:0,p.eqQ[j],sr);for(int j=0;j<2;++j)preEq[ch][j].peak(p.preF[j],p.preG[j],p.preQ[j],sr);}}
 float nonlinear(State& st,int eng,float x,float d,float bias,float color,float dyn){
 if(eng==1){st.tape+=coef(8500.f+color*9500.f,sr)*(x-st.tape);x=.75f*x+.25f*st.tape;}
 if(eng==6){if(st.counter<=0){st.hold=x;st.counter=1+int(d*28.f);}--st.counter;x=st.hold;}
 float y=curve(eng,x,d,bias,color);return y/(1.f+dyn*std::abs(y)*.35f);
 }
 float sample(int ch,float x,const Params& p,float linkedLevel=-1.f){auto& st=s[size_t(ch)]; if(!std::isfinite(x))x=0; const float dry=x;if(p.bypass)return dry;
 x*=gain(p.input);float level=linkedLevel>=0?linkedLevel:std::abs(x);float k=1.f-std::exp(-1000.f/(sr*std::max(0.1f,level>st.env?p.attack:p.release)));st.env+=k*(level-st.env);st.slow+=coef(8,sr)*(level-st.slow);
 float env=clip((st.env-gain(p.threshold))*p.sensitivity*3.f,0,1);float d=clip(p.drive+p.depth*env+p.transient*(st.env-st.slow),0,1);
 if(p.pre){st.preLow+=coef(p.hp,sr)*(x-st.preLow);x-=st.preLow;st.preHigh+=coef(p.lp,sr)*(x-st.preHigh);x=st.preHigh;for(auto& f:preEq[ch])x=f.run(x);st.shelfL+=coef(180,sr)*(x-st.shelfL);st.shelfH+=coef(4000,sr)*(x-st.shelfH);x+=(gain(p.preShelfL)-1.f)*st.shelfL+(gain(p.preShelfH)-1.f)*(x-st.shelfH); }
 float y=0;
 if(p.engine==8){st.c1+=coef(p.cross1,sr)*(x-st.c1);st.c2+=coef(std::max(p.cross1+1,p.cross2),sr)*(x-st.c2);std::array<float,3> v{st.c1,st.c2-st.c1,x-st.c2};bool solo=p.solo[0]||p.solo[1]||p.solo[2];for(int i=0;i<3;++i){if(p.mute[i]||(solo&&!p.solo[i]))continue;auto& b=bs[ch][i];float a=v[i]*gain(p.bandIn[i]);float w=nonlinear(b,p.bandEngine[i],a,p.bandDrive[i],p.bias,p.bandColor[i],p.bandDyn[i]);b.tone+=coef(1500,sr)*(w-b.tone);w+=p.bandTone[i]*(w-b.tone)*.5f;y+=(a+(w-a)*p.bandMix[i])*gain(p.bandOut[i]);}}
 else if(p.engine==9){float a=nonlinear(st,p.stageA,x,d,p.bias,p.color,p.dynamics);float b=nonlinear(hybrid[ch],p.stageB,p.parallel?x:a,p.driveB,p.bias,p.color,p.dynamics);y=a+(b-a)*p.blend;}
 else y=nonlinear(st,p.engine,x,d,p.bias,p.color,p.dynamics);
 st.tone+=coef(2200,sr)*(y-st.tone);st.body+=coef(180,sr)*(y-st.body);y+=(p.tone*.65f+(gain(p.presence)-1.f))*(y-st.tone)+(gain(p.body)-1.f)*st.body;
 if(p.post){for(auto& f:eq[ch])y=f.run(y);y+=(gain(p.low)-1.f)*st.body+(gain(p.high)-1.f)*(y-st.tone);}
 st.dc+=coef(5,sr)*(y-st.dc);y-=st.dc;
 if(p.autoGain)y/=1.f+3.f*d; // static approximation, not RMS matching
 y=(dry+(y-dry)*p.mix)*gain(p.output);return std::isfinite(y)?clip(y,-32,32):0;
 }
};
}
