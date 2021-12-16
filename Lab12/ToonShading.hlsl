#version 330 core
        out vec4 FragColor;

        in vec2 textcoord;
        in vec3 Normal;  
        in vec3 FragPos;  
  
        uniform vec3 lightPos = vec3(0,0,-1); 
        uniform vec3 view;
        uniform sampler2D textureData;

        void main() {
            float _UnlitOutlineThickness = 0.5;
            float _LitOutlineThickness = 0.5;
            vec4 _OutlineColor = vec4(1,1,0,1);
            vec3 normalDirection = normalize(Normal);
            float _Shininess = 0.5;
            vec4 _SpecColor = vec4(1,0,0,1);
            vec4 _LightColor0 = vec4(1,0,0,1);
            vec4 _UnlitColor = vec4(1,0,0,1);
            float _DiffuseThreshold = 0.5;
            vec4 _Color = vec4(1,0,0,1);
            vec3 viewDirection = normalize(view);
            vec3 lightDirection;
            float attenuation;
            vec3 _WorldSpaceLightPos0 = lightPos;
            if (0.0 == _WorldSpaceLightPos0.w) // directional light?
            {
               attenuation = 1.0; // no attenuation
               lightDirection = normalize(vec3(_WorldSpaceLightPos0));
            } 
            else // point or spot light
            {
               vec3 vertexToLightSource = vec3(_WorldSpaceLightPos0 - FragPos);
               float distance = length(vertexToLightSource);
               attenuation = 1.0 / distance; // linear attenuation 
               lightDirection = normalize(vertexToLightSource);
            }
            
            // default: unlit 
            vec3 fragmentColor = vec3(_UnlitColor); 
            
            // low priority: diffuse illumination
            if (attenuation * max(0.0, dot(normalDirection, lightDirection)) 
               >= _DiffuseThreshold)
            {
               fragmentColor = vec3(_LightColor0) * vec3(_Color); 
            }
            
            // higher priority: outline
            if (dot(viewDirection, normalDirection) 
               < mix(_UnlitOutlineThickness, _LitOutlineThickness, 
               max(0.0, dot(normalDirection, lightDirection))))
            {
               fragmentColor = 
                  vec3(_LightColor0) * vec3(_OutlineColor); 
            }
            
            // highest priority: highlights
            if (dot(normalDirection, lightDirection) > 0.0 
               // light source on the right side?
               && attenuation *  pow(max(0.0, dot(
               reflect(-lightDirection, normalDirection), 
               viewDirection)), _Shininess) > 0.5) 
               // more than half highlight intensity? 
            {
               fragmentColor = _SpecColor.a 
                  * vec3(_LightColor0) * vec3(_SpecColor)
                  + (1.0 - _SpecColor.a) * fragmentColor;
            }
 
            FragColor = vec4(fragmentColor, 1.0);
        }