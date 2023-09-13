shader_type spatial;

uniform bool inverted;
uniform bool translucent = false;

void fragment() {
	if(inverted) ALBEDO = vec3(1) - COLOR.rgb;
	if(translucent) ALPHA = 0.8;
}