#ifndef ANIMATION_HEADER
#define ANIMATION_HEADER

#include "key_frame.h"
struct Animation {
	float length;
	std::vector<KeyFrame*> children;
};
#endif