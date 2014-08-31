/*   $Id: DmpEvtBgoPed.h, 2014-08-31 23:19:53 DAMPE $
 *--------------------------------------------------------
 *  Author(s):
 *    Chi WANG  (chiwang@mail.ustc.edu.cn) 31/08/2014
*/

#ifndef DmpEvtBgoPed_H
#define DmpEvtBgoPed_H

#include "TObject.h"

class DmpEvtBgoPed : public TObject{
/*
 *  DmpEvtBgoPed
 *
 */
public:
  DmpEvtBgoPed();
  DmpEvtBgoPed(const DmpEvtBgoPed &r);
  DmpEvtBgoPed(const DmpEvtBgoPed *&r);
  ~DmpEvtBgoPed();

  void Reset();
  //void SetUsedFileName(const std::string &n){fUsedFile = n;}
  //void SetStartTime(const int &t){fStartTime = t;}
  //void SetStopTime(const int &t){fStopTime = t;}
  //void LoadTimeRange(int &b,int &e)const{b=fStartTime;e=fStopTime;}
  //std::string UsedFile()const{return fUsedFile;}

//private:
public:
  std::string   UsedFileName;      // file name of raw data
  int   StartTime;     // the time of the first event used to cal. ped
  int   StopTime;      // 
  std::vector<short>    GlobalDynodeID;
  std::vector<float>    Mean;
  std::vector<float>    Sigma;
  
  ClassDef(DmpEvtBgoPed,1)

};

#endif
