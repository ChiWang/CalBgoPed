/*
 *  $Id: DmpAlgCalibrationPed.h, 2014-09-03 10:54:21 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgCalibrationPed_H
#define DmpAlgCalibrationPed_H

#include <map>
#include <fstream>
#include "DmpVAlg.h"
#include "DmpEvtPsdRaw.h"

class DmpEvtBgoRaw;
//class DmpEvtPsdRaw;
class DmpEvtNudRaw;
class TH1D;

class DmpAlgCalibrationPed : public DmpVAlg{
/*
 *  DmpAlgCalibrationPed
 *
 */
public:
  DmpAlgCalibrationPed();
  ~DmpAlgCalibrationPed();

  //void Set(const std::string &type,const std::string &value);
  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

private:
  DmpEvtBgoRaw          *fBgoRaw;
  DmpEvtPsdRaw          *fPsdRaw;
  DmpEvtNudRaw          *fNudRaw;

  std::map<short,TH1D*>  fBgoPedHist;       // key is global dynode ID
  std::ofstream         OutBgoPedData;      //
  std::map<short,TH1D*>  fPsdPedHist;       // key is global dynode ID
  std::ofstream         OutPsdPedData;      //
  std::map<short,TH1D*>  fNudPedHist;       // key is global dynode ID
  std::ofstream         OutNudPedData;      //

};

#endif

