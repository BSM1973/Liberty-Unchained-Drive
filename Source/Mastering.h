#pragma once
#include "Core.h"
#include <vector>
#include <cstdint>
namespace ud {
// Fixed 0.1 LU bins: bounded memory regardless of session length.
struct LoudnessHistogram {
 static constexpr int bins=1201;std::array<uint64_t,bins> count{};std::array<double,bins> energy{};
 static int index(double lu){return int(std::max(0.,std::min(double(bins-1),std::floor((lu+100.)*10.))));}
 static double db(double v){return v>1.e-12?-.691+10*std::log10(v):-120.;}
 void clear(){count.fill(0);energy.fill(0);}
 void add(double power){double lu=db(power);if(lu< -70.)return;int i=index(lu);++count[i];energy[i]+=power;}
 double average(double gate)const{uint64_t n=0;double sum=0;for(int i=index(gate);i<bins;++i){n+=count[i];sum+=energy[i];}return n?sum/double(n):0.;}
 float integrated()const{double absMean=average(-70.);return absMean>0?float(db(average(std::max(-70.,db(absMean)-10.)))):-120.f;}
 float range()const{double mean=average(-70.);if(mean<=0)return -1.f;int start=index(std::max(-70.,db(mean)-20.));uint64_t n=0;for(int i=start;i<bins;++i)n+=count[i];if(n<2)return -1.f;uint64_t a=0;int lo=-1,hi=start;for(int i=start;i<bins;++i){a+=count[i];if(lo<0&&double(a)>=.1*double(n))lo=i;if(double(a)>=.95*double(n)){hi=i;break;}}return float(hi-lo)*.1f;}
};
struct Loudness {
 std::array<Biquad,2> shelf{},hp{};std::vector<double> history;size_t pos=0,mSize=1,sSize=1,filled=0;double mSum=0,sSum=0;LoudnessHistogram blocks,shortBlocks;size_t hopM=1,hopS=1,nextM=1,nextS=1;float integratedValue=-120.f,rangeValue=-1.f;
 void resetSession(){blocks.clear();shortBlocks.clear();integratedValue=-120.f;rangeValue=-1.f;nextM=hopM;nextS=hopS;}
 void prepare(double sr){
  mSize=std::max<size_t>(1,size_t(std::round(.4*sr)));sSize=std::max<size_t>(mSize,size_t(std::round(3.*sr)));history.assign(sSize,0.);hopM=std::max<size_t>(1,size_t(std::round(.1*sr)));hopS=std::max<size_t>(1,size_t(std::round(sr)));resetSession();pos=filled=0;mSum=sSum=0;shelf={};hp={};
  double k=std::tan(double(pi)*1681.974450955533/sr),q=.7071752369554196,vh=std::pow(10.,3.999843853973347/20.),vb=std::pow(vh,.4996667741545416),a=1+k/q+k*k;
  for(auto& b:shelf){b.b0=float((vh+vb*k/q+k*k)/a);b.b1=float(2*(k*k-vh)/a);b.b2=float((vh-vb*k/q+k*k)/a);b.a1=float(2*(k*k-1)/a);b.a2=float((1-k/q+k*k)/a);}
  k=std::tan(double(pi)*38.13547087602444/sr);q=.5003270373238773;a=1+k/q+k*k;
  for(auto& b:hp){b.b0=1;b.b1=-2;b.b2=1;b.a1=float(2*(k*k-1)/a);b.a2=float((1-k/q+k*k)/a);}
 }
 void push(float l,float r,int channels){if(history.empty())return;float a=hp[0].run(shelf[0].run(l));double power=double(a)*a;if(channels==2){float b=hp[1].run(shelf[1].run(r));power+=double(b)*b;}if(!std::isfinite(power))power=0;double oldM=history[(pos+sSize-mSize)%sSize],oldS=history[pos];mSum+=power-oldM;sSum+=power-oldS;history[pos]=power;pos=(pos+1)%sSize;if(filled<sSize)++filled;if(--nextM==0){nextM=hopM;if(filled>=mSize){blocks.add(mSum/double(mSize));integratedValue=blocks.integrated();}}if(--nextS==0){nextS=hopS;if(filled>=sSize){shortBlocks.add(sSum/double(sSize));rangeValue=shortBlocks.range();}}}
 static float db(double v){return v>1.e-12?float(-.691+10*std::log10(v)):-120.f;}
 float momentary()const{return filled>=mSize?db(mSum/double(mSize)):-120.f;}
 float shortTerm()const{return filled>=sSize?db(sSum/double(sSize)):-120.f;}
};
}
