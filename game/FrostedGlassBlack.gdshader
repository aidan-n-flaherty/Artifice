shader_type canvas_item;

uniform float lod: hint_range(0.0, 5) = 4.0;

uniform sampler2D SCREEN_TEXTURE : hint_screen_texture, filter_linear_mipmap;

void fragment(){
	vec4 color = texture(SCREEN_TEXTURE, SCREEN_UV, lod);
	COLOR = vec4(color.rgb * 0.25, 1.0) + vec4(vec3(0.0), 0.0) - vec4(vec3(1.0 * max(0.0, max(abs(UV.y - 0.5) - 0.45, abs(UV.x - 0.5) - 0.45))), 0.0);
}