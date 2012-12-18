/*
 * freeglut_callbacks.c
 *
 * The callbacks setting methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <GL/freeglut.h>
#include "fg_internal.h"

/* -- INTERFACE FUNCTIONS -------------------------------------------------- */


/*
 * Global callbacks.
 */
/* Sets the global idle callback */
void FGAPIENTRY glutIdleFunc( FGCBIdle callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutIdleFunc" );
    fgState.IdleCallback = callback;
}

/* Creates a timer and sets its callback */
void FGAPIENTRY glutTimerFunc( unsigned int timeOut, FGCBTimer callback, int timerID )
{
    SFG_Timer *timer, *node;

    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutTimerFunc" );

    if( (timer = fgState.FreeTimers.Last) )
    {
        fgListRemove( &fgState.FreeTimers, &timer->Node );
    }
    else
    {
        if( ! (timer = malloc(sizeof(SFG_Timer))) )
            fgError( "Fatal error: "
                     "Memory allocation failure in glutTimerFunc()" );
    }

    timer->Callback  = callback;
    timer->ID        = timerID;
    timer->TriggerTime = fgElapsedTime() + timeOut;

    for( node = fgState.Timers.First; node; node = node->Node.Next )
    {
        if( node->TriggerTime > timer->TriggerTime )
            break;
    }

    fgListInsert( &fgState.Timers, &node->Node, &timer->Node );
}

/* Deprecated version of glutMenuStatusFunc callback setting method */
void FGAPIENTRY glutMenuStateFunc( FGCBMenuState callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutMenuStateFunc" );
    fgState.MenuStateCallback = callback;
}

/* Sets the global menu status callback for the current window */
void FGAPIENTRY glutMenuStatusFunc( FGCBMenuStatus callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutMenuStatusFunc" );
    fgState.MenuStatusCallback = callback;
}


/*
 * Menu specific callbacks.
 */
/* Callback upon menu destruction */
void FGAPIENTRY glutMenuDestroyFunc( FGCBDestroy callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutMenuDestroyFunc" );
    if( fgStructure.CurrentMenu )
        fgStructure.CurrentMenu->Destroy = callback;
}


/*
 * All of the window-specific callbacks setting methods can be generalized to this:
 */
#define SET_CALLBACK(a)                                         \
do                                                              \
{                                                               \
    if( fgStructure.CurrentWindow == NULL )                     \
        return;                                                 \
    SET_WCB( ( *( fgStructure.CurrentWindow ) ), a, callback ); \
} while( 0 )
/*
 * And almost every time the callback setter function can be implemented like this:
 */
#define IMPLEMENT_CALLBACK_FUNC_2NAME(a,b)                      \
void FGAPIENTRY glut##a##Func( FGCB##b callback )               \
{                                                               \
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glut"#a"Func" );    \
    SET_CALLBACK( b );                                          \
}
#define IMPLEMENT_CALLBACK_FUNC(a) IMPLEMENT_CALLBACK_FUNC_2NAME(a,a)

/* Implement all these callback setter functions... */
IMPLEMENT_CALLBACK_FUNC(Reshape);
IMPLEMENT_CALLBACK_FUNC(Position);
IMPLEMENT_CALLBACK_FUNC(Keyboard);
IMPLEMENT_CALLBACK_FUNC(KeyboardUp);
IMPLEMENT_CALLBACK_FUNC(Special);
IMPLEMENT_CALLBACK_FUNC(SpecialUp);
IMPLEMENT_CALLBACK_FUNC(Mouse);
IMPLEMENT_CALLBACK_FUNC(MouseWheel);
IMPLEMENT_CALLBACK_FUNC(Motion);
IMPLEMENT_CALLBACK_FUNC_2NAME(PassiveMotion,Passive);
IMPLEMENT_CALLBACK_FUNC(Entry);
/* glutWMCloseFunc is an alias for glutCloseFunc; both set the window's Destroy callback */
IMPLEMENT_CALLBACK_FUNC_2NAME(Close,Destroy);
IMPLEMENT_CALLBACK_FUNC_2NAME(WMClose,Destroy);
IMPLEMENT_CALLBACK_FUNC(OverlayDisplay);
IMPLEMENT_CALLBACK_FUNC(WindowStatus);
IMPLEMENT_CALLBACK_FUNC(ButtonBox);
IMPLEMENT_CALLBACK_FUNC(Dials);
IMPLEMENT_CALLBACK_FUNC(TabletMotion);
IMPLEMENT_CALLBACK_FUNC(TabletButton);
IMPLEMENT_CALLBACK_FUNC(MultiEntry);
IMPLEMENT_CALLBACK_FUNC(MultiButton);
IMPLEMENT_CALLBACK_FUNC(MultiMotion);
IMPLEMENT_CALLBACK_FUNC(MultiPassive);
IMPLEMENT_CALLBACK_FUNC(InitContext);
IMPLEMENT_CALLBACK_FUNC(Pause);
IMPLEMENT_CALLBACK_FUNC(Resume);



/*
 * Sets the Display callback for the current window
 */
void FGAPIENTRY glutDisplayFunc( FGCBDisplay callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutDisplayFunc" );
    if( !callback )
        fgError( "Fatal error in program.  NULL display callback not "
                 "permitted in GLUT 3.0+ or freeglut 2.0.1+" );
    SET_CALLBACK( Display );
}

/*
 * Sets the Visibility callback for the current window.
 */
static void fghVisibility( int status )
{
    int glut_status = GLUT_VISIBLE;

    FREEGLUT_INTERNAL_ERROR_EXIT_IF_NOT_INITIALISED ( "Visibility Callback" );
    freeglut_return_if_fail( fgStructure.CurrentWindow );

    if( ( GLUT_HIDDEN == status )  || ( GLUT_FULLY_COVERED == status ) )
        glut_status = GLUT_NOT_VISIBLE;
    INVOKE_WCB( *( fgStructure.CurrentWindow ), Visibility, ( glut_status ) );
}

void FGAPIENTRY glutVisibilityFunc( FGCBVisibility callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutVisibilityFunc" );
    SET_CALLBACK( Visibility );

    if( callback )
        glutWindowStatusFunc( fghVisibility );
    else
        glutWindowStatusFunc( NULL );
}

/*
 * Sets the joystick callback and polling rate for the current window
 */
void FGAPIENTRY glutJoystickFunc( FGCBJoystick callback, int pollInterval )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutJoystickFunc" );
    fgInitialiseJoysticks ();

    if ( ( ( fgStructure.CurrentWindow->State.JoystickPollRate < 0 ) ||
           !FETCH_WCB(*fgStructure.CurrentWindow,Joystick) ) &&  /* Joystick callback was disabled */
         ( callback && ( pollInterval >= 0 ) ) )               /* but is now enabled */
        ++fgState.NumActiveJoysticks;
    else if ( ( ( fgStructure.CurrentWindow->State.JoystickPollRate >= 0 ) &&
                FETCH_WCB(*fgStructure.CurrentWindow,Joystick) ) &&  /* Joystick callback was enabled */
              ( !callback || ( pollInterval < 0 ) ) )              /* but is now disabled */
        --fgState.NumActiveJoysticks;

    SET_CALLBACK( Joystick );
    fgStructure.CurrentWindow->State.JoystickPollRate = pollInterval;

    fgStructure.CurrentWindow->State.JoystickLastPoll =
        fgElapsedTime() - fgStructure.CurrentWindow->State.JoystickPollRate;

    if( fgStructure.CurrentWindow->State.JoystickLastPoll < 0 )
        fgStructure.CurrentWindow->State.JoystickLastPoll = 0;
}



/*
 * Sets the spaceball motion callback for the current window
 */
void FGAPIENTRY glutSpaceballMotionFunc( FGCBSpaceMotion callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSpaceballMotionFunc" );
    fgInitialiseSpaceball();

    SET_CALLBACK( SpaceMotion );
}

/*
 * Sets the spaceball rotate callback for the current window
 */
void FGAPIENTRY glutSpaceballRotateFunc( FGCBSpaceRotation callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSpaceballRotateFunc" );
    fgInitialiseSpaceball();

    SET_CALLBACK( SpaceRotation );
}

/*
 * Sets the spaceball button callback for the current window
 */
void FGAPIENTRY glutSpaceballButtonFunc( FGCBSpaceButton callback )
{
    FREEGLUT_EXIT_IF_NOT_INITIALISED ( "glutSpaceballButtonFunc" );
    fgInitialiseSpaceball();

    SET_CALLBACK( SpaceButton );
}

/*** END OF FILE ***/
