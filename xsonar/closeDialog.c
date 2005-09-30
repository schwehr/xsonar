/*
 *   Function to close a dialog routine.
 *   Widget w is the parent dialog, and error indicates
 *   whether or not to shut down the processing (True == yes).
 *   Part of the Xsonar package.
 *
 *   4/1/93
 *
 *   William W. Danforth
 *   U.S. Geological Survey
 *   Woods Hole, MA 02543
 */

#include "xsonar.h"

#include "sonar_struct.h"

void closeDialog(MainSonar *sonar, Widget w, int error)
{

  /*void setToggle();*/

     Widget parent_widget;

     parent_widget = w;

    /*
     *    If the operation was cancelled or an error occured,
     *    reset the processing flags to 0.
     */

    if(error)
        {
        sonar->slant_flag = 0;
        setToggle("*SlantRangeToggle", sonar->toplevel, False);

        sonar->destripe_flag = 0;
        setToggle("*DestripeToggle", sonar->toplevel, False);

        sonar->beam_flag = 0;
        setToggle("*BeamPatternToggle", sonar->toplevel, False);

        sonar->linear_flag = 0;
        setToggle("*LinearStretchToggle", sonar->toplevel, False);

        sonar->equalize_flag = 0;
        setToggle("*EqualizationToggle", sonar->toplevel, False);
        }

/*
     while(!xtIsTransientShell(parent_widget))
          parent_widget = XtParent(parent_widget);
*/


    XtPopdown(parent_widget);
    XtDestroyWidget(parent_widget);

}

