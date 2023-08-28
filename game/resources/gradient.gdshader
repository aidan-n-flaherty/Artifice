shader_type spatial;

void vertex() {
	COLOR.rgb = abs(NODE_POSITION_WORLD.y + VERTEX.y) > 0.01 ? vec3(0.025) : vec3(0.5);
}

void fragment() {
	ALBEDO = COLOR.rgb;
}
