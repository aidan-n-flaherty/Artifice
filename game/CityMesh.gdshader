shader_type spatial;

uniform sampler2D heightMap;
uniform sampler2D buildingNoise;
uniform float epsilon;

vec3 normal(vec2 uv, sampler2D texture_norm) {
	float right = texture(texture_norm, uv + vec2(epsilon, 0)).r;
	float left = texture(texture_norm, uv + vec2(-epsilon, 0)).r;
	float top = texture(texture_norm, uv + vec2(0, epsilon)).r;
	float bottom = texture(texture_norm, uv + vec2(0, -epsilon)).r;
	
	return normalize(vec3(2.0 * (right - left), 2.0 * (bottom - top), -4));
}
void vertex() {
	vec2 pos = VERTEX.xz / 4.0;
	
	float dist = sqrt(dot(pos, pos));
	float baseHeight = pow(1.0 - dist, 1.5);
	
	float height = 1.0 - texture(heightMap, pos).r;
	float height2 = texture(buildingNoise, (pos + 1.0) / 2.0).r;
	height = height2;
	if(dist > 0.6) height -= 3.0 * (dist - 0.6);

	VERTEX.y += baseHeight * 10.0 * height;
	//VERTEX.x += 0.25 * max(0.0, height - 0.5) * pos.x / dist;
	//VERTEX.z += 0.25 * max(0.0, height - 0.5) * pos.y / dist;
	
	COLOR.rgb = vec3(min(1.0, 2.0 - 3.0 * height));
	if(height < 0.0) COLOR.a = 0.0;
}

void fragment() {
	vec2 pos = VERTEX.xz / 4.0;
	
	ALBEDO = COLOR.rgb;
	
	ALPHA = COLOR.a;
}
