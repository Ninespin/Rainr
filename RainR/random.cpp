#include "random.h"
#include <cstdlib> 


float Random::random_float_normalized()
{
	return 1.f * rand() / RAND_MAX;
}

Vec2 Random::random_v2()
{
	Vec2 v;
	v.x = random_float_normalized() * 2 - 1;
	v.y = random_float_normalized() * 2 - 1;
	return v;
}
Vec3 Random::random_v3()
{
	Vec3 v;
	v.x = random_float_normalized() * 2 - 1;
	v.y = random_float_normalized() * 2 - 1;
	v.z = random_float_normalized() * 2 - 1;
	return v;
}
Vec4 Random::random_v4()
{
	Vec4 v;
	v.x = random_float_normalized() * 2 - 1;
	v.y = random_float_normalized() * 2 - 1;
	v.z = random_float_normalized() * 2 - 1;
	v.w = random_float_normalized() * 2 - 1;
	return v;
}
Vec4 Random::random_v4_normalized()
{
	Vec4 v;
	v.x = random_float_normalized();
	v.y = random_float_normalized();
	v.z = random_float_normalized();
	v.w = random_float_normalized();
	return v;
}