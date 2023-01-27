import { LightInputs } from './shaders';
import { ParametricSurfaceData } from './surface-data';
import { Seashell } from './math-func';
import { CreateSurfaceWithColormap } from './surface';

const CreateSurface = async (li:LightInputs, isAnimation = true, colormapName = 'jet', scale = 2, scaley = 0) => {
    const data = ParametricSurfaceData(Seashell, 0, 6*Math.PI, 0, 2*Math.PI, 200, 40, -4, 4, -4, 4, scale, scaley, colormapName);
    await CreateSurfaceWithColormap(data?.vertexData!, data?.normalData!, data?.colorData!, li, isAnimation);
}


export function draw_surface() 
{
    let li:LightInputs = {};
    let isAnimation = true;
    let colormapName = 'jet';
    let scale = 2;
    let scaley = 0;

    CreateSurface(li, isAnimation, colormapName, scale, scaley);
}