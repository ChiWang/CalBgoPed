/*
 *  $Id: DmpAlgCalibrationPed.cc, 2015-03-02 14:44:50 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#include <stdio.h>
#include <boost/lexical_cast.hpp>

#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"

#include "DmpEvtBgoRaw.h"
#include "DmpEvtNudRaw.h"
#include "DmpAlgCalibrationPed.h"
#include "DmpDataBuffer.h"
#include "DmpParameterBgo.h"
#include "DmpParameterPsd.h"
#include "DmpLoadParameters.h"
#include "DmpBgoBase.h"
#include "DmpPsdBase.h"
#include "DmpCore.h"
#include "DmpTimeConvertor.h"

//-------------------------------------------------------------------
DmpAlgCalibrationPed::DmpAlgCalibrationPed()
 :DmpVAlg("DmpAlgCalibrationPed"),
  fBgoRaw(0),
  fPsdRaw(0),
  fNudRaw(0)
{
  gRootIOSvc->SetOutFileKey("CalPed");
}

//-------------------------------------------------------------------
DmpAlgCalibrationPed::~DmpAlgCalibrationPed(){
}

//-------------------------------------------------------------------
bool DmpAlgCalibrationPed::Initialize(){
  // read input data
  fBgoRaw = dynamic_cast<DmpEvtBgoRaw*>(gDataBuffer->ReadObject("Event/Rdc/Bgo"));
  if(0 == fBgoRaw){
    fBgoRaw = new DmpEvtBgoRaw();
    gDataBuffer->LinkRootFile("Event/Rdc/Bgo",fBgoRaw);
  }
  fPsdRaw = dynamic_cast<DmpEvtPsdRaw*>(gDataBuffer->ReadObject("Event/Rdc/Psd"));
  if(0 == fPsdRaw){
    fPsdRaw = new DmpEvtPsdRaw();
    gDataBuffer->LinkRootFile("Event/Rdc/Psd",fPsdRaw);
  }
  fNudRaw = dynamic_cast<DmpEvtNudRaw*>(gDataBuffer->ReadObject("Event/Rdc/Nud"));
  if(0 == fNudRaw){
    fNudRaw = new DmpEvtNudRaw();
    gDataBuffer->LinkRootFile("Event/Rdc/Nud",fNudRaw);
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

  layerNo = DmpParameterPsd::kPlaneNo*2;
  barNo = DmpParameterPsd::kStripNo+DmpParameterPsd::kRefStripNo;
  for(short l=0;l<layerNo;++l){
    for(short b=0;b<barNo;++b){
      for(short s=0;s<DmpParameterPsd::kSideNo;++s){
        for(short d=0;d<DmpParameterPsd::kDyNo;++d){
          char name[50];
          short gid_dy = DmpPsdBase::ConstructGlobalDynodeID(l,b,s,d*3+5);
          snprintf(name,50,"PsdPed_%05d-L%02d_S%02d_Dy%02d",gid_dy,l,b,s*10+d*3+5);
          fPsdPedHist.insert(std::make_pair(gid_dy,new TH1D(name,name,1000,0,1000)));
        }
      }
    }
  }

  for(int i=0;i<4;++i){
    char name[50];
    snprintf(name,50,"NudPed_channel%d",i);
    fNudPedHist.insert(std::make_pair(i,new TH1D(name,name,11000,-1000,10000)));
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgCalibrationPed::ProcessThisEvent(){
  if((fBgoRaw->GetRunMode() != DmpERunMode::kOriginal) || (not gCore->GetEventHeader()->EnabledPeriodTrigger()) || (not gCore->GetEventHeader()->GeneratedPeriodTrigger())){
    return false;
  }
  // bgo ped
  for(std::map<short,double>::iterator it=fBgoRaw->fADC.begin();it != fBgoRaw->fADC.end();++it){
    fBgoPedHist[it->first]->Fill(it->second);
  }
  // Psd ped
  for(std::map<short,double>::iterator it=fPsdRaw->fADC.begin();it != fPsdRaw->fADC.end();++it){
    fPsdPedHist[it->first]->Fill(it->second);
  }
  // nud ped
  for(short i=0;i<4;++i){
    fNudPedHist[i]->Fill(fNudRaw->fADC[i]);
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgCalibrationPed::Finalize(){
  TF1 *gausFit = new TF1("GausFit","gaus",-500,500);
  //std::string histFileName = gRootIOSvc->GetOutputPath()+gRootIOSvc->GetInputStem()+"_PedHist.root";
  TFile *histFile = gRootIOSvc->GetOutputRootFile();//new TFile(histFileName.c_str(),"RECREATE");

  // create output txtfile      BGO
  histFile->mkdir("Bgo");
  histFile->cd("Bgo");
  std::string name = "Bgo_"+gRootIOSvc->GetInputStem()+".ped";
  OutBgoPedData.open(name.c_str(),std::ios::out);
  OutBgoPedData<<Mark_S<<"\nFileName="<<gRootIOSvc->GetInputFileName()<<std::endl;
  OutBgoPedData<<"StartTime="<<gCore->GetTimeFirstOutput()<<"\nStopTime="<<gCore->GetTimeLastOutput()<<std::endl;
  OutBgoPedData<<Mark_D<<std::endl;
  int fitStat;
  for(std::map<short,TH1D*>::iterator aHist=fBgoPedHist.begin();aHist!=fBgoPedHist.end();++aHist){
    // Fit and save output data
      double mean = aHist->second->GetMean(), sigma = aHist->second->GetRMS();
      for(short i = 0;i<3;++i){
        gausFit->SetRange(mean-2*sigma,mean+2*sigma);
        fitStat = aHist->second->Fit(gausFit,"RQ");
        mean = gausFit->GetParameter(1);
        sigma = gausFit->GetParameter(2);
      }
      short l,b,s,d;
      DmpBgoBase::LoadLBSDID(aHist->first,l,b,s,d);
      OutBgoPedData<<aHist->first<<"\t"<<l<<"\t"<<b<<"\t"<<s<<"\t"<<d<<"\t"<<mean<<std::setw(10)<<sigma<<"\t\t"<<gausFit->GetChisquare()/ gausFit->GetNDF()<<"\t\t"<<fitStat<<std::endl;
      if(sigma>30){
         DmpLogError<<"Bgo\t\tID = "<<aHist->first<<"\tmean = "<<mean<<"\tsigma = "<<sigma<<DmpLogEndl;
      }
      aHist->second->Write();
      delete aHist->second;
  }
  OutBgoPedData<<Mark_N<<std::endl;
  OutBgoPedData.close();

  // create output txtfile      PSD
  histFile->mkdir("Psd");
  histFile->cd("Psd");
  name = "Psd_"+gRootIOSvc->GetInputStem()+".ped";
  OutPsdPedData.open(name.c_str(),std::ios::out);
  OutPsdPedData<<Mark_S<<"\nFileName="<<gRootIOSvc->GetInputFileName()<<std::endl;
  OutPsdPedData<<"StartTime="<<gCore->GetTimeFirstOutput()<<"\nStopTime="<<gCore->GetTimeLastOutput()<<std::endl;
  OutPsdPedData<<Mark_D<<std::endl;
  for(std::map<short,TH1D*>::iterator aHist=fPsdPedHist.begin();aHist!=fPsdPedHist.end();++aHist){
    // Fit and save output data
      double mean = aHist->second->GetMean(), sigma = aHist->second->GetRMS();
      for(short i = 0;i<3;++i){
        gausFit->SetRange(mean-2*sigma,mean+2*sigma);
        fitStat = aHist->second->Fit(gausFit,"RQ");
        mean = gausFit->GetParameter(1);
        sigma = gausFit->GetParameter(2);
      }
      short l,b,s,d;
      DmpPsdBase::LoadLBSDID(aHist->first,l,b,s,d);
      OutPsdPedData<<aHist->first<<"\t"<<l<<"\t"<<b<<"\t"<<s<<"\t"<<d<<"\t"<<mean<<std::setw(10)<<sigma<<"\t\t"<<gausFit->GetChisquare()/ gausFit->GetNDF()<<"\t\t"<<fitStat<<std::endl;
      if(sigma>30){
         DmpLogError<<"Psd\t\tGID = "<<aHist->first<<"\tmean = "<<mean<<"\tsigma = "<<sigma<<DmpLogEndl;
      }
      aHist->second->Write();
      delete aHist->second;
  }
  OutPsdPedData<<Mark_N<<std::endl;
  OutPsdPedData.close();

  // create output txtfile   Nud
  histFile->mkdir("Nud");
  histFile->cd("Nud");
  name = "Nud_"+gRootIOSvc->GetInputStem()+".ped";
  OutNudPedData.open(name.c_str(),std::ios::out);
  OutNudPedData<<Mark_S<<"\nFileName="<<gRootIOSvc->GetInputFileName()<<std::endl;
  OutNudPedData<<"StartTime="<<gCore->GetTimeFirstOutput()<<"\nStopTime="<<gCore->GetTimeLastOutput()<<std::endl;
  OutNudPedData<<Mark_D<<std::endl;
  for(std::map<short,TH1D*>::iterator aHist=fNudPedHist.begin();aHist!=fNudPedHist.end();++aHist){
    // Fit and save output data
      double mean = aHist->second->GetMean(), sigma = aHist->second->GetRMS();
      for(short i = 0;i<3;++i){
        gausFit->SetRange(mean-2*sigma,mean+2*sigma);
        fitStat = aHist->second->Fit(gausFit,"RQ");
        mean = gausFit->GetParameter(1);
        sigma = gausFit->GetParameter(2);
      }
      OutNudPedData<<aHist->first<<"\t"<<mean<<std::setw(10)<<sigma<<"\t\t"<<gausFit->GetChisquare()/ gausFit->GetNDF()<<"\t\t"<<fitStat<<std::endl;
      if(sigma>30){
         DmpLogError<<"Nud\t\tGID = "<<aHist->first<<"\tmean = "<<mean<<"\tsigma = "<<sigma<<DmpLogEndl;
      }
      aHist->second->Write();
      delete aHist->second;
  }
  OutNudPedData<<Mark_N<<std::endl;
  OutNudPedData.close();

  //delete histFile;
  return true;
}


