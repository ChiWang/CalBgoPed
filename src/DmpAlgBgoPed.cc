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
#include "DmpAlgBgoPed.h"
#include "DmpDataBuffer.h"
#include "DmpParameterBgo.h"
#include "DmpBgoBase.h"
#include "DmpCore.h"
#include "DmpTimeConvertor.h"

//-------------------------------------------------------------------
DmpAlgBgoPed::DmpAlgBgoPed()
 :DmpVAlg("Cal/Bgo/Ped"),
  fEvtHeader(0),
  fBgoRaw(0),
  fFirstEvtTime(-1),
  fLastEvtTime(-1)
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
          fBgoPedHist.insert(std::make_pair(gid_dy,new TH1D(name,name,1000,-500,500)));
        }
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::ProcessThisEvent(){
  if(fFirstEvtTime == -1){
    fFirstEvtTime = fEvtHeader->GetSecond();
  }
  if((fBgoRaw->GetRunMode() != DmpERunMode::kOriginal) || (not fEvtHeader->EnabledPeriodTrigger()) || (not fEvtHeader->GeneratedPeriodTrigger())){
    return false;
  }
  short nSignal = fBgoRaw->fGlobalDynodeID.size();
  for(short i=0;i<nSignal;++i){
    fBgoPedHist[fBgoRaw->fGlobalDynodeID[i]]->Fill(fBgoRaw->fADC[i]);
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgBgoPed::Finalize(){
  TF1 *gausFit = new TF1("GausFit","gaus",-500,500);
  std::string histFileName = gRootIOSvc->GetOutputPath()+gRootIOSvc->GetInputStem()+"_Hist.root";
  TFile *histFile = new TFile(histFileName.c_str(),"RECREATE");
  // create output txtfile
  std::string name = "BgoPed_"+gRootIOSvc->GetInputStem()+".txt";
  OutBgoPedData.open(name.c_str(),std::ios::out);
  fLastEvtTime = fEvtHeader->GetSecond();
  OutBgoPedData<<gRootIOSvc->GetInputFileName()<<std::endl;
  OutBgoPedData<<DmpTimeConvertor::Second2Date(fFirstEvtTime)<<"\t"<<fFirstEvtTime<<std::endl;
  OutBgoPedData<<DmpTimeConvertor::Second2Date(fLastEvtTime)<<"\t"<<fLastEvtTime<<std::endl;
  for(std::map<short,TH1D*>::iterator aHist=fBgoPedHist.begin();aHist!=fBgoPedHist.end();++aHist){
    // Fit and save output data
      double mean = aHist->second->GetMean(), sigma = aHist->second->GetRMS();
      for(short i = 0;i<3;++i){
        gausFit->SetRange(mean-2*sigma,mean+2*sigma);
        aHist->second->Fit(gausFit,"RQ");
        mean = gausFit->GetParameter(1);
        sigma = gausFit->GetParameter(2);
      }
      OutBgoPedData<<aHist->first<<std::setw(10)<<mean<<std::setw(10)<<sigma<<std::endl;
      if(sigma>30){
         DmpLogError<<"GID = "<<aHist->first<<"\tmean = "<<mean<<"\tsigma = "<<sigma<<DmpLogEndl;
      }
      aHist->second->Write();
      delete aHist->second;
  }
  OutBgoPedData.close();

  name = "PsdPed_"+gRootIOSvc->GetInputStem()+".txt";
  OutPsdPedData.open(name.c_str(),std::ios::out);

  delete histFile;
  return true;
}


