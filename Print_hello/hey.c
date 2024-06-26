
// make it print "hello" by filling in /* 1 */, /* 2 */ and /* 3 */

#include "stdio.h"
          
          /* 1 */  
void func(const char** printHey) {
  /* 2 */
  *printHey = "hey";
}
 
int main(void) {
  const char *s;

    /* 3 */
  func(&s);
  printf("%s\n", s);
  return 0;
}

/* Link to compiler explorer: https://godbolt.org/z/s4x6vYcP1 
   use compiler options: -O2 -Wall -Wextra -Werror -g -fstack-protector-strong -D_FORTIFY_SOURCE=2 -std=c11 -pedantic
*/