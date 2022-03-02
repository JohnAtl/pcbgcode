import processing.core.*; 
import processing.xml.*; 

import java.applet.*; 
import java.awt.*; 
import java.awt.image.*; 
import java.awt.event.*; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class viewer extends PApplet {

/**
 * viewer
 * 
 * Copyright 2013 by John Johnson Software, LLC
 * All Rights Reserved
 *
 * Load line coordinates from a file and draw them.
 *
 */
 
String m_viewer_version = "Viewer 1.6";

/*
 * Representation of a line.
 *
 */
class Line {
  float sx;
  float sy;
  float ex;
  float ey;
  int pass;  // What pass this line was created on.

  Line(float x1, float y1, float x2, float y2, int pass_num) {
    sx = x1;
    sy = y1;
    ex = x2;
    ey = y2;
    pass = pass_num;
  }
}

Line[]  plines = new Line[1];

/*
 * Representation of an arc.
 *
 */
class Arc {
  float xc;
  float yc;
  float sang;
  float eang;
  float radius;
  int pass;

  Arc(float xcenter, float ycenter, float start_angle, float end_angle, float a_radius, int pass_num) {
    xc = xcenter;
    yc = ycenter;
    sang = start_angle;
    eang = end_angle;
    radius = a_radius;
    pass = pass_num;
  }
}

Arc[] arcs = new Arc[1];

/*
 * When lines are drawn in color, the colors used follow the
 * standard resistor color codes. After 9, the sequence repeats.
 * 0 is cyan, rather than black, as that would be a little hard
 * to see on the black background.
 * Colors are also slightly (204/255) transparent so you can see where
 * they overlap.
 *
 */
int[] color_table = {
  0xCC3399FF, /* pass 0, cyan */
  0xCC993300, /* 1 = brown */
  0xCCFF0000,
  0xCCFF9900,
  0xCCFFFF00, /* yellow */
  0xCC33FF00,
  0xCC0000FF,
  0xCC660099, /* violet */
  0xCCCCCCCC, /* gray */
  0xCCFFFFFF  /* white */
};

int bg_color = 0xff000000;

float x_scale = -200;
float y_scale = -200;
float x_offset = 0;
float y_offset = 300;

//float minx = 999;
//float miny = 999;
//float maxx = -999;
//float maxy = -999;
float minx = 0;
float miny = 0;
float maxx = 0;
float maxy = 0;

PFont metaBold;

String filename = "optimize_me.txt";

String comment = "";
float tool_size = 0.001f;
boolean tool_size_set = false;
float tool_depth = 0.000f;
boolean tool_depth_set = false;
boolean have_spots = false;
int m_pass;
int total_passes;
boolean m_monochrome = false;

int m_window_width = 800;
int m_window_height = 600;
int m_window_x = 0;
int m_window_y = 0;

/*
 * Parse Strings and produce Lines.
 *
 */
public void prepare_objects(String[] lines, Arc[] arcs) {
  int i;

  for (i=0; i < lines.length; i++) {
    if (lines[i] != null) {
      //  println(lines[i]);
      String[] pieces = split(lines[i], ',');
      if (pieces.length == 5) {
        float x1 = PApplet.parseFloat(pieces[0]);
        float y1 = PApplet.parseFloat(pieces[1]);
        float x2 = PApplet.parseFloat(pieces[2]);
        float y2 = PApplet.parseFloat(pieces[3]);
        int m_pass = PApplet.parseInt(pieces[4]);
        plines[i] = new Line(x1, y1, x2, y2, m_pass);
        // println(nfs(plines[i].sx, 1, 5) +", " + nfs(plines[i].sy, 1, 5) + "   " + nfs(plines[i].pass, 2));
        minx = min(minx, x1);
        miny = min(miny, y1);
        maxx = max(maxx, x1);
        maxy = max(maxy, y1);
        minx = min(minx, x2);
        miny = min(miny, y2);
        maxx = max(maxx, x2);
        maxy = max(maxy, y2);
      }
    }
  }

  for (i=0; i < arcs.length; i++) {
    Arc a = arcs[i];
    if (a != null) {
      minx = min(minx, a.xc - a.radius);
      miny = min(miny, a.yc - a.radius);
      maxx = max(maxx, a.xc + a.radius);
      maxy = max(maxy, a.yc + a.radius);
    }
  }
  /* println("prepare_lines exit");
   println("minx = " + nfs(minx, 1, 3));
   println("miny = " + nfs(miny, 1, 3));
   println("maxx = " + nfs(maxx, 1, 3));
   println("maxy = " + nfs(maxy, 1, 3));
   */
}

/*
 * Calculate scaling based on the extents of the trace coordinates.
 *
 */
public void set_scaling() {
  x_scale = (width - 100) / (maxx - minx);
  y_scale = (height - 100) / (maxy - miny);
  // println("x_scale  = " + nfs(x_scale, 1, 3));
  // println("y_scale  = " + nfs(y_scale, 1, 3));

  y_scale = x_scale = min(x_scale, y_scale);
  y_scale = -abs(y_scale);

  if (minx < 0) {
    x_offset += abs(minx) * x_scale;
  }
  x_offset += 20;
  y_offset = height - 40;

  /*
  println("x_scale  = " + nfs(x_scale, 1, 3));
   println("y_scale  = " + nfs(y_scale, 1, 3));
   println("x_offset = " + nfs(x_offset, 1, 3));
   println("y_offset = " + nfs(y_offset, 1, 3));
   */
}

/*
 * Right-justified text.
 *
 */
public void rtext(String s, float x, float y) {
  text(s, x - textWidth(s), y);
}

public void resize_window() {
  m_window_x = (screen.width - m_window_width) / 2;
  m_window_y = (screen.height = m_window_height) / 4; // screen.height seems broken on Mac OS returning 600 for 900 high screen
  size(m_window_width, m_window_height);
  frame.setLocation(m_window_x, m_window_y);
  
/*  
  println("screen.width = " + nfs(screen.width, 5));
  println("m_window_x = " + nfs(m_window_x, 5));
  println("m_window_width = " + nfs(m_window_width, 5));
  println("screen.height = " + nfs(screen.height, 5));
  println("m_window_y = " + nfs(m_window_y, 5));
  println("m_window_height = " + nfs(m_window_height, 5));
  */
}


/*
 * Setup the window.
 * Read and parse the file.
 *
 */
public void setup() {
  String matches[];
  
  String line = null;
  String[] lines = new String[1];
  BufferedReader reader = createReader(filename);
  do {
    try {
      line = reader.readLine();
    }
    catch (IOException e) {
      println("IO exception caught");
    }
    if (line != null) {
      matches = match(line, "^# board=(.+)");
      if (matches != null) {
        comment = matches[1];
      }
      matches = match(line, "^# tool size=(.+)");
      if (matches != null) {
        tool_size = PApplet.parseFloat(matches[1]);
        tool_size_set = true;
      }
      matches = match(line, "^# depth=(.+)");
      if (matches != null) {
        tool_depth = PApplet.parseFloat(matches[1]);
        tool_depth_set = true;
      }
      matches = match(line, "^# pass=(.+)");
      if (matches != null) {
        m_pass = PApplet.parseInt(matches[1]);
        total_passes = max(total_passes, m_pass);
      }
      matches = match(line, "^# spot drills");
      if (matches != null) {
        m_pass = 0;
        have_spots = true;
      }
      matches = match(line, "^# arc xc=([-+]*[0-9]*\\.[0-9]+) yc=([-+]*[0-9]*\\.[0-9]+) sang=([-+]*[0-9]*\\.[0-9]+) eang=([-+]*[0-9]*\\.[0-9]+) radius=([-+]*[0-9]*\\.[0-9]+)");
      if (matches != null) {
        Arc a = new Arc(PApplet.parseFloat(matches[1]), PApplet.parseFloat(matches[2]), PApplet.parseFloat(matches[3]), PApplet.parseFloat(matches[4]), PApplet.parseFloat(matches[5]),  PApplet.parseInt(m_pass));
        //
        // Negative radius indicates arc on bottom of board
        if (a.radius < 0) {
          a.radius = abs(a.radius);
          a.sang += 180;
          a.eang += 180;
          //println("bottom arc");
        }
        
        //
        // Otherwise, arc is on top of board
        // (The code in the else statement below is about 3 hours work. Just sayin.)
        else {
          a.sang = 360 - a.sang;
          a.eang = 360 - a.eang;
          if (a.sang > a.eang) {
            float t = a.sang;
            a.sang = a.eang;
            a.eang = t;
          }
        }
        arcs = (Arc[])append(arcs, a);
        //println("made an arc " + line);
      }
      matches = match(line, "^# preview window width=([0-9]+) height=([0-9]+)");
      if (matches != null) {
        m_window_width = PApplet.parseInt(matches[1]);
        m_window_height = PApplet.parseInt(matches[2]);
        size(m_window_width, m_window_height);
        //println("window size set to (" + nf(m_window_width, 5) + ", " + nf(m_window_height, 5) + ")");
      }

      matches = match(line, "^# debug");
      if (matches != null) {
        println(line);
      }
      // the catch-all case, 
      // if the line doesn't begin with a #, assume it is a line segment
      matches = match(line, "^#");
      if (matches == null) {
        lines = (String[])append(lines, (line + "," + nf(m_pass,2)));
      }
    }
  } 
  while (line != null);
  plines = (Line[])expand(plines, lines.length);
  prepare_objects(lines, arcs);
  set_scaling();
  noLoop();
}

/*
 * Draw a Line.
 *
 */
public void draw_line(Line l) {
  if (l != null) {
    if (! m_monochrome) {
      stroke(color_table[l.pass % 9]);
    }
    line(l.sx * x_scale + x_offset, l.sy * y_scale + y_offset,
    l.ex * x_scale + x_offset, l.ey * y_scale + y_offset);
    //println(nfs(l.sx, 1, 5) +", " + nfs(l.sy, 1, 5));      
    //print(nfs(l.sx * x_scale + x_offset, 1, 5) +", " + nfs(l.sy * y_scale + y_offset, 1, 5) + ", ");      
    //println(nfs(l.ex * x_scale + x_offset, 1, 5) +", " + nfs(l.ey * y_scale + y_offset, 1, 5));      
  }
  else {
    println("null line");
  }
}

/*
 * Draw an arc.
 *
 */
public void draw_arc(Arc a) {
  if (a != null) {
    //println("non-null arc");
    if (! m_monochrome) {
      stroke(color_table[a.pass % 9]);
    }
    noFill();
    arc(a.xc * x_scale + x_offset, a.yc * y_scale + y_offset,
    a.radius * x_scale * 2, a.radius * -y_scale * 2,
    radians(a.sang), radians(a.eang));
    //println("x_scale = " + nfs(x_scale, 3, 5) + " y_scale = " + nfs(y_scale, 3, 5));
    //println(nfs(a.xc * x_scale + x_offset, 2, 5) + ", " + nfs(a.yc * y_scale + y_offset, 2, 5) + ", " + nfs(a.radius * x_scale, 1, 5) + ", " + nfs(a.sang, 3, 5) + ", " + nfs(a.eang, 3, 5) + ", " + nfs(a.pass, 1, 2));
  }
  else {
    println("null arc");
  }
}

/*
 * X scale and offset.
 *
 */
public float xso(float x) {
  return x * x_scale + x_offset;
}

/*
 * Y scale and offset.
 *
 */
public float yso(float y) {
  return y * y_scale + y_offset;
}

/*
 * Text notes around the periphery of the window.
 *
 */
public void ornaments() {
  stroke(255, 0, 0);
  fill(255, 0, 0);
  strokeWeight(1);

  // lower-left corner
  line(xso(minx), yso(miny) - 10, xso(minx), yso(miny));
  line(xso(minx), yso(miny), xso(minx) + 10, yso(miny));

  text(nfs(minx, 1, 3) + ", " + nfs(miny, 1, 3), xso(minx) + 5, yso(miny) + 12);

  // lower-right 
  line(xso(maxx), yso(miny) - 10, xso(maxx), yso(miny));
  line(xso(maxx), yso(miny), xso(maxx) - 10, yso(miny));

  // upper-right
  line(xso(maxx) - 10, yso(maxy), xso(maxx), yso(maxy));
  line(xso(maxx), yso(maxy), xso(maxx), yso(maxy) + 10);
  rtext(nfs(maxx, 1, 3) + ", " + nfs(maxy, 1, 3), xso(maxx), yso(maxy) - 5);  

  // upper-left corner
  line(xso(minx) + 10, yso(maxy), xso(minx), yso(maxy));
  line(xso(minx), yso(maxy), xso(minx), yso(maxy) + 10);

  // cross-hoirs at the origin
  line(xso(0)-10, yso(0), xso(0)+10, yso(0));
  line(xso(0), yso(0)-10, xso(0), yso(0)+10);
  noFill();
  ellipse(xso(0), yso(0), 10, 10);

  // comment from the file  
  stroke(255);
  text(comment, 10, 20);

  // tool size and number of passes
  if (tool_size_set) {
    rtext("tool size " + nfs(tool_size, 1, 3), width - 120, 20);
  }
  if (tool_depth_set) {
    rtext("depth " + nfs(tool_depth, 1, 3), width - 120, 20);
  }
  rtext(nfs(total_passes, 1) + " passes", width - 40, 40);

  // brief help
  text("Keys: +/- zoom, 1 no zoom, 2 zoom 2x, arrows move, a left, de right, w, up, so down", 10, 40);
  text("c toggle color, qx quit", 60, 60);
}

float m_scale = 1.0f;
float m_trans_x = 0;
float m_trans_y = 0;
int draw_cnt = 0;
boolean m_drawing;
boolean m_need_resize = true;

public void draw() {
  m_drawing = true;
  background(bg_color);
  stroke(127);
  fill(bg_color);
  strokeWeight(4);

  if (m_need_resize) {  
    resize_window();
    m_need_resize = false;
  }
  
  quad(0, 0, width-1, 0, width-1, height-1, 0, height-1);
  strokeWeight(1);
  
  fill(255, 0, 0);
  metaBold = loadFont("BankGothic-Light-14.vlw");
  textFont(metaBold);
  rtext(m_viewer_version, width - 20, 20);

  stroke(200);
  scale(m_scale);
  translate(m_trans_x, m_trans_y);
  if (plines != null) {
    //println("plines.length = " + nf(plines.length, 3));
    strokeWeight(tool_size * x_scale);
    for (int i = 0; i < plines.length; i++) {
      draw_line(plines[i]);
    }
  }
  else {
    text("Didn't open the file", 100, height / 2);
  }
  if (arcs != null) {
    //println("arcs.length = " + nf(arcs.length, 3));
    strokeWeight(tool_size * x_scale);
    for (int i = 0; i < arcs.length; i++) {
      draw_arc(arcs[i]);
    }
  }

  ornaments();
  m_drawing = false;
}

public void keyPressed() {
  if (m_drawing) {
    return;
  }
  switch (key) {

    /*
     * Toggle colored lines.
     *
     */
  case 'c':
  case 'C':
    m_monochrome = ! m_monochrome;
    break;

    /*
     * Zoom in or out.
     *
     */
  case '+':
  case '=':
    if (m_scale < 10) {
      m_scale += 0.2f;
      m_trans_x -= width / m_scale / 20;
      m_trans_y -= height / m_scale / 20;
    }
    break;
  case '_':
  case '-':
    if (m_scale > 0.5f) {
      m_scale -= 0.2f;
      m_trans_x += width / m_scale / 20;
      m_trans_y += height / m_scale / 20;
    }
    break;

    /*
     * Fixed zoom amounts.
     *
     */
  case '1':
    m_scale = 1;
    m_trans_x = 0;
    m_trans_y = 0;
    break;
  case '2':
    m_scale = 2;
    m_trans_x = -width / 4;
    m_trans_y = -height / 4;
    break;

    /*
     * Exit the program.
     *
     */
  case 'x':
  case 'X':
  case 'q':
  case 'Q':
    exit();
    break;

    /*
     * Small movement left, right, up, down.
     *
     * Use these keys:
     *
     *   QWERTY       Dvorak
     *      w           ,
     *   a     d     a     e
     *      s           o
     *
     */
  case 'a':
    m_trans_x += width / 80;
    break;
  case 'e':
  case 'd':
    m_trans_x -= width / 80;
    break;
  case ',':
  case 'w':
    m_trans_y += width / 80;
    break;
  case 'o':
  case 's':
    m_trans_y -= width / 80;
    break;
    
    /*
     * Move using the arrow keys.
     *
     */
  case CODED:
    switch (keyCode) {
    case LEFT:
      m_trans_x += width / 20;
      break;
    case RIGHT:
      m_trans_x -= width / 20;
      break;
    case UP:
      m_trans_y += height / 20;
      break;
    case DOWN:
      m_trans_y -= height / 20;
      break;
    }
    break;
  }
  background(bg_color);
  redraw();  
}



  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#ffffff", "viewer" });
  }
}
