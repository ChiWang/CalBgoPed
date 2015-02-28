/*
 *  $Id: DmpAlgBgoPed.h, 2014-09-03 10:54:21 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgBgoPed_H
#define DmpAlgBgoPed_H

#include <map>
#include <fstream>
#include "DmpVAlg.h"

class DmpEvtHeader;
class DmpEvtBgoRaw;
class TH1D;

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
  std::map<short,TH1D*>  fBgoPedHist;       // key is global dynode ID
  std::ofstream         OutBgoPedData;      //
  std::ofstream         OutPsdPedData;      //
  int                   fFirstEvtTime;      // unit second
  int                   fLastEvtTime;       // unit second

};

#endif

