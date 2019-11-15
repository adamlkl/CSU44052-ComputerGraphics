#ifndef ANIMATOR_HEADER
#define ANIMATOR_HEADER

#include <map>
#include <string>

#include "animated_model.h"
#include "animation.h"
#include "maths_funcs.h"

struct Animator {
	AnimatedModel entity;
	Animation animation;
	float animationTime; 
};

void doAnimation(Animator * animator, Animation animation);
void update(Animator * animator);
void increaseAnimationTime(Animator * animator);
std::map<std::string, mat4> calculateCurrentAnimationPose();
void applyPoseToJoints();

#endif
