shader_type spatial;
uniform vec3 albedo : source_color;
uniform float epsilon;
uniform sampler2D land;
uniform sampler2D ocean;

uniform vec2 outposts[50];
uniform int outpostsLength = 0;

vec3 normal(vec2 uv, sampler2D texture_norm) {
	float right = texture(texture_norm, uv + vec2(epsilon, 0)).r;
	float left = texture(texture_norm, uv + vec2(-epsilon, 0)).r;
	float top = texture(texture_norm, uv + vec2(0, epsilon)).r;
	float bottom = texture(texture_norm, uv + vec2(0, -epsilon)).r;
	
	return normalize(vec3(2.0 * (right - left), 2.0 * (bottom - top), -4));
}

void fragment() {
	float displacementY = NODE_POSITION_WORLD.y;
	
	vec2 pos = UV;
	
	float height = texture(land, pos).r;
	float height2 = texture(ocean, pos).r - 0.3 * tanh(0.1 * displacementY);
	
	ALBEDO = albedo - vec3(height2 - 0.55);
	if(abs(pos.x - 0.5) > 0.45 || abs(pos.y - 0.5) > 0.45) discard;
	if(height2 > 0.55) {
		//ALPHA = 1.0 - 10.0 * (height2 - 0.55);
		
		if(height2 > 0.65) discard;
	}
}
