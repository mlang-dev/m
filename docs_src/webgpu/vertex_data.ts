import { vec3 } from 'gl-matrix';
import { SpherePosition, CylinderPosition, ConePosition, TorusPosition } from './math-func';

export const TorusData = (R = 1.5,r = 0.4, N = 100, n = 50, center:vec3 = [0,0,0]) => {
    if(n<2 || N<2) return;
    let pts = [];
    let pt:vec3;
    for(let i = 0;i<N;i++){
        let pt1:vec3[] = [];
        for(let j=0;j<n;j++){
            pt = TorusPosition(R, r, i*360/(N-1),j*360/(n-1), center);
            pt1.push(pt);
        }
        pts.push(pt1);
    }

    let vertex = [] as any, normal = [] as any;
    let p0,p1,p2,p3;
    let ca:vec3, db:vec3, cp:vec3;
    for(let i=0;i<N-1;i++){
        for(let j=0;j<n-1;j++){
            p0 = pts[i][j];
            p1 = pts[i+1][j];
            p2 = pts[i+1][j+1];
            p3 = pts[i][j+1];

            //vertex data
            vertex.push([
                p0[0],p0[1],p0[2],p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],
                p2[0],p2[1],p2[2],p3[0],p3[1],p3[2],p0[0],p0[1],p0[2]
            ]);

            //normal data
            ca = vec3.subtract(vec3.create(), p2, p0);
            db = vec3.subtract(vec3.create(), p3, p1);
            cp = vec3.cross(vec3.create(), ca, db);
            vec3.normalize(cp, cp);
            normal.push([
                cp[0],cp[1],cp[2],cp[0],cp[1],cp[2],cp[0],cp[1],cp[2],
                cp[0],cp[1],cp[2],cp[0],cp[1],cp[2],cp[0],cp[1],cp[2]
            ]);
        }
    }
    return {
        vertexData: new Float32Array(vertex.flat()), 
        normalData: new Float32Array(normal.flat())
    };
}

export const ConeData = (rtop = 0.7, rbottom = 2, height = 2.5, n = 30, center:vec3 = [0,0,0]) => {
    if(n<2) return;
    let pts = [] as any, h = height/2;

    for(let i = 0;i<n+1;i++){
        pts.push([
             ConePosition(rtop,i*360/(n-1), h, center),
             ConePosition(rbottom,i*360/(n-1), -h, center),
             ConePosition(0,i*360/(n-1), -h, center),
             ConePosition(0,i*360/(n-1), h, center)]);
     }

    let vertex = [] as any, normal = [] as any;
    let ca:vec3, db:vec3, cp:vec3;
    let p0, p1, p2, p3, p4, p5;
    for(let i=0;i<n-1;i++){
        p0 = pts[i][0];
        p1 = pts[i][1];
        p2 = pts[i][2];
        p3 = pts[i][3];
        p4 = pts[i+1][0];
        p5 = pts[i+1][1];
      
        //vertex data
        vertex.push([
            //top face
            p0[0],p0[1],p0[2],p4[0],p4[1],p4[2],p3[0],p3[1],p3[2],
            
            //bottom face
            p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],p5[0],p5[1],p5[2],
           
            //outer face
            p0[0],p0[1],p0[2],p1[0],p1[1],p1[2],p5[0],p5[1],p5[2],
            p5[0],p5[1],p5[2],p4[0],p4[1],p4[2],p0[0],p0[1],p0[2],
        ]);

        //normal data
        ca = vec3.subtract(vec3.create(),p5,p0);
        db = vec3.subtract(vec3.create(),p4,p1);
        cp = vec3.cross(vec3.create(), ca, db);
        vec3.normalize(cp,cp);

        normal.push([
            //top face
            0,1,0,0,1,0,0,1,0,

            //bottom face
            0,-1,0,0,-1,0,0,-1,0,

            //outer face
            cp[0],cp[1],cp[2],cp[0],cp[1],cp[2],cp[0],cp[1],cp[2],
            cp[0],cp[1],cp[2],cp[0],cp[1],cp[2],cp[0],cp[1],cp[2]
        ])
    }        

    return { 
        vertexData: new Float32Array(vertex.flat()), 
        normalData: new Float32Array(normal.flat())
    };
};

export const CylinderData = (rin = 0.7, rout = 1.5, height = 3, n = 30, center:vec3 = [0,0,0]) => {
    if(n<2 || rin>=rout) return;
    let pts = [] as any, h = height/2;

    for(let i = 0;i<n;i++){
        pts.push([
            CylinderPosition(rout,i*360/(n-1), h, center),
            CylinderPosition(rout,i*360/(n-1), -h, center),
            CylinderPosition(rin,i*360/(n-1), -h, center),
            CylinderPosition(rin,i*360/(n-1), h, center)]
        );
    }

    let vertex = [] as any, normal = [] as any;
    let p0, p1, p2, p3, p4, p5, p6, p7;
    for(let i=0;i<n-1;i++){
        p0 = pts[i][0];
        p1 = pts[i][1];
        p2 = pts[i][2];
        p3 = pts[i][3];
        p4 = pts[i+1][0];
        p5 = pts[i+1][1];
        p6 = pts[i+1][2];
        p7 = pts[i+1][3];

        //vertex data
        vertex.push([               
           //top face
           p0[0],p0[1],p0[2],p4[0],p4[1],p4[2],p7[0],p7[1],p7[2],
           p7[0],p7[1],p7[2],p3[0],p3[1],p3[2],p0[0],p0[1],p0[2],

           //bottom face
           p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],p6[0],p6[1],p6[2],
           p6[0],p6[1],p6[2],p5[0],p5[1],p5[2],p1[0],p1[1],p1[2],

           //outer face
           p0[0],p0[1],p0[2],p1[0],p1[1],p1[2],p5[0],p5[1],p5[2],
           p5[0],p5[1],p5[2],p4[0],p4[1],p4[2],p0[0],p0[1],p0[2],

           //inner face
           p2[0],p2[1],p2[2],p3[0],p3[1],p3[2],p7[0],p7[1],p7[2],
           p7[0],p7[1],p7[2],p6[0],p6[1],p6[2],p2[0],p2[1],p2[2]
        ]);

        //normal data
        normal.push([               
            //top face
            0,1,0,0,1,0,0,1,0,
            0,1,0,0,1,0,0,1,0,

            //bottom face
            0,-1,0,0,-1,0,0,-1,0,
            0,-1,0,0,-1,0,0,-1,0,

            //outer face
            p0[0]/rout,p0[1]/rout,p0[2]/rout,p1[0]/rout,p1[1]/rout,p1[2]/rout,p5[0]/rout,p5[1]/rout,p5[2]/rout,
            p5[0]/rout,p5[1]/rout,p5[2]/rout,p4[0]/rout,p4[1]/rout,p4[2]/rout,p0[0]/rout,p0[1]/rout,p0[2]/rout,

            //inner face
            p3[0]/rin,p3[1]/rin,p3[2]/rin,p7[0]/rin,p7[1]/rin,p7[2]/rin,p6[0]/rin,p6[1]/rin,p6[2]/rin,
            p6[0]/rin,p6[1]/rin,p6[2]/rin,p2[0]/rin,p2[1]/rin,p2[2]/rin,p3[0]/rin,p3[1]/rin,p3[2]/rin
        ]);
    }        

    return {
        vertexData: new Float32Array(vertex.flat()), 
        normalData: new Float32Array(normal.flat())
    };
};

export const SphereData = (radius:number = 2, u:number = 20, v:number = 15, center:vec3 = [0,0,0]) => {
    if(u<2 || v<2) return;
    let pts = [];
    let pt:vec3;
    for(let i = 0;i<u;i++){
        let pt1:vec3[] = [];
        for(let j=0;j<v;j++){
            pt = SpherePosition(radius, i*180/(u-1), j*360/(v-1), center);                
            pt1.push(pt);
        }
        pts.push(pt1);
    }

    let vertex = [] as any, normal = [] as any;
    let p0:vec3,p1:vec3,p2:vec3,p3:vec3,a:vec3, b:vec3;

    for(let i=0;i<u-1;i++){
        for(let j=0;j<v-1;j++){
            p0 = pts[i][j];
            p1 = pts[i+1][j];
            p2 = pts[i+1][j+1];
            p3 = pts[i][j+1];
            a = vec3.create();
            b = vec3.create();
            vec3.subtract(a, p2,p0);
            vec3.subtract(b, p1,p3); 
            
            // vertex data
            vertex.push([
                //first triangle                    
                p0[0],p0[1],p0[2], p1[0],p1[1],p1[2], p3[0],p3[1],p3[2],  

                //second triangle                    
                p1[0],p1[1],p1[2], p2[0],p2[1],p2[2], p3[0],p3[1],p3[2]  
            ]);

            //normal data
            normal.push([
                //first triangle
                p0[0]/radius,p0[1]/radius,p0[2]/radius,
                p1[0]/radius,p1[1]/radius,p1[2]/radius,
                p3[0]/radius,p3[1]/radius,p3[2]/radius,

                //second triangle                    
                p1[0]/radius,p1[1]/radius,p1[2]/radius, 
                p2[0]/radius,p2[1]/radius,p2[2]/radius,
                p3[0]/radius,p3[1]/radius,p3[2]/radius
            ]);
        }
    }
    return {
        vertexData: new Float32Array(vertex.flat()),
        normalData: new Float32Array(normal.flat())
    }
};

export const TorusWireframeData = (R:number, r:number, N:number, n:number, center:vec3 = [0,0,0]) => {
    if(n<2 || N<2) return;
    let pts = [];
    let pt:vec3;
    for(let i = 0;i<N;i++){
        let pt1:vec3[] = [];
        for(let j=0;j<n;j++){
            pt = TorusPosition(R, r, i*360/(N-1),j*360/(n-1), center);               
            pt1.push(pt);
        }
        pts.push(pt1);
    }

    let pp = [] as any;
    let p0, p1, p2, p3;
    for(let i=0;i<N-1;i++){
        for(let j=0;j<n-1;j++){
            p0 = pts[i][j];
            p1 = pts[i+1][j];
            p2 = pts[i+1][j+1];
            p3 = pts[i][j+1];
            pp.push([
                p0[0],p0[1],p0[2],p1[0],p1[1],p1[2],                   
                p3[0],p3[1],p3[2],p0[0],p0[1],p0[2]
            ]);
        }
    }

    return new Float32Array(pp.flat());
};

export const ConeWireframeData = (rtop:number, rbottom:number, height:number, n:number, center:vec3 = [0,0,0]) => {
    if(n<2) return;
    let pts = [] as any, h = height/2;

    for(let i = 0;i<n+1;i++){
        pts.push([
            ConePosition(rtop,i*360/(n-1), h, center),
            ConePosition(rbottom,i*360/(n-1), -h, center),
            ConePosition(0,i*360/(n-1), -h, center),
            ConePosition(0,i*360/(n-1), h, center)]);
     }

    let p = [] as any;
    let p0, p1, p2, p3, p4, p5;
    for(let i=0;i<n-1;i++){
        p0 = pts[i][0];
        p1 = pts[i][1];
        p2 = pts[i][2];
        p3 = pts[i][3];
        p4 = pts[i+1][0];
        p5 = pts[i+1][1];

        p.push([
            //top 
            p0[0],p0[1],p0[2],p3[0],p3[1],p3[2],
            p4[0],p4[1],p4[2],p0[0],p0[1],p0[2],

            //bottom 
            p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],
            p5[0],p5[1],p5[2],p1[0],p1[1],p1[2],

            //side 
            p0[0],p0[1],p0[2],p1[0],p1[1],p1[2]
        ]);
    }        

    return new Float32Array(p.flat());
};

export const CylinderWireframeData = (rin:number,rout:number, height:number, n:number, center:vec3 =[0,0,0]) => {
    if(n<2 || rin>=rout) return;
    let pts = [] as any, h = height/2;
    
    for(let i = 0;i<n;i++){
        pts.push([
            CylinderPosition(rout,i*360/(n-1), h, center),
            CylinderPosition(rout,i*360/(n-1), -h, center),
            CylinderPosition(rin,i*360/(n-1), -h, center),
            CylinderPosition(rin,i*360/(n-1), h, center)
        ]);
    }

    let p = [] as any;
    let p0, p1, p2, p3, p4, p5, p6, p7;
    for(let i = 0; i < n-1; i++){
        p0 = pts[i][0];
        p1 = pts[i][1];
        p2 = pts[i][2];
        p3 = pts[i][3];
        p4 = pts[i+1][0];
        p5 = pts[i+1][1];
        p6 = pts[i+1][2];
        p7 = pts[i+1][3];

        p.push([
            //top face – 3 lines
            p0[0],p0[1],p0[2],p3[0],p3[1],p3[2],
            p3[0],p3[1],p3[2],p7[0],p7[1],p7[2],
            p4[0],p4[1],p4[2],p0[0],p0[1],p0[2],

            //bottom face – 3 lines
            p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],
            p2[0],p2[1],p2[2],p6[0],p6[1],p6[2],
            p5[0],p5[1],p5[2],p1[0],p1[1],p1[2],

            //side – 2 lines
            p0[0],p0[1],p0[2],p1[0],p1[1],p1[2],
            p3[0],p3[1],p3[2],p2[0],p2[1],p2[2]
        ]);
    }        
    return new Float32Array(p.flat());
};

export const SphereWireframeData = (radius:number, u:number, v:number, center:vec3 =[0,0,0]) => {
    if(u<2 || v<2) return;
    let pts = [];
    let pt:vec3;
    for(let i = 0;i<u;i++){
        let pt1:vec3[] = [];
        for(let j=0;j<v;j++){
            pt = SpherePosition(radius, i*180/(u-1), j*360/(v-1), center);                
            pt1.push(pt);
        }
        pts.push(pt1);
    }

    let pp = [] as any;
    let p0, p1, p2, p3;
    for(let i=0;i<u-1;i++){
        for(let j=0; j<v-1; j++){
            p0 = pts[i][j];
            p1 = pts[i+1][j];
            //p2 = pts[i+1][j+1];
            p3 = pts[i][j+1];               
            pp.push([
                p0[0],p0[1],p0[2],p1[0],p1[1],p1[2],                  
                p0[0],p0[1],p0[2],p3[0],p3[1],p3[2]
            ]);
        }
    }
    return new Float32Array(pp.flat());
}

export const CubeData1 = () => {
    const vertexData = new Float32Array([
        // position,   color
        -1, -1,  1,    0, 0, 1,     // vertex a, index 0
         1, -1,  1,    1, 0, 1,     // vertex b, index 1
         1,  1,  1,    1, 1, 1,     // vertex c, index 2
        -1,  1,  1,    0, 1, 1,     // vertex d, index 3
        -1, -1, -1,    0, 0, 0,     // vertex e, index 4
         1, -1, -1,    1, 0, 0,     // vertex f, index 5
         1,  1, -1,    1, 1, 0,     // vertex g, index 6
        -1,  1, -1,    0, 1, 0,     // vertex h, index 7 
    ]);

    const indexData = new Uint32Array([
        // front
        0, 1, 2, 2, 3, 0,

        // right
        1, 5, 6, 6, 2, 1,

        // back
        4, 7, 6, 6, 5, 4,

        // left
        0, 3, 7, 7, 4, 0,

        // top
        3, 2, 6, 6, 7, 3,

        // bottom
        0, 4, 5, 5, 1, 0
    ]);

    return {
        vertexData,
        indexData
    };
};

export const CubeData = () => {
    const positions = new Float32Array([
        // front
        -1, -1,  1,  
         1, -1,  1,  
         1,  1,  1,
         1,  1,  1,
        -1,  1,  1,
        -1, -1,  1,

        // right
         1, -1,  1,
         1, -1, -1,
         1,  1, -1,
         1,  1, -1,
         1,  1,  1,
         1, -1,  1,

        // back
        -1, -1, -1,
        -1,  1, -1,
         1,  1, -1,
         1,  1, -1,
         1, -1, -1,
        -1, -1, -1,

        // left
        -1, -1,  1,
        -1,  1,  1,
        -1,  1, -1,
        -1,  1, -1,
        -1, -1, -1,
        -1, -1,  1,

        // top
        -1,  1,  1,
         1,  1,  1,
         1,  1, -1,
         1,  1, -1,
        -1,  1, -1,
        -1,  1,  1,

        // bottom
        -1, -1,  1,
        -1, -1, -1,
         1, -1, -1,
         1, -1, -1,
         1, -1,  1,
        -1, -1,  1
    ]);

    const colors = new Float32Array([
        // front - blue
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,

        // right - red
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,
        1, 0, 0,

        //back - yellow
        1, 1, 0,
        1, 1, 0,
        1, 1, 0,
        1, 1, 0,
        1, 1, 0,
        1, 1, 0,

        //left - aqua
        0, 1, 1,
        0, 1, 1,
        0, 1, 1,
        0, 1, 1,
        0, 1, 1,
        0, 1, 1,

        // top - green
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,

        // bottom - fuchsia
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1
    ]);

    const normals = new Float32Array([
        // front
        0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,

        // right
        1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,

        // back           
        0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,

        // left
        -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,

        // top
        0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,

        // bottom
        0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0
    ]);

    return {
        positions,
        colors,
        normals
    };
}