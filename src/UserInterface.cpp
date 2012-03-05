/*************************************************************************
 *
 * FILE  UserInterface.cpp
 * $Id: UserInterface.cpp,v 1.73 2002/06/12 00:22:35 zonaj Exp $
 *
 * DESCRIPTION 
 *   User interface functions.
 *
 * This file is part of ztracker - a tracker-style MIDI sequencer.
 *
 * Copyright (c) 2000-2001, Christopher Micali <micali@concentric.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of their
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS´´ AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******/
#include "zt.h"

/* OO UI code - YUM - Can you tell this is the first time i've done this? */

int needaclear = 0;

void dev_sel(int dev, MidiOutDeviceSelector *mds ) {
    LBNode *p = mds->getNode(dev);
    if (p) {
        dev = mds->findItem(p->caption);
        if (dev < MidiOut->GetNumOpenDevs()) 
            song->instruments[cur_inst]->midi_device = MidiOut->GetDevID(dev);
    }
}

void midi_out_sel(int dev) {
    int a;
    char *errmsg;
    if ((unsigned int)dev < MidiOut->numOuputDevices) {
        if (MidiOut->QueryDevice(dev)) {
            MidiOut->RemDevice(dev);
            sprintf(szStatmsg,"Closed[out]: %s",(MidiOut->outputDevices[dev]->alias != NULL)?MidiOut->outputDevices[dev]->alias:MidiOut->outputDevices[dev]->szName);
        } else {
            if (a = MidiOut->AddDevice(dev)) {
                switch(a) {
                case MIDIERR_NODEVICE:
                    errmsg = "No device (mapper open?)";
                    break;
                case MMSYSERR_NOMEM:
                    errmsg = "Out of memory (reboot)";
                    break;
                case MMSYSERR_ALLOCATED:
                    errmsg = "Device busy";
                    break;
                default:
                    errmsg = "Unknown error";
                    break;
                }
                sprintf(szStatmsg,"Could not open %s (%d: %s)",(MidiOut->outputDevices[dev]->alias != NULL)?MidiOut->outputDevices[dev]->alias:MidiOut->outputDevices[dev]->szName,a,errmsg);
                statusmsg = szStatmsg;
                status_change = 1;
            } else {
                sprintf(szStatmsg,"Opened[out]: %s",(MidiOut->outputDevices[dev]->alias != NULL)?MidiOut->outputDevices[dev]->alias:MidiOut->outputDevices[dev]->szName);
            }
        }
    }
}

void midi_in_sel(int dev) {
    int a;
    char *errmsg;
    if ((unsigned int)dev < MidiIn->numMidiDevs) {
        if (MidiIn->QueryDevice(dev)) {
            MidiIn->RemDevice(dev);
            sprintf(szStatmsg,"Closed[in]: %s",MidiIn->midiInDev[dev]->szName);
        } else {
            if (a = MidiIn->AddDevice(dev)) {
                switch(a) {
                case MIDIERR_NODEVICE:
                    errmsg = "No device (mapper open?)";
                    break;
                case MMSYSERR_NOMEM:
                    errmsg = "Out of memory (reboot)";
                    break;
                case MMSYSERR_ALLOCATED:
                    errmsg = "Device busy";
                    break;
                default:
                    errmsg = "Unknown error";
                    break;
                }
                sprintf(szStatmsg,"Could not open %s (%d: %s)",MidiIn->midiInDev[dev]->szName,a,errmsg);
                statusmsg = szStatmsg;
                status_change = 1;
            } else {
                sprintf(szStatmsg,"Opened[in]: %s",MidiIn->midiInDev[dev]->szName);
            }
        }
    }
}


UserInterface::UserInterface(void) {
    num_elems = cur_element = 0;
    UIEList = NULL;
    GFXList = NULL;
    this->dontmess = 0;
    
}

UserInterface::~UserInterface(void) {
    reset();
}

void UserInterface::reset(void) {
    UserInterfaceElement *e;
    while (UIEList) {
        e = UIEList->next;
        delete UIEList;
        UIEList = e;
    }
    while (GFXList) {
        e = GFXList->next;
        delete GFXList;
        GFXList = e;
    }
}
void UserInterface::set_redraw(int ID) {
    UserInterfaceElement *e = UIEList;
    while(e) {
        if (e->ID == ID) {
            e->need_redraw = 1;
            break;
        }
        e = e->next;
    }
}
void UserInterface::full_refresh() {
    UserInterfaceElement *e = UIEList;
    while(e) {
        e->need_redraw = 1;
        e = e->next;
    }
    e = this->GFXList;
    while(e) {
        e->need_redraw = 1;
        e = e->next;
    }
}
void UserInterface::enter(void) {
    UserInterfaceElement *e = UIEList;
    while(e) {
        e->enter();
        e = e->next;
    }
}
void UserInterface::set_focus(int id) {
    cur_element = id;
    this->full_refresh();
}
void UserInterface::update() {
    UserInterfaceElement *e = UIEList;
    KBKey key,act=0,kstate;
    int a,new_redraw = 0;
    int o;
    while(e) {
        o = cur_element;
        cur_element = e->mouseupdate(cur_element);
        if (o!=cur_element)
            full_refresh();
        if (e->ID == cur_element) {
            a = e->update();
            if (a) {
                set_redraw(cur_element); new_redraw++;
                if (a==1)
                    cur_element++;
                else
                    cur_element--;
            }
        }
        e = e->next;
    }
    if (!dontmess) {
        key = Keys.checkkey();
        kstate = Keys.getstate();
        if (key == DIK_TAB) 
            SDL_Delay(1);
        if (key) {
            
            ////////////////////////////////////////////////////////////////////////
            // MODIFIED FOR SHIFT-TABBING
            
            if(kstate == KS_SHIFT) {
                switch(key) {
                case DIK_TAB:
                    set_redraw(cur_element); new_redraw++;
                    cur_element--;
/*
                    if(cur_element <= 1 || cur_element >= num_elems)
                    {
                        cur_element = num_elems-1;
                    }
                    else
                    {
                        cur_element--;
                    }*/
                    act++;
                    break;
                }
            }
            else if (kstate == KS_NO_SHIFT_KEYS) {
                switch(key) {
                case DIK_TAB:
                    set_redraw(cur_element); new_redraw++;
                    cur_element++; act++; 
                    break;
                }
            }
            
            //////////////////////////////////////////////////////////////////////////
        }
        if (act) {
            key = Keys.getkey();
            need_refresh++;

        }
    }
    if (cur_element==num_elems) 
        cur_element=0;
    if (cur_element<0 || cur_element>num_elems) 
        cur_element = num_elems-1;
    if (new_redraw)
        set_redraw(cur_element);
}

void UserInterface::draw(Drawable *S) {
    UserInterfaceElement *e = UIEList;
    if (needaclear) {
        S->fillRect(col(1),row(12),CONSOLE_WIDTH,424,COLORS.Background);
        screenmanager.Update(col(1),row(12),CONSOLE_WIDTH,424);
    }
    needaclear=0;
    while(e) {
        if (e->need_redraw) {
            e->draw(S,(e->ID == cur_element));
            e->need_redraw = 0;
        }
        e = e->next;
    }
    e = GFXList;
    while(e) {
        if (e->need_redraw) {
            e->draw(S,0);
            e->need_redraw = 0;
        }
        e = e->next;
    }
}
void UserInterface::add_element(UserInterfaceElement *uie, int ID) {
        uie->next = UIEList;
        UIEList = uie;
        uie->ID = ID;
        num_elems++;
}
void UserInterface::add_gfx(UserInterfaceElement *uie, int ID) {
        uie->next = GFXList;
        GFXList = uie;
        uie->ID = ID;
}
UserInterfaceElement* UserInterface::get_element(int ID) {
    UserInterfaceElement *e = UIEList;
    while(e) {
        if (e->ID == ID)
            return e;
        e=e->next;
    }
    return NULL;
}
UserInterfaceElement* UserInterface::get_gfx(int ID) {
    UserInterfaceElement *e = GFXList;
    while(e) {
        if (e->ID == ID)
            return e;
        e=e->next;
    }
    return NULL;
}
UserInterfaceElement::UserInterfaceElement(void) {
    changed = 1; x = 0; y = 0; xsize = 0; ysize = 1; mousestate=0; frame=1;
    next = NULL;
    need_redraw = 1;
}
UserInterfaceElement::~UserInterfaceElement(void) {
}
void UserInterfaceElement::enter(void) {
    
}
int UserInterfaceElement::mouseupdate(int cur_element) {
    KBKey key,act=0;
    key = Keys.checkkey();
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize))) {
                    mousestate = 1;
                    act++;
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) act++;
                mousestate=0;
                break;
        }
    }
    if (act)  key = Keys.getkey();
    if (cur_element != this->ID && mousestate && act) {
        need_refresh++;
        need_redraw++;
        return this->ID;
    }

    return cur_element;
}

ValueSlider::ValueSlider(void) {
    force_v = 0;
    force_f = 0;
    changed = 0;
    ysize   = 1;
    newclick= 1;
    focus   = 0;
}

ValueSlider::ValueSlider(int fset) {
    force_v = 0;
    force_f = 0;
    changed = 0;
    ysize   = 1;
    newclick= 1;
    focus = fset;
}

ValueSlider::~ValueSlider(void) {
}

int ValueSlider::mouseupdate(int cur_element) {
    int newval;
    float f;
    KBKey key,act=0;
    key = Keys.checkkey();
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+1))) {
                    mousestate = 1;
                    act++;
                    newclick=0; // unset click to focus
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) act++;
                mousestate=0;
                break;

                ///////////////////////////////////////////////////
                // Here is right click mouse focus

            case DIK_MOUSE_2_ON:
                if(focus)           // HERE we encapsulate the event in focus determination
                {                   // same thing for all the other varieties of value slider
                    if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+1))) {
                    newclick=1;
                    mousestate=1;
                    }
                }
                break;
            case DIK_MOUSE_2_OFF:
                if(focus)           // again here encapsulated. this is all we need to do to ignore focus
                {
                    mousestate=0;
                    break;
                }

                ///////////////////////////////////////////////////
        }
    }

    /////////////////////////////////////////////////////
    // Modified mousestate for click to focus

    if (mousestate) {
        if(!newclick){
            f = ((float)(LastX-col(this->x)) / (float)col(this->xsize));
            f = (f * (float)(max-min)) + min;
            if (f-floor(f) >= 0.5) f++;
            newval = (int) floor(f);
            if (newval<min) newval=min;
            if (newval>max) newval=max;
            if (newval != value) {
                        need_refresh++;
        need_redraw++;

                fixmouse++;
                changed++;
                value = newval;
            }
            this->need_redraw++;
        }
        return this->ID;
    }

    ///////////////////////////////////////////////////////
    if (act) {
        key = Keys.getkey();
        need_refresh++;
        need_redraw++;

        fixmouse++;
    }
    return cur_element;
}

int ValueSlider::update() {
    KBKey key,act=0;
    int ret=0,pop=0,c;
    key = Keys.checkkey();
    unsigned char kstate = Keys.getstate();

    if (!UIP_SliderInput->checked){
        c = UIP_SliderInput->getresult();
        if (!UIP_SliderInput->canceled) {
            changed++;
            value = c;
        }
        if (window_stack.isempty())
            needaclear++; 
        need_refresh++;
        need_redraw++;

    }

    if (key) {
        switch(key) {

            case DIK_UP: ret = -1; act++; break;
            case DIK_DOWN: ret = 1; act++; break;
            case DIK_LEFT: if (kstate&KS_CTRL) value-=(max/8); else value--;  act++; changed++; break;
            case DIK_RIGHT: if (kstate&KS_CTRL) value+=(max/8); else value++;  act++; changed++; break;
            case DIK_HOME: value=min; act++; changed++; break;
            case DIK_END: value=max; act++; changed++; break;

            case DIK_0: 
                UIP_SliderInput->setfirst(0);
                pop++;
                break;
            case DIK_1:
                UIP_SliderInput->setfirst(1);
                pop++;
                break;
            case DIK_2:
                UIP_SliderInput->setfirst(2);
                pop++;
                break;
            case DIK_3:
                UIP_SliderInput->setfirst(3);
                pop++;
                break;
            case DIK_4:
                UIP_SliderInput->setfirst(4);
                pop++;
                break;
            case DIK_5:
                UIP_SliderInput->setfirst(5);
                pop++;
                break;
            case DIK_6:
                UIP_SliderInput->setfirst(6);
                pop++;
                break;
            case DIK_7:
                UIP_SliderInput->setfirst(7);
                pop++;
                break;
            case DIK_8:
                UIP_SliderInput->setfirst(8);
                pop++;
                break;
            case DIK_9:
                UIP_SliderInput->setfirst(9);
                pop++;
                break;
            case DIK_MINUS:
                UIP_SliderInput->setfirst(-1);
                pop++;
                break;

        }
        
        if (pop) {
            key = Keys.getkey();    
            need_refresh++;
        need_redraw++;

            popup_window(UIP_SliderInput);          
        }
        if (act) {
            key = Keys.getkey();
            need_refresh++;
        need_redraw++;

        }
    }
    if (value<min) value=min;
    if (value>max) value=max;
    return ret;
}

void ValueSlider::draw(Drawable *S, int active) {
    int cx,cy=y;
    TColor col;
    char str[32];
    changed = 0;
    for(cx=x;cx<x+xsize;cx++) {
        printBG(col(cx),row(cy)," ",COLORS.Text,COLORS.EditBG,S);
    }

    int v = value;
    if (force_f)
        v = force_v;
    if (v>=0)
        sprintf(str," %.3d",v);
    else
        sprintf(str,"%.3d",v);

    printBG(col(x+xsize),row(cy),str,COLORS.Text,COLORS.Background,S);
    
    if (active)
        col = COLORS.Highlight;
    else
        col = COLORS.EditText;

    printchar(col(x)+(((value-min)*(((xsize-1)*8)-1))/(max-min))+1,row(cy),155,col,S);//156
    if (frame) {
        printline(col(x),row(y-1),0x86,xsize,COLORS.Lowlight,S);
        printline(col(x),row(y+1),0x81,xsize,COLORS.Highlight,S);
        printchar(col(x-1),row(y),0x84,COLORS.Lowlight,S);
        printchar(col(x+xsize),row(y),0x83,COLORS.Highlight,S);
    }
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+5),row(y+1));
    changed = 0;
}


ValueSliderOFF::ValueSliderOFF(void) {
    changed = 0;
    ysize = 1;
    newclick=1;
    focus=0;
}

ValueSliderOFF::ValueSliderOFF(int fset) {
    changed = 0;
    ysize = 1;
    newclick=1;
    focus=fset;
}

ValueSliderOFF::~ValueSliderOFF(void) {
}

int ValueSliderOFF::mouseupdate(int cur_element) {
    int newval;
    float f;
    KBKey key,act=0;
    key = Keys.checkkey();
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+1))) {
                    mousestate = 1;
                    act++;
                    newclick=0; // unset click to focus
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) act++;
                mousestate=0;
                break;

                ///////////////////////////////////////////////////
                // Here is right click mouse focus

            case DIK_MOUSE_2_ON:
                if(focus)
                {
                    if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+1))) {
                        newclick=1;
                        mousestate=1;
                    }
                }
                break;
            case DIK_MOUSE_2_OFF:
                if(focus)
                {
                    mousestate=0;
                }
                break;

                ///////////////////////////////////////////////////
        }
    }
    /////////////////////////////////////////
    // click to focus stuff

    if (mousestate) {
        if(!newclick){
            f = ((float)(LastX-col(this->x)) / (float)col(this->xsize));
            f = (f * (float)(max-min)) + min;
            if (f-floor(f) >= 0.5) f++;
            newval = (int) floor(f);
            if (newval<min) newval=min;
            if (newval>max) newval=max;
            if (newval != value) {
                need_refresh++;
                need_redraw++;
    
                fixmouse++;
                changed++;
                value = newval;
            }
        }
        return this->ID;
    }

    //////////////////////////////////

    if (act) {
        key = Keys.getkey();
        need_refresh++;
        need_redraw++;

        fixmouse++;
    }
    return cur_element;
}


int ValueSliderOFF::update() {
    KBKey key,act=0;
    int ret=0,pop=0,c;
    key = Keys.checkkey();
    unsigned char kstate = Keys.getstate();

    if (!UIP_SliderInput->checked){
        c = UIP_SliderInput->getresult();
        if (!UIP_SliderInput->canceled) {
            changed++;
            value = c;
        }
        needaclear++; need_refresh++;
    }
    
    if (key) {
        switch(key) {
            case DIK_UP: ret = -1; act++; break;
            case DIK_DOWN: ret = 1; act++; break;
            case DIK_LEFT: if (kstate&KS_CTRL) value-=(max/8); else value--;  act++; changed++; break;
            case DIK_RIGHT: if (kstate&KS_CTRL) value+=(max/8); else value++;  act++; changed++; break;
            case DIK_HOME: value=min; act++; changed++; break;
            case DIK_END: value=max; act++; changed++; break;

            case DIK_0: 
                UIP_SliderInput->setfirst(0);
                pop++;
                break;
            case DIK_1:
                UIP_SliderInput->setfirst(1);
                pop++;
                break;
            case DIK_2:
                UIP_SliderInput->setfirst(2);
                pop++;
                break;
            case DIK_3:
                UIP_SliderInput->setfirst(3);
                pop++;
                break;
            case DIK_4:
                UIP_SliderInput->setfirst(4);
                pop++;
                break;
            case DIK_5:
                UIP_SliderInput->setfirst(5);
                pop++;
                break;
            case DIK_6:
                UIP_SliderInput->setfirst(6);
                pop++;
                break;
            case DIK_7:
                UIP_SliderInput->setfirst(7);
                pop++;
                break;
            case DIK_8:
                UIP_SliderInput->setfirst(8);
                pop++;
                break;
            case DIK_9:
                UIP_SliderInput->setfirst(9);
                pop++;
                break;
            case DIK_MINUS:
                UIP_SliderInput->setfirst(-1);
                pop++;
                break;
        }
        if (pop) {
            key = Keys.getkey();    
            if (cur_state != STATE_PEDIT_WIN) {
                need_refresh++;
                need_redraw++;

                popup_window(UIP_SliderInput);          
            }
        }
        if (act) {
            key = Keys.getkey();
            need_refresh++;
            need_redraw++;

        }
    }
    if (value<min) value=min;
    if (value>max) value=max;
    return ret;
}

void ValueSliderOFF::draw(Drawable *S, int active) {
    int cx,cy=y,v;
    TColor col;
    char str[32];
    changed = 0;
    
//    S->fillRect(col(x-1),row(y-1),col(x+xsize+7),row(y+1),RGB(0xA0,0xA0,0xFF));
    for(cx=x;cx<x+xsize;cx++) {
        printBG(col(cx),row(cy)," ",COLORS.Text,COLORS.EditBG,S);
    }
    if (value<0) {
        if (max>9999)
            sprintf(str,"OFF  "); 
        else
            sprintf(str,"OFF "); 
        v=0;
    } else {
        if (max>9999) 
            sprintf(str,"%.5d",value);
        else
        if (max>999) 
            sprintf(str,"%.4d",value);
        else
        if (max>99) 
            sprintf(str,"%.3d",value);
        else
            sprintf(str,"%.2d",value);
        v = value;
    }
    printBG(col(x+xsize+1),row(cy),str,COLORS.Text,COLORS.Background,S);
    if (active)
        col = COLORS.Highlight;
    else
        col = COLORS.EditText;
    printchar(col(x)+(((v-min)*(((xsize-1)*8)-1))/(max-min))+1,row(cy),155,col,S);//156
    if (frame) {
        printline(col(x),row(y-1),0x86,xsize,COLORS.Lowlight,S);
        printline(col(x),row(y+1),0x81,xsize,COLORS.Highlight,S);
        printchar(col(x-1),row(y),0x84,COLORS.Lowlight,S);
        printchar(col(x+xsize),row(y),0x83,COLORS.Highlight,S);
    }
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+7),row(y+1));
    changed = 0;
}

ValueSliderDL::ValueSliderDL(void) {
    changed = 0;
    ysize = 1;
    newclick=1;     // NEW Click set
    focus=0;
}

ValueSliderDL::ValueSliderDL(int fset) {
    changed = 0;
    ysize = 1;
    newclick=1;     // NEW Click set
    focus=fset;
}

ValueSliderDL::~ValueSliderDL(void) {
}

int ValueSliderDL::mouseupdate(int cur_element) {
    int newval;
    float f;
    KBKey key,act=0;
    key = Keys.checkkey();
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+1))) {
                    mousestate = 1;
                    act++;
                    newclick=0; // unset click to focus
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) act++;
                mousestate=0;
                break;

                ///////////////////////////////////////////////////
                // Here is right click mouse focus

            case DIK_MOUSE_2_ON:
                if(focus)
                {
                    if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+1))) {
                        newclick=1;
                        mousestate=1;
                    }
                }
                break;
            case DIK_MOUSE_2_OFF:
                if(focus)
                {
                    mousestate=0;
                }
                break;

                ///////////////////////////////////////////////////
        }
    }
    /////////////////////////////////////////
    // Click to focus

    if (mousestate) {
        if(!newclick)
        {
            f = ((float)(LastX-col(this->x)) / (float)col(this->xsize));
            f = (f * (float)(max-min)) + min;
            if (f-floor(f) >= 0.5) f++;
            newval = (int) floor(f);
            if (newval<min) newval=min;
            if (newval>max) newval=max;
            if (newval != value) {
                need_refresh++;
                need_redraw++;

                fixmouse++;
                changed++;
                value = newval;
            }
        }
        return this->ID;
    }

    /////////////////////////////////////////

    if (act) {
        key = Keys.getkey();
        need_refresh++;
        need_redraw++;

        fixmouse++;
    }
    return cur_element;
}


int ValueSliderDL::update() {
    KBKey key,act=0;
    int ret=0,pop=0,c;
    key = Keys.checkkey();
    unsigned char kstate = Keys.getstate();
    if (!UIP_SliderInput->checked){
        c = UIP_SliderInput->getresult();
        if (!UIP_SliderInput->canceled) {
            changed++;
            value = c;
        }
        needaclear++; need_refresh++;
    }
    
    if (key) {
        switch(key) {
            case DIK_UP: ret = -1; act++; break;
            case DIK_DOWN: ret = 1; act++; break;
            case DIK_LEFT: if (kstate&KS_CTRL) value-=(max/8); else value--;  act++; changed++; break;
            case DIK_RIGHT: if (kstate&KS_CTRL) value+=(max/8); else value++;  act++; changed++; break;
            case DIK_HOME: value=min; act++; changed++; break;
            case DIK_END: value=max; act++; changed++; break;

            case DIK_0: 
                UIP_SliderInput->setfirst(0);
                pop++;
                break;
            case DIK_1:
                UIP_SliderInput->setfirst(1);
                pop++;
                break;
            case DIK_2:
                UIP_SliderInput->setfirst(2);
                pop++;
                break;
            case DIK_3:
                UIP_SliderInput->setfirst(3);
                pop++;
                break;
            case DIK_4:
                UIP_SliderInput->setfirst(4);
                pop++;
                break;
            case DIK_5:
                UIP_SliderInput->setfirst(5);
                pop++;
                break;
            case DIK_6:
                UIP_SliderInput->setfirst(6);
                pop++;
                break;
            case DIK_7:
                UIP_SliderInput->setfirst(7);
                pop++;
                break;
            case DIK_8:
                UIP_SliderInput->setfirst(8);
                pop++;
                break;
            case DIK_9:
                UIP_SliderInput->setfirst(9);
                pop++;
                break;
            case DIK_MINUS:
                UIP_SliderInput->setfirst(-1);
                pop++;
                break;
        }
        if (pop) {
            key = Keys.getkey();    
            if (cur_state != STATE_PEDIT_WIN) {
                need_refresh++;
                need_redraw++;

                popup_window(UIP_SliderInput);          
            }
        }
        if (act) {
            key = Keys.getkey();
            need_refresh++;
            need_redraw++;

        }
    }
    if (value<min) value=min;
    if (value>max) value=max;
    return ret;
}

void ValueSliderDL::draw(Drawable *S, int active) {
    int cx,cy=y;
    TColor col;
    char str[32];
    changed = 0;
    for(cx=x;cx<x+xsize;cx++) {
        printBG(col(cx),row(cy)," ",COLORS.Text,COLORS.EditBG,S);
    }
    if (value>999)
        sprintf(str,"INF");
    else
        sprintf(str,"%.3d",value);
    printBG(col(x+xsize+1),row(cy),str,COLORS.Text,COLORS.Background,S);
    if (active)
        col = COLORS.Highlight;
    else
        col = COLORS.EditText;
    printchar(col(x)+(((value-min)*(((xsize-1)*8)-1))/(max-min))+1,row(cy),155,col,S);//156
    if (frame) {
        printline(col(x),row(y-1),0x86,xsize,COLORS.Lowlight,S);
        printline(col(x),row(y+1),0x81,xsize,COLORS.Highlight,S);
        printchar(col(x-1),row(y),0x84,COLORS.Lowlight,S);
        printchar(col(x+xsize),row(y),0x83,COLORS.Highlight,S);
    }
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+5),row(y+1));
    changed = 0;
}

TextInput::TextInput(void) {
    cursor = 0;
}
TextInput::~TextInput(void) {
}

int TextInput::update() {
    KBKey key,act=0,ret=0;
    signed int retletter =0;
    int i;
    key = Keys.checkkey();
    unsigned char kstate = Keys.getstate();
    if (key) {
        switch(key) {
            case DIK_UP: ret = -1; act++; break;
            case DIK_DOWN: ret = 1; act++; break;
            case DIK_LEFT: cursor--;  act++; break;
            case DIK_RIGHT: if (str[cursor]) cursor++; act++; break;
            case DIK_A: retletter=1; act++; break;
            case DIK_B: retletter=2; act++; break;
            case DIK_C: retletter=3; act++; break;
            case DIK_D: retletter=4; act++; break;
            case DIK_E: retletter=5; act++; break;
            case DIK_F: retletter=6; act++; break;
            case DIK_G: retletter=7; act++; break;
            case DIK_H: retletter=8; act++; break;
            case DIK_I: retletter=9; act++; break;
            case DIK_J: retletter=10; act++; break;
            case DIK_K: retletter=11; act++; break;
            case DIK_L: retletter=12; act++; break;
            case DIK_M: retletter=13; act++; break;
            case DIK_N: retletter=14; act++; break;
            case DIK_O: retletter=15; act++; break;
            case DIK_P: retletter=16; act++; break;
            case DIK_Q: retletter=17; act++; break;
            case DIK_R: retletter=18; act++; break;
            case DIK_S: retletter=19; act++; break;
            case DIK_T: retletter=20; act++; break;
            case DIK_U: retletter=21; act++; break;
            case DIK_V: retletter=22; act++; break;
            case DIK_W: retletter=23; act++; break;
            case DIK_X: retletter=24; act++; break;
            case DIK_Y: retletter=25; act++; break;
            case DIK_Z: retletter=26; act++; break;
            case DIK_0: retletter=0xFF; act++; break;
            case DIK_1: retletter=0xFF; act++; break;
            case DIK_2: retletter=0xFF; act++; break;
            case DIK_3: retletter=0xFF; act++; break;
            case DIK_4: retletter=0xFF; act++; break;
            case DIK_5: retletter=0xFF; act++; break;
            case DIK_6: retletter=0xFF; act++; break;
            case DIK_7: retletter=0xFF; act++; break;
            case DIK_8: retletter=0xFF; act++; break;
            case DIK_9: retletter=0xFF; act++; break;
            case DIK_SPACE: retletter=0xff; act++; break;
            case DIK_PERIOD: retletter=0xff; act++; break;
            case DIK_COMMA: retletter=0xff; act++; break;
            case DIK_BACKSLASH: retletter=0xff; act++; break;
            case DIK_MINUS: retletter=0xff; act++; break;
            case DIK_EQUALS: retletter=0xff; act++; break;
            case DIK_BACKSPACE: retletter=0xFFF; act++; break;
            case DIK_DELETE: retletter=0xFFF; act++; break;
            case DIK_SEMICOLON: retletter = 0xfff; act++; break;
        }
        if (act) {
            key = Keys.getkey();
            need_refresh++;
            need_redraw++;

            if (retletter) {
                this->changed = 1;
                if (retletter != 0xFFF) { // Regular Key
                    for(i=length-1;i>cursor;i--) {
                        str[i] = str[i-1];
                    }
                    if (cursor<length) {
                        if (retletter != 0xff) {
                            if (kstate&KS_SHIFT) 
                                str[cursor] = ((int)retletter + 'A' - 1);
                        else    
                                str[cursor] = ((int)retletter + 'a' - 1);
                        } else {    
                            switch(key) {
                                case DIK_SEMICOLON: if (kstate&KS_SHIFT)  str[cursor] = ':'; else str[cursor] = ';'; break;
                                case DIK_SPACE:  str[cursor] = ' '; break;
                                case DIK_PERIOD: str[cursor] = '.'; break;
                                case DIK_COMMA:  str[cursor] = ','; break;
                                case DIK_BACKSLASH:  str[cursor] = '\\'; break;
                                case DIK_MINUS:  str[cursor] = '-'; break;
                                case DIK_EQUALS: str[cursor] = '='; break;
                                case DIK_0: str[cursor] = '0'; break;
                                case DIK_1: str[cursor] = '1'; break;
                                case DIK_2: str[cursor] = '2'; break;
                                case DIK_3: str[cursor] = '3'; break;
                                case DIK_4: str[cursor] = '4'; break;
                                case DIK_5: str[cursor] = '5'; break;
                                case DIK_6: str[cursor] = '6'; break;
                                case DIK_7: str[cursor] = '7'; break;
                                case DIK_8: str[cursor] = '8'; break;
                                case DIK_9: str[cursor] = '9'; break;
                            }
                        }
                        cursor++;
                    }
                } else { // Is backspace/del
                    if (key==DIK_BACKSPACE)
                        cursor--;
                    if (key!=DIK_BACKSPACE || (cursor>=0)) {  /* FIX THIS SHIT! */
                        if (key == DIK_BACKSPACE && cursor == length-1) {
                            str[length-2] = str[length-1];
                            str[length-1] = '\0';
                        } else {
                            str[length-1] = '\0';
                            for(i=cursor;i<length-1;i++)
                                str[i] = str[i+1];
                        }
                    }
                } // End RetLetter
            }
        }
        if (cursor<0) cursor=0;
        if (cursor>length-1) cursor=length-1;
    }
    return ret;
}

void TextInput::draw(Drawable *S, int active) {
    int cx,cy=y,instr=1,i=0;
    char s[2];
    TColor f,b;
    s[1]=0;
    for(cx=x;cx<x+xsize;cx++) {
        if (instr) {
            if (str[i])
                s[0] = str[i];
            else {
                instr=0;
                s[0] = ' ';
            }
        }   
        b = COLORS.EditBG;
        f = COLORS.EditText;
        if (active && i==cursor) {
                f = COLORS.EditBG;
                b = COLORS.Highlight;
        }
        printBG(col(cx),row(cy),&s[0],f,b,S); i++;
    }
    changed = 0;
    if (frame) {
        printline(col(x),row(y-1),0x86,xsize,COLORS.Lowlight,S);
        printline(col(x),row(y+1),0x81,xsize,COLORS.Highlight,S);
        printchar(col(x-1),row(y),0x84,COLORS.Lowlight,S);
        printchar(col(x+xsize),row(y),0x83,COLORS.Highlight,S);
    }
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+1),row(y+1));

    this->changed = 0;
}


CheckBox::CheckBox(void) {
    value = NULL;
}
CheckBox::~CheckBox(void) {
}

int CheckBox::mouseupdate(int cur_element) {
    KBKey key,act=0;
    key = Keys.checkkey();
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize))) {
                    mousestate = 1;
                    act++;
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) {
                    act++;
                    if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize))) {
                        if (*value)
                            *value = 0;
                        else
                            *value = 1;
                        need_refresh++;
                        need_redraw++;

                        changed++; 
                        fixmouse++;
                    }
                }
                mousestate=0;
                break;
        }
    }
    if (act)  key = Keys.getkey();
    if (cur_element != this->ID && mousestate && act) {
                        need_refresh++;
                        need_redraw++;

        fixmouse++;
        return this->ID;
    }

    return cur_element;
}

int CheckBox::update() {
    KBKey key,act=0;
    int ret=0;
    key = Keys.checkkey();
    if (key) {
        switch(key) {
            case DIK_UP: ret = -1; act++; break;
            case DIK_DOWN: ret = 1; act++; break;
            case DIK_SPACE: 
                if (*value)
                    *value = 0;
                else
                    *value = 1;
                act++; changed++; break;
        }
        if (act) {
            Keys.getkey();
            need_refresh++;
            need_redraw++;

        }
    }
    return ret;
}

void CheckBox::draw(Drawable *S, int active) {
    int cx,cy=y;
    TColor f,b;
    char *str;
    for(cx=x;cx<x+xsize;cx++) {
        printBG(col(cx),row(cy)," ",COLORS.Text,COLORS.EditBG,S);
    }
    if (*value)
        str = "On";
    else
        str = "Off";
    if (active) {
        f = COLORS.EditBG;
        b = COLORS.Highlight; 
    } else {
        f = COLORS.EditText;
        b = COLORS.EditBG;
    }
    printBG(col(x),col(y),str,f,b,S);
    if (frame) {
        printline(col(x),row(y-1),0x86,xsize,COLORS.Lowlight,S);
        printline(col(x),row(y+1),0x81,xsize,COLORS.Highlight,S);
        printchar(col(x-1),row(y),0x84,COLORS.Lowlight,S);
        printchar(col(x+xsize),row(y),0x83,COLORS.Highlight,S);
    }
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+1),row(y+1));
    changed = 0;
}

Frame::Frame(void) {
    type = 0;
}
Frame::~Frame(void) {
}

int Frame::update() {
    return 0; // This doesnt get key actions (it's never active)
}

void Frame::draw(Drawable *S, int active) {
    int cy;
    if (type) {
        printline(col(x),row(y-1),0x81,xsize,COLORS.Lowlight,S);
        printline(col(x),row(y+ysize),0x86,xsize,COLORS.Highlight,S);
        for (cy=y;cy<y+ysize;cy++) {
            printchar(col(x-1),row(cy),0x83,COLORS.Lowlight,S);
            printchar(col(x+xsize),row(cy),0x84,COLORS.Highlight,S);
        }
    } else {
        printline(col(x),row(y-1),0x86,xsize,COLORS.Lowlight,S);
        printline(col(x),row(y+ysize),0x81,xsize,COLORS.Highlight,S);
        for (cy=y;cy<y+ysize;cy++) {
            printchar(col(x-1),row(cy),0x84,COLORS.Lowlight,S);
            printchar(col(x+xsize),row(cy),0x83,COLORS.Highlight,S);
        }
    }
}



InstEditor::InstEditor(void) {
    list_start = text_cursor = cursor = 0;
    frm = new Frame;
    frm->type = 0;

}
InstEditor::~InstEditor(void) {
    delete frm;
}
int InstEditor::mouseupdate(int cur_element) {
                        int i = (MousePressY/8) - this->y;
    KBKey key,act=0;
    key = Keys.checkkey();

    
    
    
    //    int old_cur_sel = cur_sel;
//   int old_y_start = y_start;
/*
    if (!bMouseIsDown)
        mousestate = 0;
    if (mousestate) {
        int i = (LastY/8) - this->y ;
        if (cur_sel+y_start != i) {
            if (i<0) {
                if (y_start >0 ) {
                    setCursor(y_start-1);
//                    y_start--;
                    act++;
                }
            } else
            if (i>ysize && cur_sel==ysize && y_start<num_elements-ysize-1) {
//                y_start++;
                setCursor(y_start+cur_sel+1);
                act++;
            } else
            if (i<=ysize && i+y_start<num_elements && i+y_start>=0) {
                setCursor(i+y_start);
                act++;
            }
        }
    }
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize+1))) {
                    int i = (MousePressY/8) - this->y;
                    if (cur_element == this->ID && i==this->cur_sel) {
                        LBNode *p = this->getNode(this->cur_sel+this->y_start);
                        if (p)
                            OnSelect( p );
                    }
                    mousestate = 1;
                    if (i<=ysize && i<num_elements)
                        setCursor(i+y_start);                    
                    act++;
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) {
                    act++;
                }
                mousestate=0;
                break;
        }
    }
    if (cur_sel != old_cur_sel || y_start != old_y_start)
        OnSelectChange();
    if (act)  key = Keys.getkey();
    if (act) {
        need_refresh++;
        need_redraw++;
        return this->ID;
    }

    return cur_element;
*/
    if (mousestate) {
        int i = (LastY/8)-this->y;
        if (cursor != i) {
            act++;
            cursor = i;
        }
        int j = (LastX/8) - this->x;
        if (text_cursor != j) {
            this->text_cursor = j;
            act++;
        }
    }
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize))) {
                    int i = (MousePressY/8) - this->y;// + list_start;
                    int j = (MousePressX/8) - this->x;
                    this->text_cursor = j;
                    this->cursor = i;
                    act++;
                    mousestate = 1;
                    
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) {
                    act++;
                }
                mousestate=0;
                break;

        }
    }
    if (act) {
        key = Keys.getkey();
        need_refresh++;
        need_redraw++;
        if (cursor>ysize-1)
            cursor = ysize-1;
        if (cursor<0)
            cursor=0;
        if (text_cursor<0)
            text_cursor = 0;
        if (text_cursor>24) text_cursor=24;     

        return this->ID;
    }
    return cur_element;
}

int InstEditor::update() {
    KBKey key,act=0,kstate;
    int ret=0,length=25;
    signed int retletter = 0,i;
    char *str = (char *)&song->instruments[list_start+cursor]->title[0];
    key = Keys.checkkey();
    kstate = Keys.getstate();
    if (key) {
        if (!(kstate&KS_CTRL) && !(kstate&KS_ALT)) {
            switch(key) {
                case DIK_HOME:
                    if (text_cursor>0) 
                        text_cursor = 0;
                    else {
                        if (cursor == 0)
                            list_start = 0;
                        else
                            cursor = 0;
                    }
                    act++;
                    break;
                case DIK_END:
                    if (text_cursor<length-1)
                        text_cursor = length-1;
                    else {
                        if (cursor == ysize-1)
                            list_start = MAX_INSTS-ysize;
                        else
                            cursor = ysize-1;
                    }
                    act++;
                    break;
                case DIK_PGUP:
                    if (cursor == 0) {
                        list_start-=16;
                    } else {
                        cursor-=16;
                    }
                    act++;
                    break;
                case DIK_PGDN:
                    if (cursor == ysize-1) {
                        list_start+=16;
                    } else {
                        cursor+=16;
                    }
                    act++;
                    break;
                case DIK_DOWN: 
                    cursor++;
                    if (cursor>=ysize)
                        list_start++;
                    act++; 
                    break;
                case DIK_UP:
                    cursor--;
                    if (cursor<0)
                        list_start--;
                    act++;
                    break;
                case DIK_LEFT: text_cursor--;  act++; break;
                case DIK_RIGHT: text_cursor++; act++; break;
            }
            if (text_cursor == length-1) {
                switch(key) {
                    case DIK_TAB: 
                        if (kstate == KS_SHIFT)
                            ret = -1;
                        else
                            ret = 1;
                        
                        act++;
                        break;
                }
            } else {
                switch(key) {
                    case DIK_TAB: text_cursor = length-1; act++; break;
                    case DIK_A: retletter=1; act++; break;
                    case DIK_B: retletter=2; act++; break;
                    case DIK_C: retletter=3; act++; break;
                    case DIK_D: retletter=4; act++; break;
                    case DIK_E: retletter=5; act++; break;
                    case DIK_F: retletter=6; act++; break;
                    case DIK_G: retletter=7; act++; break;
                    case DIK_H: retletter=8; act++; break;
                    case DIK_I: retletter=9; act++; break;
                    case DIK_J: retletter=10; act++; break;
                    case DIK_K: retletter=11; act++; break;
                    case DIK_L: retletter=12; act++; break;
                    case DIK_M: retletter=13; act++; break;
                    case DIK_N: retletter=14; act++; break;
                    case DIK_O: retletter=15; act++; break;
                    case DIK_P: retletter=16; act++; break;
                    case DIK_Q: retletter=17; act++; break;
                    case DIK_R: retletter=18; act++; break;
                    case DIK_S: retletter=19; act++; break;
                    case DIK_T: retletter=20; act++; break;
                    case DIK_U: retletter=21; act++; break;
                    case DIK_V: retletter=22; act++; break;
                    case DIK_W: retletter=23; act++; break;
                    case DIK_X: retletter=24; act++; break;
                    case DIK_Y: retletter=25; act++; break;
                    case DIK_Z: retletter=26; act++; break;
                    case DIK_0: retletter=0xFF; act++; break;
                    case DIK_1: retletter=0xFF; act++; break;
                    case DIK_2: retletter=0xFF; act++; break;
                    case DIK_3: retletter=0xFF; act++; break;
                    case DIK_4: retletter=0xFF; act++; break;
                    case DIK_5: retletter=0xFF; act++; break;
                    case DIK_6: retletter=0xFF; act++; break;
                    case DIK_7: retletter=0xFF; act++; break;
                    case DIK_8: retletter=0xFF; act++; break;
                    case DIK_9: retletter=0xFF; act++; break;
                    case DIK_SEMICOLON: retletter=0xff; act++; break;
                    case DIK_SPACE: retletter=0xff; act++; break;
                    case DIK_PERIOD: retletter=0xff; act++; break;
                    case DIK_COMMA: retletter=0xff; act++; break;
                    case DIK_BACKSLASH: retletter=0xff; act++; break;
                    case DIK_MINUS: retletter=0xff; act++; break;
                    case DIK_EQUALS: retletter=0xff; act++; break;
                    case DIK_BACKSPACE: retletter=0xFFF; act++; break;
                    case DIK_DELETE: retletter=0xFFF; act++; break;
                }
            }           
        } else {
            switch(key) {
                case DIK_C:
                    if (kstate & KS_ALT)
                        memset(&song->instruments[list_start+cursor]->title,' ',24);
                    act++;
                    break;
            }
        }
        if (act) {
            Keys.getkey();
            need_refresh++;
            need_redraw++;

            length=24;
            
            
            
            if (retletter) {
                if (retletter != 0xFFF) { // Regular Key
                    for(i=length-1;i>text_cursor;i--) {
                        str[i] = str[i-1];
                    }
                    if (text_cursor<length) {
                        if (retletter != 0xff) {
                            if (kstate & KS_SHIFT) 
                                str[text_cursor] = ((int)retletter + 'A' - 1);
                        else    
                                str[text_cursor] = ((int)retletter + 'a' - 1);
                        } else {    
                            switch(key) {
                                case DIK_SEMICOLON: if (kstate & KS_SHIFT)  str[text_cursor] = ':'; else str[text_cursor] = ';'; break;
                                case DIK_SPACE:  str[text_cursor] = ' '; break;
                                case DIK_PERIOD: str[text_cursor] = '.'; break;
                                case DIK_COMMA:  str[text_cursor] = ','; break;
                                case DIK_BACKSLASH:  str[text_cursor] = '\\'; break;
                                case DIK_MINUS:  str[text_cursor] = '-'; break;
                                case DIK_EQUALS: str[text_cursor] = '='; break;
                                case DIK_0: str[text_cursor] = '0'; break;
                                case DIK_1: str[text_cursor] = '1'; break;
                                case DIK_2: str[text_cursor] = '2'; break;
                                case DIK_3: str[text_cursor] = '3'; break;
                                case DIK_4: str[text_cursor] = '4'; break;
                                case DIK_5: str[text_cursor] = '5'; break;
                                case DIK_6: str[text_cursor] = '6'; break;
                                case DIK_7: str[text_cursor] = '7'; break;
                                case DIK_8: str[text_cursor] = '8'; break;
                                case DIK_9: str[text_cursor] = '9'; break;
                            }
                        }
                        text_cursor++;
                    }
                } else { // Is backspace/del
                    if (key==DIK_BACKSPACE)
                        text_cursor--;
                    if (key!=DIK_BACKSPACE || (text_cursor>=0)) {  /* FIX THIS SHIT! */
                        if (key == DIK_BACKSPACE && text_cursor == length-1) {
                            str[length-2] = str[length-1];
                            str[length-1] = ' ';
                        } else {
                            str[length-1] = ' ';
                            for(i=text_cursor;i<length-1;i++)
                                str[i] = str[i+1];
                        }
                    }
                } // End RetLetter
            }
        }
        length=25;
        if (cursor<0) cursor=0;
        if (cursor>=ysize) cursor=ysize-1;
        if (list_start<0) list_start = 0;
        if (list_start>=MAX_INSTS-ysize) list_start=MAX_INSTS-ysize;

        if (text_cursor<0) text_cursor=0;
        if (text_cursor>length-1) text_cursor=length-1;     
    
    }
    return ret;
}
void InstEditor::strc(char *dst, char *src) {
    int i=0;
    while (i<xsize && src[i]) {
        dst[i] = src[i];
        i++;
    }
}
void InstEditor::draw(Drawable *S, int active) {
    int cy;
    TColor f,b;
    char str[26],str2[5];
//    memset(str,' ',26); str[25]=0;
    for (cy=0;cy<ysize;cy++) {
        memset(str,' ',26); str[25]=0;
        strc(str,(char *)&song->instruments[list_start+cy]->title[0]);
        printBG(col(x),row(cy+y),str,COLORS.EditText,COLORS.EditBG,S);
        sprintf(str2,"%.2d",cy+list_start);
        printBG(col(x-3),row(cy+y),str2,COLORS.Text,COLORS.Background,S);
        if (active) {
            f = COLORS.EditBG;  
            b = COLORS.Highlight;
        } else {
            f = COLORS.EditBG;  
            b = COLORS.EditText;
        }
        if (cy == cursor) {
            if (text_cursor<24) {
                printcharBG(col(x + text_cursor),row(cy+y),song->instruments[list_start+cy]->title[text_cursor],f,b,S);
                printBG(col(x+25),row(cy+y),"Play",COLORS.EditText,COLORS.EditBG,S);
            }
            else
                printBG(col(x+25),row(cy+y),"Play",f,b,S);
        } else {
            printBG(col(x+25),row(cy+y),"Play",COLORS.EditText,COLORS.EditBG,S);
        }
        printcharBG(col(x + 24),row(cy+y),168,COLORS.Lowlight,COLORS.EditBG,S);
    }
    frm->type=0;
    frm->x=x; frm->y=y; frm->xsize=xsize; frm->ysize=ysize;
    frm->draw(S,0);
    screenmanager.Update(col(x-4),row(y-1),col(x+xsize+1),row(y+ysize+1));
}




OrderEditor::OrderEditor(void) {
    frm = new Frame;
    cursor_y = cursor_x = list_start = 0;
    old_playing_ord = -1;
}
OrderEditor::~OrderEditor(void) {
    delete frm;
}
int OrderEditor::mouseupdate(int cur_element) {
    if (ztPlayer->playing) {
        if (ztPlayer->playing_cur_order != old_playing_ord) {
            need_redraw++;
            need_refresh++;
            old_playing_ord = ztPlayer->playing_cur_order;
        }
    } 
    else if (old_playing_ord != -1 && !ztPlayer->playing) {
        old_playing_ord = -1;
        need_redraw++;
        need_refresh++;
    }
    return cur_element;
}
int OrderEditor::update() {
    KBKey key,act=0;
    int kstate;
    int ret=0,i,val=0xFF;
    short int p1,p2,p3,p;
    key = Keys.checkkey();
    kstate = Keys.getstate();

    if (key) {
        switch(key) {
            case DIK_TAB: ret = 1; act++; break;
            case DIK_HOME:
                if (cursor_y>0)
                    cursor_y=0;
                else
                    list_start=0;
                act++;
                break;
            case DIK_END:
                if (cursor_y<ysize-1)
                    cursor_y=ysize-1;
                else
                    list_start=256-ysize;
                act++;
                break;
            case DIK_PGUP:
                if (cursor_y>0)
                    cursor_y-=16;
                else
                    list_start-=16;
                act++;
                break;
            case DIK_PGDN:
                if (cursor_y<ysize-2)
                    cursor_y+=16;
                else
                    list_start+=16;
                act++;
                break;
            case DIK_UP: 
                cursor_y--; 
                if (cursor_y<0)
                    list_start--;
                act++; 
                break;
            case DIK_DOWN: 
                cursor_y++; 
                if (cursor_y>=ysize)
                    list_start++;
                act++; 
                break;
            case DIK_LEFT: cursor_x--; act++; break;
            case DIK_RIGHT: cursor_x++; act++; break;
            
            case DIK_PERIOD:
            case DIK_SPACE:
            case DIK_MINUS:
                song->orderlist[cursor_y+list_start] = 0x100;
                cursor_x=0; cursor_y++;
                if (cursor_y>=ysize)
                    list_start++;
                act++;
                break;
            case DIK_ADD:
                if(kstate == KS_SHIFT)
                {
                    if(cur_edit_order < 255 && song->orderlist[cur_edit_order + 1] != 0x100)
                    {
                        cur_edit_order++;
                        cur_edit_pattern = song->orderlist[cur_edit_order];
                            need_refresh++;
                        need_redraw++;
                    }
                    break;
                }
                song->orderlist[cursor_y+list_start] = 0x101;
                cursor_x=0; cursor_y++;
                if (cursor_y>=ysize)
                    list_start++;
                act++;
                break;
            case DIK_SUBTRACT:
                if(kstate == KS_SHIFT)
                {
                    if(cur_edit_order > 0 && song->orderlist[cur_edit_order -1] != 0x100)
                    {
                        cur_edit_order--;
                        cur_edit_pattern = song->orderlist[cur_edit_order];
                            need_refresh++;
                        need_redraw++;
                    }
                    break;
                }
                break;
            case DIK_DELETE:
                for(i=cursor_y+list_start;i<255;i++)
                    song->orderlist[i] = song->orderlist[i+1];
                song->orderlist[255] = 0x100;
                if(song->orderlist[cur_edit_order -1 ] == 0x100 || song->orderlist[cur_edit_order + 1] == 0x100)
                    for(;song->orderlist[cur_edit_order] == 0x100 && cur_edit_order >=0;cur_edit_order--);
                act++;
                if(cur_edit_order<0)
                    cur_edit_order=0;
                break;
            case DIK_INSERT:
                for(i=255;i>cursor_y+list_start;i--)
                    song->orderlist[i] = song->orderlist[i-1];
                song->orderlist[cursor_y+list_start] = 0x100;
                act++;
                break;
            case DIK_0: val = 0; act++; break;
            case DIK_1: val = 1; act++; break;
            case DIK_2: val = 2; act++; break;
            case DIK_3: val = 3; act++; break;
            case DIK_4: val = 4; act++; break;
            case DIK_5: val = 5; act++; break;
            case DIK_6: val = 6; act++; break;
            case DIK_7: val = 7; act++; break;
            case DIK_8: val = 8; act++; break;
            case DIK_9: val = 9; act++; break;

            case DIK_G: 
                p = song->orderlist[ cursor_y + list_start ];
                if (p < 0x100) {
                    cur_edit_pattern = p;
                    switch_page( UIP_Patterneditor );
                    act++;
                    doredraw++;
                    Keys.flush();
                    midiInQueue.clear();
                }
                break;
        }
        if (act) {
            if (val<0xFF) {
                file_changed++;
                p = song->orderlist[cursor_y+list_start];
                if (p>0xFF) p = 0;
                p1 = p / 100;   // p1
                p2 = (p-(p1*100)) / 10;
                p3 = p % 10;    // p3 = (54 % 10)  (=4)
                switch(cursor_x) {
                    case 2: p3=val; break;
                    case 1: p2=val; break;
                    case 0: p1=val; break;
                }
                p = p3 + (p2*10) + (p1*100);
                if (p<0) p=0;
                if (p>0xFF) p=0xFF;
                song->orderlist[cursor_y+list_start] = p;
                if (!zt_globals.step_editing) {
                    cursor_x++;
                    if (cursor_x>2) {
                        cursor_y++;
                        if (cursor_y>=ysize)
                            list_start++;
                        cursor_x=0;
                    }                        
                }
                else
                    cursor_y++;
            }
            if (cursor_x>2) {
                cursor_x = 2;
                if (cursor_y>=ysize)
                    list_start++;
            }
            if (cursor_y<0) cursor_y = 0;
            if (cursor_y>=ysize) cursor_y = ysize-1;
            if (list_start<0) list_start=0;
            if (list_start>256 - ysize) list_start = 256-ysize;
            if (cursor_x<0) cursor_x=0;
            Keys.getkey();
                need_refresh++;
            need_redraw++;

        }
    }
    sel_pat = song->orderlist[cursor_y+list_start];
    sel_order = cursor_y+list_start;
    return ret;
}
void OrderEditor::draw(Drawable *S, int active) {
    int cy;
    char str[8];
    TColor f;
    for (cy=0;cy<ysize;cy++) {
        if (cy+list_start == ztPlayer->cur_order && ztPlayer->playing)
            f = COLORS.Highlight;
        else if(cy+list_start == cur_edit_order)
            f= COLORS.Lowlight;
        else
            f = COLORS.Text;
        sprintf(&str[0],"%.3d",cy+list_start);
        printBG(col(x),row(cy+y),str,f,COLORS.Background,S);
        if (song->orderlist[cy+list_start] < 255) {
            sprintf(&str[0],"%.3d",song->orderlist[cy+list_start]);
        } else {
            if (song->orderlist[cy+list_start] == 0x101 )
                sprintf(&str[0],"+++");
            else    
                sprintf(&str[0],"---");
        }
        printBG(col(x+4),row(cy+y),str,COLORS.EditText,COLORS.EditBG,S);
        if (active) 
            f = COLORS.Highlight;
        else
            f = COLORS.EditText;
        if (cy == cursor_y)
            printcharBG(col(x+4+cursor_x),row(cy+y),str[cursor_x],COLORS.EditBG,f,S);
    }
    frm->type=0;
    frm->x=x+4; frm->y=y; frm->xsize=3; frm->ysize=ysize;
    frm->draw(S,0);
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+1),row(y+ysize+1));
}


Button::Button(void) {
    caption = NULL;
    OnClick = NULL;
    state = 0;
    updown = 0;
}
Button::~Button(void) {
}

int Button::mouseupdate(int cur_element) {
    KBKey key,act=0;
    key = Keys.checkkey();
    if (mousestate) {
        if (checkmousepos(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize))) {
            if (state != 3) {
                state = 3;
                act++;
                changed++;
                fixmouse++;
                    need_refresh++;
            need_redraw++;

            }
        } else {
            if (state != updown) {
                state = updown;
                act++;
                changed++;
                fixmouse++;
                    need_refresh++;
            need_redraw++;

            }
        }
        
    }
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize))) {
                    mousestate = 1;
                    state = 3;
                    act++;
                        need_refresh++;
            need_redraw++;

                    changed++; 
                    fixmouse++;
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) {
                    act++;
                    if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize))) {
                            need_refresh++;
            need_redraw++;

                        changed++; 
                        fixmouse++;
                        if (this->OnClick)
                            this->OnClick(this);
                    }
                }
                state = updown;
                mousestate=0;
                break;
        }
    }
    if (act)  key = Keys.getkey();
    if (cur_element != this->ID && mousestate && act) {
            need_refresh++;
            need_redraw++;

        fixmouse++;
        return this->ID;
    }
    return cur_element;
}

int Button::update() {
    KBKey key,act=0;
    int ret=0;
    key = Keys.checkkey();
    if (state==2  && !last_cmd_keyjazz){
        changed++;
            need_refresh++;
            need_redraw++;

        if (this->OnClick)
            this->OnClick(this);
        state = updown;
    }
    if (key) {
        switch(key) {
            case DIK_UP: ret = -1; act++; break;
            case DIK_DOWN: ret = 1; act++; break;

            case DIK_SPACE:  //  / Same thing
            case DIK_RETURN: //  \ Same thingg
                if (!last_cmd_keyjazz) {
                    act++; changed++; 
                    state=2;
                    last_keyjazz = key;
                    last_cmd_keyjazz = 1;
                }
                break;
        }
        if (act) {
            Keys.getkey();
                need_refresh++;
            need_redraw++;

        }
    }
    return ret;
}

void Button::draw(Drawable *S, int active) {
    if (active) 
        print(col(x),row(y),caption,COLORS.Highlight,S);
    else
        print(col(x),row(y),caption,COLORS.Text,S);

    if (state | updown) {
        printline(col(x),row(y-1),0x86,xsize,COLORS.Lowlight,S);
        printline(col(x),row(y+1),0x81,xsize,COLORS.Highlight,S);
//        printline(col(x),row(y+1),0x81,xsize,COLORS.Lowlight,S);

        printchar(col(x-1),row(y),0x84,COLORS.Lowlight,S);
        printchar(col(x+xsize),row(y),0x83,COLORS.Highlight,S);

//        printchar(col(x+xsize),row(y),0x83,COLORS.Lowlight,S);
    } else {
        printline(col(x),row(y-1),0x86,xsize,COLORS.Highlight,S);
        printline(col(x),row(y+1),0x81,xsize,COLORS.Lowlight,S);
        printchar(col(x-1),row(y),0x84,COLORS.Highlight,S);
        printchar(col(x+xsize),row(y),0x83,COLORS.Lowlight,S);
    }
    changed = 0;
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+2),row(y+2));
}




GfxButton::GfxButton(void) {
    OnClick = NULL;
    state = 0;
    bmDefault = NULL;
    bmOnMouseOver = NULL;
    bmOnClick = NULL;
    StuffKey = -1;
    StuffKeyState = KS_NO_SHIFT_KEYS;
}

GfxButton::~GfxButton(void) {
    if (bmDefault)
        delete bmDefault;
    if (bmOnMouseOver)
        delete bmOnMouseOver;
    if (bmOnClick)
        delete bmOnClick;
/*
    if (bmDefault)
        RELEASEINT(bmDefault);
    if (bmOnMouseOver)
        RELEASEINT(bmOnMouseOver);
    if (bmOnClick)
        RELEASEINT(bmOnClick);
*/
  }

int GfxButton::mouseupdate(int cur_element) {
    KBKey key,act=0,bustit=0;
    key = Keys.checkkey();
    if (mousestate) {
        if (checkmousepos(this->x,this->y,this->x+this->xsize,this->y+this->ysize)) {
            if (state != 3) {
                state = 3;
                act++;
                changed++;
                fixmouse++;
                    need_refresh++;
            need_redraw++;

            }
        } else {
            if (state != 0) {
                state = 0;
                act++;
                changed++;
                fixmouse++;
                    need_refresh++;
            need_redraw++;

            }
        }
        
    } else {
        if (bmOnMouseOver) {
            if (checkmousepos(this->x,this->y,this->x+this->xsize,this->y+this->ysize)) {
                if (state == 0) {
                    state = 1;
                    act++;
                    changed++;
                    fixmouse++;
                        need_refresh++;
            need_redraw++;

                }
            } else {
                if (state == 1) {
                    state = 0;
                    act++;
                    changed++;
                    fixmouse++;
                        need_refresh++;
            need_redraw++;

                }
            }       
        }
    }
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(this->x,this->y,this->x+this->xsize,this->y+this->ysize)) {
                    mousestate = 1;
                    state = 3;
                    act++;
                        need_refresh++;
            need_redraw++;

                    changed++; 
                    fixmouse++;
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) {
                    act++;
                    state = 0;
                    if (checkclick(this->x,this->y,this->x+this->xsize,this->y+this->ysize)) {
                            need_refresh++;
            need_redraw++;

                        changed++; 
                        fixmouse++;
                        bustit = 1;
                        if (this->OnClick)
                            this->OnClick(this);
                        if (this->bmOnMouseOver)
                            state = 1; else
                            state = 2;
                    }
                }
                mousestate = 0;
                break;
        }
    }
    if (act) {
        key = Keys.getkey();
        if (StuffKey != -1 && bustit)
            Keys.insert(StuffKey,StuffKeyState);
    }
    if (cur_element != this->ID && mousestate && act) {
            need_refresh++;
            need_redraw++;
        //fixmouse++;
        return this->ID;
    }
    return cur_element;
}

int GfxButton::update() {
    return 0;
}

void GfxButton::draw(Drawable *S, int active) {

//  if (active) 
//      print(col(x),row(y),caption,COLORS.Highlight,S);
//  else
//      print(col(x),row(y),caption,COLORS.Text,S);

    switch(state) {
        case 1: // MouseOver
            if (this->bmOnMouseOver)
                S->copy(this->bmOnMouseOver,x,y);
            break;
        case 3: // Click
            if (this->bmOnClick)
                S->copy(this->bmOnClick,x,y);
            break;
        default: // Default
            if (this->bmDefault)
                S->copy(this->bmDefault,x,y);
            break;
    }
    screenmanager.Update(x,y,x+xsize,y+ysize);
    changed = 0;
}


PatternDisplay::PatternDisplay(void) {
    frm = new Frame;
    cur_pat_view = 0;
    starttrack = 0; 
    cur_track = 0;
    disp_row = 0;
}
PatternDisplay::~PatternDisplay(void) {
    if (frm)
        delete frm;
}


int PatternDisplay::update() {
    KBKey key,act=0;
    int ret=0,kstate,i,val;
    key = Keys.checkkey();
    kstate = Keys.getstate();
    
    if (ztPlayer->playing)
        if (this->disp_row != ztPlayer->playing_cur_row)
                need_refresh++;
            need_redraw++;


    if (key) {

        if (kstate == KS_NO_SHIFT_KEYS) {
            switch(key) {
                case DIK_LEFT:
                    cur_track--; act++;
                    break;
                case DIK_RIGHT:
                    cur_track++; act++;
                    break;
                case DIK_SPACE: 
                    toggle_track_mute(this->cur_track); 
                    cur_track++; 
                    act++;
                        need_refresh++;
            need_redraw++;
 
                    break;
                case DIK_M: // feature 447390 cm
                    toggle_track_mute(this->cur_track); 
                    act++;
                        need_refresh++;
            need_redraw++;
 
                    break;
            }
        }
        if (kstate == KS_ALT) {
            switch(key) {
                case DIK_1: toggle_track_mute(0); need_refresh++; act++; break;
                case DIK_2: toggle_track_mute(1); need_refresh++; act++; break;
                case DIK_3: toggle_track_mute(2); need_refresh++; act++; break;
                case DIK_4: toggle_track_mute(3); need_refresh++; act++; break;
                case DIK_5: toggle_track_mute(4); need_refresh++; act++; break;
                case DIK_6: toggle_track_mute(5); need_refresh++; act++; break;
                case DIK_7: toggle_track_mute(6); need_refresh++; act++; break;
                case DIK_8: toggle_track_mute(7); need_refresh++; act++; break;
                case DIK_9: toggle_track_mute(8); need_refresh++; act++; break;
                case DIK_0: toggle_track_mute(9); need_refresh++; act++; break;

                case DIK_F9: 
                    toggle_track_mute(this->cur_track); 
                        need_refresh++;
                        need_redraw++;
                        act++;
                    break;

                case DIK_F10: 
                    val = 0;
                    for(i=0;i<MAX_TRACKS;i++) {
                        if (i==this->cur_track) {
                            if (song->track_mute[i])
                                val = 1;
                        } else {
                            if (!song->track_mute[i])
                                val = 1;
                        }   
                    }
                    if (val) {
                        for(i=0;i<MAX_TRACKS;i++)
                            if (i==this->cur_track) {
                                unmutetrack(i);
                            }
                            else {
                                mutetrack(i);
                            }
                    } else {
                        for(i=0;i<MAX_TRACKS;i++) {
                            unmutetrack(i);
                        }
                    }
                        need_refresh++;
            need_redraw++;
 act++ ;
                    break;
            }
        }

        if (act) {
            Keys.getkey();
                need_refresh++;
            need_redraw++;

            if (cur_track>this->tracks+this->starttrack-1) {
                this->starttrack++;
                if (this->starttrack > MAX_TRACKS - this->tracks) {
                    this->starttrack = MAX_TRACKS - this->tracks ;
                    cur_track = MAX_TRACKS - 1;
                }
            }
            if (cur_track<this->starttrack) {
                if (this->starttrack>0) {
                    this->starttrack--;
                } else {
                    cur_track = 0;
                }
            }
        }
    }
    this->update_frame();
    return ret;
}

void PatternDisplay::draw(Drawable *S, int active) {
    if (ztPlayer->playing)
        this->disp_playing_pattern(S);
    else {
        this->disp_track_headers(S);
    }
    frm->draw(S,0);
    screenmanager.UpdateWH(col(frm->x),row(frm->y-1),col(frm->xsize+1),row(frm->ysize+1));
    changed = 0;
}

int PatternDisplay::next_order(void) {
    int pass=0,cur_order;
    cur_order = ztPlayer->playing_cur_order+1;
    while(song->orderlist[cur_order] > 0xFF && pass<3) {
        if (song->orderlist[cur_order] == 0x100)
            cur_order = 0;
        while(song->orderlist[cur_order] == 0x101 && cur_order < 256) 
            cur_order++;
        pass++;
    }
    return song->orderlist[cur_order];
}

void PatternDisplay::disp_playing_row(int x, int y, int pattern, int row, Drawable *S, TColor bg) {
    char str[40];
    int ctrack;
//  int i;
    TColor color;
    event *e;

    sprintf(str, "%.3d",row);
    printBG(col(x),row(y),str,COLORS.Text,COLORS.Background,S);
    for (ctrack = 0; ctrack<tracks; ctrack++) {
        e = song->patterns[pattern]->tracks[ctrack+starttrack]->get_event(row);
        if (e)
            ctrack = ctrack;
        this->printNote(str,e);
//      switch(i) {
                color = COLORS.EditText;
//              break;
//      }
        printBG(col(x + 4 + (ctrack*tracksize)),row(y),str,color,bg,S);
    }
}

void PatternDisplay::update_frame(void) {
    switch(this->cur_pat_view) {
        case 0:
            tracks = 24;
            tracksize = 3;
            break;
        case 1:
            tracks = 10;
            tracksize = 7;
    }

    frm->x = x+4;
    frm->y = y;
    frm->ysize=ysize;
    frm->xsize=tracks*tracksize;

}

void PatternDisplay::disp_track_headers(Drawable *S) {
    TColor bg;
    char str[50];
    int j,p=0;
    for(j=this->starttrack;j<this->starttrack+this->tracks;j++) {
        if (this->cur_pat_view >1)
            sprintf(str," Track %.2d ",j+1);
        else
            sprintf(str,"%.2d",j+1);
        
        if (song->track_mute[j]) {
            if (j == cur_track)
                bg = COLORS.EditText;
            else
                bg = COLORS.Lowlight;
        } else {
            if (j == cur_track)
                bg = COLORS.Brighttext;
            else
                bg = COLORS.Black;
        }
        printBG(col(4+x+(p*(this->tracksize))),row(y-1),str,bg,COLORS.Lowlight,S);
        p++;
    }
}

void PatternDisplay::disp_playing_pattern(Drawable *S) {
    int cy,rows,cur_row,pat,dontchangeitasshole = 0,last = 0;
    int next = next_order();
    TColor bg;

    this->disp_row = cur_row = ztPlayer->playing_cur_row;
    rows = ysize/2;
    cur_row -= rows;
    pat = ztPlayer->playing_cur_pattern;
    for (cy = y; cy<y+ysize; cy++) {
        if (cur_row < 0) {
            pat = ztPlayer->last_pattern;
            last = 1;
            if (pat<0)
                pat = ztPlayer->playing_cur_pattern;
            cur_row = song->patterns[pat]->length + cur_row ;
        }/* else
            if (!dontchangeitasshole)
                pat = ztPlayer->playing_cur_pattern;
        */
        if (cur_row >= song->patterns[pat]->length) {
            if (ztPlayer->playmode) {
                dontchangeitasshole = 1;
                if (last) {
                    pat = ztPlayer->playing_cur_pattern;
                    last = 0;
                } else {
                    pat = next;
                }
            }
            cur_row = 0;
        }
        if (cur_row == this->disp_row)
            bg = COLORS.EditBGhigh;
        else
            bg = COLORS.EditBG;
        this->disp_playing_row(x,cy,pat,cur_row++,S,bg);
    }
    this->disp_track_headers(S);    
}

char *PatternDisplay::printNote(char *str, event *r) {
    char note[4],in[3],vol[3],len[4],fx[3],fxd[5];
    if (!r) r = &blank_event;
    hex2note(note,r->note);
    if (r->vol < 0x80) {
        sprintf(vol,"%.2x",r->vol);
        vol[0] = toupper(vol[0]);
        vol[1] = toupper(vol[1]);
    } else
        strcpy(vol,"..");
    if (r->inst<MAX_INSTS) {
        sprintf(in,"%.2d",r->inst);
        in[0] = toupper(in[0]);
        in[1] = toupper(in[1]);
    } else
        strcpy(in,"..");
    if (r->length>0x0) {
        if (r->length>999)
            sprintf(len,"INF");
        else
            sprintf(len,"%.3d",r->length);
    } else 
        strcpy(len,"...");
    if (r->effect<0xFF) {
        sprintf(fx,"%c",r->effect);
        fx[0] = toupper(fx[0]);
    } else
        strcpy(fx,".");
    sprintf(fxd,"%.4x",r->effect_data);
    fxd[0] = toupper(fxd[0]);
    fxd[1] = toupper(fxd[1]);
    fxd[2] = toupper(fxd[2]);
    fxd[3] = toupper(fxd[3]);
    switch(this->cur_pat_view) {
        case 0:
            sprintf(str,"%.3s",note,vol); // 2 cols
            break;
        case 1:
            sprintf(str,"%.3s %.2s",note,vol); // 2 cols
            break;
        case 2:
            sprintf(str,"%.3s %.2s %.2s %.3s",note,in,vol,len); // 4 cols
            break;
        case 3:
            sprintf(str,"%.3s %.2s %.2s %.3s %s%.4s",note,in,vol,len,fx,fxd); // 7 cols
            // NOT IN VL CH LEN fx PARM 
            break;
    }
    return str;
}

VUPlay::VUPlay() {
    for(int i=0; i < 64; i++)
        latency[i].e.note = latency[i].longevity = latency[i].init_longevity = latency[i].init_vol = 0;
    starttrack = 0;
}

VUPlay::~VUPlay() {

}

int VUPlay::update() {

    KBKey key,act=0;
    int kstate;
    key = Keys.checkkey();
    kstate = Keys.getstate();
    int SPEED = 2; // speed of fading

    switch(key)
    {
    case DIK_DOWN:
        if(starttrack < MAX_TRACKS - 32)
            starttrack++;
        break;
    case DIK_UP:
        if(starttrack > 0)
            starttrack--;
        break;
    default:
        break;
    };

    if (ztPlayer->playing)
    {
        if (this->cur_row != ztPlayer->playing_cur_row) {
            for(int i = 0; i < 64; i++)
                if(latency[i].longevity - SPEED > 0)
                    latency[i].longevity -=SPEED;
                else
                    latency[i].longevity = 0;
        }
    }
    need_refresh++;
    need_redraw++;

    return(0);
}

void VUPlay::draw(Drawable *S, int active) {
    char str[72];
    int ctrack;
    TColor color;
    event *e;
    int pattern;
    int x = 4;
    int y = 15;
    int tracksize = 128;
    int vol_len;
    int i;

    if(ztPlayer->playing && (this->cur_row < ztPlayer->cur_row - 2 || this->cur_row > ztPlayer->cur_row))
    {
        pattern = ztPlayer->playing_cur_pattern;
        cur_row = ztPlayer->playing_cur_row;
        cur_order = ztPlayer->playing_cur_order;

        sprintf(str,"\
 Tk  Instrument               Note  Vol  FX    Length ................");

        printBG(col(x),row(y - 1),str,COLORS.Text,COLORS.Background,S);
        color = COLORS.EditText;
        for (ctrack = starttrack; ctrack < starttrack + num_channels; ctrack++) {
            e = song->patterns[pattern]->tracks[ctrack]->get_event(cur_row);
            vol_len = this->draw_one_row(str,e, ctrack);
            printBG(col(x),row(y + 1 + ctrack - starttrack),str,color,(ctrack%2)?COLORS.EditBGlow:COLORS.Black,S);

            // Print the bar
            if(vol_len > 0) {
                //strcpy(str,"                ");
                strcpy(str,"");
                for(i = 0; i < 17; str[i++] = '\0');
                for(i = 0; i < vol_len; str[i++] = '=');
                printBG(col(x+54),row(y + 1 + ctrack - starttrack),str,COLORS.Highlight,COLORS.EditBGhigh,S);
            }
        }
    }
    else if(! ztPlayer->playing) {
        color = COLORS.EditText;
        for (ctrack = starttrack; ctrack < starttrack + num_channels; ctrack++) {
            e = NULL;
            this->draw_one_row(str,e, ctrack);
            printBG(col(x),row(y + 1 + ctrack - starttrack),str,color,(ctrack%2)?COLORS.EditBGlow:COLORS.Black,S);
        }
    }
    // Display the outline

    S->drawHLine((y+num_channels+1)*8,(x)*8,(x+71)*8,COLORS.Highlight);
    S->drawVLine((x+71)*8,y*8,(y+num_channels)*8,COLORS.Highlight); // why doesn't this work ?
    

    screenmanager.UpdateWH(col(x),row(y),col(x+71),row(y+28));//num_channels)); why oens'tthis work either
}

int VUPlay::draw_one_row(char str[72], event *e, int track) {

    char *w;
    int measure;
    char c[2] = { (char)'=', (char)0 };

    //c = 155;
    measure = 0;
    sprintf(str,"");
    // New Data
    if(e && e->note <128) {
        latency[track].e = *e;
        latency[track].longevity = latency[track].init_longevity = e->length ? e->length : song->instruments[e->inst]->default_length;
        latency[track].init_vol = (e->vol < 128) ? e->vol : song->instruments[e->inst]->default_volume;
        measure = latency[track].init_vol / 16 * 127;
    }
    else if(e)
    {
        if(e->note > 0x80)
            latency[track].longevity = 0;
        if(e->effect_data)
            latency[track].e.effect_data = e->effect_data;
    }

    // Let's display the Information !
    if( e == NULL || !latency[track].longevity) // || e->note > 127)
        sprintf(str,"\
 %.2d                                                                   ", track);
    else
        if(latency[track].e.note)
        {
            sprintf(str,"\
 %.2d  %s  %.2d  %.3d  %.5d  %.5d",
track,
song->instruments[latency[track].e.inst]->title,
latency[track].e.note,
(latency[track].e.vol < 128)?latency[track].e.vol:song->instruments[latency[track].e.inst]->default_volume,
latency[track].e.effect_data, latency[track].longevity);

        // add some bars...

        strcat(str," ");
        if(!measure) {
            measure = ((float)latency[track].init_vol - (float)((float)(1 - (float)((float)latency[track].init_vol * (float)((float)latency[track].longevity / (float)latency[track].init_longevity )))* (float)latency[track].init_vol) / 10);
            measure /= 100;
        }
        
        // we really want to be safe about this unscientific calculation

        if(measure > 16)
            measure = 16;
        if(measure < 0)
            measure = 0;
        //for(; measure--; strcat(str, c)); // 155 = 9B
    }

    // Cleanup
    for(w = &str[strlen(str)]; w != &str[71]; w++)
        *w = ' ';
    *w = '\0';

    return(measure);
}

BarGraph::BarGraph() {
    value = maxval = max = trackmaxval = 0;
}

BarGraph::~BarGraph() {
}

int BarGraph::update() {
    return 0;
}

void BarGraph::draw(Drawable *S, int active) {
    int howfar,maxlen;

    howfar = value*(xsize-1)/max;
    maxlen = maxval*(xsize-1)/max;
    for (int cy=y;cy<y+ysize;cy++) {
        S->drawHLine(cy,x,x+xsize-1,COLORS.LCDLow);
        S->drawHLine(cy,x,x+howfar,COLORS.LCDMid);
    }
    if (this->trackmaxval)
        S->drawVLine(x+maxlen,y,y+ysize-1,COLORS.LCDHigh);
    S->drawHLine(y,x,x+xsize,COLORS.Lowlight);
    S->drawVLine(x,y,y+ysize-1,COLORS.Lowlight);
    S->drawHLine(y+ysize-1,x,x+xsize,COLORS.Highlight);
    S->drawVLine(x+xsize,y,y+ysize-1,COLORS.Highlight);
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+1),row(y+1));
    this->changed = 0;
}

void BarGraph::setvalue(int val) {
    if (this->value == val) return;
    if (val>max) val = max;
    if (trackmaxval) {
        if (this->value<val) 
            maxval = val;
    }
    this->value = val;
    need_refresh++;
    need_redraw++;
    this->changed = 1;
}

LCDDisplay::LCDDisplay() {
    xsize = 0;
    ysize = 8*3;
}

LCDDisplay::~LCDDisplay() {
}

int LCDDisplay::update() {
    return 0;
}

void LCDDisplay::draw(Drawable *S, int active) {
    if (!xsize)
        xsize = length*8*3;
    printLCD(x+1,y+1,istr,S);
    x-=2;
    S->drawHLine(y,x,x+xsize,COLORS.Lowlight);
    S->drawVLine(x,y,y+ysize-1,COLORS.Lowlight);
    S->drawHLine(y+ysize,x,x+xsize,COLORS.Highlight);
    S->drawVLine(x+xsize,y,y+ysize-1,COLORS.Highlight);
    x+=2;
    screenmanager.UpdateWH(x,y,xsize,ysize);
    this->changed = 0;
}

void LCDDisplay::setstr(char *s) {
    str = istr;
    strcpy(istr,s);
    need_redraw++;
    need_refresh++;
    changed = 1;
}


AboutBox::AboutBox() {
    xsize = 0;
    ysize = 8*3;
}

AboutBox::~AboutBox() {
}

int AboutBox::update() {
    need_refresh++;
    this->need_redraw++;
    return 0;
}

void AboutBox::draw(Drawable *S, int active) {
/*
    if (!xsize)
        xsize = length*8*3;
    printLCD(x+1,y+1,str,S);
    x-=2;
    S->drawHLine(y,x,x+xsize,COLORS.Lowlight);
    S->drawVLine(x,y,y+ysize-1,COLORS.Lowlight);
    S->drawHLine(y+ysize,x,x+xsize,COLORS.Highlight);
    S->drawVLine(x+xsize,y,y+ysize-1,COLORS.Highlight);
    x+=2;
*/  q += S->surface->pitch;
    int i=q;
    Uint32 *b = (Uint32 *)S->surface->pixels;
    for (int iy = col(y); iy < col(y+ysize+1); iy++)    
        for (int ix = col(x); ix < col(x+xsize); ix++)  {  
            int bpp = S->surface->format->BytesPerPixel;
            Uint8 *p = (Uint8 *)S->surface->pixels + iy * S->surface->pitch + ix * bpp;
            *(Uint32 *)p = i++;
        }
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+1),row(y+1));
    frm.type=0;
    frm.x=x; frm.y=y; frm.xsize=xsize; frm.ysize=ysize+1;
    frm.draw(S,0);
    this->changed = 0;
}




TextBox::TextBox(int ro) {
    readonly = ro;
    mousestate = soff = startline = 0;
    bEof = false;
    bUseColors = 1;
    bWordWrap = false;
}


TextBox::~TextBox() {

}
int TextBox::mouseupdate(int cur_element) {
    KBKey key,act=0;
    key = Keys.checkkey();
    if (!bMouseIsDown)
        mousestate = 0;
    if (mousestate) {
        int i = (LastY/8) - this->y ;
        int s = startl + (starti-i);
        if (s!=startline) {
            if (! (s>startline && bEof)) {
                act++;
                startline = s;
                if (startline<0) startline = 0;
            }
        }
    }
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize+1))) {
                    int i = (MousePressY/8) - this->y;
                    mousestate = 1;
                    startl = startline;
                    starti = i;
                    act++;
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) {
                    act++;
                }
                mousestate=0;
                break;
        }
    }
    if (act)  key = Keys.getkey();
    if (act) {
        need_refresh++;
        need_redraw++;
        return this->ID;
    }

    return cur_element;
}

int TextBox::update() {
    KBKey key,act=0;
    int ret=0;
    key = Keys.checkkey();
    if (key) {
        switch(key) {
        //case SDLK_m
        case DIK_UP  : act++; startline--;  break;
        case DIK_DOWN: act++; if (!bEof) startline++;  break;
        case DIK_PGUP: act++; startline-=16;  break;
        case DIK_PGDN: act++; if (!bEof) startline+=16;  break;
        case DIK_LEFT: act++; soff--; break;
        case DIK_RIGHT:act++; soff++; break;
        case DIK_HOME: if (soff>0) soff=0; else startline=0; act++; break;
        case DIK_SPACE:  //  / Same thing
        case DIK_RETURN: //  \ Same thingg
            break;
        }
        if (act) {
            Keys.getkey();
                need_refresh++;
            need_redraw++;

        }
    }
    if (startline<0) startline = 0;
    if (soff<0) soff=0;
    return ret;
}

int nextline(char *str, int p) {
    while(str[p]) {
        if (str[p]=='\n')
            return p+1;
        p++;
    }
    return -1;
}

void TextBox::draw(Drawable *S, int active) {
    int line=0, done = 0, sc=0,d=0, cx;
    TColor use = COLORS.EditText;
//    for(int cy=col(y); cy<col(y+ysize+1); cy++) 
//        S->drawHLine(cy, row(x), row(x+xsize), 0);
    S->fillRect(row(x),col(y),row(x+xsize)-1,col(y+ysize+1)-1,CurrentSkin->Colors.EditBG);
    int l=0; bEof = false;

    if (!text) goto skipme;
    while (l<startline) {
        d = nextline(this->text, sc);
        if (d==-1) {
            l = startline;
            done = 1;
        } else
            sc = d;
        l++;
    }

    /*if (str[c] == '|' && str[c+1] != '|') {
        c++;
        switch(toupper(str[c])) {
        case 'H':
            use = COLORS.Highlight;
            break;
        case 'T':
            use = COLORS.Text;
            break;
        case 'U':
            use = col;
            break;
        }
        c+=2;
    } else if (str[c] == '|' && str[c+1] == '|') c++;
    */
    while(line <= ysize && !done && text[sc]) {
        d=0;
        cx = 0;
/*
        for(int p=0;p<soff;p++)
            if (text[sc] && text[sc]!='\n')
                sc++;
*/
        int p = 0;
        while(text[sc] && !d) {
            if (text[sc]=='\n')
                d = 1;
            else
                switch(text[sc]) {
                    case '|': 
                        if (!bUseColors) goto printer;
                        if (text[sc+1] == '|')
                            goto printer;
                        else {
                            sc++;
                            switch(toupper(text[sc])) {
                                case 'H':
                                    use = COLORS.Highlight;
                                    break;
                                case 'L':
                                    use = COLORS.Lowlight;
                                    break;
                                case 'U':
                                    use = COLORS.EditText;
                                    break;
                            }
                            sc++;
                        }
                        break;
                    case '\r': break;
                    case '\t': p++; if (p>=soff) cx+=2; break;
                    default:
                        printer:
                        p++;
                        if (p>=soff)  {
                            if (cx<x+xsize-4)
                                printchar(col(x + 1 +cx), row(y + line), text[sc], use, S);
                            else
                                if (bWordWrap)
                                    d++;
                            cx++;
                        }
            }
            sc++;
        }
        if (!text[sc])
            done = 1;
        line++;
    } 
skipme:
    if (done) bEof = true;
    frm.type=0;
    frm.x=x; frm.y=y; frm.xsize=xsize; frm.ysize=ysize+1;
    frm.draw(S,0);
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+1),row(y+ysize+1));
    changed = 0;
}


ListBox::ListBox() {
    items = NULL;
    clear();
    is_sorted = false;
    use_key_select = true;
    empty_message = NULL;
    use_checks = false;
    check_on = 251;
    check_off = 250;
    color_itemsel = &COLORS.EditBG;
    color_itemnosel = &COLORS.EditText;
}

ListBox::~ListBox() {
    clear();
}

int ListBox::mouseupdate(int cur_element) {
    KBKey key,act=0;
    key = Keys.checkkey();
    int old_cur_sel = cur_sel;
    int old_y_start = y_start;
    color_itemsel = &COLORS.EditBG;
    color_itemnosel = &COLORS.EditText;
    if (!bMouseIsDown)
        mousestate = 0;
    if (mousestate) {
        int i = (LastY/8) - this->y ;
        if (cur_sel+y_start != i) {
            if (i<0) {
                if (y_start >0 ) {
                    setCursor(y_start-1);
//                    y_start--;
                    act++;
                }
            } else
            if (i>ysize && cur_sel==ysize && y_start<num_elements-ysize-1) {
//                y_start++;
                setCursor(y_start+cur_sel+1);
                act++;
            } else
            if (i<=ysize && i+y_start<num_elements && i+y_start>=0) {
                setCursor(i+y_start);
                act++;
            }
        }
    }
    if (key) {
        switch(key) {
            case DIK_MOUSE_1_ON:
                if (checkclick(col(this->x),row(this->y),col(this->x+this->xsize),row(this->y+this->ysize+1))) {
                    int i = (MousePressY/8) - this->y;
                    if (cur_element == this->ID && i==this->cur_sel) {
                        LBNode *p = this->getNode(this->cur_sel+this->y_start);
                        if (p)
                            OnSelect( p );
                    }
                    mousestate = 1;
                    if (i<=ysize && i<num_elements)
                        setCursor(i+y_start);                    
                    act++;
                }
                break;
            case DIK_MOUSE_1_OFF:
                if (mousestate) {
                    act++;
                }
                mousestate=0;
                break;
        }
    }
    if (cur_sel != old_cur_sel || y_start != old_y_start)
        OnSelectChange();
    if (act)  key = Keys.getkey();
    if (act) {
        need_refresh++;
        need_redraw++;
        return this->ID;
    }

    return cur_element;
}

int ListBox::sortstr(char *s1, char *s2) {
    int i=0;
    if (!s1 || !s2)
        return 0;
    while(s1[i] && s2[i]) {
        if (tolower(s1[i]) < tolower(s2[i])) // s1 comes before, order is correct
            return 1;
        if (tolower(s1[i]) > tolower(s2[i])) // s2 comes before, flip order
            return -1;
        i++;
    }
    if (!s1[i] && s2[i]) // s1 shorter than s2, order is ok
        return 1;
    if (s1[i] && !s2[i]) // s2 shorter than s1, flip order
        return -1;
    return 0; // they are the same
}

void ListBox::strc(char *dst, char *src) {
    int i=0;
    while (i<xsize && src[i]) {
        dst[i] = src[i];
        i++;
    }
}

void ListBox::clear(void) {
    while(items) {
        LBNode *p = items->next;
        delete items;
        items = p;
    }
    items = NULL;
    y_start = num_elements = cur_sel = 0;
}

LBNode *ListBox::getNode(int index) {
    if (index<0) return NULL;
    int i=0;
    LBNode *p = items;
    while (p) {
        if (index == i)
            return p;
        p = p->next;
        i++;
    }
    return NULL;
}

char *ListBox::getItem(int index) {
    if (index<0) return NULL;
    LBNode *p = getNode(index);
    if (p)
        return p->caption;
    else
        return NULL;
}


int ListBox::findItem(char *text) {
    int i=0;
    LBNode *p = items;
    while( p ) {
        if (zcmp(p->caption, text))
            return i;
        p = p->next;
        i++;
    }
    return -1;
}

LBNode * ListBox::insertItem(char *text) {
    LBNode *p = new LBNode;
//    p->caption = strdup(text);
    if (text) {
        p->caption = new char[strlen(text)+1];
        strcpy(p->caption, text);
    }
    p->checked = false;
    if (is_sorted) {
        if (!items)
            items = p;
        else {
            if (sortstr(text,items->caption) >=0 ) { // 0 = same, 1 = order ok
                p->next = items;
                items = p;
            } else {
                LBNode *l = items;
                LBNode *n = items->next;
                while (n) {
                    if (sortstr(n->caption, text) < 0)
                        break;
                    l = n;
                    n = n->next;
                }
                p->next = n;
                l->next = p;
            }
        }
    } else {
        if (items) {
            p->next = items;
        }
        items = p;
    }
    num_elements++;
    return p;
}

void ListBox::removeItem(int index) {
    if (index<0) return;
    int i=1;
    LBNode *p = items;
    LBNode *l = items;
    if (index == 0) {
        items = items->next;
        delete p;
        num_elements--;
        return;
    }
    p = p->next;
    while(p) {
        if (i==index) {
            l->next = p->next;
            delete p;
            num_elements--;
            return;
        }
        l = p;
        p = p->next;
        i++;
    }
    return;
}

bool ListBox::getCheck(int index) {
    if (index<0) return false;
    LBNode *p = getNode(index);
    if (p)
        return p->checked;
    else
        return false;
}

void ListBox::setCheck(int index, bool state) {
    if (index<0) return;
    LBNode *p = getNode(index);
    if (p) {
        p->checked = state;
    }
}
void ListBox::selectAll() {
    LBNode *p = items;
    while(p) {
        p->checked = true;
        p = p->next;
    }
}
void ListBox::selectNone() {
    LBNode *p = items;
    while(p) {
        p->checked = false;
        p = p->next;
    }
}
void ListBox::setCursor(int index) {
    if (index<0) return;
    if (index>=num_elements) return;
    if (index<y_start) {
        cur_sel = 0;
        y_start = index;
    } else
    if (index>y_start+ysize) {
        cur_sel = ysize;
        y_start = index - ysize ;
    } else
        cur_sel = index-y_start;
}

LBNode *ListBox::findNodeWithChar(char c, LBNode *start) {
    LBNode *work = start;
    if (start) {
        work = start->next;
        while( work != start ) {
            if (!work)
                work = items;
            if (work->next == NULL)
                start = NULL;
            char *p = work->caption;
            if (p) {
                if (tolower(p[0]) == tolower(c))
                    return work;
            }
            work = work->next;
        }
    }
    return NULL;
}

int ListBox::update() {
    KBKey key,act=0;
    int ret=0;
    key = Keys.checkkey();
    int old_cur_sel = cur_sel;
    int old_y_start = y_start;
    if (key) {
        if (num_elements && use_key_select) {
            signed int retchar = key;//getKeyChar(key);
            LBNode *p = getNode(cur_sel + y_start);
            if (isalpha(retchar) && p) {
                char c = toupper(retchar);
                LBNode *n = findNodeWithChar(c,p);
                if (n) {
                    this->setCursor(findItem(n->caption));
                    act++;
                }
            }
        }
        
        switch(key) {
            case DIK_TAB: ret = 1; act++; break;
            case DIK_UP: 
                if (cur_sel>0)
                    cur_sel--;
                else
                if (y_start>0)
                    y_start--;
                act++; 
                break;
            case DIK_DOWN: 
                if (cur_sel+y_start<num_elements-1) {
                    if (cur_sel<ysize)
                        cur_sel++;
                    else
                        y_start++;
                }
                act++;
                break;
            case DIK_PGDN:
                if (cur_sel<ysize)
                    cur_sel = ysize;
                else
                    y_start+=ysize;
                act++;
                break;
            case DIK_PGUP:
                if (cur_sel>0)
                    cur_sel = 0;
                else
                    y_start -= ysize;
                act++;
                break;
            case DIK_HOME:
                if (cur_sel>0)
                    cur_sel = 0;
                else
                    y_start=0;
                act++;
                break;
            case DIK_END:
                if (cur_sel<ysize)
                    cur_sel = ysize;
                else
                    y_start = num_elements - ysize -1;
                act++;
                break;
            case DIK_RETURN: 
                OnSelect(getNode(cur_sel+y_start));
                act++; 
                break;
//            case DIK_HOME: cur_sel=0; act++; break;
//            case DIK_END: cur_sel = MidiOut->numMidiDevs-1; act++; break;
        }
        if (act) {
            Keys.getkey();
            need_refresh++;
            need_redraw++;
            if (cur_sel<0) cur_sel=0;
            if (cur_sel>ysize) cur_sel = ysize;
            if (y_start > num_elements - ysize -1 ) y_start = num_elements - ysize -1;
            if (y_start < 0) y_start = 0;
            if (cur_sel>y_start+num_elements) cur_sel = num_elements - y_start-1;
        }
    }
    if (cur_sel != old_cur_sel || y_start != old_y_start)
        OnSelectChange();
    return ret;
}
char *ListBox::getCurrentItem(void) {
    char * p = getItem(cur_sel + y_start);
    if (!p) p = "";
    return p;
}
int ListBox::getCurrentItemIndex(void) {
    return cur_sel+y_start;
}
void ListBox::OnSelect(LBNode *selected) {
    mousestate = 0;
}

void ListBox::draw(Drawable *S, int active) {
    int cy;
    TColor f,b;
    unsigned char *str;
    str = (unsigned char *)malloc(xsize+1+2);
    LBNode *node = getNode(y_start);
    for (cy=0;cy<=ysize;cy++) {
        memset(str,0,xsize+1);
        memset(str,' ',xsize);
        if (node) {
            if (use_checks) {
                if (node->checked)
                    str[0] = check_on;
                else
                    str[0] = check_off;
                strc((char *)str+2, node->caption);
            } else {
                strc((char *)str, node->caption);
            }
            node = node->next;
        }
        if (cy == cur_sel) {
            if (active)
                b = COLORS.Highlight;
            else
                b = COLORS.EditText;
            f = *color_itemsel;
        } else {
            b = COLORS.EditBG;
            f = *color_itemnosel;
        }
        if (num_elements == 0 && cy==0) {
            f = COLORS.EditText;
            b = COLORS.EditBG;
            if (empty_message)
                strc((char*)str, empty_message);
        }
        printBGu(col(x),row(cy+y),str,f,b,S);
    }
    frm.type=0;
    frm.x=x; frm.y=y; frm.xsize=xsize; frm.ysize=ysize+1;
    frm.draw(S,0);
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+1),row(y+ysize+1));
    free(str);
}

SkinSelector::SkinSelector() {
    empty_message = "No skins found";
    is_sorted = true;
    use_checks = true;
    OnChange();
}
SkinSelector::~SkinSelector() {
}
bool file_exists(char *file) {
    FILE *fp;
    fp = fopen(file,"rb");
    if (!fp) return false;
    fclose(fp);
    return true;
}
void SkinSelector::OnChange() {
    clear();
    long res,hnd;
    _finddata_t f;
    char s[512];
    strcpy(s,cur_dir);
    strcat(s,"/skins/*.*");
    hnd = res = _findfirst(s,&f);
    while(res != -1) {
        if (f.attrib & _A_SUBDIR) {
            LBNode *p;
            if (f.name[0] != '.'){
                char str[200];
                strcpy(str,cur_dir);
                strcat(str,"/skins/");
                strcat(str, f.name);
                strcat(str, "/colors.conf");
                if (file_exists(str)) {
                    p = insertItem(f.name);
                    if (zcmp(p->caption,zt_globals.skin))
                        p->checked = true;
                }
            }
        }
        res = _findnext(hnd, &f);
    }
    _findclose(hnd);
}


void SkinSelector::OnSelect(LBNode *selected) {
//    if (selected->checked)
//        return;
    Skin *old = CurrentSkin;
    Skin *todel = CurrentSkin->switchskin(selected->caption);    
    if (old == todel) {
        strcpy(zt_globals.skin,CurrentSkin->strSkinName);
        color_itemsel = &COLORS.EditBG;
        color_itemnosel = &COLORS.EditText;
        selectNone();
        selected->checked = true;
        ListBox::OnSelect(selected);
    }
    delete todel;    
    this->mousestate = 0;
}
void SkinSelector::OnSelectChange() {
}


MidiOutDeviceSelector::MidiOutDeviceSelector() {
    empty_message = "No MIDI-OUT devices open";
    is_sorted = true;
    use_checks = true;
    use_key_select = false; // so you can audition different devices
    OnChange();
}
MidiOutDeviceSelector::~MidiOutDeviceSelector() {
}

void MidiOutDeviceSelector::enter(void) {
    OnChange();
}

void MidiOutDeviceSelector::OnChange() {
    clear();
    for (int i=0;i<MidiOut->GetNumOpenDevs();i++) {
        int dev = MidiOut->GetDevID(i);
        LBNode *p = insertItem((strlen(MidiOut->outputDevices[dev]->alias) > 1)?MidiOut->outputDevices[dev]->alias:MidiOut->outputDevices[dev]->szName);
        if (dev == song->instruments[cur_inst]->midi_device)
            p->checked = true;
        else
            p->checked = false;
        p->int_data = dev;
    }
}
void MidiOutDeviceSelector::OnSelect(LBNode *selected) {
    if (selected->checked) {
        song->instruments[cur_inst]->midi_device =  MAX_MIDI_DEVS;
        selected->checked = false;
    } else {
        song->instruments[cur_inst]->midi_device =  selected->int_data;
        this->selectNone();
        selected->checked = true;
    }
    ListBox::OnSelect(selected);
}
void MidiOutDeviceSelector::OnSelectChange() {
}


MidiOutDeviceOpener::MidiOutDeviceOpener() {
    empty_message = "No MIDI-OUT devices found";
    is_sorted = true;
    use_checks = true;
    OnChange();
}
MidiOutDeviceOpener::~MidiOutDeviceOpener() {
}

void MidiOutDeviceOpener::enter(void) {
    OnChange();
}

void MidiOutDeviceOpener::OnChange() {
    clear();        
    for (unsigned int i=0;i<MidiOut->numOuputDevices;i++) {
//        LBNode *p = insertItem((MidiOut->outputDevices[i]->alias != NULL)?MidiOut->outputDevices[i]->alias:MidiOut->outputDevices[i]->szName);
        LBNode *p = insertItem(MidiOut->outputDevices[i]->szName);
        if (MidiOut->QueryDevice(i))
            p->checked = true;
        else
            p->checked = false;
        p->int_data = i;
    }
}
void MidiOutDeviceOpener::OnSelect(LBNode *selected) {
    midi_out_sel(selected->int_data);
    if (MidiOut->QueryDevice(selected->int_data))
        selected->checked = true;
    else
        selected->checked = false;
    statusmsg = szStatmsg;
    status_change = 1;
    ListBox::OnSelect(selected);
    Keys.flush();
}
void MidiOutDeviceOpener::OnSelectChange() {
}
int MidiOutDeviceOpener::mouseupdate(int cur_element) {
/*    if (cur_element == this->ID)
        bDontKeyRepeat = true;
    else
        bDontKeyRepeat = false;
*/
    return ListBox::mouseupdate(cur_element);
    //return cur_element;
}
int MidiOutDeviceOpener::update() {
    LatencyValueSlider *l = (LatencyValueSlider *)lvs;
    l->sync();
    BankSelectCheckBox *b = (BankSelectCheckBox *)this->bscb;
    b->sync();
    AliasTextInput *t = (AliasTextInput *)al;
    t->sync();
    return ListBox::update();
    //return ret;
}

MidiInDeviceOpener::MidiInDeviceOpener() {
    empty_message = "No MIDI-IN devices found";
    is_sorted = true;
    use_checks = true;
    OnChange();
}
MidiInDeviceOpener::~MidiInDeviceOpener() {
}
int MidiInDeviceOpener::mouseupdate(int cur_element) {
/*
    if (cur_element == this->ID)
        bDontKeyRepeat = true;
    else
        bDontKeyRepeat = false;
*/
    return ListBox::mouseupdate(cur_element);
}

void MidiInDeviceOpener::enter(void) {
    OnChange();
}

void MidiInDeviceOpener::OnChange() {
    clear();        
    for (unsigned int i=0;i<MidiIn->numMidiDevs;i++) {
        LBNode *p = insertItem(MidiIn->midiInDev[i]->szName);
        if (MidiIn->QueryDevice(i))
            p->checked = true;
        else
            p->checked = false;
        p->int_data = i;
    }
}
void MidiInDeviceOpener::OnSelect(LBNode *selected) {
    midi_in_sel(selected->int_data);
    if (MidiIn->QueryDevice(selected->int_data))
        selected->checked = true;
    else
        selected->checked = false;
    statusmsg = szStatmsg;
    status_change = 1;
    ListBox::OnSelect(selected);
    Keys.flush();
}
void MidiInDeviceOpener::OnSelectChange() {
}


DriveList::DriveList() {
    empty_message = "No drives found";
    is_sorted = true;
    OnChange();
    updated = 1;
    
}
DriveList::~DriveList() {
}
void DriveList::enter(void) {
    OnChange();
}
void DriveList::draw(Drawable *S, int active) {
    color_itemnosel = &COLORS.Data;
    ListBox::draw(S,active);
    updated = 0;
}
void DriveList::OnChange() {
    clear();        
    int r;
//    unsigned char *s;
    char str[4];
    unsigned char cap[16],cur[512],save[16];
    strcpy(str,"A:\\");
    r = GetLogicalDrives();
    int cd=0;
    LBNode *p;
    save[0] = 0;
    if(!already_changed_default_directory)
    {
        if(zt_globals.default_directory[0] != '\0')
            SetCurrentDirectory((LPCTSTR)zt_globals.default_directory);
        already_changed_default_directory = 1;
    }
    GetCurrentDirectory(512,(LPSTR)&cur[0]);
    for(int i=0;i<26;i++) {
        if (r&1) {
            strcpy((char *)&cap[0], "  A:");
            cap[2]+=i;
            str[0] = 'A' + i;
            switch(GetDriveType(str)) {
                case DRIVE_CDROM:
                case DRIVE_REMOVABLE:
                    cap[0] = (unsigned char)225;
                    break;
                default:
                    cap[0] = (unsigned char)224;
                    break;
            }
            p = insertItem((char *)&cap[0]);
            p->int_data = i;
            if (tolower(cur[0])==tolower(str[0]))
                strcpy((char *)save,(const char *)cap);
        }
        r>>=1;
    }
    i = findItem((char *)save);
    if (i>=0)
        setCursor(i);
       
}
void DriveList::OnSelect(LBNode *selected) {
    chdir(&selected->caption[2]);
    OnChange();
    updated++;
    ListBox::OnSelect(selected);
}
void DriveList::OnSelectChange() {
}

DirList::DirList() {
    empty_message = "";
    is_sorted = true;
    use_checks = false;
    updated = 1;
    OnChange();
}
DirList::~DirList() {
}
void DirList::enter(void) {
    OnChange();
}
int DirList::update() {
    int key = Keys.checkkey();
    int act=0;
    int ret=0;
    switch(key) {
        case DIK_LEFT: ret = -1; act++; break;
    }
    if (act) {
        Keys.getkey();
        need_refresh++;
        need_redraw++;
        return ret;
    } else
    return ListBox::update();
}
void DirList::draw(Drawable *S, int active) {
    ListBox::draw(S,active);
    updated = 0;
}
void DirList::OnChange() {
    clear();
    long res,hnd;
    _finddata_t f;
    hnd = res = _findfirst("*.*",&f);
    while(res != -1) {
        if (f.attrib & _A_SUBDIR) {
            LBNode *p;
//            if (f.name[0] != '.' && f.name[1]!=0){
                p = insertItem(f.name);
//            }
        }
        res = _findnext(hnd, &f);
    }
    _findclose(hnd);
    need_redraw++;
}

void DirList::OnSelect(LBNode *selected) {
    chdir(selected->caption);
    OnChange();
    updated++;
    char d[1024];
    GetCurrentDirectory(1024,d);
    sprintf(szStatmsg,"Browsing %.70s",d);
    statusmsg = szStatmsg;
    status_change = 1;    
    ListBox::OnSelect(selected);
}
void DirList::OnSelectChange() {
}



FileList::FileList() {
    empty_message = "";
    is_sorted = true;
    use_checks = false;
    updated = 1;
    OnChange();
    onEnter = NULL;
}
FileList::~FileList() {
}
int FileList::update() {
    int key = Keys.checkkey();
    int act=0;
    int ret=0;
    switch(key) {
        case DIK_RIGHT: ret = 1; act++; break;
    }
    if (act) {
        Keys.getkey();
        need_refresh++;
        need_redraw++;
        return ret;
    } else
    return ListBox::update();
}
void FileList::enter(void) {
    OnChange();
}
void FileList::draw(Drawable *S, int active) {
    int cy;
    TColor f,b;
    unsigned char *str;
    str = (unsigned char *)malloc(xsize+1+2);
    LBNode *node = getNode(y_start);
    for (cy=0;cy<=ysize;cy++) {
        memset(str,0,xsize+1);
        memset(str,' ',xsize);
        if (node) {
            strc((char *)str, node->caption);
            f = node->int_data;
            node = node->next;
        } else {
        }
        if (cy == cur_sel) {
            if (active)
                b = COLORS.Highlight;
            else
                b = COLORS.EditText;
            f = COLORS.EditBG;
        } else {
            b = COLORS.EditBG;
        }
        if (num_elements == 0 && cy==0) {
            f = COLORS.EditText;
            b = COLORS.EditBG;
            if (empty_message)
                strc((char*)str, empty_message);
        }
        printBGu(col(x),row(cy+y),str,f,b,S);
    }
    frm.type=0;
    frm.x=x; frm.y=y; frm.xsize=xsize; frm.ysize=ysize+1;
    frm.draw(S,0);
    screenmanager.Update(col(x-1),row(y-1),col(x+xsize+1),row(y+ysize+1));
    free(str);
    updated = 0;
}
void FileList::AddFiles(char *pattern, TColor c) {
    long res,hnd;
    _finddata_t f;
    hnd = res = _findfirst(pattern,&f);
    while(res != -1) {
        LBNode *p;
        p = insertItem(f.name);
        p->int_data = c;
        res = _findnext(hnd, &f);
    }
    _findclose(hnd);
}

void FileList::OnChange() {
    clear();
    AddFiles("*.zt", COLORS.Data);
    AddFiles("*.it", COLORS.Highlight);
    need_redraw++;
}

void FileList::OnSelect(LBNode *selected) {
    if (onEnter)
        onEnter(this);
    updated++;
    ListBox::OnSelect(selected);
    SDL_Delay(1);
}

void FileList::OnSelectChange() {
}


LatencyValueSlider::LatencyValueSlider(MidiOutDeviceOpener *m) {    
    listbox = m;
    sel = -1;
    value=0;
    sync();
}
LatencyValueSlider::~LatencyValueSlider() {
}
void LatencyValueSlider::sync() {
    LBNode *p = listbox->getNode(listbox->getCurrentItemIndex() );
    if (p) {
        if (p->int_data != sel) {
            sel = p->int_data;
            value = MidiOut->get_delay_ticks(sel);
            need_redraw++;
            need_refresh++;
        } else {
            if (MidiOut->get_delay_ticks(sel) != this->value) {
                MidiOut->set_delay_ticks(sel, this->value);
                need_redraw++;
                need_refresh++;
            }
        }
    }
}
int LatencyValueSlider::update() {
    sync(); 
    return ValueSlider::update();
}

BankSelectCheckBox::BankSelectCheckBox(MidiOutDeviceOpener *m) {
    this->value = &i_value;
    listbox = m;
    sel = -1;
    i_value = 0;
    sync();
}

BankSelectCheckBox::~BankSelectCheckBox() {
}
void BankSelectCheckBox::sync(void) {
    LBNode *p = listbox->getNode(listbox->getCurrentItemIndex() );
    if (p) {
        if (p->int_data != sel) {
            sel = p->int_data;
            i_value = MidiOut->get_bank_select(sel);
            need_redraw++;
            need_refresh++;
        } else {
            if (MidiOut->get_bank_select(sel) != this->i_value) {
                MidiOut->set_bank_select(sel, this->i_value);
                need_redraw++;
                need_refresh++;
            }
        }
    }    
}
int BankSelectCheckBox::update() {
    sync(); 
    return CheckBox::update();
}

AliasTextInput::AliasTextInput(MidiOutDeviceOpener *m) {

	for(int i = 0; i < 1024; i++)
		alias[i] = '\0';
    this->str = (alias != NULL)?(unsigned char*)alias:(unsigned char*)"";
    listbox = m;
    cursor = 0;
    length = 1023 ;//(alias != NULL)?strlen(alias):0;
    sel = -1;
    sync();
}

AliasTextInput::~AliasTextInput() {
}

void AliasTextInput::sync(void) {
    
    LBNode *p = listbox->getNode(listbox->getCurrentItemIndex() );
    if (p) {
        if (p->int_data != sel) {
            sel = p->int_data;
            strcpy(alias,MidiOut->get_alias(sel));
            this->cursor = strlen(this->alias);
            need_redraw++;
            need_refresh++;
        } else {
            if (strcmp(MidiOut->get_alias(sel),this->alias)) {
                MidiOut->set_alias(sel, this->alias);
                this->cursor = strlen(this->alias);
                need_redraw++;
                need_refresh++;
            }
        }
    }
}

int AliasTextInput::update() {
    sync(); 
    return TextInput::update();
}

CommentEditor::CommentEditor() {
    bUseColors = FALSE;
    bWordWrap = true;
}

CommentEditor::~CommentEditor() {

}

int CommentEditor::update() {
    KBKey key,act=0;
    int ret=0;
    key = Keys.checkkey();
    unsigned char ch = Keys.getactualchar();
    if (key) {
        switch(key) {
        //case SDLK_m
        case DIK_UP  : act++; startline--;  break;
        case DIK_DOWN: act++; if (!bEof) startline++;  break;
        case DIK_PGUP: act++; startline-=16;  break;
        case DIK_PGDN: act++; if (!bEof) startline+=16;  break;
        case DIK_LEFT: act++; soff--; break;
        case DIK_RIGHT:act++; soff++; break;
        case DIK_HOME: if (soff>0) soff=0; else startline=0; act++; break;

        default:
            if (ch != 0x0) {
                if (target) {
                    target->pushc(ch);
                    act++;
                }
            }
            //        case DIK_SPACE:  //  / Same thing
//        case DIK_RETURN: //  \ Same thingg
//            break;
        }
        if (act) {
            Keys.getkey();
                need_refresh++;
            need_redraw++;

        }
    }
    if (startline<0) startline = 0;
    if (soff<0) soff=0;
    return ret;
}



/* eof */
