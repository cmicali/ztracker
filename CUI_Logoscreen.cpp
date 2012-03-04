#include "zt.h"

CUI_Logoscreen::CUI_Logoscreen(void) {

}

CUI_Logoscreen::~CUI_Logoscreen(void) {

}

void CUI_Logoscreen::enter(void) {
    cur_state = STATE_LOGO;
    faded = 0;
    ready_fade = 0;
    CurrentSkin->getLogo();
    need_refresh++;
}

void CUI_Logoscreen::leave(void) {
    CurrentSkin->freeLogo();
}

void CUI_Logoscreen::update() {
    if (Keys.size()) {
        Keys.flush();
        doredraw = 1;
        cur_state = STATE_HELP;
        Keys.insert(SDLK_F2,KMOD_NONE);
    }
}

void CUI_Logoscreen::draw(Drawable *S) {
    if (CurrentSkin->bmLogo->surface->w == CONSOLE_WIDTH && CurrentSkin->bmLogo->surface->h == CONSOLE_HEIGHT) {
        S->copy(CurrentSkin->bmLogo, 0,0);
    } else {
        double xscale = (double)CONSOLE_WIDTH / (double)CurrentSkin->bmLogo->surface->w;
        double yscale = (double)CONSOLE_HEIGHT / (double)CurrentSkin->bmLogo->surface->h;
        Drawable s( zoomSurface(CurrentSkin->bmLogo->surface, xscale, yscale ,SMOOTHING_ON) , true );
        S->copy(&s,0,0);
    }
    need_refresh = 0; updated=2; faded++;
}
