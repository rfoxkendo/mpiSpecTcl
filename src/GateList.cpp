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
#include "GateList.h"
#include "GSlice.h"
#include "GGate.h"

#include <GateInfo.h>

#include <algorithm>

using namespace std;

bool GateList::Compare1D::operator()(const std::unique_ptr<GSlice>& lhs,
                                    const std::unique_ptr<GSlice>& rhs) const 
{
  return (lhs->getName().compare(rhs->getName()))<0;
}

bool GateList::Compare2D::operator()(const std::unique_ptr<GGate>& lhs,
                                      const std::unique_ptr<GGate>& rhs) const 
{
  return (lhs->getName().compare(rhs->getName()))<0;
}

GateList::GateList()
    : m_cuts1d(),
    m_cuts2d()
{
}

void GateList::synchronize(std::vector<SpJs::GateInfo*> gates)
{
 // make sure that we add all non existing gates to the this
 for (auto pGate : gates) {
    
    SpJs::GateType type = pGate->getType();
    if (type == SpJs::SliceGate) {

        auto it = find1D(QString::fromStdString(pGate->getName()));

        // if we did not find it, then add it
        if ( it == end1d() ) {
            addCut1D(*pGate);
        }

    } else if ( type == SpJs::BandGate || type == SpJs::ContourGate ) {

        auto it = find2D(QString::fromStdString(pGate->getName()));

        // if we did not find it, then add it
        if (it == end2d() ) {
            SpJs::GateInfo2D& g2d = dynamic_cast<SpJs::GateInfo2D&>(*pGate);
            addCut2D(g2d);
        }
    }
  }


  // remove all gates in gate list that are no longer is spectcl

  // 1d cuts first
  auto it_1d = begin1d();
  auto itend_1d = end1d();
  while ( it_1d != itend_1d ) {
    QString name = (*it_1d)->getName();
    auto it = find_if(gates.begin(), gates.end(), [&name](SpJs::GateInfo* pInfo) {
                      return (name == QString::fromStdString(pInfo->getName()));
                      });
    if ( it == gates.end() ) {
      removeCut1D(name);
    }

    // increment!
    ++it_1d;
  }

  // 2d cuts next
  auto it_2d = begin2d();
  auto itend_2d = end2d();
  while ( it_2d != itend_2d ) {
    QString name = (*it_2d)->getName();
    auto it = find_if(gates.begin(), gates.end(), [&name](SpJs::GateInfo* pInfo) {
                        return (name == QString::fromStdString(pInfo->getName()));
                      });
  if ( it == gates.end() ) {
      removeCut2D(name);
    }

    // increment
    ++it_2d;
  }

}

void GateList::addCut1D(const SpJs::GateInfo& slice)
{
  const SpJs::Slice& jsSlice = dynamic_cast<const SpJs::Slice&>(slice);

  unique_ptr<GSlice> gsl(new GSlice(jsSlice));
  
  addCut1D( move(gsl) );
}

void GateList::addCut1D(unique_ptr<GSlice> slice)
{
  m_cuts1d.insert( move(slice) );
}

void GateList::addCut2D(const SpJs::GateInfo2D& gate)
{
  unique_ptr<GGate> ggate(new GGate(gate));
  
  addCut2D( move(ggate) );
}

void GateList::addCut2D(unique_ptr<GGate> gate)
{
  m_cuts2d.insert( move(gate) );
}

size_t GateList::size() const 
{
  return m_cuts1d.size() + m_cuts2d.size();
}


void GateList::removeCut1D(const QString& name)
{
  auto it = find1D(name);
  if (it != m_cuts1d.end()) {
    m_cuts1d.erase(it);
  }
}

void GateList::removeCut2D(const QString& name)
{}

GateList::iterator1d GateList::find1D(const QString& name)
{
  return find_if(m_cuts1d.begin(), m_cuts1d.end(), 
                  [&name](const unique_ptr<GSlice>& slice) {
                    return (name == slice->getName());
                  });
}

GateList::iterator2d GateList::find2D(const QString& name)
{
  return find_if(m_cuts2d.begin(), m_cuts2d.end(), 
                  [&name](const unique_ptr<GGate>& slice) {
                    return (name == slice->getName());
                  });
}
