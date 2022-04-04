#include <iostream>
#include <vector>

namespace my
{
   template<typename Iter, typename Item, class equal = std::equal_to<Item>>
   bool contains(Iter beg, Iter end, Item const & val)
   {
      equal is_equal;
      for(; beg != end; ++beg)
         if(is_equal(*beg, val))
            return true;
      return false;
   }
}

struct CustomComp
{
   bool operator() (int x, int y) { return (x ^ y) == 0x0; };
};

int main()
{
   int val = 4;

   std::vector<int> vec{0,2,4,6,8,10};
   std::cout << std::boolalpha;

   using my::contains;

   auto result = contains(vec.cbegin(), vec.cend(), val);
   std::cout << "Result: " << result << std::endl;

   result = contains<decltype(vec)::const_iterator, int, CustomComp>(vec.cbegin(), vec.cend(), val);
   std::cout << "Result: " << result << std::endl;
}
