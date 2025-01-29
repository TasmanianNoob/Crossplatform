$input v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(_texSmile, 0);
SAMPLER2D(_texContainer, 1);

void main()
{
	gl_FragColor = mix(texture2D(_texSmile, v_texcoord0), texture2D(_texContainer, v_texcoord0), 0.5);
}
