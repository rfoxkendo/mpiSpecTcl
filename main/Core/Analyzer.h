/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

//  CAnalyzer.h:
//
//    This file defines the CAnalyzer class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
// Major Modification history:
//
// May 2001, Ron Fox
//  Cache event creation so that we don't have to create again and again.
//  Events are re-used from physics buffer to buffer.
//
/////////////////////////////////////////////////////////////

#ifndef ANALYZER_H  //Required for current class
#define ANALYZER_H

#include "EventList.h"
#include "BufferDecoder.h"
#include "EventSink.h"
#include <histotypes.h>


// Forward class declaration:
class CFilterEventProcessor;

class CAnalyzer {
  // Attributes:
  UInt_t          m_nEventThreshold; // Events processed before histogramming.
  UInt_t          m_nParametersInEvent;	// Parameters in an event.
  CEventList      m_EventList;
  CEventList      m_EventPool;
  CBufferDecoder* m_pDecoder;
  Bool_t          m_fAbort;
  Bool_t          m_fPartialEntity;

  CFilterEventProcessor* m_pFilterEventProcessor; // For event filtering.

 public:
  static UInt_t   m_nDefaultEventThreshold;
  static UInt_t   m_nDefaultParameterCount;

  // constructors destructors and other canonical member funtions.
 public:
  // Default constructor
  CAnalyzer() :
  m_nEventThreshold(CAnalyzer::m_nDefaultEventThreshold),  
    m_nParametersInEvent(CAnalyzer::m_nDefaultParameterCount),
    m_EventList(CAnalyzer::m_nDefaultEventThreshold),
    m_EventPool(),
    m_pDecoder(0),
    m_pFilterEventProcessor((CFilterEventProcessor*)kpNULL) // For event filtering.
    {};

  virtual ~CAnalyzer();       //Destructor

  // Constructor with arguments
  CAnalyzer(UInt_t am_nParametersInEvent,
	    UInt_t nThreshold = CAnalyzer::m_nDefaultEventThreshold);

  // Copy constructor
  CAnalyzer(const CAnalyzer& aCAnalyzer) {
    m_nEventThreshold    = aCAnalyzer.m_nEventThreshold;
    m_nParametersInEvent = aCAnalyzer.m_nParametersInEvent;
    CopyEventList(aCAnalyzer.m_EventList);
    CopyEventPool(aCAnalyzer.m_EventPool);
    m_pDecoder           = aCAnalyzer.m_pDecoder;
    if(m_pDecoder)
      m_pDecoder->OnAttach(*this);
  
  };

  // Operator= Assignment Operator
  CAnalyzer& operator=(const CAnalyzer& aCAnalyzer) {
    if (this == &aCAnalyzer) return *this;

    m_nEventThreshold    = aCAnalyzer.m_nEventThreshold;
    m_nParametersInEvent = aCAnalyzer.m_nParametersInEvent;
    CopyEventList(aCAnalyzer.m_EventList);
    CopyEventPool(aCAnalyzer.m_EventPool);

    // The various attached objects must be detached before assigned:
    DetachAll();
    m_pDecoder           = aCAnalyzer.m_pDecoder;
    if(m_pDecoder)
      m_pDecoder->OnAttach(*this);
    
    return *this;
  };

  // Operator== Equality Operator
  int operator==(const CAnalyzer& aCAnalyzer) {
    return (
	    (m_nEventThreshold    == aCAnalyzer.m_nEventThreshold)    &&
	    (m_nParametersInEvent == aCAnalyzer.m_nParametersInEvent) &&
	    (m_EventList          == aCAnalyzer.m_EventList)          &&
	    (m_pDecoder           == aCAnalyzer.m_pDecoder)   
	    );
  };

  // Selectors:
 public:
  UInt_t getEventThreshold() const {
    return m_nEventThreshold;
  }

  UInt_t getParametersInEvent() const {
    return m_nParametersInEvent;
  }

  CEventList& getEventList()  {
    return m_EventList;
  }

 CBufferDecoder* getDecoder() {
    return m_pDecoder;
  }

  CEventSink* getSink();

  // Mutators -- available only to derived objects:
 protected:
  void setEventThreshold(UInt_t am_nEventThreshold) {
    m_nEventThreshold = am_nEventThreshold;
  }

  void setParametersInEvent(UInt_t am_nParametersInEvent) { 
    m_nParametersInEvent = am_nParametersInEvent;
  }

  void setEventList(CEventList* am_EventList) {
    CopyEventList(*am_EventList);
  }

  void setDecoder(CBufferDecoder* am_pDecoder) {
    if(m_pDecoder)
      m_pDecoder->OnDetach(*this);
    m_pDecoder = am_pDecoder;
    if(m_pDecoder)
      m_pDecoder->OnAttach(*this);
  }

  void setEventSink(CEventSink* aSink);

  //  Public interface:
 public:
  virtual void OnBuffer(UInt_t nBytes, Address_t pData);
  virtual void OnStateChange(UInt_t nType, CBufferDecoder& rDecoder);
  virtual void OnPhysics(CBufferDecoder& rDecoder);
  virtual void OnScaler(CBufferDecoder& rDecoder);
  virtual void OnOther(UInt_t nType, CBufferDecoder& rDecoder);
  virtual void OnEndFile();
  virtual void OnInitialize();
  
  CBufferDecoder* AttachDecoder(CBufferDecoder& rDecoder);
  CBufferDecoder* DetachDecoder();
  void            entityNotDone();

  // Utility functions.
 protected:
  virtual UInt_t OnEvent(Address_t pRawData, CEvent& anEvent);

  void           AbortEvent() {
    m_fAbort = kfTRUE;
  }

  void CopyEventList(const CEventList& rhs);
  void CopyEventPool(const CEventList& rhs);
  void DetachAll();

  CEvent* CreateEvent() {
    CEventVector& rVec(m_EventPool.getVector());
    CEvent* pEvent;
    if(rVec.empty()) {
      pEvent =  new CEvent(m_nParametersInEvent);

    } else {
      pEvent = rVec.back();
      rVec.pop_back();
    }
    if(pEvent) pEvent->clear(); // BUG prevention.
    else pEvent= new CEvent(m_nParametersInEvent);
    return pEvent;
  }
  void ReturnEvent(CEvent* pEvent);

  void ClearEventList() {
    CEventVector& evlist(m_EventList.getVector());
    CEventVector& evpool(m_EventPool.getVector());
    CEventListIterator p = evlist.begin();
    for(; p != evlist.end(); p++) {
      if(*p) {
	CEvent* pEvent = *p;
	evpool.push_back(pEvent);
	*p = (CEvent*)kpNULL;
      }
    }
    // evlist.erase(m_EventList.begin(), m_EventList.end());
  }

};

#endif
