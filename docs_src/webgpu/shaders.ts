export interface LightInputs {
    ambientIntensity?: string;
    diffuseIntensity?: string;
    specularIntensity?: string;
    shininess?: string;
    specularColor?: string;
    isPhong?: string;
    isTwoSideLighting?: string;
} 

export const Shaders = (li: LightInputs) => {
    
    // define default input values:
    li.ambientIntensity = li.ambientIntensity == undefined ? '0.2' : li.ambientIntensity;
    li.diffuseIntensity = li.diffuseIntensity == undefined ? '0.8' : li.diffuseIntensity;
    li.specularIntensity = li.specularIntensity == undefined ? '0.4' : li.specularIntensity;
    li.shininess = li.shininess == undefined ? '30.0' : li.shininess;
    li.specularColor = li.specularColor == undefined ? '(1.0, 1.0, 1.0)' : li.specularColor;
    li.isPhong = li.isPhong == undefined ? '0' : li.isPhong;
    li.isTwoSideLighting = li.isTwoSideLighting == undefined ? '1' : li.isTwoSideLighting;

    const vertex = `
        struct Uniforms {
            viewProjectionMatrix : mat4x4<f32>,
            modelMatrix : mat4x4<f32>,      
            normalMatrix : mat4x4<f32>,            
        };
        @binding(0) @group(0) var<uniform> uniforms : Uniforms;

        struct Input {
            @location(0) position : vec4<f32>,
            @location(1) normal : vec4<f32>,
            @location(2) color : vec3<f32>,
        };

        struct Output {
            @builtin(position) Position : vec4<f32>,
            @location(0) vPosition : vec4<f32>,
            @location(1) vNormal : vec4<f32>,
            @location(2) vColor : vec3<f32>,
        };

        @vertex
        fn main(input: Input) -> Output {        
            var output: Output;        
            let mPosition:vec4<f32> = uniforms.modelMatrix * input.position; 
            output.vPosition = mPosition;                  
            output.vNormal =  uniforms.normalMatrix*input.normal;
            output.Position = uniforms.viewProjectionMatrix * mPosition;     
            output.vColor = input.color;          
            return output;
        }`;

    const fragment = `
        struct Uniforms {
            lightPosition : vec4<f32>,
            eyePosition : vec4<f32>,
        };
        @binding(1) @group(0) var<uniform> uniforms : Uniforms;
        
        struct Input {
            @location(0) vPosition : vec4<f32>,
            @location(1) vNormal : vec4<f32>,
            @location(2) vColor : vec3<f32>,
        };
        @fragment
        fn main(input: Input) -> @location(0) vec4<f32> {
            let N:vec3<f32> = normalize(input.vNormal.xyz);                
            let L:vec3<f32> = normalize(uniforms.lightPosition.xyz - input.vPosition.xyz);     
            let V:vec3<f32> = normalize(uniforms.eyePosition.xyz - input.vPosition.xyz);          
            let H:vec3<f32> = normalize(L + V);
            let twoSide:i32 = ${li.isTwoSideLighting};
            var diffuse:f32 = ${li.diffuseIntensity} * max(dot(N, L), 0.0);
            if(twoSide == 1){
                diffuse = diffuse + ${li.diffuseIntensity} * max(dot(-N, L), 0.0);
            } 
            var specular:f32;
            var isp:i32 = ${li.isPhong};
            if(isp == 1){                   
                specular = ${li.specularIntensity} * pow(max(dot(V, reflect(-L, N)),0.0), ${li.shininess});
                if(twoSide == 1) {
                    specular = specular + ${li.specularIntensity} * pow(max(dot(V, reflect(-L, -N)),0.0), ${li.shininess});
                }
            } else {
                specular = ${li.specularIntensity} * pow(max(dot(N, H),0.0), ${li.shininess});
                if(twoSide == 1){                     
                    specular = specular + ${li.specularIntensity} * pow(max(dot(-N, H),0.0), ${li.shininess});
                }
            }               
            let ambient:f32 = ${li.ambientIntensity};               
            let finalColor:vec3<f32> = input.vColor * (ambient + diffuse) + vec3<f32>${li.specularColor}*specular; 
            return vec4<f32>(finalColor, 1.0);
        }`;

    return {
        vertex, 
        fragment
    };
}
