//  $Id$
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "track_sel.hpp"
#include "widget_set.hpp"
#include "race_manager.hpp"
#include "track_manager.hpp"
#include "track.hpp"
#include "menu_manager.hpp"
#include "config.hpp"
#include "material.hpp"
#include "material_manager.hpp"

enum WidgetTokens {
  WTOK_RETURN,
  WTOK_OPTIONS,
  WTOK_RESTART,
  WTOK_EXIT,
};

TrackSel::TrackSel() {
  menu_id = widgetSet -> vstack(0);

  widgetSet -> label(menu_id, "Choose a Track", GUI_LRG, GUI_TOP, 0, 0);
  widgetSet -> space(menu_id);

  int ha = widgetSet -> harray(menu_id);

  int col1 = widgetSet -> varray(ha);
  int col2 = widgetSet -> varray(ha);

  for (size_t i = 0; i != track_manager->getTrackCount()/2; ++i)
    widgetSet -> state(col1, track_manager->getTrack(i)->getName(), GUI_SML, i, 0);

  for (size_t i = track_manager->getTrackCount()/2; 
      i != track_manager->getTrackCount(); ++i)
  {
    int tmp = widgetSet -> state(col2, track_manager->getTrack(i)->getName(), GUI_SML, i, 0);
    if (i == track_manager->getTrackCount()/2)
      widgetSet -> set_active(tmp);
  }

  widgetSet -> layout(menu_id, 0, 1);
  rect = widgetSet->rect(10, 10, config->width-20, 34, GUI_ALL, 10);
}

TrackSel::~TrackSel() {
  widgetSet -> delete_widget(menu_id);
  glDeleteLists(rect, 1);
}
	
void TrackSel::update(float dt) {
  BaseGUI::update(dt);

  glClear(GL_DEPTH_BUFFER_BIT);

  // draw a track preview of the currently highlighted track menu entry
  int clicked_token= widgetSet->token(widgetSet->click());
  const Track* track= track_manager->getTrack(clicked_token);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, config->width, 0.0, config->height, -1.0, +1.0);
  const std::string& screenshot = track->getScreenshotFile();
  const std::string& topview    = track->getTopviewFile();
  if(screenshot.size()==0 && topview.size()==0) {
    glDisable ( GL_TEXTURE_2D ) ;
    glColor3f ( 0.0f, 0.0f, 0.0f ) ;
    track->drawScaled2D(0.0, 50, config->width, config->height/3); // (x, y, w, h)
    glColor3f ( 1.0f, 1.0f, 1.0f ) ;
    track->drawScaled2D(1.0, 51, config->width, config->height/3);
    glEnable ( GL_TEXTURE_2D ) ;
  } else {                   // either topview or screenshot specified
    int xLeft   = config->width/2;
    int yBottom = 50;
    int w       = config->width/3;
    int h       = config->height/3;
    if(topview.size()==0) {  // no topview, but there is a screenshot!
      glDisable ( GL_TEXTURE_2D ) ;
      glColor3f ( 0.0f, 0.0f, 0.0f ) ;
      track->drawScaled2D(xLeft, yBottom, w, h);
      glColor3f ( 1.0f, 1.0f, 1.0f ) ;
      track->drawScaled2D(xLeft + 1, yBottom + 1, w, h);
      glEnable ( GL_TEXTURE_2D ) ;
    } else {                 // topview is defined
      Material *m=material_manager->getMaterial(topview);
      m->apply();
      glBegin(GL_QUADS) ;
        glColor4f(1, 1, 1, 1 );
        glTexCoord2f(0, 0); glVertex2i( xLeft,   yBottom   );
        glTexCoord2f(1, 0); glVertex2i( xLeft+w, yBottom  );
        glTexCoord2f(1, 1); glVertex2i( xLeft+w, yBottom+h);
        glTexCoord2f(0, 1); glVertex2i( xLeft,   yBottom+h);
      glEnd () ;
      
    }   // topview is defined
    Material *m=material_manager->getMaterial(screenshot);
    xLeft = xLeft - w - 10;
    m->apply();
    glBegin(GL_QUADS) ;
      glColor4f(1, 1, 1, 1 );
      glTexCoord2f(0, 0); glVertex2i( xLeft,   yBottom   );
      glTexCoord2f(1, 0); glVertex2i( xLeft+w, yBottom  );
      glTexCoord2f(1, 1); glVertex2i( xLeft+w, yBottom+h);
      glTexCoord2f(0, 1); glVertex2i( xLeft,   yBottom+h);
    glEnd () ;
  }   // either topview or screenshot specified
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_BLEND);

  glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    const GLfloat backgroundColour[4] = { 0.3f, 0.3f, 0.3f, 0.5f };
    glColor4fv(backgroundColour);
    glCallList(rect);
  glPopMatrix();
  widgetSet->drawText(track->getDescription(), GUI_MED, SCREEN_CENTERED_TEXT, 10,
		      255,255,255);
  glDisable(GL_BLEND);
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  
}

void TrackSel::select() {
  int clicked_token= widgetSet->token(widgetSet->click());
  const Track* track= track_manager->getTrack(clicked_token);
  race_manager->setTrack(track->getIdent());

  menu_manager->pushMenu(MENUID_NUMLAPS);
}

/* EOF */
