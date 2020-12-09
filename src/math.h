#ifndef _MATH_H_
#define _MATH_H_

#include <math.h>

#define MOD(a, b)	((((a) % (b)) + (b)) % (b))
#define FMOD(a, b)	(fmodf(fmodf((a), (b)) + (b), (b)))
#define PI		3.14159265358979323846
#define DEGREE_MAX	256

extern const float sine[DEGREE_MAX];
extern const float cosine[DEGREE_MAX];

int distance_squared(int x, int y, int x2, int y2);
int distance(int x, int y, int x2, int y2);

#endif /* _MATH_H_ */
