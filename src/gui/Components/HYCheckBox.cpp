/*
 
 HyPhy - Hypothesis Testing Using Phylogenies.
 
 Copyright (C) 1997-now
 Core Developers:
 Sergei L Kosakovsky Pond (sergeilkp@icloud.com)
 Art FY Poon    (apoon@cfenet.ubc.ca)
 Steven Weaver (sweaver@temple.edu)
 
 Module Developers:
 Lance Hepler (nlhepler@gmail.com)
 Martin Smith (martin.audacis@gmail.com)
 
 Significant contributions from:
 Spencer V Muse (muse@stat.ncsu.edu)
 Simon DW Frost (sdf22@cam.ac.uk)
 
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 */

#include "HYEventTypes.h"
#include "HYLabel.h"
#include "HYGraphicPane.h"

#ifdef    __HYPHYDMALLOC__
#include "dmalloc.h"
#endif
//__________________________________________________________________

_HYCheckbox::_HYCheckbox (_HYRect r,Ptr p, bool isR):_HYLabel (r,p), _HYPlatformCheckbox (isR)
{
    checkState   = false;
    isEnabled    = true;
    checkSpacing = 5;
}

//__________________________________________________________________

_HYCheckbox::~_HYCheckbox()
{
}

//__________________________________________________________________

void    _HYCheckbox::SetState(bool ns, bool sendMessage)
{
    if (checkState!=ns) {
        checkState = ns;
        _SetState (ns);
        if (sendMessage&&messageRecipient) {
            messageRecipient->ProcessEvent (generateButtonPushEvent (GetID(), ns));
        }
    }
}

//__________________________________________________________________

void    _HYCheckbox::SetSpacing (long ns)
{
    if (checkSpacing!=ns) {
        checkSpacing = ns;
        SetVisibleSize (rel);
    }
}

//__________________________________________________________________

void    _HYCheckbox::Enable (bool e)
{
    if (isEnabled!=e) {
        isEnabled = e;
        _Enable (e);
    }
}


//__________________________________________________________________
void        _HYCheckbox::SetVisibleSize  (_HYRect rel)
{
    _HYComponent::SetVisibleSize (rel);
    _HYPlatformCheckbox::_SetVisibleSize (rel);
}