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

#include "HYComponent.h"
#include "HYEventTypes.h"
#include "HYCanvas.h"
#include "HYUtils.h"
#include "HYPlatformWindow.h"
#include "math.h"

#ifdef    __HYPHYDMALLOC__
#include "dmalloc.h"
#endif
//__________________________________________________________________

_HYComponent::_HYComponent(void)
{
    hOrigin = vOrigin = 0;
    hSize = vSize = 0;
    needUpdate = false;
    messageRecipient = nil;
}

//__________________________________________________________________

_HYComponent::_HYComponent(_HYRect r,Ptr p):_HYPlatformComponent (r,p)
{
    settings.width = r.width;
    SetDimensions (r,r);
    hSize = r.left;
    vSize = r.top;
    messageRecipient = nil;
}

//__________________________________________________________________

_HYComponent::~_HYComponent(void)
{
    if (nInstances<=1) {
        _CleanUp();
    }
}

//__________________________________________________________________

BaseRef     _HYComponent::makeDynamic(void)
{
    _HYComponent* res = new _HYComponent;
    res->Duplicate(this);
    nInstances++;
    return res;
}

//__________________________________________________________________
void        _HYComponent::Duplicate (BaseRef s)
{
    _HYComponent * theS = (_HYComponent*)s;
    theS->_HYPlatformComponent::Duplicate(s);
    settings = theS->settings;
    hOrigin = theS->hOrigin;
    vOrigin = theS->vOrigin;
    hSize = theS->hSize;
    vSize = theS->vSize;
    needUpdate = theS->needUpdate;
}

//__________________________________________________________________
int         _HYComponent::GetMinW (void)
{
    return settings.left;
}

//__________________________________________________________________
int         _HYComponent::GetMaxW (void)
{
    return settings.right;
}

//__________________________________________________________________
int         _HYComponent::GetMaxLW (void)
{
    return GetMaxW();
}

//__________________________________________________________________
int         _HYComponent::GetMaxH (void)
{
    return settings.bottom;
}

//__________________________________________________________________

int         _HYComponent::GetMinH (void)
{
    return settings.top;
}

//__________________________________________________________________
int         _HYComponent::GetMaxLH (void)
{
    return GetMaxH();
}

//__________________________________________________________________

int     _HYComponent::GetHSize (void)
{
    return hSize;
}

//__________________________________________________________________

int     _HYComponent::GetVSize (void)
{
    return vSize;
}

//__________________________________________________________________

bool        _HYComponent::IsHElastic (void)
{
    return settings.left!=settings.right;
}

//__________________________________________________________________

bool        _HYComponent::IsVElastic (void)
{
    return settings.top!=settings.bottom;
}

//__________________________________________________________________

void        _HYComponent::SetDimensions (_HYRect r,_HYRect rel)
{
    int saveSettings = settings.width;
    hOrigin = vOrigin = 0;
    hSize = rel.right-rel.left;
    vSize = rel.bottom-rel.top;
    needUpdate = true;
    settings = r;
    settings.width = saveSettings;
    _SetDimensions (r,rel);
}

//__________________________________________________________________
void        _HYComponent::SetOrigin (int nv,int nh)
{
    int m = GetMaxW();
    if (nh+hSize>m) {
        nh = m-hSize;
    }

    if (nh<0) {
        nh = 0;
    }
    needUpdate = false;
    if (hOrigin != nh) {
        needUpdate = true;
        hOrigin = nh;
    }
    m = GetMaxH();
    if (nv+vSize>m) {
        nv = m-vSize;
    }
    if (nv<0) {
        nv = 0;
    }
    if (vOrigin != nv) {
        needUpdate = true;
        vOrigin = nv;
    }
    if (needUpdate) {
        _MarkForUpdate();
        needUpdate = false;
    }
}

//__________________________________________________________________
void        _HYComponent::SetVisibleSize (_HYRect r)
{
    int h = r.right-r.left,
        v = r.bottom-r.top;

    //if ((hSize != h)||(vSize != v))
    {
        needUpdate = true;
        hSize = h;
        vSize = v;
        SetOrigin (vOrigin,hOrigin);
        _SetVisibleSize (r);
    }
}

//__________________________________________________________________
void        _HYComponent::Paint (Ptr p)
// p holds a pointer to _HYRect - relative rect in host window
{
    _Paint (p);
    needUpdate = false;
}

//__________________________________________________________________

void        _HYComponent::Update (Ptr p)
// p holds a pointer to _HYRect - relative rect in host window
{
    _Update (p);
    needUpdate = false;
}

//__________________________________________________________________

_HYRect     _HYComponent::VisibleContents (Ptr p)
{
    return _VisibleContents(p);
}

//__________________________________________________________________
bool    _HYComponent::ProcessEvent(_HYEvent* e)
{
    if (e->EventClass() == _hyScrollingEvent) {
        _MarkForUpdate();
        DeleteObject (e);
        return true;
    }
    DeleteObject (e);
    return false;
}

//__________________________________________________________________

_HYCanvas::_HYCanvas(_HYRect s, Ptr p, int h, int w, int d):_HYGraphicPane (h,w,d),_HYComponent (s,p)
{
    doMouseClicks = false;
}


//__________________________________________________________________
void _HYCanvas::SetCanvasSize(int ht, int wd, int d)
{
    SetPaneSize (ht,wd,d);
    _HYRect     newD = {settings.top,settings.left,ht,wd,Settings()};
    SetDimensions (newD,newD);
}
//__________________________________________________________________
_HYRect _HYCanvas::GetCanvasSize(void)
{
    _HYRect res = {0,0,0,0,0};
    res.right = GetMaxW();
    res.bottom = GetMaxH();
    return res;
}

//__________________________________________________________________
bool    _HYCanvas::ProcessEvent(_HYEvent* e)
{
    if (e->EventClass() == _hyScrollingEvent) {
        long h,
             v;

        _String firstArg = e->EventCode().Cut (0,(v=e->EventCode().Find(','))-1);
        h = firstArg.toNum();
        firstArg = e->EventCode().Cut (v+1,-1);
        DeleteObject(e);
        v = firstArg.toNum();
        if (h||v) {
            //char buf [255];
            //sprintf (buf,"Canvas Scroll %d %d\n", h,v);
            //BufferToConsole(buf);
            Paint ((Ptr)&rel);
        }
        return true;
    }
    return _HYGuiObject::ProcessEvent (e);
}
//__________________________________________________________________
void    _HYCanvas::CrossfadeText (_String& , _String&t2, _HYRect& b, long t, long d, char al, char rValue)
{
    /*_HYColor saveFG   = color,
             colorOut = color,
             colorIn  = bColor,
             white    = {255,255,255},
             black    = {0,0,0};

    if (t<=1)
        t = 1;

    _Parameter diffR = (color.R-bColor.R)/(_Parameter)t,
               diffG = (color.G-bColor.G)/(_Parameter)t,
               diffB = (color.B-bColor.B)/(_Parameter)t;

    StartDraw   ();
    EraseRect   (b);
    EndDraw     ();

    for (long k=1; k<t; k++)
    {
        colorIn.R += diffR;
        colorIn.G += diffG;
        colorIn.B += diffB;

        colorOut.R -= diffR;
        colorOut.G -= diffG;
        colorOut.B -= diffB;


        StartDraw   ();
        EraseRect   (b);
        SetColor    (colorIn);
        DisplayText (t2,b,al);
        SetColor    (colorOut);
        DisplayText (t1,b,al);
        EndDraw     ();
        DelayNMs    (d);
        Paint       ((Ptr)&rel);
    }

    StartDraw   ();
    EraseRect   (b);
    SetColor    (saveFG);
    DisplayText (t2,b,al);
    EndDraw     ();
    Paint       ((Ptr)&rel);*/

    if (t<=1) {
        t = 1;
    }

    _HYRect     slideOut = b,
                slideIn  = b;


    long        diff   = (b.bottom-b.top-font.size)/t,
                diff2  = (b.bottom-b.top)/t,
                hDiff  = (b.right-b.left)/t,
                diffR  = (color.R-bColor.R)/t,
                diffG  = (color.G-bColor.G)/t,
                diffB  = (color.B-bColor.B)/t;

    _HYColor saveFG   = color,
             colorIn  = bColor;

    if (rValue<0) {
        rValue = (4*genrand_int32())/RAND_MAX_32;
    }

//  rValue = 3;

    switch (rValue) {
    case 0: // up
        slideIn.bottom -= font.size;
        slideIn.top = slideIn.bottom;
        break;
    case 1: // down
        slideIn.bottom = slideIn.top;
        break;
    case 2: // left
        slideIn.left = slideIn.right;
        break;
    default: // right
        slideIn.right = slideIn.left;
        break;

    }

    for (; t>1; t--) {
        colorIn.R += diffR;
        colorIn.G += diffG;
        colorIn.B += diffB;
        StartDraw();
        SetColor   (colorIn);
        switch (rValue) {
        case 0: // up
            slideIn.top -= diff;
            _SlideRect (slideOut,-diff2,0);
            DisplayText(t2,slideIn,al);
            slideOut.bottom -= diff2;
            break;
        case 1: // down
            slideIn.bottom += diff;
            _SlideRect (slideOut,diff,0);
            DisplayText(t2,slideIn,al);
            slideOut.top += diff;
            break;
        case 2: // left
            slideIn.left -= hDiff;
            _SlideRect (slideOut,0,-hDiff);
            DisplayText(t2,slideIn,al);
            slideOut.right -= hDiff;
            break;
        default: // right
            slideIn.right += hDiff;
            _SlideRect (slideOut,0,hDiff);
            DisplayText(t2,slideIn,al);
            slideOut.left += hDiff;
            break;

        }
        EndDraw();
        DelayNMs    (d);
        Paint       ((Ptr)&rel);
    }
    StartDraw();
    SetColor (saveFG);
    EraseRect  (b);
    DisplayText(t2,b,al);
    EndDraw();
    Paint       ((Ptr)&rel);

}
//__________________________________________________________________

_HYStretchCanvas::_HYStretchCanvas(_HYRect s, Ptr p, int h, int w, int d, char flags):_HYCanvas (s,p,h,w,d)
{
    canvasFlags = flags;
    canvasDepth = d;
}

//__________________________________________________________________

int _HYStretchCanvas::GetMaxW(void)
{
    if (canvasFlags&HY_SCANVAS_HORIZONTAL) {
        return 0x7FFF;
    }
    return _HYComponent::GetMaxW();
}

//__________________________________________________________________

int _HYStretchCanvas::GetMaxH(void)
{
    if (canvasFlags&HY_SCANVAS_VERTICAL) {
        return 0x7FFF;
    }
    return _HYComponent::GetMaxH();
}

//__________________________________________________________________

void        _HYStretchCanvas::SetVisibleSize (_HYRect r)
{
    long h,v;
    if (canvasFlags&HY_SCANVAS_HORIZONTAL) {
        h = r.right-r.left;
    } else {
        h = GetMaxW();
    }
    if (canvasFlags&HY_SCANVAS_VERTICAL) {
        v = r.bottom-r.top;
    } else {
        v = GetMaxH();
    }
    //if ((h!=w)||(v!=h))
    //{
    SetCanvasSize(v,h,canvasDepth);
    _HYComponent::SetVisibleSize(r);
    if (messageRecipient) {
        messageRecipient->ProcessEvent (generateRebuildSCanvas (GetID()));
    }
    //}
}

//__________________________________________________________________
_HYRect _HYStretchCanvas::GetCanvasSize(void)
{
    _HYRect res = {0,0,0,0,0};
    res.right =  _HYCanvas::GetMaxW();
    res.bottom = _HYCanvas::GetMaxH();
    return res;
}