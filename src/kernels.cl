/*
 * Radiosity OpenCL kernels
 * Authors: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *          Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 */

/*
 * Sort patches (their indices) into indices array - parallel
 */
double formFactor(float4 RecvPos, float4 ShootPos, float4 RecvNormal, float4 ShootNormal, float ShootDArea)
{
    double pi = 3.14159265358979323846;

    // a normalized vector from shooter to receiver
    if (ShootPos.x == RecvPos.x &&
        ShootPos.y == RecvPos.y &&
        ShootPos.z == RecvPos.z)
    {
        return 0.0;
    }

    float4 r = ShootPos - RecvPos;

    float distance2 = distance(ShootPos, RecvPos);//dot(r, r);
    r = normalize(r);

    // the angles of the receiver and the shooter from r
    double cosi = dot(RecvNormal, r);
    double cosj = -dot(ShootNormal, r);

    // retrun computed disc approximation form factor
    return (max(cosi * cosj, (double) 0) / (pi * distance2)) * ShootDArea;
}


/*
 * Compute radiosity step - energy distribution of N most energized patches
 */
__kernel void radiosity(__global float16* patchesGeo, __global float4* patchesInfo, uint patchesCount, __global uint* indices, uint indicesCount)
{
    int emitor = get_global_id(0); //position in indices array

    for (unsigned int i = 0; i < indicesCount; i++)
    {
        float16 lightGeo = patchesGeo[indices[i]];
        float4 lightInfo = patchesInfo[indices[i]];

        float x, y, z;

        // center of patches
        x = (lightGeo.s0 + lightGeo.s3 + lightGeo.s6 + lightGeo.s9) / 4.0;
        y = (lightGeo.s1 + lightGeo.s4 + lightGeo.s7 + lightGeo.sA) / 4.0;
        z = (lightGeo.s2 + lightGeo.s5 + lightGeo.s8 + lightGeo.sB) / 4.0;

        float4 ShootPos = {x,y,z,0};

        x = lightGeo.sC;
        y = lightGeo.sD;
        z = lightGeo.sE;

        float4 ShootNormal = {x,y,z,0};

        float ShootDArea = lightGeo.sF;

        for(int j = 0; j < patchesCount; j++) {

            float16 patchGeo = patchesGeo[j];
            float4 patchInfo = patchesInfo[j];

            x = (patchGeo.s0 + patchGeo.s3 + patchGeo.s6 + patchGeo.s9) / 4.0;
            y = (patchGeo.s1 + patchGeo.s4 + patchGeo.s7 + patchGeo.sA) / 4.0;
            z = (patchGeo.s2 + patchGeo.s5 + patchGeo.s8 + patchGeo.sB) / 4.0;
            float4 RecvPos = {x,y,z,0};

            x = patchGeo.sC;
            y = patchGeo.sD;
            z = patchGeo.sE;
            float4 RecvNormal = {x,y,z,0};

            double delta = formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);

            patchInfo.s0 += lightInfo.s0 * delta;
            patchInfo.s1 += lightInfo.s1 * delta;
            patchInfo.s2 += lightInfo.s2 * delta;
            patchInfo.s3 += lightInfo.s3 * delta;

        }
    }
}


