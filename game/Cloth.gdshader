shader_type spatial;

render_mode unshaded;
render_mode cull_disabled;

uniform bool dark;

void vertex() {
	float amount = min(1.0, max(0.0, -4.0 - VERTEX.y));
	
	
	VERTEX.x += VERTEX.x * 0.01 * amount * sin(1.0 * TIME) + 0.25 * amount * sin(TIME + 2.0 * VERTEX.y);
	VERTEX.z += VERTEX.z * 0.01 * amount * sin(1.0 * TIME) - 0.25 * amount * cos(TIME + 2.0 * VERTEX.y);
	VERTEX.y += 0.02 * abs(VERTEX.y) * cos(1.0 * TIME);
}

void fragment() {
	if(dark) ALBEDO = vec3(-1.0);
	else ALBEDO = vec3(0.75 + 1.5 * UV.y * UV.y);
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
