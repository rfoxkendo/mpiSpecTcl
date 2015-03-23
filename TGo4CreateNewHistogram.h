// $Id: TGo4CreateNewHistogram.h 489 2009-11-04 12:20:26Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4CREATENEWHISTOGRAM_H
#define TGO4CREATENEWHISTOGRAM_H


#include <QDialog>
#include "ui_TGo4CreateNewHistogram.h"

class QNetworkAccessManager;
class QNetworkReply;

class TGo4CreateNewHistogram : public QDialog, public Ui::TGo4CreateNewHistogram
 {
     Q_OBJECT

 public:
   TGo4CreateNewHistogram( QWidget* parent = 0);

 public slots:

   virtual void createRemoteHist();
   void showYAxisWidgets(int);
   void onCreateDone(QNetworkReply*);

private:
   void encodeRequest();

private:
   QNetworkAccessManager *m_pNAM;

 };
#endif
