shader_type spatial;

render_mode unshaded;
uniform sampler2D noise;
uniform sampler2D current;

uniform vec3 color;

uniform float speed;

void vertex() {
	float amount = min(1.0, 0.25 * max(0.0, -2.0 + VERTEX.z));
	
	VERTEX.z += -1.0 - 0.5 * amount * cos(TIME);
	VERTEX.x *= max(0.75 + 0.25 * amount * cos(TIME), 0.5);
	VERTEX.y *= max(0.75 + 0.25 * amount * cos(TIME), 0.5);
	
	float x = VERTEX.x;
	float y = VERTEX.y;
	
	float angle = atan(y/x);
	
	if(y > 0.0 && x < 0.0) angle += PI;
	else if(y < 0.0 && x < 0.0) angle += PI;
	
	float mag = sqrt(x * x + y * y);
	
	VERTEX.x = mag * cos(angle + 0.2 * TIME);
	VERTEX.y = mag * sin(angle + 0.2 * TIME);
	
	float currentX = max(0.0, 2.0 * amount - 1.0) * (texture(current, speed * vec2(TIME, VERTEX.z)).r - 0.5);
	float currentY = max(0.0, 2.0 * amount - 1.0) * (texture(current, speed * vec2(-TIME, VERTEX.z)).r - 0.5);
	
	VERTEX.x += currentX;
	VERTEX.y += currentY;
}

void fragment() {
	ALBEDO = vec3(1.5);
	ALPHA = 0.1 + 0.9 * pow(texture(noise, UV + vec2(1.0, -1.0) * speed *  TIME).r, 3.0) + (1.0 - 4.0 * abs(UV.x - 0.5));
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
