shader_type spatial;

uniform float cameraSize;

uniform float mapWidth;
uniform float mapHeight;

uniform float meshWidth;
uniform float meshHeight;

uniform float resolution;

uniform vec2 outposts[50];
uniform int outpostsLength = 0;

uniform sampler2D rockTexture;
uniform sampler2D ridgeTexture;
uniform sampler2D stoneTexture;
uniform sampler2D ravineTexture;
uniform sampler2D detailedNormal;
uniform sampler2D ridgeNormal;
uniform sampler2D ravineNormal;
uniform sampler2D largeRidgeNormal;
uniform sampler2D stoneNormal;
uniform sampler2D sunlightTexture1;
uniform sampler2D sunlightTexture2;
uniform sampler2D lightTexture1;
uniform sampler2D lightTexture2;

uniform float speed = 0.1;
uniform float tmp = 0.0;

vec4 getTexture(vec2 pos, sampler2D tex) {
	
	if(resolution <= 0.0) return vec4(0.0);
	
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
	
	pos /= resolution;
	vec2 pos2 = pos;
	
	while(pos.x >= 0.8 * size.x) pos.x -= 0.8 * size.x;
	while(pos.x < 0.0) pos.x += 0.8 * size.x;
	while(pos.y >= 0.8 * size.y) pos.y -= 0.8 * size.y;
	while(pos.y < 0.0) pos.y += 0.8 * size.y;
	
	vec4 val = texelFetch(tex, ivec2(int(pos.x), int(pos.y)), 0);
	
	vec2 displacement = pos;
	
	//if(pos2.x < 0.2 * size.x) displacement.x = pos2.x;
	if(displacement.x < 0.2 * size.x) displacement.x += 0.8 * size.x;
	//else if(normalized.x > 0.9) displacement.x -= 0.9;
	//if(pos2.y < 0.2 * size.y) return vec4(1.0);
	if(displacement.y < 0.2 * size.y) displacement.y += 0.8 * size.y;
	//else if(normalized.y > 0.9) displacement.y -= 0.9;
	
	if(displacement != pos) {
		//return vec4(0.0);
		float a = 1.0 - max(0.0, displacement.x/size.x - 0.8) * 5.0;
		float b = 1.0 - max(0.0, displacement.y/size.y - 0.8) * 5.0;
		if(displacement.x == pos.x) a = 0.0;
		if(displacement.y == pos.y) b = 0.0;
		
		vec4 val2 = vec4(0.0);
		float n = 0.0;
		
		if(displacement.x != pos.x) {
			if(pos2.x < 0.2 * size.x) val2 += a * texelFetch(tex, ivec2((int(pos.x) + int(widthDist / resolution)) % int(0.8 * size.x), int(pos.y)), 0);
			else val2 += a * texelFetch(tex, ivec2(int(displacement.x), int(pos.y)), 0);
			n += a;
		}
		
		if(displacement.y != pos.y) {
			if(pos2.y < 0.2 * size.y) val2 += b * texelFetch(tex, ivec2(int(pos.x), (int(pos.y) + int(heightDist / resolution)) % int(0.8 * size.y)), 0);
			else val2 += b * texelFetch(tex, ivec2(int(pos.x), int(displacement.y)), 0);
			n += b;
		}
		
		if(displacement.x != pos.x && displacement.y != pos.y) {
			if(pos2.x < 0.2 * size.x && pos2.y < 0.2 * size.y) val2 += 2.0 * min(a, b) * texelFetch(tex, ivec2((int(pos.x) + int(widthDist / resolution)) % int(0.8 * size.x), (int(pos.y) + int(heightDist / resolution)) % int(0.8 * size.y)), 0);
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

void fragment() {
	float x = (UV.x - 0.5) * cameraSize - NODE_POSITION_WORLD.x;
	float y = (UV.y - 0.5) * cameraSize * meshHeight/meshWidth - NODE_POSITION_WORLD.z;
	vec2 pos = vec2(x, y);

	float rock = getTexture(pos, rockTexture).r;
	
	float ridge = getTexture(pos, ridgeTexture).r;
	
	float ravine = getTexture(pos, ravineTexture).r;
	
	float stone = getTexture(pos, stoneTexture).r;
	
	float sunlight = 0.5 * getTexture(pos + speed * TIME * vec2(1.0), sunlightTexture1).r + 0.5 * getTexture(pos + speed * TIME * vec2(1.0, -1.0), sunlightTexture2).r;
	
	float lightStrength = 0.5 * getTexture(pos + speed * 0.5 * TIME * vec2(1.0), lightTexture1).r + 0.5 * getTexture(pos + speed * 0.5 * TIME * vec2(1.0, -1.0), lightTexture2).r;
	
	float val = 0.01 + 0.01 * stone + 0.01 * rock - 0.01 * ridge - 0.08 * ravine * ravine + 0.5 * sunlight * sunlight * max(1.5 * lightStrength - 0.5, 0.0);
	
	if(ravine > 0.8) val -= ravine - 0.8;
	
	METALLIC = 0.5;
	ROUGHNESS = 0.75;
	
	vec3 normal = 0.5 * (getTexture(pos, detailedNormal).rgb - getTexture(pos, ridgeNormal).rgb) + 0.5 * getTexture(pos, stoneNormal).rgb + 0.5 * getTexture(pos, largeRidgeNormal).rgb;
	
	
	
	for(int i = 0; i < outpostsLength; i++) {
		float dX = pos.x + outposts[i].x;
		float dY = pos.y + outposts[i].y;
		
		while(dX > mapWidth/2.0) dX -= mapWidth;
		while(dX < -mapWidth/2.0) dX += mapWidth;
		while(dY > mapHeight/2.0) dY -= mapHeight;
		while(dY < -mapHeight/2.0) dY += mapHeight;
		
		float mag = sqrt(dX * dX + dY * dY);
		float radius = 10.0;
		if(mag < radius) {
			normal *= mag/radius;
			val *= max(0.0, -1.0 + mag/radius*2.0);
			ROUGHNESS = max(ROUGHNESS, 1.0 - max(0.0, -1.0 + mag/radius*2.0));
			METALLIC = max(METALLIC, 1.0 - max(0.0, -1.0 + mag/radius*2.0));
		}
	}
	
	ALBEDO = vec3(val);
	
	NORMAL_MAP = normal;
}
