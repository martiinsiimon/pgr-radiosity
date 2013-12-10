/*
 * Radiosity OpenCL kernels
 * Authors: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *          Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 */

/*
 * Sort patches (their indices) into indices array - parallel
 */
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics: enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics: enable

inline void AtomicAdd4(__global float4 *source, int i, const float operand)
{
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do
    {
        if(i == 0)
            {prevVal.floatVal = (*source).s0;}
        if(i == 1)
            {prevVal.floatVal = (*source).s1;}
        if(i == 2)
            {prevVal.floatVal = (*source).s2;}
        if(i == 3)
            {prevVal.floatVal = (*source).s3;}

        newVal.floatVal = prevVal.floatVal + operand;
     } while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);

}

inline void AtomicAdd1(__global float *source, const float operand)
{
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do
    {
        prevVal.floatVal = *source;

        newVal.floatVal = prevVal.floatVal + operand;
     } while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);

}

float formFactor(float4 RecvPos, float4 ShootPos, float4 RecvNormal, float4 ShootNormal, float ShootDArea)
{
    float pi = (float)3.14159265358979323846f;

    // a normalized vector from shooter to receiver
    if (ShootPos.x == RecvPos.x &&
        ShootPos.y == RecvPos.y &&
        ShootPos.z == RecvPos.z)
    {
        return 0.0;
    }

    float4 r = ShootPos - RecvPos;

    float distance2 = dot(r, r);
    r = normalize(r);

    // the angles of the receiver and the shooter from r
    float cosi = dot(RecvNormal, r);
    float cosj = -dot(ShootNormal, r);

    // retrun computed disc approximation form factor
    return (max(cosi * cosj, 0.0f) / (pi * distance2)) * ShootDArea;
}

__kernel void sort(__global float* energies, uint patchesCount, __global uint* indices, __global uint* indicesCount, uint n, float limit, __global float* maximalEnergy)
{

    int count = 0; //real count
    uint pos = 0; //position of maximal energy
    uint maxPos = patchesCount; //sentinel
    float max = 0.0f; //maximal energy
    float lastMax = 10000000.0f; //last maximal energy
    float lastMaxOld = lastMax;

    for (int i = 0; i < n; i++)
    {
        max = 0.0;
        int j;
        for (j = 0; j < maxPos; j++)
        {
            float energy = energies[j];
            if (energy >= max && energy < lastMax)
            {
                pos = j;
                max = energy;
            }
        }

        if (i == 0)
        {
            maximalEnergy[0] = max;
        }


        if (max == 0.0 || max < limit)
        {
            if (j == maxPos)
            {
                break;
            }

            lastMax = lastMaxOld;
            maxPos = patchesCount;
            continue;
        }


        indices[i] = pos;
        count++;
        maxPos = pos;
        lastMaxOld = max;
    }

    indicesCount[0] = count;
}


/*
 * Compute radiosity step - energy distribution of N most energized patches
 */
__kernel void radiosity(__global float8* patchesGeo,
                        __global uchar3* patchesColors,
                        uint patchesCount,
                        __global uint* indices,
                        __global uint* indicesCount,
                        __global float* energies,
                        __global uchar3* diffColors,
                        __global float* intensities,
                        __global uchar3* texture,
                        __global bool* visited
                       // uint textureSize,
                       // __local bool* isSetEnergy
                    )
{

    int i = get_global_id(0); //position in indices array

    if (i >= indicesCount[0])
    {
        return;
    }



    uint offset = i*256;
//printf ("%d: nula",i);
    float8 lightGeo = patchesGeo[indices[i]];
    uchar3 lightColor = patchesColors[indices[i]];
    float lightEnergy = energies[indices[i]];
    energies[indices[i]] = 0;

    float x, y, z;

    /* Center of light patch */
    float4 ShootPos = {lightGeo.s0, lightGeo.s1, lightGeo.s2, 0};

    /* Normal of light patch */
    x = lightGeo.s3;
    y = lightGeo.s4;
    z = lightGeo.s5;
    float4 ShootNormal = {x,y,z,0};

    /* Area of light patch */
    float ShootDArea = lightGeo.s6;

    //go through all the texture
    //every point convert to index and if visited[index]==0 -> compute form factor and set to correct index
    //otherwise continue
//printf ("%d: jedna\n",i);
    for(uint h = 0; h < 768; h++)
    {
        for(uint w = offset; w < offset+256; w++)
        {
            uchar3 texColor = texture[w + h * 768];
            int j = texColor.s2;
            j <<= 8;
            j |= texColor.s1;
            j <<= 8;
            j |= texColor.s0;

            if (j >= patchesCount || j < 0)
            {
                //printf("%d: pruser\n",i);
                continue;
            }
//printf("%d: OK\n",i);
            if(visited[j + patchesCount*i])
            {
                float8 patchGeo = patchesGeo[j];
                float4 RecvPos = {patchGeo.s0, patchGeo.s1, patchGeo.s2, 0};

                /* Normal of patch */
                x = patchGeo.s3;
                y = patchGeo.s4;
                z = patchGeo.s5;
                float4 RecvNormal = {x,y,z,0};

                /* Compute form factor */
                float delta = formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);

                /* Distribute color */
                uchar colorDiff0 = lightColor.s0 * 0.5 * delta;
                uchar colorDiff1 = lightColor.s1 * 0.5 * delta;
                uchar colorDiff2 = lightColor.s2 * 0.5 * delta;

                //atom_add(&diffColors[j].s0, colorDiff0);
                diffColors[j].s0 += colorDiff0; //FIXME ATOMIC!!!
                diffColors[j].s1 += colorDiff1; //FIXME ATOMIC!!!
                diffColors[j].s2 += colorDiff2; //FIXME ATOMIC!!!

                 /* Distribute energy */
                float energyDiff = lightEnergy * delta;

                energies[j] += energyDiff * 0.5; //FIXME ATOMIC!!!
                intensities[j] += energyDiff; //FIXME ATOMIC!!!

                visited[j + patchesCount*i] = true;
            }
        }
    }
}


