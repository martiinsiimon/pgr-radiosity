/*
 * Radiosity OpenCL kernels
 * Authors: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *          Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 */

/*
 * Sort patches (their indices) into indices array - parallel
 */
__kernel void sort_parallel(__global float4* patches, __global uchar* indices, uint count)
{

}


/*
 * Compute radiosity step - energy distribution of N most energized patches
 */
__kernel void radiosity()
{}