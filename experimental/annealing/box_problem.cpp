#include "my_anneal.cpp"

class Vec2 {
public:
  float x, y;

  Vec2() { x=0; y=0; }
  Vec2(const float _x, const float _y) { x = _x; y = _y; }
  Vec2(const Vec2& v) { x = v.x; y = v.y; }

  Vec2 operator+(const Vec2& v) const {
    return Vec2(v.x + x, v.y + y);
  }

  Vec2 operator-(const Vec2& v) const {
    return Vec2(x - v.x, y - v.y);
  }

  // ang in rad
  Vec2 rot(const float ang) const {
    return Vec2(cos(ang)*x - sin(ang)*y,
                sin(ang)*x + cos(ang)*y);
  }

  friend ostream &operator<<(ostream &out, const Vec2 &v);
};

inline ostream &operator<<(ostream &out, const Vec2 &v) {
  out << v.x << " " << v.y;
  return out;
}

// unit-boxes with center at x,y and orientation angle ang
class Box {
public:
  // center
  Vec2 c;
  // angle
  float ang;

  Vec2 v0() const { return Vec2(-.5,-.5).rot(ang) + c; }
  Vec2 v1() const { return Vec2( .5,-.5).rot(ang) + c; }
  Vec2 v2() const { return Vec2( .5, .5).rot(ang) + c; }
  Vec2 v3() const { return Vec2(-.5, .5).rot(ang) + c; }

  Box(const Box& b) { c = b.c; ang = b.ang; }
  Box() { ang = 0; }

  void bbox(Vec2 *bl, Vec2 *ur) const {

    // vertex bottom left
    Vec2 v[4];
    v[0] = Vec2(-.5,-.5);
    v[1] = Vec2( .5,-.5);
    v[2] = Vec2( .5, .5);
    v[3] = Vec2(-.5, .5);
 
    if (ang!=0)
      for (int i=0;i<4;++i) v[i] = v[i].rot(ang) + c;
    else
      for (int i=0;i<4;++i) v[i] = v[i] + c;

    float minx = v[0].x, miny = v[0].y;
    float maxx = v[0].x, maxy = v[0].y;

    for (int i=1;i<4;++i) {
      if (v[i].x < minx) minx = v[i].x;
      if (v[i].x > maxx) maxx = v[i].x;
      if (v[i].y < miny) miny = v[i].y;
      if (v[i].y > maxy) maxy = v[i].y;
    }

    (*bl).x = minx; (*bl).y = miny;
    (*ur).x = maxx; (*ur).y = maxy;
  }

  int vecInBox(const Vec2& v) const {
    // transform v back in "box-space" and check if inside
    Vec2 in = (v - c).rot(-ang);
    if (in.x < -.5) return 0;
    if (in.x > .5)  return 0;
    if (in.y < -.5) return 0;
    if (in.y > .5)  return 0;
    return 1;
  }

  int overlap(const Box& b) const {
    
    // are centers further apart than 2 sqrt(2) ?
		// if yes, stop (no overlap)
		Vec2 cc(b.c-c); float d = cc.x*cc.x+cc.y*cc.y;
		if (d > 2.*M_SQRT2) return 0;
		// closer than 1?
		if (d < 1.) return 1;

    Vec2 v[4];
    v[0] = v0(); v[1] = v1();
    v[2] = v2(); v[3] = v3();

    for (int i=0;i<4;++i)
      if (b.vecInBox(v[i])) return 1;

    v[0] = b.v0(); v[1] = b.v1();
    v[2] = b.v2(); v[3] = b.v3();

    for (int i=0;i<4;++i)
      if (vecInBox(v[i])) return 1;

    return 0;
  }

  friend ostream &operator<<(ostream& out, const Box &b);
};

inline ostream &operator<<(ostream& out, const Box &b) {
  Vec2 bl,ur; b.bbox(&bl,&ur);
  out << "Box : c=" << b.c << ",ang=" << b.ang << endl;
  out << "  bbox : " << bl << " " << ur;
}

void write_boxes(const vector<Box>& boxes);

class BoxAnneal : public BasicAnneal {
public:

  int no_of_nodes, N;
  vector<Box> nodes, best_nodes;

  /*!
     params  -  like BasicAnneal. 
                Addionally 
                N -  Number of Boxes
  */
  BoxAnneal(const char* params = "") {

    std_init(params);
    N = 2;

    map<string,string> param_map;
    str2hash(params, param_map);
    extract_i(N,param_map);

    no_of_nodes = N;

    Box b;
    for (int i=0;i<no_of_nodes;++i) {
      b.c = Vec2(i*2*N,0); // i*2*N-.5*i);
      b.ang = (i%2==1?M_PI/8:0);
      nodes.push_back(b);
    }

    best_nodes = nodes;

    // XXX completely forbid overlap moves! it makes stuff unstable
    //     we need a BoxMove(&box,step,...)
    // where we wiggle either on x,y or ang
    // and try again if we have an overlap
    for (int i=0;i<no_of_nodes;++i) {
      possible_moves.push_back(new SimpleFloatMove(&(nodes[i].c.x),0.01));
      possible_moves.push_back(new SimpleFloatMove(&(nodes[i].c.y),0.01));
      possible_moves.push_back(new SimpleFloatMove(&(nodes[i].ang),0.001));
    }

  }

/*
  void std_init(const char* params = "") {
    BasicAnneal::std_init(params);
  }
*/

  bool stop() {
		// XXX I need a better criterium
		//     if 2 boxes are stuck (let's say finished)
    if (Temp < 0.0001/N) return true;
    if (min_step<1e-10 || max_step > 10 ) return true;
    return false;
  }

  int check_for_overlap() {
    for (int i=0;i<nodes.size();++i)
			for (int j=i+1;j<nodes.size();++j)
				if (nodes[i].overlap(nodes[j])) return 1;
		return 0;
	}

  void wiggle() {
		int r;
		if (possible_moves.size()>0) {
  	  r = randint(possible_moves.size());
	  	possible_moves[r]->move();
			// XXX keep track of what box gets changed and
			//     check overlaps only for that box
			while (check_for_overlap()) {
				possible_moves[r]->reject();
     	  r = randint(possible_moves.size());
	    	possible_moves[r]->move();
			}
			curr_moves.push_back(possible_moves[r]);
		}
		else {
			cerr << "BasicAnneal::wiggle : No available moves.\n";
		}
	}

  void best_found() {
    best_nodes = nodes;
    BasicAnneal::best_found();
		ofstream out(best_filename.c_str());
    for (int i=0;i<N;++i)
      out << nodes[i].c.x << " " << nodes[i].c.y << " " << nodes[i].ang << endl;
    out.close();
  }

  float energy() {
    Vec2 bl, ur, bb1, bb2;
		
		while (nodes[0].ang>M_PI) nodes[0].ang -= M_PI;
		while (nodes[0].ang<-M_PI) nodes[0].ang += M_PI;

    nodes[0].bbox(&bl, &ur);
    for (int i=1;i<N;++i) {

	  	while (nodes[i].ang>M_PI)  nodes[i].ang -= M_PI;
		  while (nodes[i].ang<-M_PI) nodes[i].ang += M_PI;

      nodes[i].bbox(&bb1, &bb2);
      if (bb1.x < bl.x) bl.x = bb1.x;
      if (bb2.x > ur.x) ur.x = bb2.x;
      if (bb1.y < bl.y) bl.y = bb1.y;
      if (bb2.y > ur.y) ur.y = bb2.y;
    }
		Vec2 dl = ur - bl;
    float e = dl.x>dl.y?dl.x:dl.y;
		if (e<2) {
			write_boxes(nodes);
			cout << "died below 2 : dx / dy : "
			     << dl.x << " " << dl.y << "\n";
			exit(0);
		}
		/*
    if (e>10*N) {
			write_boxes(nodes);
		  cout << "Energy too big : " << e << '\n';
			exit(0);
		}

    static int cccc = 0;
		if (cccc%400==0)
		write_boxes(nodes);
		cccc++;
		*/
    
		return e;
  }
};

// #define TEST

#ifndef TEST
void write_boxes(const vector<Box>& boxes) {
	static int box_write_count = 0;
	Vec2 v[4];
	
	v[0] = Vec2(-.5,-.5);
	v[1] = Vec2( .5,-.5);
	v[2] = Vec2( .5, .5);
	v[3] = Vec2(-.5, .5);

  char name[1024]; sprintf(name,"%08d.txt",box_write_count++);
  ofstream out(name,ios::out);
	for (int c=0;c<5;++c) {
    for (uint i=0;i<boxes.size();++i) {
      out << v[c%4].rot(boxes[i].ang) + boxes[i].c << " ";
    }
		out << endl;
	}
	out.close();
}

int main(int argc, char** argv) {

  srand(time(NULL));

  BoxAnneal* ba;
  const char* def = "T=0.01,N=2,best_filename=best.txt";

  if (argc>1) ba = new BoxAnneal(argv[1]);
  else {
    cout << "Use default params : " << def << endl;
    ba = new BoxAnneal(def);
  }

  ba->show_config(cout);
  ba->update_minmax_step();
  cout << "steps min/max " << ba->min_step << "/" << ba->max_step << endl;

  float e = ba->energy();
  cout << "Init E=" << e << endl;
  if (e<0) { cout << "Neg energy\n"; exit(0); }

  ba->do_anneal();

  // shift the boxes' center of mass to 0,0
	Vec2 com;
	for (int i=0;i<ba->nodes.size();++i)
		com = com + ba->nodes[i].c;
  com.x /= (float)(ba->nodes.size());
  com.y /= (float)(ba->nodes.size());
	for (int i=0;i<ba->nodes.size();++i)
		ba->nodes[i].c = ba->nodes[i].c - com;

	// write that down
	ba->best_found();
	write_boxes(ba->nodes);

  return 0;
}
#else
int main() {

  Box b1, b2;
	b1.c = Vec2(-0.420446, -0.420453);
	b1.ang = 0;
	b2.c = Vec2(0.420447, 0.420453);
	b2.ang = 0.392699;

  if (b1.overlap(b2)) cout << "Overlap\n";
  else cout << "No overlap\n";

  // bbox

  return 0;
}
#endif
