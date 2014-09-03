/*   $Id: DmpEvtBgoPed.cc, 2014-08-31 23:23:25 DAMPE $
 *--------------------------------------------------------
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustd.edu.cn)
 *--------------------------------------------------------
*/

#include "DmpEvtBgoPed.h"

ClassImp(DmpEvtBgoPed)

//-------------------------------------------------------------------
DmpEvtBgoPed::DmpEvtBgoPed(){
  Reset();
}

//-------------------------------------------------------------------
DmpEvtBgoPed::DmpEvtBgoPed(const DmpEvtBgoPed &r){
  Reset();
  UsedFileName = r.UsedFileName;
  StartTime = r.StartTime;
  StopTime = r.StopTime;
  short n = GlobalDynodeID.size();
  for(size_t i = 0;i<n;++i){
    GlobalDynodeID.push_back(r.GlobalDynodeID[i]);
    Mean.push_back(r.Mean[i]);
    Sigma.push_back(r.Sigma[i]);
  }
}

//-------------------------------------------------------------------
DmpEvtBgoPed::DmpEvtBgoPed(const DmpEvtBgoPed *&r){
  Reset();
  UsedFileName = r->UsedFileName;
  StartTime = r->StartTime;
  StopTime = r->StopTime;
  short n = GlobalDynodeID.size();
  for(size_t i = 0;i<n;++i){
    GlobalDynodeID.push_back(r->GlobalDynodeID[i]);
    Mean.push_back(r->Mean[i]);
    Sigma.push_back(r->Sigma[i]);
  }
}

//-------------------------------------------------------------------
DmpEvtBgoPed::~DmpEvtBgoPed()
{
}

//-------------------------------------------------------------------
void DmpEvtBgoPed::Reset()
{
  UsedFileName="NO";
  StartTime = 0;
  StopTime  =0xafffffff;
  GlobalDynodeID.clear();
  Mean.clear();
  Sigma.clear();
}


