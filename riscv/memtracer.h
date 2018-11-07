// See LICENSE for license details.
// See LICENSE_CHERI for license details.

#ifndef _MEMTRACER_H
#define _MEMTRACER_H

#include <cstdint>
#include <string.h>
#include <vector>

enum access_type {
  LOAD,
  STORE,
  FETCH,
};

class memtracer_t
{
 public:
  memtracer_t() {}
  virtual ~memtracer_t() {}

  virtual bool interested_in_range(uint64_t begin, uint64_t end, access_type type) = 0;
  virtual void trace(uint64_t addr, size_t bytes, access_type type) = 0;
  virtual void reset(void) = 0;
  virtual void printstats(void) = 0;
  virtual void read_counter(uint64_t *ret_value, int idx) = 0;
};

class memtracer_list_t : public memtracer_t
{
 public:
  bool empty() { return list.empty(); }
  bool interested_in_range(uint64_t begin, uint64_t end, access_type type)
  {
    for (std::vector<memtracer_t*>::iterator it = list.begin(); it != list.end(); ++it)
      if ((*it)->interested_in_range(begin, end, type))
        return true;
    return false;
  }
  void trace(uint64_t addr, size_t bytes, access_type type)
  {
    for (std::vector<memtracer_t*>::iterator it = list.begin(); it != list.end(); ++it)
      (*it)->trace(addr, bytes, type);
  }
  void reset(void)
  {
    for (std::vector<memtracer_t*>::iterator it = list.begin(); it != list.end(); ++it)
      (*it)->reset();
  }
  void printstats(void)
  {
    for (std::vector<memtracer_t*>::iterator it = list.begin(); it != list.end(); ++it)
      (*it)->printstats();
  }
  void read_counter(uint64_t *ret_value, int idx)
  {
    for (std::vector<memtracer_t*>::iterator it = list.begin(); it != list.end(); ++it)
      (*it)->read_counter(ret_value, idx);
  }
  void hook(memtracer_t* h)
  {
    list.push_back(h);
  }
 private:
  std::vector<memtracer_t*> list;
};

#endif
