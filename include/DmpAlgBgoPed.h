/*
 *  $Id: DmpAlgBgoPed.h, 2014-09-03 10:54:21 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgBgoPed_H
#define DmpAlgBgoPed_H

#include "DmpVAlg.h"

class DmpEvtHeader;
class DmpEvtBgoRaw;
class DmpEvtBgoPed;
class TH1F;

class DmpAlgBgoPed : public DmpVAlg{
/*
 *  DmpAlgBgoPed
 *
 */
public:
  DmpAlgBgoPed();
  ~DmpAlgBgoPed();

  //void Set(const std::string &type,const std::string &value);
  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

private:
  DmpEvtHeader          *fEvtHeader;
  DmpEvtBgoRaw          *fBgoRaw;
  DmpEvtBgoPed          *fBgoPed;
  std::map<short,TH1F*>  fPedHist;          // key is global dynode ID

};

#endif

