shader_type canvas_item;
uniform bool inverted;
uniform bool translucent = false;

void fragment() {
	if(inverted) COLOR.rgb = vec3(1) - COLOR.rgb;
	if(translucent) {
		COLOR.rgb *= 0.8;
		COLOR.a = min(COLOR.a, 0.7);
	}
}
