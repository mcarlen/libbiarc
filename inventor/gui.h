#ifndef _GUI_H_
#define _GUI_H_

#include "main.h"

SbBool myAppEventHandler(void *userData, QEvent *anyevent);

#ifdef XXX
static void motionfunc(void *data, SoEventCallback *eventCB);
static void mousefunc(void *data, SoEventCallback *eventCB);
#endif

#endif /* _GUI_H_ */
