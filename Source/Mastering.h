#pragma once
#include "Core.h"
#include <vector>
namespace ud {
struct Loudness {
 std::array<Biquad,2> shelf{},hp{};std::vector<double> history;size_t pos=0,mSize=1,sSize=1,filled=0;double mSum=0,sSum=0;
 void prepare(double sr){
  mSize=std::max<size_t>(1,size_t(std::round(.4*sr)));sSize=std::max<size_t>(mSize,size_t(std::round(3.*sr)));history.assign(sSize,0.);pos=filled=0;mSum=sSum=0;shelf={};hp={};
  double k=std::tan(double(pi)*1681.974450955533/sr),q=.7071752369554196,vh=std::pow(10.,3.999843853973347/20.),vb=std::pow(vh,.4996667741545416),a=1+k/q+k*k;
  for(auto& b:shelf){b.b0=float((vh+vb*k/q+k*k)/a);b.b1=float(2*(k*k-vh)/a);b.b2=float((vh-vb*k/q+k*k)/a);b.a1=float(2*(k*k-1)/a);b.a2=float((1-k/q+k*k)/a);}
  k=std::tan(double(pi)*38.13547087602444/sr);q=.5003270373238773;a=1+k/q+k*k;
  for(auto& b:hp){b.b0=1;b.b1=-2;b.b2=1;b.a1=float(2*(k*k-1)/a);b.a2=float((1-k/q+k*k)/a);}
 }
 void push(float l,float r,int channels){if(history.empty())return;float a=hp[0].run(shelf[0].run(l));double power=double(a)*a;if(channels==2){float b=hp[1].run(shelf[1].run(r));power+=double(b)*b;}if(!std::isfinite(power))power=0;double oldM=history[(pos+sSize-mSize)%sSize],oldS=history[pos];mSum+=power-oldM;sSum+=power-oldS;history[pos]=power;pos=(pos+1)%sSize;if(filled<sSize)++filled;}
 static float db(double v){return v>1.e-12?float(-.691+10*std::log10(v)):-120.f;}
 float momentary()const{return filled>=mSize?db(mSum/double(mSize)):-120.f;}
 float shortTerm()const{return filled>=sSize?db(sSum/double(sSize)):-120.f;}
};
}
