#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2009.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321



package require Tk
package require snit

package provide treeParameterEditor 1.0

##
#  Provide a snit megawidget that allows for the display of a single
#  tree parameter.
#  The tree parameter looks like this:
#  +------------------------------------------------------------------------------------+
#  | [parameter name] [low value]  [high value] [units] <Load> <Set> <Change Spectra>   |
#  +------------------------------------------------------------------------------------+
#
#
#  OPTIONS:
#     -name      name of parameter to have in spectrum.
#     -low       get/set low value of parameter in widget.
#     -high      get/set high value of parameter in widget.
#     -units     get/set units of parameter in widget.
#     -loadcmd   Script executed on load button click, supports %W substitution
#     -setcmd    Script executed on  set button click supports %W  substitution.
#     -changecmd Script executed on Change spectra button click supports %W substitution.
#
# METHODS:
#   resetChanged   - Turn off changed flag (this can be done by save).
#
#

snit::widget treeParameterEditor {
    hulltype ttk::frame

    option -name
    option -low
    option -high
    option -units
    option -loadcmd   [list]
    option -setcmd    [list]
    option -changecmd [list]
    option -title     false;	# If true titles are put above the text entries.


    ##
    # Constructor -- see the summary comments above the class.
    # @param args - an option/value list.
    #

    constructor args {
	$self configurelist $args

	# Build the widgets:

	set editorRow 0
	if {$options(-title)} {
	    ttk::label $win.n -text Name -borderwidth 3
	    ttk::label $win.l -text Low  -borderwidth 3
	    ttk::label $win.h -text High -borderwidth 3
	    ttk::label $win.u -text Units -borderwidth 3

	    grid $win.n $win.l $win.h $win.u
	    set editorRow 1
	}

	# First the labels:

	foreach label [list .name .low .high .unit] optionname [list -name -low -high -units] \
	    width [list 32 5 5 10] {
	    ::ttk::entry $win$label -textvariable ${selfns}::options($optionname) \
		-width $width
	}

	# then the buttons:

	foreach button [list .load .set .changespectra] \
	    label [list Load Set "Change Spectra"]  \
	    option [list -loadcmd -setcmd -changecmd] width [list 6 6 14] {
		::ttk::button $win$button -text $label -width $width \
		    -command [mymethod callback $option]
	}

	#  Now grid them from left to right:
	
	set col 0


	foreach widget [list .name .low .high .unit .load .set .changespectra] {
	    grid $win$widget -row $editorRow -column $col -sticky ns
	    incr col
	}
    }
    ##
    # Dispatch a button push to the approprate option command.
    # prior to dispatch all occurences of %W in the command are replaced by
    # $win (our top level widgte name).  The callback is executed at global level 
    # (uplevel #0).
    #
    # @param optionName -name of the option holding the script to execute.
    #
    method callback optionName {
	set script $options($optionName)
	
	# empty scripts are no-ops.

	if {$script ne ""} {
	    
	    # Substitute for %W:

	    regsub -all {%W} $script $win script
	    uplevel #0 $script
	}
    }

}