/*
 *  $Id: DmpAlgBgoPed.cc, 2014-09-21 15:34:23 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#include <stdio.h>
#include <boost/lexical_cast.hpp>

#include "TH1D.h"
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
  gRootIOSvc->SetOutputKey("ped");
}

//-------------------------------------------------------------------
DmpAlgBgoPed::~DmpAlgBgoPed(){
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::Initialize(){
  // read input data
  fEvtHeader = dynamic_cast<DmpEvtHeader*>(gDataBuffer->ReadObject("Event/Rdc/EventHeader"));
  if(0 == fEvtHeader){
    fEvtHeader = new DmpEvtHeader();
    gDataBuffer->LinkRootFile("Event/Rdc/EventHeader",fEvtHeader);
  }
  fBgoRaw = dynamic_cast<DmpEvtBgoRaw*>(gDataBuffer->ReadObject("Event/Rdc/Bgo"));
  if(0 == fBgoRaw){
    fBgoRaw = new DmpEvtBgoRaw();
    gDataBuffer->LinkRootFile("Event/Rdc/Bgo",fBgoRaw);
  }
  // create output data holder
  fBgoPed = new DmpEvtBgoPed();
  gDataBuffer->RegisterObject("Calibration/Bgo/Pedestal",fBgoPed,"DmpEvtBgoPed");
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
          fPedHist.insert(std::make_pair(gid_dy,new TH1D(name,name,1000,-500,500)));
        }
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::ProcessThisEvent(){
  if((fBgoRaw->GetRunMode() != DmpERunMode::kOriginal) || (not fEvtHeader->EnabledPeriodTrigger()) || (not fEvtHeader->GeneratedPeriodTrigger())){
    return false;
  }
  short nSignal = fBgoRaw->fGlobalDynodeID.size();
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
  for(std::map<short,TH1D*>::iterator aHist=fPedHist.begin();aHist!=fPedHist.end();++aHist){
      fBgoPed->GlobalDynodeID.push_back(aHist->first);
    // Fit and save output data
      double mean = aHist->second->GetMean(), sigma = aHist->second->GetRMS();
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


