shader_type spatial;

uniform sampler2D screen_texture;

uniform sampler2D ridges;
uniform sampler2D erosion;
uniform sampler2D texturing;
uniform sampler2D texturing2;
uniform sampler2D rocks;
uniform sampler2D stoneTexture;
uniform sampler2D noise1;
uniform sampler2D noise2;
uniform sampler2D noise3;
uniform sampler2D hole;
uniform sampler2D texture_normal;
uniform sampler2D ridge_normal;

uniform float cameraSize;
uniform float epsilon;

uniform vec2 outposts[50];
uniform int outpostsLength = 0;
uniform float mapWidth;
uniform float mapHeight;

vec3 getTexture(vec2 pos, sampler2D tex) {
	ivec2 size = textureSize(tex, 0);
	
	return vec3(0.0);
}

vec3 normal(vec2 uv, sampler2D texture_norm) {
	float right = texture(texture_norm, uv + vec2(epsilon, 0)).r;
	float left = texture(texture_norm, uv + vec2(-epsilon, 0)).r;
	float top = texture(texture_norm, uv + vec2(0, epsilon)).r;
	float bottom = texture(texture_norm, uv + vec2(0, -epsilon)).r;
	
	return normalize(vec3(2.0 * (right - left), 2.0 * (bottom - top), -4));
}

void fragment() {
	vec2 pos = (UV - 0.5) * (cameraSize/100.0) - (NODE_POSITION_WORLD.xz)/vec2(100.0, 260.0);
	
	float screen = texture(screen_texture, vec2(UV.x, 1.0 - UV.y)).r;
	
	float ridge = texture(ridges, pos).r;
	
	float erode = texture(erosion, pos).r;
	
	ridge *= erode;
	
	float holes = texture(hole, pos).r;
	
	for(int i = 0; i < outpostsLength; i++) {
		float dX = pos.x * 100.0 + outposts[i].x;
		float dY = pos.y * 260.0 + outposts[i].y;
		
		while(dX > mapWidth/2.0) dX -= mapWidth;
		while(dX < -mapWidth/2.0) dX += mapWidth;
		while(dY > mapHeight/2.0) dY -= mapHeight;
		while(dY < -mapHeight/2.0) dY += mapHeight;
		
		float mag = sqrt(dX * dX + dY * dY);
		if(mag < 20.0) {
			float add = max(0.0, ((10.0 - mag)/10.0)) + holes * (20.0 - mag) / 10.0;
			ridge += add;
			if(ridge > 0.9 && add > 0.5) discard;
		}
	}
	
	float tex = texture(texturing, pos).r;
	
	float tex2 = texture(texturing2, pos).r;
	
	float rock = texture(rocks, pos).r;
	
	float angle = 2.0 * PI * texture(noise3, (UV - 0.5) * (cameraSize/100.0) + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;
	float mag = 0.3 * texture(noise2, (UV - 0.5) * (cameraSize/100.0) + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;
	
	float dX = mag * cos(angle);
	float dY = mag * sin(angle);
	
	float displacementX = 0.5 * texture(noise1, (UV - 0.5 + 0.05 * vec2(dX, dY)) * (cameraSize/100.0) + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;
	float displacementY = 0.5 * texture(noise1, (UV - 0.5 + 0.05 * vec2(dY, dX)) * (cameraSize/100.0) + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;

	vec3 c = texture(screen_texture, vec2(UV.x, 1.0 - UV.y) + vec2(displacementX/cameraSize, displacementY/cameraSize)).rgb;
	
	ALBEDO = (0.6 * vec3(1.0) + 0.4 * c) * 0.5 + 0.5 * erode + 0.5 * tex - 0.4 * tex2;
	
	//if(ridge > 0.1) ALBEDO *= max(0.01, 1.0/0.15 * (0.95 - ridge));
	
	if(ridge > 0.6) {
		vec3 newAlbedo = ALBEDO * max(0.0, 1.0 - 5.0 * (ridge - 0.6));
		
		/*if(newAlbedo.r < 0.5) {
			newAlbedo *= 0.5 + 0.5 * rock;
			METALLIC = 0.25;
		}*/
		
		if(newAlbedo.r < 0.1) {
			METALLIC = 1.0;
			
			float height = 0.0;
			
			while(height < 0.1) {
				height += 0.005;
				
				if(texture(ridges, pos - vec2(0.0, 0.2 * height)).r < 0.6) break;
			}
			
			ALBEDO = (0.75 * rock + 0.25) * vec3(0.1 - height);
		} else {
			ALBEDO = newAlbedo;
			NORMAL_MAP = texture(ridge_normal, pos).rgb;
		}
		
		if(ALBEDO.r < 0.001) discard;
	} else {
		NORMAL_MAP = texture(texture_normal, pos).rgb;
	}
}
