#version 330 compatibility

uniform float	uTime;		                 // "Time", from Animate( )
in vec2  	vST;		                     // texture coords

uniform float   uKa, uKd, uKs;		              // coefficients of each type of lighting
uniform vec3    uColor;			                  // object color
uniform vec3    uSpecularColor;		              // light color
uniform float   uShininess;		                  // specular exponent
uniform float   uS0, uT0;
uniform float   uSize;

in  vec3  vN;			// normal vector
in  vec3  vL;			// vector from point to light
in  vec3  vE;			// vector from point to eye


void
main( )
{
	//vec3 myColor = vec3( 1., 0.5, 0. );
	
	//gl_FragColor = vec4( myColor,  1. );

	vec3 Normal     = normalize(vN);
	vec3 Light      = normalize(vL);
	vec3 Eye        = normalize(vE);

	vec3 myColor = uColor;

	if( (vST.s - uS0) * (vST.s - uS0) * 4  + (vST.t - uT0) * (vST.t - uT0) <= uSize * uSize && 
	(vST.s - uS0) * (vST.s - uS0) * 4 + (vST.t - uT0) * (vST.t - uT0) >=  0.25 *  uSize * uSize)
	{
		myColor = vec3( 1., 0., 0. );
	}

	vec3 ambient = uKa * myColor;

	float d = max( dot(Normal,Light), 0. );         // only do diffuse if the light can see the point
	vec3 diffuse = uKd * d * myColor;

	float s = 0.;
	if( dot(Normal,Light) > 0. )	                // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		s = pow( max( dot(Eye,ref),0. ), uShininess );
	}
	vec3 specular = uKs * s * uSpecularColor;
	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}

