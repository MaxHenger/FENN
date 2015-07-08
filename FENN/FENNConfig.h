#ifndef FENNCONFIG_H
#define FENNCONFIG_H

namespace fenn
{
	//intentionally empty
}

//typedefinitions
typedef float WEIGHT_TYPE;
typedef float BIAS_TYPE;
typedef float VALUE_TYPE;
typedef float CHANCE_TYPE;
typedef unsigned int GLOBAL_INDEX_TYPE;
typedef unsigned short LOCAL_INDEX_TYPE;
typedef unsigned char SLOT_INDEX_TYPE;

//definitions - default values
#define WEIGHT_DEFAULT (WEIGHT_TYPE)(1.0f)
#define BIAS_DEFAULT (BIAS_TYPE)(0.0f)
#define SLOT_DEFAULT (SLOT_INDEX_TYPE)(0)

//definitions - minima and maxima
#define SLOT_MAX (SLOT_INDEX_TYPE)(1)

#define WEIGHT_MIN (WEIGHT_TYPE)(-20)
#define WEIGHT_MAX (WEIGHT_TYPE)(20)
#define WEIGHT_ADDITIVE_MIN (WEIGHT_TYPE)(-1)
#define WEIGHT_ADDITIVE_MAX (WEIGHT_TYPE)(1)

#define WEIGHT_RANGE (WEIGHT_TYPE)(WEIGHT_MAX - WEIGHT_MIN)
#define WEIGHT_ADDITIVE_RANGE (WEIGHT_TYPE)(WEIGHT_ADDITIVE_MAX - WEIGHT_ADDITIVE_MIN)

#define BIAS_MIN (BIAS_TYPE)(-20)
#define BIAS_MAX (BIAS_TYPE)(20)
#define BIAS_ADDITIVE_MIN (BIAS_TYPE)(-1)
#define BIAS_ADDITIVE_MAX (BIAS_TYPE)(1)

#define BIAS_RANGE (BIAS_TYPE)(BIAS_MAX - BIAS_MIN)
#define BIAS_ADDITIVE_RANGE (BIAS_TYPE)(BIAS_ADDITIVE_MAX - BIAS_ADDITIVE_MIN)

#endif