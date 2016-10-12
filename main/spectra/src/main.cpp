//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
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

#include "CmdLineOptions.h"
#include "MainWindow.h"
#include "QHistInfo.h"
#include "GlobalSettings.h"

#include <TQApplication.h>
#include <TQRootApplication.h>
#include <TEnv.h>
#include <TStyle.h>

#include <QStringList>
#include <QCoreApplication>
#include <QHostInfo>
#include <QHostAddress>

#include <iostream>
#include <algorithm>
#include <iterator>
#include <list>


using namespace Viewer;

bool serverIsOnThisMachine(QString serverName);


/*! \brief The Main function  */
int main(int argc, char *argv[])
{

  CmdLineOptions opts;
  opts.parse(argc, argv);

  Q_INIT_RESOURCE(resources);

  QApplication::setGraphicsSystem("native");

  // Set some default values for ROOT
  gEnv->SetValue("Unix.*.Root.UseTTFonts",true);
  gStyle->SetOptStat(0); // this is not useful at the moment. I can do it better using Qt widgets

  if (opts.disableTrueTypeFonts()) {
      gEnv->SetValue("Unix.*.Root.UseTTFonts",false);
  }

  if (serverIsOnThisMachine(opts.getHost())) {
      std::cout << "Starting local session" << std::endl;
      GlobalSettings::setSessionMode(GlobalSettings::LOCAL);
  } else {
      std::cout << "Starting remote session" << std::endl;
      GlobalSettings::setSessionMode(GlobalSettings::REMOTE);
  }
  GlobalSettings::setServerHost(opts.getHost());
  GlobalSettings::setServerPort(opts.getPort());
  GlobalSettings::setPollInterval(5000);

  // start Qt AND ROOT event loops... yes we need both
  TQApplication qtEventLoop("app", &argc, argv);
  TQRootApplication rootEventLoop(argc, argv, 0);

  MainWindow w;
  w.show();

  return rootEventLoop.exec();
}


/*!
 * \brief Check to see if REST server is local
 *
 * The basic premise is that we compare all of the IP addresses associated
 * with the local machine with those of the machine specified. If there is
 * overlap between these lists of IP addresses, then the REST server is
 * local. Otherwise, it is remote.
 *
 * \param serverName    name of machine REST server is running
 *
 * \return boolean indicating whether REST server is on localhost
 */
bool serverIsOnThisMachine(QString serverName)
{
    using namespace std;

    // get all IP addresses of localhost machine
    QHostInfo thisMachine = QHostInfo::fromName("localhost");
    QList<QHostAddress> thisMachineAddresses = thisMachine.addresses();

    // localhost will likely return different ip addresses than resolving
    // the ip addresses associated with "hostname".
    thisMachine = QHostInfo::fromName(QHostInfo::localHostName());
    QList<QHostAddress> moreThisAddresses = thisMachine.addresses();
    thisMachineAddresses.append(moreThisAddresses);

    // get all IP addresses of server
    QHostInfo thatMachine = QHostInfo::fromName(serverName);
    QList<QHostAddress> thatMachineAddresses = thatMachine.addresses();

    // if these two sets of addresses overlap, then we have the same machine
    auto comparison = [](const QHostAddress& h0, const QHostAddress& h1) {
        return (h0.toString() < h1.toString());
    };

    // set_intersection requires that the lists are ordered.
    sort(thisMachineAddresses.begin(), thisMachineAddresses.end(), comparison);
    sort(thatMachineAddresses.begin(), thatMachineAddresses.end(), comparison);

    list<QHostAddress> intersection;
    auto last = set_intersection(thisMachineAddresses.begin(), thisMachineAddresses.end(),
                                 thatMachineAddresses.begin(), thatMachineAddresses.end(),
                                 back_inserter(intersection), comparison);

    return (intersection.size()>0);

}
