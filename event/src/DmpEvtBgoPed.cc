/*   $Id: DmpEvtBgoPed.cc, 2014-10-13 11:36:42 DAMPE $
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
DmpEvtBgoPed& DmpEvtBgoPed::operator=(const DmpEvtBgoPed &r){
  Reset();
  GlobalDynodeID = r.GlobalDynodeID;
  Mean = r.Mean;
  Sigma = r.Sigma;
}

//-------------------------------------------------------------------
void DmpEvtBgoPed::LoadFrom(DmpEvtBgoPed *r){
  Reset();
  GlobalDynodeID = r->GlobalDynodeID;
  Mean = r->Mean;
  Sigma = r->Sigma;
}

//-------------------------------------------------------------------
DmpEvtBgoPed::~DmpEvtBgoPed(){
}

//-------------------------------------------------------------------
void DmpEvtBgoPed::Reset(){
  GlobalDynodeID.clear();
  Mean.clear();
  Sigma.clear();
}


