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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "SpecTclInterfaceControl.h"

#include <QMainWindow>
#include <memory>

// Forward declaration of UI class (generated by uic)
namespace Ui {
class MainWindow;
}


namespace Viewer
{

// Forward declarations
class SpectrumView;
class HistogramView;
class DockableGateManager;
class SpecTclInterface;
class ControlPanel;

/*!
 * \brief The MainWindow class
 *
 * This is main window. It handles the menus, the view, the control panel,
 * and the dockable widgets.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     *
     * \param parent - the parent widget
     */
    explicit MainWindow(QWidget *parent = 0);

    /*!
     * \brief Destructor
     */
    ~MainWindow();

    /*!
     * \brief Constructs the dockable widgets
     *
     * The dockable widgets are the gate manager (DockableGateManager)
     * and the histogram manager (HistogramView).
     */
    void createDockWindows();

    /*!
     * \brief Replace the SpecTclInterface with a new one
     *
     * \param pInterface the new interface
     *
     * This handles the connection of some of the signals and slots
     */
    void setSpecTclInterface(std::shared_ptr<SpecTclInterface> pInterface);


    ///////////////////////////////////////////////////////////////////////////

public slots:

    /*!
     * \brief onConfigure
     */
    void onConfigure();

    /*!
     * \brief Make the histogram view visible
     */
    void dockHistograms();

    /*!
     * \brief Make the gate manager visible
     */
    void dockGates();

    /*!
     * \brief Launch the dialog to create a new histogram
     */
    void onNewHistogram();

    ///////////////////////////////////////////////////////////////////////////

private:
    /*!
     * \brief Build the main window into a megawidget
     */
    void assembleWidgets();

    /*!
     * \brief Creates and adds the interface observers to the m_specTclControl
     */
    void addInterfaceObservers();

    /*!
     * \brief Connect the signals and slots
     */
    void connectSignalsAndSlots();

    /*!
     * \brief Construct a new spectcl interface
     */
    void constructSpecTclInterface();

    ///////////////////////////////////////////////////////////////////////////
private:
    std::unique_ptr<Ui::MainWindow>   pUI;
    SpectrumView                      *m_pView;         ///< viewer
    HistogramView                     *m_histView;      ///< dockable histogram widget
    DockableGateManager               *m_gateView;      ///< dockable gate widget
    ControlPanel                      *m_pControls;     ///< button panel
    SpecTclInterfaceControl           m_specTclControl; ///< owns unique SpecInterface
};



} // end of namespace
#endif // MAINWINDOW_H
