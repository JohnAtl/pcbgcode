/**
 * viewer
 * 
 * Load line coordinates from a file and draw them.
 *
 */

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
 * When lines are drawn in color, the colors used follow the
 * standard resistor color codes. After 9, the sequence repeats.
 * 0 is cyan, rather than black, as that would be a little hard
 * to see on the black background.
 * Colors are also slightly (204/255) transparent so you can see where
 * they overlap.
 *
 */
color[] color_table = {
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

color bg_color = #000000;

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
float tool_size = 0.001;

int m_pass;
boolean m_monochrome = true;

/*
 * Parse Strings and produce Lines.
 *
 */
void prepare_lines(String[] lines) {
  int i;
  
  for (i=0; i < lines.length; i++) {
    if (lines[i] != null) {
      //  println(lines[i]);
      String[] pieces = split(lines[i], ',');
      if (pieces.length == 5) {
        float x1 = float(pieces[0]);
        float y1 = float(pieces[1]);
        float x2 = float(pieces[2]);
        float y2 = float(pieces[3]);
        int m_pass = int(pieces[4]);
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
void set_scaling() {
  x_scale = (width - 70) / (maxx - minx);
  y_scale = (height - 70) / (maxy - miny);
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
void rtext(String s, float x, float y) {
  text(s, x - textWidth(s), y);
}

/*
 * Setup the window.
 * Read and parse the file.
 *
 */
void setup() {
  String matches[];
  size(800, 600);
  
  String line = null;
  String[] lines = new String[1];
  BufferedReader reader = createReader(filename);
  do {
    try {
      line = reader.readLine();
    }
    catch (IOException e) {
      println("Mama I'm sick");
    }
    if (line != null) {
      matches = match(line, "^# board=(.+)");
      if (matches != null) {
        comment = matches[1];
      }
      matches = match(line, "^# tool size=(.+)");
      if (matches != null) {
        tool_size = float(matches[1]);
      }
      matches = match(line, "^# pass=(.+)");
      if (matches != null) {
        m_pass = int(matches[1]);
      }
      matches = match(line, "^#");
      if (matches == null) {
        lines = (String[])append(lines, (line + "," + nf(m_pass,2)));
      }
    }
  } while (line != null);
  plines = (Line[])expand(plines, lines.length);
  prepare_lines(lines);
  set_scaling();
  noLoop();
}

/*
 * Draw a Line.
 *
 */
void line(Line l) {
  if (l != null) {
    if (! m_monochrome) {
      stroke(color_table[l.pass % 9]);
    }
    line(l.sx * x_scale + x_offset, l.sy * y_scale + y_offset,
      l.ex * x_scale + x_offset, l.ey * y_scale + y_offset);
//    println(nfs(l.sx, 1, 5) +", " + nfs(l.sy, 1, 5));      
//    print(nfs(l.sx * x_scale + x_offset, 1, 5) +", " + nfs(l.sy * y_scale + y_offset, 1, 5) + ", ");      
//    println(nfs(l.ex * x_scale + x_offset, 1, 5) +", " + nfs(l.ey * y_scale + y_offset, 1, 5));      
  }
  else {
    println("null line");
  }
}

/*
 * X scale and offset.
 *
 */
float xso(float x) {
  return x * x_scale + x_offset;
}

/*
 * Y scale and offset.
 *
 */
float yso(float y) {
  return y * y_scale + y_offset;
}

/*
 * Text notes around the periphery of the window.
 *
 */
void ornaments() {
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
  rtext("tool size " + nfs(tool_size, 1, 3), width - 120, 20);
  rtext(nfs(m_pass, 1) + " passes", width - 40, 40);
  
  // brief help
  text("Keys: +/- zoom, 1 no zoom, 2 zoom 2x, arrows move, a left, de right, w, up, so down", 10, 40);
  text("c toggle color, qx quit", 60, 60);
}

float m_scale = 1.0;
float m_trans_x = 0;
float m_trans_y = 0;
int draw_cnt = 0;
boolean m_drawing;
void draw() {
  m_drawing = true;
  background(bg_color);
  stroke(127);
  fill(bg_color);
  strokeWeight(4);
  quad(0, 0, width-1, 0, width-1, height-1, 0, height-1);
  strokeWeight(1);

  fill(255, 0, 0);
  metaBold = loadFont("BankGothic-Light-14.vlw");
  textFont(metaBold);
  rtext("Viewer 1.5", width - 20, 20);

  stroke(200);
  scale(m_scale);
  translate(m_trans_x, m_trans_y);
  if (plines != null) {
    println("plines.length = " + nf(plines.length, 3));
    strokeWeight(tool_size * x_scale);
    for (int i = 0; i < plines.length; i++) {
      line(plines[i]);
    }
  }
  else {
    text("Didn't open the file", 100, height / 2);
  }
  
  ornaments();
  m_drawing = false;
}

void keyPressed() {
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
        m_scale += 0.2;
        m_trans_x -= width / m_scale / 20;
        m_trans_y -= height / m_scale / 20;
      }
      break;
    case '_':
    case '-':
      if (m_scale > 0.5) {
        m_scale -= 0.2;
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

