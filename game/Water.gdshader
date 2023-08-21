shader_type spatial;
uniform vec3 albedo : source_color;
uniform float time_speed;
uniform sampler2D texture_normal;
uniform sampler2D texture_normal2;
uniform sampler2D texture_normal3;
uniform sampler2D texture_normal4;

uniform float cameraSize;
uniform vec2 offset;

void vertex() {
	vec2 pos = (UV - 0.5) * cameraSize/100.0 + vec2(offset.x/100.0, offset.y/260.0);
	
	vec2 displacement1 = vec2(2.0, 0) * time_speed * TIME;
	vec2 displacement2 = vec2(0, 2.0) * time_speed * TIME;
	vec3 normal = mix(texture(texture_normal, pos + displacement1).rgb, texture(texture_normal2, pos + displacement2).rgb, 0.5);
	NORMAL = normal;
	
	vec2 displacement3 = vec2(4, 2) * time_speed * TIME;
	vec2 displacement4 = vec2(0, -4) * time_speed * TIME;
	vec3 tex = mix(texture(texture_normal3, pos + displacement3).rgb, texture(texture_normal4, pos + displacement4).rgb, 0.5);
	VERTEX.y += -7.0 + 5.0 * tex.r;
}

void fragment(){
	//vec2 pos = (UV - 0.5) * cameraSize/100.0 + vec2(offset.x/100.0, offset.y/260.0);
	
	//vec2 displacement1 = vec2(2.0, 0) * time_speed * TIME;
	//vec2 displacement2 = vec2(0, 2.0) * time_speed * TIME;
	
 	ALBEDO = albedo;
	METALLIC = 0.0;
	ROUGHNESS = 0.5;
}

