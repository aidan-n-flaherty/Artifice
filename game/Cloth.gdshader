shader_type spatial;

render_mode unshaded;

uniform sampler2D noise;

uniform bool dark;

uniform float rotation;

uniform float speed;

void vertex() {
	float amount = min(1.0, max(0.0, -4.0 - VERTEX.y));
	
	float x = VERTEX.x;
	float z = VERTEX.z;
	
	float angle = atan(z/x);
	
	if(z > 0.0 && x < 0.0) angle += PI;
	else if(z < 0.0 && x < 0.0) angle += PI;
	
	float mag = sqrt(x * x + z * z);
	
	VERTEX.x = mag * cos(angle + 0.1 * TIME);
	VERTEX.z = mag * sin(angle + 0.1 * TIME);
	
	VERTEX.x += VERTEX.x * 0.01 * amount * sin(1.0 * TIME) + 0.25 * amount * sin(rotation + TIME + 2.0 * VERTEX.y);
	VERTEX.z += VERTEX.z * 0.01 * amount * sin(1.0 * TIME) - 0.25 * amount * cos(rotation + TIME + 2.0 * VERTEX.y);
	//VERTEX.y += 0.02 * abs(VERTEX.y) * cos(1.0 * TIME);
	VERTEX.x += abs(0.002 * VERTEX.y * VERTEX.y * VERTEX.y) * cos(rotation);
	VERTEX.z += abs(0.002 * VERTEX.y * VERTEX.y * VERTEX.y) * sin(rotation);
	
	VERTEX.y += min(2.0, max(0.0, -2.0 - VERTEX.y)) * (0.5 + 0.5 * cos(rotation + 0.5 * TIME));
}

void fragment() {
	if(dark) ALBEDO = vec3(-1.0);
	else {
		ALBEDO = vec3(1.5) - texture(noise, UV).r;
		ALPHA = 0.2;
		if(0.5 - abs(UV.x - 0.5) > 0.25) {
			ALBEDO += 5.0 * (0.5 - abs(UV.x - 0.5) - 0.25);
			ALPHA += 5.0 * (0.5 - abs(UV.x - 0.5) - 0.25);
		}
	}
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
