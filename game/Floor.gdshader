shader_type spatial;
uniform vec3 albedo : source_color;
uniform float epsilon;
uniform sampler2D land;
uniform sampler2D ocean;
uniform sampler2D mountains;

uniform vec2 offset;
uniform vec2 outposts[50];
uniform int outpostsLength = 0;

vec3 normal(vec2 uv, sampler2D texture_norm) {
	float right = texture(texture_norm, uv + vec2(epsilon, 0)).r;
	float left = texture(texture_norm, uv + vec2(-epsilon, 0)).r;
	float top = texture(texture_norm, uv + vec2(0, epsilon)).r;
	float bottom = texture(texture_norm, uv + vec2(0, -epsilon)).r;
	
	return normalize(vec3(2.0 * (right - left), 2.0 * (bottom - top), -4));
}

void vertex() {
	vec2 pos = (UV - 0.5) + vec2(offset.x/100.0, offset.y/260.0);
	
	float height2 = texture(ocean, pos).r;
	
	float height = texture(land, pos).r;
	
	if(height2 > 0.45 && height2 < 0.6) {
		vec2 norm = normalize(normal(pos, ocean).xy);
		
		float scalar = 5.0 * (height2 - 0.5);
		VERTEX.z += scalar * norm.y;
		VERTEX.x -= scalar * norm.x;
		height = 0.75;
	}
	
	for(int i = 0; i < outpostsLength; i++) {
		vec2 v = outposts[i];
		float deltaX = v.x - pos.x * 100.0;
		float deltaY = v.y - pos.y * 260.0;
		float remove = max(0.0, 1.5 - 0.15 * sqrt(deltaX * deltaX + deltaY * deltaY));
		height2 = max(0.0, height2 - remove);
	}
	
	VERTEX.y += -2.0 + 2.0 * height - 100.0 * max(0.0, height2 - 0.6);
	
	COLOR = vec4(albedo - 0.5 * height, 1.0);
	
	if(height2 > 0.6) {
		vec2 norm = normalize(normal(pos, ocean).xy);
		float scalar = 4.0 * min(1.0, 20.0 * (height2 - 0.6));
		VERTEX.z += scalar * norm.y;
		VERTEX.x -= scalar * norm.x;
		
		COLOR = vec4(vec3(0.25 - 5.0 * (height2 - 0.6)), 1.0);
	}
}

void fragment() {
	vec2 pos = (UV - 0.5) + vec2(offset.x/100.0, offset.y/260.0);
	
	ALBEDO = COLOR.rgb;
	
	vec3 norm = normal(pos, land);
	norm.y *= 0.75;
	NORMAL = normalize(norm);
}
