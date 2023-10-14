shader_type spatial;

uniform sampler2D screen_texture;

uniform float heightmap_scale;
uniform int heightmap_min_layers;
uniform int heightmap_max_layers;
uniform vec2 heightmap_flip;

uniform sampler2D ridges;
uniform sampler2D erosion;
uniform sampler2D texturing;
uniform sampler2D rocks;
uniform sampler2D hole;

uniform float cameraSize;
uniform float epsilon;

uniform vec2 outposts[50];
uniform int outpostsLength = 0;

float getDepth(vec2 pos) {
	float height = 0.5;
	
	float ridge = texture(ridges, pos).r;
	
	float erode = texture(erosion, pos).r;
	
	float rock = texture(rocks, pos).r;
	
	
	//if(ridge > 0.2) height += 0.5 * (ridge - 0.2);
	//else height += 0.5 * (0.8 - ridge);
	
	height -= erode;
	
	//if(ridge > 0.6) height -= 1.0 * (ridge - 0.6);
	if(ridge > 0.5) height += 10.0 * pow(ridge - 0.5, 4.0);
	//if(ridge > 0.5) height += rock;
	
	for(int i = 0; i < outpostsLength; i++) {
		float deltaX = 100.0 * (outposts[i].x - pos.x);
		float deltaY = 260.0 * (outposts[i].y - pos.y);
		float dist = sqrt(deltaX * deltaX + deltaY * deltaY);
		
		if(dist < 10.0) {
			height = 20.0;
		}
	}
	
	return max(0.0, height);
}

vec3 normal(vec2 pos) {
	float right = getDepth(pos + vec2(epsilon, 0));
	float left = getDepth(pos + vec2(-epsilon, 0));
	float top = getDepth(pos + vec2(0, epsilon));
	float bottom = getDepth(pos + vec2(0, -epsilon));
	
	return normalize(vec3(2.0 * (right - left), 2.0 * (bottom - top), -4));
}

void fragment() {
	vec2 baseUV = (UV - 0.5) * (cameraSize/100.0) - (NODE_POSITION_WORLD.xz)/vec2(100.0, 260.0);
	
	{
		vec3 view_dir = normalize(normalize(-VERTEX + EYE_OFFSET) * mat3(TANGENT * heightmap_flip.x, -BINORMAL * heightmap_flip.y, NORMAL));
		float num_layers = mix(float(heightmap_max_layers),float(heightmap_min_layers), abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));
		float layer_depth = 1.0 / num_layers;
		float current_layer_depth = 0.0;
		vec2 P = view_dir.xy * heightmap_scale * 0.01;
		vec2 delta = P / num_layers;
		vec2 ofs = baseUV;
		float depth = getDepth(ofs);
		float current_depth = 0.0;
		while(current_depth < depth) {
			ofs -= delta;
			depth = getDepth(ofs);
			current_depth += layer_depth;
		}
		vec2 prev_ofs = ofs + delta;
		float after_depth  = depth - current_depth;
		float before_depth = getDepth(prev_ofs) - current_depth + layer_depth;
		float weight = after_depth / (after_depth - before_depth);
		ofs = mix(ofs,prev_ofs,weight);
		baseUV=ofs;
	}
	
	vec2 pos = baseUV;
	
	float screen = texture(screen_texture, UV).r;
	
	float ridge = texture(ridges, pos).r;
	
	float erode = texture(erosion, pos).r;
	
	float tex = texture(texturing, pos).r;
	
	float rock = texture(rocks, pos).r;
	
	ALBEDO = (0.9 * vec3(1.0) + 0.1 * vec3(screen)) * 0.5 + 0.5 * erode - 0.2 * tex;
	
	//if(ridge > 0.1) ALBEDO *= max(0.01, 1.0/0.15 * (0.95 - ridge));
	
	if(ridge > 0.7) ALBEDO -= 3.0 * (ridge - 0.7);
	vec3 norm = normal(pos);
	NORMAL = -normalize(norm);
}
