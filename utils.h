#ifndef LEGOFAN_UTILS_H
#define LEGOFAN_UTILS_H 1

#include "dumpdbg.h"

// Does the z reading averaging magic
unsigned short z_update(unsigned short new_value);


// A cleaner way to keep track of counters, maybe
class CycleCounter : public Dumpable {
    unsigned short top_value;
    unsigned short curr_value;

  public:
    // Initialize a new counter with 0 as top value
    CycleCounter();
    
    // Initialize a new counter with top as top value
    CycleCounter(unsigned short top);

    // Reset the counter to the top value
    void reset();

    // Sets the top value to new_top, then resets the counter
    void reset(unsigned short new_top);

    // Forces counter to zero
    void stop();

    // Decrements the counter by one, returns true if the counter has reached 0
    bool tick();

    // Returns true if the counter is currently nonzero
    bool is_running();

    // Returns the raw value
    unsigned short val();

    virtual void dd_do_dump(Print*);
};


// Glorified += operator
void modify_clamp(unsigned short* value, short delta);

#endif
