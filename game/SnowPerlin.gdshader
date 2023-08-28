shader_type spatial;
uniform vec3 albedo : source_color;
uniform float epsilon;
uniform sampler2D land;
uniform sampler2D ocean;

uniform float cameraSize;

vec3 normal(vec2 uv, sampler2D texture_norm) {
	float right = texture(texture_norm, uv + vec2(epsilon, 0)).r;
	float left = texture(texture_norm, uv + vec2(-epsilon, 0)).r;
	float top = texture(texture_norm, uv + vec2(0, epsilon)).r;
	float bottom = texture(texture_norm, uv + vec2(0, -epsilon)).r;
	
	return normalize(vec3(2.0 * (right - left), 2.0 * (bottom - top), -4));
}

void fragment() {
	vec2 pos = (UV - 0.5) * (cameraSize/100.0) - NODE_POSITION_WORLD.xz/vec2(100.0, 260.0);
	
	float height = texture(land, pos).r;
	float height2 = texture(ocean, pos).r;
	
	ALBEDO = albedo - vec3(0.75 * height);
	if(height2 > 0.45) {
		ALPHA = 1.0 - 10.0 * (height2 - 0.45);
		
		if(height2 > 0.55) discard;
	}
	
	float diff = height - (1.0 - NODE_POSITION_WORLD.y);
	float diff2 = height - (0.5 - NODE_POSITION_WORLD.y);
	
	if(diff2 < 0.0) discard;
	if(diff > 0.0) {
		ALPHA = min(ALPHA, 1.0 - 10.0 * diff);
		if(diff > 0.1) discard;
	}

	//vec3 norm = normal(pos, land);
	//norm.y *= 1.0;
	//NORMAL = normalize(norm);
}