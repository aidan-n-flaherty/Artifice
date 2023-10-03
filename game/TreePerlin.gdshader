shader_type spatial;
uniform vec3 albedo : source_color;
uniform float epsilon;
uniform sampler2D land;
uniform sampler2D ocean;
uniform sampler2D stones;

uniform sampler2D texture_heightmap : hint_default_black,filter_linear_mipmap,repeat_enable;
uniform float heightmap_scale;
uniform int heightmap_min_layers;
uniform int heightmap_max_layers;
uniform vec2 heightmap_flip;

uniform float cameraSize;

vec3 normal(vec2 uv, sampler2D texture_norm) {
	float right = texture(texture_norm, uv + vec2(epsilon, 0)).r;
	float left = texture(texture_norm, uv + vec2(-epsilon, 0)).r;
	float top = texture(texture_norm, uv + vec2(0, epsilon)).r;
	float bottom = texture(texture_norm, uv + vec2(0, -epsilon)).r;
	
	return normalize(vec3(2.0 * (right - left), 2.0 * (bottom - top), -4));
}

void fragment() {
	vec2 baseUV = (UV - 0.5) * (cameraSize/100.0) - (NODE_POSITION_WORLD.xz - vec2(0.0, 4.0))/vec2(100.0, 260.0);
	
	{
		vec3 view_dir = normalize(normalize(-VERTEX + EYE_OFFSET) * mat3(TANGENT * heightmap_flip.x, -BINORMAL * heightmap_flip.y, NORMAL));
		float num_layers = mix(float(heightmap_max_layers),float(heightmap_min_layers), abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));
		float layer_depth = 1.0 / num_layers;
		float current_layer_depth = 0.0;
		vec2 P = view_dir.xy * heightmap_scale * 0.01;
		vec2 delta = P / num_layers;
		vec2 ofs = baseUV;
		float depth = texture(texture_heightmap, ofs).r;
		float current_depth = 0.0;
		while(current_depth < depth) {
			ofs -= delta;
			depth = texture(texture_heightmap, ofs).r;
			current_depth += layer_depth;
		}
		vec2 prev_ofs = ofs + delta;
		float after_depth  = depth - current_depth;
		float before_depth = (texture(texture_heightmap, prev_ofs).r  ) - current_depth + layer_depth;
		float weight = after_depth / (after_depth - before_depth);
		ofs = mix(ofs,prev_ofs,weight);
		baseUV=ofs;
	}
	
	vec2 pos = baseUV;
	
	float height = texture(land, pos).r;
	float height2 = texture(ocean, pos).r;
	
	float height3 = texture(texture_heightmap, pos).r;
	if(height3 > 0.9) discard;
	ALBEDO = albedo - vec3(0.1 * height);

	if(height2 > 0.3) {
		ALPHA = min(ALPHA, 1.0 - 20.0 * (height2 - 0.3));

		if(height2 > 0.35) discard;
	}
	
	//vec3 norm = normal(pos, land);
	//norm.y *= 1.0;
	//NORMAL = normalize(norm);
}
