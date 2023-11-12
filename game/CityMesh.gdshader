shader_type spatial;

uniform sampler2D heightMap;
uniform sampler2D buildingNoise;
uniform sampler2D streetNoise;
uniform float epsilon;

varying vec2 pos;

void vertex() {
	vec2 position = VERTEX.xz / 10.0;
	pos = position;
	
	float dist = 2.0 * sqrt(dot(pos, pos)) - 1.2;
	float baseHeight = pow(1.0 - dist, 1.5);
	
	float building = texture(buildingNoise, pos).r;
	
	float street = texture(streetNoise, pos).r;
	
	VERTEX.y += baseHeight * 2.0;
	VERTEX.y *= building * street;
	
	COLOR.rgb =  vec3(0.0);
}

void fragment() {
	ALBEDO = COLOR.rgb;
	
	float dist = 2.0 * sqrt(dot(pos, pos)) - 1.2;
	float baseHeight = pow(1.0 - dist, 1.5);
	
	float building = texture(buildingNoise, pos).r;
	
	float street = texture(streetNoise, pos).r;
	
	float height = texture(heightMap, pos).r;
	
	if(building * street < 0.001) ALPHA = 0.0;
	
	if(baseHeight < 0.5 * height) ALPHA = 0.0;
	
	METALLIC = 1.0;
	ROUGHNESS = 0.0;
}
