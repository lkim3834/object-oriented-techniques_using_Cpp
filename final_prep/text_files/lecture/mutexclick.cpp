// $Id: mutexclick.cpp,v 1.4 2016-08-09 15:28:20-07 - - $

// Hello to threads.

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
using namespace std;

constexpr size_t LOOPS = 10'000'000;

class mutex_counter {
   private:
      size_t count {0};
      mutex lock;
   public:
      mutex_counter& operator++() {
         unique_lock<mutex> unique (lock);
         ++count;
         return *this;
      }
      friend ostream& operator<< (ostream&, const mutex_counter&);
};

ostream& operator<< (ostream& out, const mutex_counter& ctr) {
   out << ctr.count;
   return out;
}

void racer (size_t id, mutex_counter* count) {
   for (size_t i = 0; i < LOOPS; ++i) ++*count;
   cout << "mutex racer " << id << " looped " << LOOPS
        << " finished mutex_counter " << *count << endl;
}

struct comma_t: numpunct<char> {
   char do_thousands_sep() const { return ','; }
   string do_grouping() const { return "\03"; }
};
locale comma (locale(), new comma_t());

int main () {
   cout.imbue (comma);
   mutex_counter count;
   cout << "main starting" << endl;
   vector<thread> vecthr;
   for (size_t i = 0; i < 4; ++i) {
      vecthr.push_back (thread (racer, i, &count));
   }
   for (auto& t: vecthr) t.join();
   cout << "main finished mutex_counter " << count << endl;
   return 0;
}


//TEST// alias TIME='/usr/bin/time -f "%E elapsed, %S kernel, %U user"'
//TEST// for i in 1 2 3 4
//TEST// do
//TEST//    TIME mutexclick >mutexclick.out$i 2>&1
//TEST// done
//TEST// more mutexclick.out* >mutexclick.lis </dev/null
//TEST// mkpspdf mutexclick.ps mutexclick.cpp* mutexclick.lis

