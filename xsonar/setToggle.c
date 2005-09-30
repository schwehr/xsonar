/*
 *    Set a toggle button state to True or False.
 */

#include <Xm/Xm.h>
#include <Xm/ToggleB.h> /* XmToggleButtonGetState */

#include "xsonar.h"

void setToggle(String toggleName, Widget shell, int status)
{

    Widget toggle;

    if((toggle = XtNameToWidget(shell, toggleName)) == NULL)
        return;

    XmToggleButtonSetState(toggle, status, False);

    return;
}

