// $Id: inherit.cpp,v 1.8 2020-10-27 17:24:10-07 - - $

// Simple example using inheritance.

#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////
// class shape
/////////////////////////////////////////////////////////////////

class shape {
   private:
      static size_t next_id;
      size_t id {++next_id};
   public:
      virtual ~shape() = default;
      virtual double area() const = 0;
      virtual double circum() const = 0;
      virtual ostream& show (ostream&) const;
      friend ostream& operator<< (ostream&, const shape&);
};

size_t shape::next_id {0};

ostream& shape::show (ostream& out) const {
   return out << "shape(" << id << ")";
}

ostream& operator<< (ostream& out, const shape& sh) {
   return sh.show (out);
}

/////////////////////////////////////////////////////////////////
// class nothing
/////////////////////////////////////////////////////////////////

class nothing: public shape {
   public:
      double area() const override { return 0; }
      double circum() const override { return 0; }
      ostream& show (ostream&) const override;
};

ostream& nothing::show (ostream& out) const {
   return shape::show (out) << ": nothing ";
}


/////////////////////////////////////////////////////////////////
// class circle
/////////////////////////////////////////////////////////////////

class circle: public shape {
   private:
      double radius {};
   public:
      circle (double r = 0): radius(r) {}
      double area() const override { return M_PI * pow (radius, 2); }
      double circum() const override { return 2 * M_PI * radius; }
      ostream& show (ostream&) const override;
};

ostream& circle::show (ostream& out) const {
   return shape::show (out) << ": circle radius " << radius;
}

/////////////////////////////////////////////////////////////////
// class square
/////////////////////////////////////////////////////////////////

class square: public shape {
   private:
      double side {};
   public:
      square (double s): side(s) {}
      double area() const override { return pow (side, 2); }
      double circum() const override { return 4 * side; }
      ostream& show (ostream&) const override;
};


ostream& square::show (ostream& out) const {
   return shape::show (out) << ": square side " << side;
}



/////////////////////////////////////////////////////////////////
// main function
/////////////////////////////////////////////////////////////////

using shape_ptr = shared_ptr<shape>;
int main (int argc, char** argv) {
   vector<string> args (&argv[1], &argv[argc]);
   vector<shape_ptr> vsp;
   for (const auto& arg: args) {
      cout << arg << ":" << endl;
      istringstream argstream (arg);
      char type {}; double num {};
      argstream >> type >> num;
      shape_ptr obj = nullptr;
      switch (arg[0]) {
         case 'c': obj = make_shared<circle> (num); break;
         case 's': obj = make_shared<square> (num); break;
         default:  obj = make_shared<nothing>(); break;
      }
      vsp.push_back (obj);
   }
   for (const auto& item: vsp) {
      cout << *item << ", area " << item->area() << ", circum "
           << item->circum() << endl;
   }
   return 0;
}

//TEST// alias grind='valgrind --leak-check=full --show-reachable=yes'
//TEST// grind inherit c c10 s s15 x >inherit.out 2>&1
//TEST// mkpspdf inherit.ps inherit.cpp* inherit.out

