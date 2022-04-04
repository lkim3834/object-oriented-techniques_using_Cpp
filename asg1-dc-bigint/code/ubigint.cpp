// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>
#include <iterator>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

//number 4629 
//v3 = 4, v2 = 6, v1= 2, v0 = 9 (10^3*4 + 10^2*6 + 10^1 *2 + 10^0*9 )
//while (size() > 0 and back() == 0) pop_back(); for all 6 operator function 
ubigint::ubigint (unsigned long that): ubig_value(that) {
//   DEBUGF ('~', this << " -> " << ubig_value)
}

ubigint::ubigint (const string& that): ubig_value(0) {
 //  DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
//      ubig_value = ubig_value * 10 + digit - '0';
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   DEBUGF ('u', *this << "+" << that);
	ubigint result;
	vector <uint8_t> that_vector ;
//reverse iterate 
	for (auto it = that.ubig_value.crbegin(); it!=that.ubig_value.crend(); ++it){
		 that_vector.push_back(*it);
	}

	vector <uint8_t> this_vector;
	this_vector = ubig_value;
	if (that_vector.size() < ubig_value.size()){
		 int i =  ubig_value.size() - that_vector.size();
		for(int j = 1; j <= i; j++){ 
//the sign of 0 is positive
		that_vector.push_back(0);
		}
	}
	else if (that_vector.size() > ubig_value.size()){
 		int i = that_vector.size() -  ubig_value.size();
		for(int j = 1; j <= i; j++){
		this_vector.push_back(0);
		}
	}

	for (uint8_t in = 0; in < that_vector.size(); ++in ){
	
		int addition = that_vector[in] + this_vector[in];
		if (result.ubig_value[in+1]){
		addition += result.ubig_value[in+1];
		}
		if(addition >= 10 ){
		addition -= 10;
		result.ubig_value.push_back(addition);	
		result.ubig_value.push_back(1);
		}else{result.ubig_value.push_back(addition); }
	}
DEBUGF ('u', result);
	return result; 
	}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
 //  return ubigint (ubig_value - that.ubig_value);
}

ubigint ubigint::operator* (const ubigint& that) const {
//return vector<uint8_t> product(m + n)
 //  return ubigint (ubig_value * that.ubig_value);
}

void ubigint::multiply_by_2() {
 //  ubig_value *= 2;
}

void ubigint::divide_by_2() {
 //  ubig_value /= 2;
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   DEBUGF ('/', "quotient = " << quotient);
   DEBUGF ('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {
 //  return ubig_value < that.ubig_value;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
 //  return out << "ubigint(" << that.ubig_value << ")";
}

