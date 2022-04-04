// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>

//using value_type = xpair<const key_type, mapped_type>;
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   auto it = begin(); 
  
   // increasing order 1 2 
   //If the key is already there, the value is replaced

   if (auto i= find(pair.first)){
      i->second = pair.second;
      //cout << "found " << endl; 
      return i; 
   }
  
   node* nd = new node(anchor()->next,anchor(),pair);
   //cout << nd->value.first << nd->value.second << endl; 
   bool flag = false; 
   node *curr;
   for(node* cur = anchor()->next ; cur!= anchor(); cur = cur->next){
           if (less( cur->value.first, nd->value.first )){
                //cout << " curr value is less than new" << endl; 
                flag = true; 
                 curr = cur; 
                 
            } 
   }
   // 
   if (flag == false){
      anchor()->next = nd; 
      nd->next->prev = nd; 
      
      }else{

         nd->prev = curr;
         nd->next = curr->next; 
         curr->next->prev = nd; 
         //cout << " setting prev of cur next pass " << endl; 
         curr->next = nd; 
                
         it = ++find(curr->value.first);
      }
      
   
   //DEBUGF ('l', &pair << "->" << pair);
   return it;
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   for(auto beg = this->begin(); beg != this->end(); ++beg){
      if (beg->first == that ){
         return beg; 
      } 
   }
   //DEBUGF ('l', that);
   return iterator();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   node *to_erase;
   iterator nex = ++position;
   //iterator copy = position;
   for (node * cur = anchor()->next ; cur != anchor(); cur = cur->next){
      if (position->first == cur->value.first ){
         to_erase = cur; 
      }
   }
   if (empty() == true){
      return nex;
   }
   // update position--

    to_erase->prev->next =  to_erase->next;
   // update position++
   to_erase->next->prev =  to_erase->prev;

   return nex;
}


