/*   $Id: DmpEvtBgoPed.h, 2014-10-13 11:36:41 DAMPE $
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
  ~DmpEvtBgoPed();
  void Reset();
  DmpEvtBgoPed &operator=(const DmpEvtBgoPed &r);
  void LoadFrom(DmpEvtBgoPed *r);

public:
  std::vector<short>    GlobalDynodeID;
  std::vector<double>   Mean;
  std::vector<double>   Sigma;
  
  ClassDef(DmpEvtBgoPed,1)

};

#endif

