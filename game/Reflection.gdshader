shader_type spatial;

uniform sampler2D screen;

uniform float cameraSize;

uniform float mapWidth;
uniform float mapHeight;

uniform float meshWidth;
uniform float meshHeight;

uniform float resolution;

uniform float speed;

uniform sampler2D normalTexture;
uniform sampler2D floorTexture;
uniform sampler2D floorTexture2;

vec4 getTextureRes(vec2 pos, sampler2D tex, float res) {
	if(res <= 0.0) return vec4(0.0);
	
	vec2 size = vec2(textureSize(tex, 0));
	
	float heightDist = 0.0;
	while(heightDist < mapHeight) heightDist += 0.8 * size.y;
	heightDist -= 0.8 * size.y;
	heightDist = mapHeight - heightDist;
	
	float widthDist = 0.0;
	while(widthDist < mapWidth) widthDist += 0.8 * size.x;
	widthDist -= 0.8 * size.x;
	widthDist = mapWidth - widthDist;
	
	while(pos.x >= mapWidth) pos.x -= mapWidth;
	while(pos.x < 0.0) pos.x += mapWidth;
	while(pos.y >= mapHeight) pos.y -= mapHeight;
	while(pos.y < 0.0) pos.y += mapHeight;
	
	pos /= res;
	vec2 pos2 = pos;
	
	while(pos.x >= 0.8 * size.x) pos.x -= 0.8 * size.x;
	while(pos.x < 0.0) pos.x += 0.8 * size.x;
	while(pos.y >= 0.8 * size.y) pos.y -= 0.8 * size.y;
	while(pos.y < 0.0) pos.y += 0.8 * size.y;
	
	vec4 val = texelFetch(tex, ivec2(int(pos.x), int(pos.y)), 0);
	
	vec2 displacement = pos;
	
	if(displacement.x < 0.2 * size.x) displacement.x += 0.8 * size.x;
	if(displacement.y < 0.2 * size.y) displacement.y += 0.8 * size.y;
	
	if(displacement != pos) {
		float a = 1.0 - max(0.0, displacement.x/size.x - 0.8) * 5.0;
		float b = 1.0 - max(0.0, displacement.y/size.y - 0.8) * 5.0;
		if(displacement.x == pos.x) a = 0.0;
		if(displacement.y == pos.y) b = 0.0;
		
		vec4 val2 = vec4(0.0);
		float n = 0.0;
		
		if(displacement.x != pos.x) {
			if(pos2.x < 0.2 * size.x) val2 += a * texelFetch(tex, ivec2((int(pos.x) + int(widthDist / res)) % int(0.8 * size.x), int(pos.y)), 0);
			else val2 += a * texelFetch(tex, ivec2(int(displacement.x), int(pos.y)), 0);
			n += a;
		}
		
		if(displacement.y != pos.y) {
			if(pos2.y < 0.2 * size.y) val2 += b * texelFetch(tex, ivec2(int(pos.x), (int(pos.y) + int(heightDist / res)) % int(0.8 * size.y)), 0);
			else val2 += b * texelFetch(tex, ivec2(int(pos.x), int(displacement.y)), 0);
			n += b;
		}
		
		if(displacement.x != pos.x && displacement.y != pos.y) {
			if(pos2.x < 0.2 * size.x && pos2.y < 0.2 * size.y) val2 += 2.0 * min(a, b) * texelFetch(tex, ivec2((int(pos.x) + int(widthDist / res)) % int(0.8 * size.x), (int(pos.y) + int(heightDist / res)) % int(0.8 * size.y)), 0);
			else val2 += 2.0 * min(a, b) * texelFetch(tex, ivec2(int(displacement.x), int(displacement.y)), 0);
			n += 2.0 * min(a, b);
		}
		
		if(n == 0.0) {
			return vec4(0.0);
		}
		
		val2 /= float(n);
		
		float lerp = max(a, b);

		return (1.0 - lerp) * val + lerp * val2;
		// return vec4(lerp);
		//return val2;
	}
	
	return val;
}

vec4 getTexture(vec2 pos, sampler2D tex) {
	return getTextureRes(pos, tex, resolution);
}

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	float x = (UV.x - 0.5) * cameraSize - NODE_POSITION_WORLD.x;
	float y = (UV.y - 0.5) * cameraSize * meshHeight/meshWidth - NODE_POSITION_WORLD.z;
	vec2 pos = vec2(x, y);
	
	vec3 norm = getTexture(pos, normalTexture).rgb;
	METALLIC = 1.0;
	ROUGHNESS = 1.0;
	
	vec3 color1 = getTexture(pos, floorTexture).rgb;
	vec3 color2 = getTexture(pos, floorTexture2).rgb;
	
	vec4 c = texture(screen, vec2(UV.x + 1.0 / cameraSize * norm.x * norm.z, 1.0 - UV.y + 1.0 / cameraSize * norm.y * norm.z));
		
	ALBEDO = 0.25 * vec3(0.1, 0.2, 0.3) * color1 - 0.25 * vec3(0.1, 0.2, 0.3) * vec3(max(1.0 - 2.0 * pow(2.0 * abs(color2.r - 0.5), 0.5), 0.0)) + UV.y * 0.25 * vec3(0.1, 0.2, 0.3);
	c.a *= 0.75;
	ALBEDO = c.a * c.rgb + (1.0 - c.a) * ALBEDO;
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
