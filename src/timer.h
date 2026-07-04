#ifndef TIMER_H
#define TIMER_H

void timer_init(void);
void timer_handle_interrupt(void);
unsigned long timer_ticks(void);

#endif /* TIMER_H */
