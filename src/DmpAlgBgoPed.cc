/*
 *  $Id: DmpAlgBgoPed.cc, 2014-09-19 11:30:13 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#include <stdio.h>

#include "TH1F.h"
#include "TF1.h"
#include "TFile.h"

#include "DmpEvtHeader.h"
#include "DmpEvtBgoRaw.h"
#include "DmpEvtBgoPed.h"
#include "DmpAlgBgoPed.h"
#include "DmpDataBuffer.h"
#include "DmpParameterBgo.h"
#include "DmpBgoBase.h"
#include "DmpCore.h"

//-------------------------------------------------------------------
DmpAlgBgoPed::DmpAlgBgoPed()
 :DmpVAlg("Cal/Bgo/Ped"),
  fEvtHeader(0),
  fBgoRaw(0),
  fBgoPed(0)
{
  gRootIOSvc->Set("OutData/Key","ped");
  gRootIOSvc->Set("OutData/FileName",gRootIOSvc->GetInputFileName());
}

//-------------------------------------------------------------------
DmpAlgBgoPed::~DmpAlgBgoPed(){
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::Initialize(){
  // read input data
  fEvtHeader = new DmpEvtHeader();
  if(not gDataBuffer->ReadObject("Event/Rdc/EventHeader",fEvtHeader)){
    return false;
  }
  fBgoRaw = new DmpEvtBgoRaw();
  if(not gDataBuffer->ReadObject("Event/Rdc/Bgo",fBgoRaw)){
    return false;
  }
  // create output data holder
  fBgoPed = new DmpEvtBgoPed();
  if(not gDataBuffer->RegisterObject("Calibration/Bgo/Pedestal",fBgoPed,"DmpEvtBgoPed")){
    return false;
  }
  fBgoPed->UsedFileName = gRootIOSvc->GetInputFileName();
  gRootIOSvc->PrepareEvent(gCore->GetCurrentEventID());
  fBgoPed->StartTime = fEvtHeader->fSecond;
  // create Hist map
  short layerNo = DmpParameterBgo::kPlaneNo*2;
  short barNo = DmpParameterBgo::kBarNo+DmpParameterBgo::kRefBarNo;
  for(short l=0;l<layerNo;++l){
    for(short b=0;b<barNo;++b){
      for(short s=0;s<DmpParameterBgo::kSideNo;++s){
        for(short d=0;d<DmpParameterBgo::kDyNo;++d){
          char name[50];
          short gid_dy = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,d*3+2);
          snprintf(name,50,"BgoPed_%05d-L%02d_B%02d_Dy%02d",gid_dy,l,b,s*10+d*3+2);
          fPedHist.insert(std::make_pair(gid_dy,new TH1F(name,name,1000,-500,500)));
        }
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::ProcessThisEvent(){
  short nSignal = fBgoRaw->fADC.size();
  for(short i=0;i<nSignal;++i){
    fPedHist[fBgoRaw->fGlobalDynodeID[i]]->Fill(fBgoRaw->fADC[i]);
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::Finalize(){
  TF1 *gausFit = new TF1("GausFit","gaus",-500,500);
  std::string histFileName = gRootIOSvc->GetOutputPath()+gRootIOSvc->GetOutputStem()+"_Hist.root";
  TFile *histFile = new TFile(histFileName.c_str(),"RECREATE");
  fBgoPed->StopTime = fEvtHeader->fSecond;
  for(std::map<short,TH1F*>::iterator aHist=fPedHist.begin();aHist!=fPedHist.end();++aHist){
      fBgoPed->GlobalDynodeID.push_back(aHist->first);
// *
// *  TODO: fit and save output data 
// *
      float mean = aHist->second->GetMean(), sigma = aHist->second->GetRMS();
      for(short i = 0;i<3;++i){
        gausFit->SetRange(mean-2*sigma,mean+2*sigma);
        aHist->second->Fit(gausFit,"RQ");
        mean = gausFit->GetParameter(1);
        sigma = gausFit->GetParameter(2);
      }
      fBgoPed->Mean.push_back(mean);
      fBgoPed->Sigma.push_back(sigma);
      if((mean > 160 || mean<-160) && sigma >30){
         DmpLogWarning<<"GID = "<<aHist->first<<"\tmean = "<<mean<<"\tsigma = "<<sigma<<DmpLogEndl;
      }
      aHist->second->Write();
      delete aHist->second;
  }
  delete histFile;
  return true;
}


