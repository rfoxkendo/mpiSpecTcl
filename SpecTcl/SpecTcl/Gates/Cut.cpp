//  CCut.cpp
// Encapsulates a cut on a parameter.
// The cut is described by a high and low limit 
// in parameter space.

//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "Cut.h"                               
#include "SingleItemIterator.h"
#include <stdio.h>

static const char* Copyright = 
"CCut.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CCut

Bool_t
CCut::operator()(CEvent& rEvent)
{
  vector<UInt_t> Params;
  if(!wasChecked()) {
    Params.push_back(getId());
    Set(inGate(rEvent, Params));
  }

  return getCachedValue();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaluator
//
Bool_t
CCut::inGate(CEvent& rEvent, vector<UInt_t>& Params)
// Returns the evaluation of the gate on this
// an event.
//
// Formal Parameters:
//    CEvent& rEvent:
//      Event on which the gate will be evaluated.
//    vector<UInt_t>& Params
//      vector of parameters in the gate (empty)
{
  if(Params[0] >= rEvent.size()) {
    return kfFALSE;
  }
  else {
    UInt_t nPoint = rEvent[Params[0]];
    return((nPoint >= getLow()) && (nPoint <= getHigh()));
  }
  
  return kfFALSE;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     Construction
//
CGate* 
CCut::clone() 
{
// Returns a pointer to a gate which is a copy of
// the current gate.

  return new CCut(*this);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator Begin (  )
//  Operation Type:
//     Selector
//
CConstituentIterator 
CCut::Begin() 
{
// Returns an iterator which represents the
// start of the constituent list.

  CSingleItemIterator It;
  return CConstituentIterator(It);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator& End (  )
//  Operation Type:
//     selector
//
CConstituentIterator
CCut::End() 
{
// Returns an iterator which represents the end of the 
// constituent list.
// Exceptions:  

  CConstituentIterator p = Begin();
  p++;
  return p;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector
//
UInt_t 
CCut::Size() 
{
// Returns the number of constituents (1)
// Exceptions:  

  return 1;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetConstituent (  )
//  Operation Type:
//     Selector.
//
std::string 
CCut::GetConstituent(CConstituentIterator& rIterator) 
{
// Returns a text encoded version of the 
// constituent 'pointed to' by the iterator.
// 

  if(rIterator != End()) {
    char Text[100];
    sprintf(Text, "%d %d %d", m_nId, m_nLow, m_nHigh);
    return std::string(Text);
  }
  else {
    return std::string("");
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     Selector.
//
std::string 
CCut::Type() const
{
// Returns the type of gate.  In this case
// a single character string "s" for slice.  Note that
// "c" is used by contour.
// Exceptions:  

  return std::string("s");
}
