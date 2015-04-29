//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2015.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins 
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
#include "HistogramView.h"
#include "ui_HistogramView.h"
#include "ListRequestHandler.h"
#include "HistogramList.h"
#include "HistogramBundle.h"
#include "SpecTclRESTInterface.h"
#include "QHistInfo.h"

#include <HistFactory.h>

#include <TH1.h>

#include <QTimer>
#include <QList>
#include <QListWidgetItem>

#include <vector>
#include <iostream>

using namespace std;

namespace Viewer
{

HistogramView::HistogramView(SpecTclInterface* pSpecTcl, QWidget *parent) :
    QDockWidget(tr("Histograms"),parent),
    ui(new Ui::HistogramView),
    m_req(new ListRequestHandler(this)),
    m_pSpecTcl(pSpecTcl)
{
    ui->setupUi(this);


    connect(m_pSpecTcl, SIGNAL(histogramListChanged()),
            this, SLOT(onHistogramListChanged()));

    connect(ui->histList,SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(onDoubleClick(QModelIndex)));
}

HistogramView::~HistogramView()
{
  delete ui;
}

void HistogramView::onHistogramListChanged()
{
  auto pHistList = m_pSpecTcl->getHistogramList();

  auto it = pHistList->begin();
  auto itend = pHistList->end();

  // add new histograms if they have changed
  while (it!=itend) {

      const QString& name = it->first;
      if (! histExists(name)) {
          // Histograms are uniquely named, so we can use the name as the key
          auto item = new QListWidgetItem(name, ui->histList,
                                          QListWidgetItem::UserType);

          // store a point to the histogram bundle
          item->setData(Qt::UserRole,
                        QVariant::fromValue<void*>(reinterpret_cast<void*>(it->second.get())));

          QSize geo = ui->histList->size();
          ui->histList->insertItem(geo.height(), item);
      } else {
          // the value already exists...get the ListWidgetItem associated with it
          auto items = ui->histList->findItems(name, Qt::MatchExactly);
          // make sure we found something
          if ( items.size() == 1 ) {
              // get the first and only item found
              auto pItem = items.at(0);

              pItem->setData(Qt::UserRole,
                             QVariant::fromValue<void*>(reinterpret_cast<void*>(it->second.get())));
              setIcon(pItem);
          }
      }

    ++it;
  }

  // now remove stale items

  int nRows = ui->histList->count();
  for (int row=nRows-1; row>=0; --row) {

      auto pItem = ui->histList->item(row);

      auto text = pItem->text();

      auto compareName = [&text](const pair<const QString, unique_ptr<HistogramBundle> >& prBundle) {
                            return (text == prBundle.second->getName());
                          };

      auto itFound = find_if( pHistList->begin(), pHistList->end(), compareName );
      if ( itFound == pHistList->end() ) {
          delete (ui->histList->takeItem(row));
      }
  }

  cout << "HistogramView::onHistogramListChanged()" << endl;
}

void HistogramView::setIcon(QListWidgetItem *pItem)
{
  return;
}

void HistogramView::setList(std::vector<SpJs::HistInfo> names)
{
    SpJs::HistFactory factory;

    auto iter = names.begin();
    auto end = names.end();

    while (iter!=end) {

        SpJs::HistInfo& info = (*iter);
        QString name = QString::fromStdString(info.s_name);
        if (!histExists(name)) {

            // this should  get a unique ptr, we want to get the raw pointer
            // and strip the unique_ptr of its ownership (i.e. release())
            auto upHist = factory.create(info);

            auto pHistList = m_pSpecTcl->getHistogramList();
            auto pHist = pHistList->addHist(std::move(upHist), info);

            // a new hist needs to find the necessary
            pHist->synchronizeGates(m_pSpecTcl->getGateList());

            // Histograms are uniquely named, so we can use the name as the key
            QString name = QString::fromStdString((*iter).s_name);
            auto item = new QListWidgetItem(name, ui->histList, 
                                            QListWidgetItem::UserType);

            item->setData(Qt::UserRole,QVariant(name));

            QSize geo = ui->histList->size();
            ui->histList->insertItem(geo.height(), item);

        }

      ++iter;
    }

}

void HistogramView::onDoubleClick(QModelIndex index)
{
    auto pHistBundle = reinterpret_cast<HistogramBundle*>(index.data(Qt::UserRole).value<void*>());
    emit histSelected(pHistBundle);
}


bool HistogramView::histExists(const QString& name)
{
    size_t nRows = ui->histList->count();
    for(size_t  entry=0; entry<nRows; ++entry) {
        QListWidgetItem* item = ui->histList->item(entry);
        if (item->text() == name) {
            return true;
        }
    }
    return false;
}

void HistogramView::deleteHists()
{
    size_t nEntries = ui->histList->count();
    for(size_t  entry=0; entry<nEntries; ++entry) {
        QListWidgetItem* item = ui->histList->item(entry);
        TH1* hist = (TH1*)(item->data(Qt::UserRole).value<void*>());
        delete hist;
    }
}

} // end of namespace
