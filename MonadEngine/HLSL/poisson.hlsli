// ##########################################################################
// ### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
// ##########################################################################

#ifndef __MND_POISSON_HLSLI__
#define __MND_POISSON_HLSLI__

static const float g_blurfactor = 1.0 / 1024.0;
static const uint g_poissonCount32 = 32u;
static const uint g_poissonCount16 = 16u;
static const uint g_poissonCount4 = 4u;
static const float2 g_PoissonTable32[] =
{
    float2(-0.975402 * g_blurfactor, -0.0711386 * g_blurfactor),
	float2(-0.920347 * g_blurfactor, -0.41142 * g_blurfactor),
	float2(-0.883908 * g_blurfactor, 0.217872 * g_blurfactor),
	float2(-0.884518 * g_blurfactor, 0.568041 * g_blurfactor),
	float2(-0.811945 * g_blurfactor, 0.90521 * g_blurfactor),
	float2(-0.792474 * g_blurfactor, -0.779962 * g_blurfactor),
	float2(-0.614856 * g_blurfactor, 0.386578 * g_blurfactor),
	float2(-0.580859 * g_blurfactor, -0.208777 * g_blurfactor),
	float2(-0.53795 * g_blurfactor, 0.716666 * g_blurfactor),
	float2(-0.515427 * g_blurfactor, 0.0899991 * g_blurfactor),
	float2(-0.454634 * g_blurfactor, -0.707938 * g_blurfactor),
	float2(-0.420942 * g_blurfactor, 0.991272 * g_blurfactor),
	float2(-0.261147 * g_blurfactor, 0.588488 * g_blurfactor),
	float2(-0.211219 * g_blurfactor, 0.114841 * g_blurfactor),
	float2(-0.146336 * g_blurfactor, -0.259194 * g_blurfactor),
	float2(-0.139439 * g_blurfactor, -0.888668 * g_blurfactor),
	float2(0.0116886 * g_blurfactor, 0.326395 * g_blurfactor),
	float2(0.0380566 * g_blurfactor, 0.625477 * g_blurfactor),
	float2(0.0625935 * g_blurfactor, -0.50853 * g_blurfactor),
	float2(0.125584 * g_blurfactor, 0.0469069 * g_blurfactor),
	float2(0.169469 * g_blurfactor, -0.997253 * g_blurfactor),
	float2(0.320597 * g_blurfactor, 0.291055 * g_blurfactor),
	float2(0.359172 * g_blurfactor, -0.633717 * g_blurfactor),
	float2(0.435713 * g_blurfactor, -0.250832 * g_blurfactor),
	float2(0.507797 * g_blurfactor, -0.916562 * g_blurfactor),
	float2(0.545763 * g_blurfactor, 0.730216 * g_blurfactor),
	float2(0.56859 * g_blurfactor, 0.11655 * g_blurfactor),
	float2(0.743156 * g_blurfactor, -0.505173 * g_blurfactor),
	float2(0.736442 * g_blurfactor, -0.189734 * g_blurfactor),
	float2(0.843562 * g_blurfactor, 0.357036 * g_blurfactor),
	float2(0.865413 * g_blurfactor, 0.763726 * g_blurfactor),
	float2(0.872005 * g_blurfactor, -0.927 * g_blurfactor),
};
static const float2 g_PoissonTable16[] =
{
    float2(-0.975402 * g_blurfactor, -0.0711386 * g_blurfactor),
	float2(-0.883908 * g_blurfactor, 0.217872 * g_blurfactor),
	float2(-0.811945 * g_blurfactor, 0.90521 * g_blurfactor),
	float2(-0.614856 * g_blurfactor, 0.386578 * g_blurfactor),
	float2(-0.53795 * g_blurfactor, 0.716666 * g_blurfactor),
	float2(-0.454634 * g_blurfactor, -0.707938 * g_blurfactor),
	float2(-0.261147 * g_blurfactor, 0.588488 * g_blurfactor),
	float2(-0.146336 * g_blurfactor, -0.259194 * g_blurfactor),
	float2(0.0116886 * g_blurfactor, 0.326395 * g_blurfactor),
	float2(0.0625935 * g_blurfactor, -0.50853 * g_blurfactor),
	float2(0.169469 * g_blurfactor, -0.997253 * g_blurfactor),
	float2(0.359172 * g_blurfactor, -0.633717 * g_blurfactor),
	float2(0.507797 * g_blurfactor, -0.916562 * g_blurfactor),
	float2(0.56859 * g_blurfactor, 0.11655 * g_blurfactor),
	float2(0.736442 * g_blurfactor, -0.189734 * g_blurfactor),
	float2(0.865413 * g_blurfactor, 0.763726 * g_blurfactor),
};
static const float2 g_PoissonTable4[] =
{
    float2(-0.614856 * g_blurfactor, 0.386578 * g_blurfactor),
	float2(-0.146336 * g_blurfactor, -0.259194 * g_blurfactor),
	float2(0.359172 * g_blurfactor, -0.633717 * g_blurfactor),
	float2(0.865413 * g_blurfactor, 0.763726 * g_blurfactor),
};
#endif