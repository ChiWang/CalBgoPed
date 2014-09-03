/*
 *  $Id: DmpAlgBgoPed.cc, 2014-09-01 14:52:22 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#include <stdio.h>

#include "TH1F.h"

#include "DmpEvtHeader.h"
#include "DmpEvtBgoRaw.h"
#include "DmpEvtBgoPed.h"
#include "DmpAlgBgoPed.h"
#include "DmpDataBuffer.h"
#include "DmpParameterBgo.h"
#include "DmpBgoBase.h"

//-------------------------------------------------------------------
DmpAlgBgoPed::DmpAlgBgoPed()
 :DmpVAlg("Cal/Bgo/Ped"),
  fEvtHeader(0),
  fBgoRaw(0),
  fBgoPed(0)
{
}

//-------------------------------------------------------------------
DmpAlgBgoPed::~DmpAlgBgoPed(){
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::Initialize(){
  // read input data
  fEvtHeader = dynamic_cast<DmpEvtHeader*>(gDataBuffer->ReadObject("Event/Rdc/EventHeader"));
  if(0 == fEvtHeader){
    DmpLogError<<"[DmpAlgBgoPed::Initialize] didn't find data \'Event/Rdc/EventHeader\'"<<DmpLogEndl;
    return false;
  }
  fBgoRaw = dynamic_cast<DmpEvtBgoRaw*>(gDataBuffer->ReadObject("Event/Rdc/Bgo"));
  if(0 == fBgoRaw){
    DmpLogError<<"[DmpAlgBgoPed::Initialize] didn't find data \'Event/Rdc/Bgo\'"<<DmpLogEndl;
    return false;
  }
  // create output data holder
  fBgoPed = new DmpEvtBgoPed();
  if(not gDataBuffer->RegisterObject("Calibration/Bgo/Pedestal",fBgoPed,"DmpEvtBgoPed")){
    return false;
  }
  // create Hist map
  short layerNo = DmpParameterBgo::kPlaneNo*2;
  short barNo = DmpParameterBgo::kBarNo+DmpParameterBgo::kRefBarNo;
  for(short l=0;l<layerNo;++l){
    for(short b=0;b<barNo;++b){
      for(short s=0;s<DmpParameterBgo::kSideNo;++s){
        for(short d=0;d<DmpParameterBgo::kDyNo;++d){
          char name[50];
          snprintf(name,50,"BgoPed_L%2d_B%02d_Dy%02d",l,b,s*10+d*3+2);
          fPedHist.insert(std::make_pair(DmpBgoBase::ConstructGlobalDynodeID(l,b,s,d*3+2),new TH1F(name,name,1000,-500,1500)));
        }
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::ProcessThisEvent(){
  short nSignal = fBgoRaw->GetSignalSize();
  short gid = 0,adc = -999;
  for(short i=0;i<nSignal;++i){
    if(fBgoRaw->GetSignal(i,gid,adc)){
      fPedHist[gid]->Fill(adc);
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::Finalize(){
  for(std::map<short,TH1F*>::iterator aHist=fPedHist.begin();aHist!=fPedHist.end();++aHist){
      aHist->second->Write();
// *
// *  TODO: fit and save output data 
// *
      delete aHist->second;
  }
  return true;
}


