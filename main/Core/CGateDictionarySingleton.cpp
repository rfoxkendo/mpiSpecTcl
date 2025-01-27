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

#include "CGateDictionarySingleton.h"

CGateDictionary* CGateDictionarySingleton::m_pInstance(0);


CGateDictionary* 
CGateDictionarySingleton::getInstance() {
    if (!m_pInstance) {
        m_pInstance = new CGateDictionary;
    }
    return m_pInstance;
}