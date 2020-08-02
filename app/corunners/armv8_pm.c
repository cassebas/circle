#include "armv8_pm.h"

/**
 * Global enable of PMU
 */
extern inline void enable_pmu();
extern inline void enable_cycle_counter();
extern inline void disable_cycle_counter();
extern inline u64 read_cycle_counter();
/**
 * Reset the cycle counter PMCCNTR_EL0 to zero.
 */
extern inline void reset_cycle_counter();
extern inline u64 read_nr_eventcounters();
/**
 * Read the common event identification register
 */
extern inline u64 read_cei_reg();
extern inline void config_event_counter(unsigned int counter, unsigned int event);
extern inline void enable_event_counter(unsigned int counter);
extern inline void disable_event_counter(unsigned int counter);
extern inline unsigned int read_event_counter(unsigned int counter);
/**
 * Reset all event counters to zero (not including PMCCNTR_EL0).
 */
extern inline void reset_event_counters();

