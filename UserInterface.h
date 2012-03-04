#ifndef _UserInterface_H
#define _UserInterface_H

#include "zt.h"

void midi_sel(int dev);

class UserInterfaceElement {

    public:
        int x,y;
        int xsize,ysize;
        int changed;
        int ID,mousestate;
        int frame;
        int need_redraw;

        UserInterfaceElement(void);
        virtual ~UserInterfaceElement(void);
        
        virtual int mouseupdate(int cur_element);
        virtual void enter(void);
        virtual int update()=0;
        virtual void draw(Drawable *S, int active)=0;

        UserInterfaceElement *next;
};

typedef void (*ActFunc)(UserInterfaceElement *b);

class UserInterface {

    private:

        UserInterfaceElement *UIEList,*GFXList;

    public:

        UserInterface();
        ~UserInterface();

        signed int cur_element,num_elems;
        int dontmess;

        void update();
        void draw(Drawable *S);
        void set_redraw(int ID);
        void full_refresh(void);
        void enter(void);
        void add_element(UserInterfaceElement *uie, int ID);
        void add_gfx(UserInterfaceElement *uie, int ID);
        void set_focus(int id);
        UserInterfaceElement *get_element(int ID);  
        UserInterfaceElement *get_gfx(int ID);

        void reset(void);
};


class Frame : public UserInterfaceElement {
    public:
        int type;
        Frame();
        ~Frame();
        int update();
        void draw(Drawable *S, int active);
};

class CheckBox : public UserInterfaceElement {
    public:
        int *value;
        CheckBox();
        ~CheckBox();
        int mouseupdate(int cur_element);
        int update();
        void draw(Drawable *S, int active);
};


class ValueSlider : public UserInterfaceElement {
    public:
        int min;
        int max;
        int value;
        int force_f;
        int force_v;
        int changed;
        int clear;
        int newclick;
        int focus;  // new value here
        ValueSlider();
        ValueSlider(int fset); // new constructor
        ~ValueSlider();
        int mouseupdate(int cur_element);
        virtual int update();
        void draw(Drawable *S, int active);
};
 

class ValueSliderDL : public UserInterfaceElement {
    public:
        int min;
        int max;
        int value;
        int changed;
        int newclick;
        int focus;  // new value here
        ValueSliderDL();
        ValueSliderDL(int fset); // new constructor
        ~ValueSliderDL();
        int mouseupdate(int cur_element);
        int update();
        void draw(Drawable *S, int active);
};
class ValueSliderOFF : public UserInterfaceElement {
    public:
        int min;
        int max;
        signed int value;
        int changed;
        int newclick;
        int focus;  // new value here
        ValueSliderOFF();
        ValueSliderOFF(int fset); // new constructor
        ~ValueSliderOFF();
        int mouseupdate(int cur_element);
        int update();
        void draw(Drawable *S, int active);
};
class TextInput : public UserInterfaceElement {
    public:
        int cursor;
        int length;
        unsigned char *str;
        TextInput();
        ~TextInput();
        int update();
        void draw(Drawable *S, int active);
};

class InstEditor : public UserInterfaceElement {
    public:
        Frame *frm;
        int cursor,text_cursor;
        int list_start;
//      int length;
        unsigned char *str;
        InstEditor();
        ~InstEditor();

        void strc(char *dst, char *src);
        int update();
        void draw(Drawable *S, int active);
        int mouseupdate(int cur_element);
};

class OrderEditor : public UserInterfaceElement {
    public:
        Frame *frm;
        int cursor_y,cursor_x;
        int list_start;
        int old_playing_ord;

        OrderEditor();
        ~OrderEditor();
        int mouseupdate(int cur_element);
        int update();
        void draw(Drawable *S, int active);
};

#define LSFILE_NONE 0
#define LSFILE_ZT 1
#define LSFILE_IT 2


class Button : public UserInterfaceElement {
    public:
        char *caption;
        int state;
        int updown;
        ActFunc OnClick;

        Button();
        ~Button();

        int update();
        void draw(Drawable *S, int active);
        void setOnClick(ActFunc func);
        int mouseupdate(int cur_element);

};

class GfxButton : public UserInterfaceElement {
    public:
        int state;
        int updown;
        int StuffKey,StuffKeyState;
        ActFunc OnClick;

        Bitmap *bmDefault;
        Bitmap *bmOnMouseOver;
        Bitmap *bmOnClick;

        GfxButton();
        ~GfxButton();

        int update();
        void draw(Drawable *S, int active);
        void setOnClick(ActFunc func);
        int mouseupdate(int cur_element);

};

struct playedinfo {

    event e;
    int longevity;
    int init_vol;
    int init_longevity;
};

class VUPlay : public UserInterfaceElement {

    public:

        int cur_row;
        int cur_pattern;
        int cur_order;
        int num_channels;
        int starttrack;
        playedinfo latency[64]; // These are set to some number when a note on track n is played
                                // and is decremented each run for doing a fake-ass volume

        VUPlay();
        ~VUPlay();

        int update();
        void draw(Drawable *S, int active);
        int draw_one_row(char str[72], event *e, int track);

};

class PatternDisplay : public UserInterfaceElement {

    public:

        Frame *frm;
        int disp_row;
        int cur_pat_view;
        int starttrack;
        int tracks,tracksize;
        int clear;

        int cur_track;


        PatternDisplay();
        ~PatternDisplay();

        int update();
        void draw(Drawable *S, int active);

        int next_order(void);
        void disp_playing_row(int x,int y, int pattern, int row, Drawable *S, TColor bg);
        void update_frame(void);
        void disp_playing_pattern(Drawable *S);
        char* printNote(char *str, event *r);
        void disp_track_headers(Drawable *S);

};

class BarGraph : public UserInterfaceElement {

    public:
        int max, maxval, value, trackmaxval;

        BarGraph();
        ~BarGraph();

        int update();
        void draw(Drawable *S, int active);

        void setvalue(int val);
};

class LCDDisplay : public UserInterfaceElement {
    public:
        int length;
        char *str;
        char istr[200];
        LCDDisplay();
        ~LCDDisplay();

        int update();
        void draw(Drawable *S, int active);
        void setstr(char *s);
};

class AboutBox : public UserInterfaceElement {
    public:
//        int length;
//        char *str;
        Frame frm;
        int q;
        AboutBox();
        ~AboutBox();

        int update();
        void draw(Drawable *S, int active);

};

class TextBox : public UserInterfaceElement {
    public:
        bool bEof,bUseColors,bWordWrap;
        int readonly, startline, soff,mousestate,starti,startl;
        char *text;
        Frame frm;

        TextBox(int ro = 0);
        ~TextBox();

        int update();
        void draw(Drawable *S, int active);
        int mouseupdate(int cur_element);
};

class CommentEditor : public TextBox {
    public:
        
        CDataBuf *target;

        CommentEditor();
        ~CommentEditor();

        int update();
        
};
class LBNode {
    public:
        LBNode() {
            next = NULL;
            caption = NULL;
            int_data = 0;
        }
        ~LBNode() {
            if (caption)
                delete[] caption;
        }
        char *caption;
        bool checked;
        int int_data;
        LBNode *next;
};

class ListBox : public UserInterfaceElement {
    protected:
    public:
        int cur_sel;
        int y_start;
        int num_elements;
//        int sorted;
        bool use_checks, use_key_select, is_sorted;
        unsigned char check_on, check_off;
        char *empty_message;
        LBNode *items;
        Frame frm;
            
        TColor *color_itemsel, *color_itemnosel;


        ListBox();
        ~ListBox();

        virtual int update();
        virtual void draw(Drawable *S, int active);

        int getCurrentItemIndex(void);
        char *getCurrentItem(void);
        LBNode *getNode(int index);
        char *getItem(int index);
        int findItem(char *text);
        LBNode *insertItem(char *text);
        void removeItem(int index);
        void clear(void);
        bool getCheck(int index);
        void setCheck(int index, bool state);
        void selectAll(void);
        void selectNone(void);
        void setCursor(int index);
        void strc(char *dst, char *src);
        int sortstr(char *s1, char *s2);
        int mouseupdate(int cur_element);
        LBNode *findNodeWithChar(char c, LBNode *start);

        virtual void OnChange()=0;
        virtual void OnSelect(LBNode *selected);
        virtual void OnSelectChange()=0;
};

class SkinSelector : public ListBox {
    public:

        SkinSelector();
        ~SkinSelector();

//        virtual int update();
//        virtual void draw(Drawable *S, int active);

        virtual void OnChange();
        virtual void OnSelect(LBNode *selected);
        virtual void OnSelectChange();
};


class MidiOutDeviceSelector : public ListBox {
    public:

        MidiOutDeviceSelector();
        ~MidiOutDeviceSelector();

//        virtual int update();
//        virtual void draw(Drawable *S, int active);

        virtual void OnChange();
        virtual void OnSelect(LBNode *selected);
        virtual void OnSelectChange();
        virtual void enter(void);

};



class MidiInDeviceOpener : public ListBox {
    public:

        MidiInDeviceOpener ();
        ~MidiInDeviceOpener ();

//        virtual int update();
//        virtual void draw(Drawable *S, int active);

        virtual int mouseupdate(int cur_element);
        virtual void OnChange();
        virtual void OnSelect(LBNode *selected);
        virtual void OnSelectChange();
        virtual void enter(void);

};

class DriveList : public ListBox {
    public:

        int updated;
        
        DriveList();
        ~DriveList();
        virtual void draw(Drawable *S, int active);
        virtual void OnChange();
        virtual void OnSelect(LBNode *selected);
        virtual void OnSelectChange();
        virtual void enter(void);

};


class DirList : public ListBox {
    public:

        int updated;
        
        DirList();
        ~DirList();
        virtual int update();
        virtual void draw(Drawable *S, int active);
        virtual void OnChange();
        virtual void OnSelect(LBNode *selected);
        virtual void OnSelectChange();
        virtual void enter(void);

};

class FileList: public ListBox {
    public:

        int updated;
        ActFunc onEnter;
        
        FileList();
        ~FileList();
        virtual int update();
        virtual void draw(Drawable *S, int active);
        virtual void OnChange();
        virtual void OnSelect(LBNode *selected);
        virtual void OnSelectChange();
        virtual void enter(void);
        void AddFiles(char *pattern, TColor c);
};
void dev_sel(int dev, MidiOutDeviceSelector *mds );


class MidiOutDeviceOpener : public ListBox {
    public:

        MidiOutDeviceOpener();
        ~MidiOutDeviceOpener();
        UserInterfaceElement *lvs;
        UserInterfaceElement *bscb;
        UserInterfaceElement *al;
//        virtual int update();
//        virtual void draw(Drawable *S, int active);
        virtual int mouseupdate(int cur_element);

        virtual void OnChange();
        virtual void OnSelect(LBNode *selected);
        virtual void OnSelectChange();
        virtual void enter(void);
        virtual int update(void);

};

class LatencyValueSlider : public ValueSlider {
    public:
        LatencyValueSlider(MidiOutDeviceOpener *m);
        ~LatencyValueSlider();
        MidiOutDeviceOpener *listbox;
        virtual int update();
        void sync(void);
        int sel;
}; 
class BankSelectCheckBox : public CheckBox {
    public:
        int i_value,sel;
        MidiOutDeviceOpener *listbox;
        BankSelectCheckBox(MidiOutDeviceOpener *m);
        ~BankSelectCheckBox();
        virtual int update();
        void sync();
};

class AliasTextInput : public TextInput {
    public:
        char alias[1024];
        int sel;
        MidiOutDeviceOpener *listbox;
        AliasTextInput(MidiOutDeviceOpener *m);
        ~AliasTextInput();
        virtual int update();
        void sync();
};

#endif