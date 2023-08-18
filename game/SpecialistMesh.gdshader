shader_type canvas_item;
uniform bool inverted;

void fragment() {
	if(inverted) COLOR.rgb = vec3(1) - COLOR.rgb;
}
