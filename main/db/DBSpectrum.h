/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  DBSpectrum.h
 *  @brief: Defines database spectrum class.
 */

#ifndef DBSPECTRUM_H
#define DBSPECTRUM_H
#include <string>
#include <vector>

class CSqlite;

namespace SpecTcl {
    /**
     * @class DBSpectrum
     *     This class provides the ability to create and retrieve
     *     spectra from the database.  Spectra live in a saveset
     *     and consist of the following elements:
     *     *  Base specification:
     *        -  save set id.
     *        -  name
     *        -  Spectrum type.
     *        -  Spectrum datatype (defaults to long).
     *     *  One or more parameters the allowed number and interpretation of these
     *        depends on the spectrum type.
     *     *  One or more axes, the allowed number depends on the spectrum type:
     *        - low real coordinate limit.
     *        - high real coordinate limit
     *        - Number of bins.
     *        
     */
    class DBSpectrum {
    public:
        // Data structures:
        
        struct BaseInfo {
            int         s_id;       
            int         s_saveset;  
            std::string s_name;
            std::string s_type;
            std::string s_dataType;
            
            BaseInfo() {}
            BaseInfo(const BaseInfo& rhs) {
                copyIn(rhs);
            }
            BaseInfo& operator=(const BaseInfo& rhs) {
                if (&rhs != this) copyIn(rhs);
                return *this;
            }
            void copyIn(const BaseInfo& rhs) {
                s_id      = rhs.s_id;
                s_saveset = rhs.s_saveset;
                s_name    = rhs.s_name;
                s_type    = rhs.s_type;
                s_dataType= rhs.s_dataType;
            }
        };
        typedef std::vector<int> Parameters;
        struct Axis {
            int      s_id;
            double   s_low;
            double   s_high;
            int      s_bins;        // Default copy/assignment is fine.
        };
        typedef std::vector<Axis> Axes;
        // Now pulling it all together:
        
        struct Info {
            BaseInfo   s_base;
            Parameters s_parameters;
            Axes       s_axes;
            Info() {}
            Info(const Info& rhs) {
                copyIn(rhs);
            }
            Info& operator=(const Info& rhs) {
                if (&rhs != this) copyIn(rhs);
                return *this;
            }
            void copyIn(const Info& rhs) {
                s_base       = s_base;
                s_parameters = s_parameters;
                s_axes       = s_axes;
            }
        };
        private:
            CSqlite& m_conn;
            Info     m_Info;
        
        // public canonicals:
        
        public: 
            virtual ~DBSpectrum() {}
            
        //   Disallowed canonicals:
        
        private:
            DBSpectrum(const DBSpectrum&);
            DBSpectrum& operator=(const DBSpectrum&);
            int operator==(const DBSpectrum&);
            int operator!=(const DBSpectrum&);
        
        //   Internally used constructor:
        private:
            DBSpectrum(CSqlite& connection, const Info& info);
        
        //  static methods:
        
        public:
            DBSpectrum* create(
                CSqlite& connection, int sid, const char* name, const char* type,
                const std::vector<const char*>& parameterNames,
                const Axes& axes,
                const char* datatype="long"
            );
        private:
            Parameters fetchParameters(
                CSqlite& connection, int sid, const std::vector<const char*>& parameterNames
            );
            static void validateBaseInfo(
                CSqlite& connection, int sid, const BaseInfo& base
            );
            static void validateParameterCount(const char* spType, size_t n);
            static void validateAxisCount(const char* spType, size_t n);
            static void validateSpectrumType(const char* spType);
            static void validateDataType(const char* dtype);
            static void fetchInfo(CSqlite& connection, int sid, const char* name);
            static void enterSpectrum(CSqlite& connection, Info& info);
        
    };
}                                         // namespace SpecTcl.

#endif