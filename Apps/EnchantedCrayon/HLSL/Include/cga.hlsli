float3 cga_fx(const float2 pPosition, const float3 pColor)
{
	const float zero = 0.0;
	const float _one = 1.0;
	const float _1_3 = 1.0 / 3.0;
	const float _2_3 = 2.0 / 3.0;

	const float3 black = { zero,zero,zero };
	const float3 cyan = { zero,_2_3,_2_3 };
	const float3 magenta = { _2_3,zero,_2_3 };
	const float3 white = { _one,_one,_one };

	const float2 CGAresolution = { 320, 200 };

	const float brigthness = pColor.r + pColor.g + pColor.b;
	const float dithering = (ceil(pPosition.x * CGAresolution.x) % 2) == (ceil(pPosition.y * CGAresolution.y) % 2) ? 0.9f : 1.1f;

	if (brigthness < 0.7f * dithering)
		return black;
	else if (brigthness > 1.4 / dithering)
		return white;
	else if (round(pColor.r * 1.1) / 1.1 * dithering < pColor.g)
		return magenta;
	else
		return cyan;
}