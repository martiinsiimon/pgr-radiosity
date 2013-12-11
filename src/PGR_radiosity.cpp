/*
 * File:   PGR_radiosity.cpp
 * Author: Martin Simon      <xsimon14@stud.fit.vutbr.cz>
 *         Lukas Brzobohaty  <xbrzob06@stud.fit.vutbr.cz>
 *
 * Created on 2013-11-29, 10:10
 */

#include "PGR_radiosity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif //WIN32


bool debug_log = true; // enable debug log

double GetTime(void)
{
#if _WIN32  /* toto jede na Windows */
    static int initialized = 0;
    static LARGE_INTEGER frequency;
    LARGE_INTEGER value;

    if (!initialized)
    { /* prvni volani */
        initialized = 1;
        if (QueryPerformanceFrequency(&frequency) == 0)
        { /* pokud hi-res pocitadlo neni podporovano */
            exit(-1);
        }
    }

    QueryPerformanceCounter(&value);
    return (double) value.QuadPart / (double) frequency.QuadPart; /* vrat hodnotu v sekundach */

#else  /* toto jede na Linux/Unixovych systemech */
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1)
    { /* vezmi cas */
        exit(-2);
    }
    return (double) tv.tv_sec + (double) tv.tv_usec / 1000000.; /* vrat cas v sekundach */
#endif
}

PGR_radiosity::PGR_radiosity()
{
}

PGR_radiosity::PGR_radiosity(PGR_model * m, int c)
{
    this->model = m;
    this->computedRadiosity = false;
    this->core = c;
    cout << "Radiosity via " << ((c == PGR_CPU) ? "CPU" : "GPU") << endl;
}

PGR_radiosity::PGR_radiosity(const PGR_radiosity& orig)
{
}

PGR_radiosity::~PGR_radiosity()
{
}

bool PGR_radiosity::isComputed()
{
    return this->computedRadiosity;
}

void PGR_radiosity::setFramebuffer(GLuint fbo)
{
    this->framebuffer = fbo;
}

void PGR_radiosity::setTexture(GLuint tex)
{
    this->texture = tex;
}

void PGR_radiosity::compute()
{
    double t_start, t_end;
    int cycles = 0;

    this->model->generateUniqueColor();

    if (this->core == PGR_CPU)
    {
        t_start = GetTime();
        while (this->model->getMaximalEnergy() > LIMIT)
        {
            this->computeRadiosity();
            cout << cycles << " energy: " << this->model->getMaximalEnergy() << endl;
            cycles++;
            //break;
        }
        this->model->recomputeColors();
        this->model->updateArrays();
        t_end = GetTime();
        cout << "cycles: " << cycles << endl;
    }
    else
    {
        t_start = GetTime();
        this->computeRadiosityCL();
        t_end = GetTime();
    }
    this->computedRadiosity = true;

    cout << "Radiosity computation time: " << t_end - t_start << "s" << endl;
}

void PGR_radiosity::computeRadiosity()
{
    /**
     * Note: Na form faktory nejspis nezbude misto - pokud spravne pocitan a na jeden faktor je treba 4 byty, tak dohromady pro vsechny se jedna pri milionu ploskach o 36 terabytu... doufam, ze jsem nekde udelal mega chybu a mluvime jen o MB...
     */

    vector<uint> ids;
    int count = this->model->getIdsOfNMostEnergizedPatches(&ids, N, LIMIT);
    bool * isSetEnergy = new bool[this->model->patches.size()];

    uint * indices = new uint[count];
    for (int i = 0; i < count; i++)
    {
        indices[i] = ids[i];
    }

    //    cl_uchar3 *texFront = new cl_uchar3 [256 * 256];
    //    cl_uchar3 *texTop = new cl_uchar3 [256 * 128];
    //    cl_uchar3 *texBottom = new cl_uchar3 [256 * 128];
    //    cl_uchar3 *texLeft = new cl_uchar3 [128 * 256];
    //    cl_uchar3 *texRight = new cl_uchar3 [128 * 256];
    //
    //    memset(texFront, 0, sizeof (cl_uchar3) * 256 * 256);
    //    memset(texTop, 0, sizeof (cl_uchar3) * 256 * 128);
    //    memset(texBottom, 0, sizeof (cl_uchar3) * 256 * 128);
    //    memset(texLeft, 0, sizeof (cl_uchar3) * 128 * 256);
    //    memset(texRight, 0, sizeof (cl_uchar3) * 128 * 256);

    cl_uchar3 *tex = new cl_uchar3[768 * 256 * count];
    memset(tex, 0, 768 * 256 * sizeof (cl_uchar3));
    this->model->getTextureCL(tex, indices, count);


    for (int i = 0; i < count; i++)
    {
        /* Clear the flags */
        for (int n = 0; n < this->model->patches.size(); n++)
        {
            isSetEnergy[n] = false;
        }

        // center of patches
        glm::vec3 ShootPos(this->model->patches[ids[i]]->center.s0,
                           this->model->patches[ids[i]]->center.s1,
                           this->model->patches[ids[i]]->center.s2);
        glm::vec3 ShootNormal(this->model->patches[ids[i]]->vertices[0].normal[0],
                              this->model->patches[ids[i]]->vertices[0].normal[1],
                              this->model->patches[ids[i]]->vertices[0].normal[2]);

        float ShootDArea = this->model->patches[ids[i]]->area;

        uint offset = i * 256;

        for (uint y = offset; y < offset + 256; y++)
        {
            for (uint x = 0; x < 768; x++)
            {
                int j = this->model->uniqueColorToId(tex[x + y * 768]);
                if (j >= this->model->patches.size() || j < 0)
                {
                    continue;
                }

                if (isSetEnergy[j] == false)
                {
                    glm::vec3 RecvPos(this->model->patches[j]->center.s0,
                                      this->model->patches[j]->center.s1,
                                      this->model->patches[j]->center.s2);
                    glm::vec3 RecvNormal(this->model->patches[j]->vertices[0].normal[0],
                                         this->model->patches[j]->vertices[0].normal[1],
                                         this->model->patches[j]->vertices[0].normal[2]);

                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);

                    this->model->patches[j]->newDiffColor.s0 += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
                    this->model->patches[j]->newDiffColor.s1 += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
                    this->model->patches[j]->newDiffColor.s2 += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;

                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
                    this->model->patches[j]->intensity += this->model->patches[ids[i]]->energy * delta;
                    isSetEnergy[j] = true;
                }
            }
        }

        //this->model->getViewFromPatch(ids[i], &texFront, &texTop, &texBottom, &texLeft, &texRight);


        // Front
        //        for (int h = 0; h < 256; h++)
        //        {
        //            for (int w = 0; w < 256; w++)
        //            {
        //                int j = this->model->uniqueColorToId(texFront[w + h * 256]);
        //                if (j >= this->model->patches.size() || j < 0)
        //                {
        //                    continue;
        //                }
        //
        //                if (isSetEnergy[j] == false)
        //                {
        //                    glm::vec3 RecvPos(this->model->patches[j]->center.s0, this->model->patches[j]->center.s1, this->model->patches[j]->center.s2);
        //
        //                    x = this->model->patches[j]->vertices[0].normal[0];
        //                    y = this->model->patches[j]->vertices[0].normal[1];
        //                    z = this->model->patches[j]->vertices[0].normal[2];
        //                    glm::vec3 RecvNormal(x, y, z);
        //
        //                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);
        //
        //                    this->model->patches[j]->newDiffColor.s0 += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s1 += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s2 += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
        //
        //                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
        //                    this->model->patches[j]->intensity += this->model->patches[ids[i]]->energy * delta;
        //                    isSetEnergy[j] = true;
        //                }
        //            }
        //        }
        //
        //        // Top
        //        for (int h = 0; h < 128; h++)
        //        {
        //            for (int w = 0; w < 256; w++)
        //            {
        //                int j = this->model->uniqueColorToId(texTop[w + h * 256]);
        //                if (j >= this->model->patches.size() || j < 0)
        //                {
        //                    continue;
        //                }
        //
        //                if (isSetEnergy[j] == false)
        //                {
        //                    glm::vec3 RecvPos(this->model->patches[j]->center.s0, this->model->patches[j]->center.s1, this->model->patches[j]->center.s2);
        //
        //                    x = this->model->patches[j]->vertices[0].normal[0];
        //                    y = this->model->patches[j]->vertices[0].normal[1];
        //                    z = this->model->patches[j]->vertices[0].normal[2];
        //                    glm::vec3 RecvNormal(x, y, z);
        //
        //                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);
        //
        //                    this->model->patches[j]->newDiffColor.s0 += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s1 += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s2 += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
        //
        //                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
        //                    this->model->patches[j]->intensity += this->model->patches[ids[i]]->energy * delta;
        //                    isSetEnergy[j] = true;
        //                }
        //            }
        //        }
        //
        //        // Bottom
        //        for (int h = 0; h < 128; h++)
        //        {
        //            for (int w = 0; w < 256; w++)
        //            {
        //                int j = this->model->uniqueColorToId(texBottom[w + h * 256]);
        //                if (j >= this->model->patches.size() || j < 0)
        //                {
        //                    continue;
        //                }
        //
        //                if (isSetEnergy[j] == false)
        //                {
        //                    glm::vec3 RecvPos(this->model->patches[j]->center.s0, this->model->patches[j]->center.s1, this->model->patches[j]->center.s2);
        //
        //                    x = this->model->patches[j]->vertices[0].normal[0];
        //                    y = this->model->patches[j]->vertices[0].normal[1];
        //                    z = this->model->patches[j]->vertices[0].normal[2];
        //                    glm::vec3 RecvNormal(x, y, z);
        //
        //                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);
        //
        //                    this->model->patches[j]->newDiffColor.s0 += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s1 += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s2 += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
        //
        //                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
        //                    this->model->patches[j]->intensity += this->model->patches[ids[i]]->energy * delta;
        //                    isSetEnergy[j] = true;
        //                }
        //            }
        //        }
        //
        //        // Left
        //        for (int h = 0; h < 256; h++)
        //        {
        //            for (int w = 0; w < 128; w++)
        //            {
        //                int j = this->model->uniqueColorToId(texLeft[w + h * 128]);
        //                if (j >= this->model->patches.size() || j < 0)
        //                {
        //                    continue;
        //                }
        //
        //                if (isSetEnergy[j] == false)
        //                {
        //                    glm::vec3 RecvPos(this->model->patches[j]->center.s0, this->model->patches[j]->center.s1, this->model->patches[j]->center.s2);
        //
        //                    x = this->model->patches[j]->vertices[0].normal[0];
        //                    y = this->model->patches[j]->vertices[0].normal[1];
        //                    z = this->model->patches[j]->vertices[0].normal[2];
        //                    glm::vec3 RecvNormal(x, y, z);
        //
        //                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);
        //
        //                    this->model->patches[j]->newDiffColor.s0 += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s1 += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s2 += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
        //
        //                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
        //                    this->model->patches[j]->intensity += this->model->patches[ids[i]]->energy * delta;
        //                    isSetEnergy[j] = true;
        //                }
        //            }
        //        }
        //
        //        // Right
        //        for (int h = 0; h < 256; h++)
        //        {
        //            for (int w = 0; w < 128; w++)
        //            {
        //                int j = this->model->uniqueColorToId(texRight[w + h * 128]);
        //                if (j >= this->model->patches.size() || j < 0)
        //                {
        //                    continue;
        //                }
        //
        //                if (isSetEnergy[j] == false)
        //                {
        //                    glm::vec3 RecvPos(this->model->patches[j]->center.s0, this->model->patches[j]->center.s1, this->model->patches[j]->center.s2);
        //
        //                    x = this->model->patches[j]->vertices[0].normal[0];
        //                    y = this->model->patches[j]->vertices[0].normal[1];
        //                    z = this->model->patches[j]->vertices[0].normal[2];
        //                    glm::vec3 RecvNormal(x, y, z);
        //
        //                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);
        //
        //                    this->model->patches[j]->newDiffColor.s0 += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s1 += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
        //                    this->model->patches[j]->newDiffColor.s2 += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
        //
        //                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
        //                    this->model->patches[j]->intensity += this->model->patches[ids[i]]->energy * delta;
        //                    isSetEnergy[j] = true;
        //                }
        //            }
        //        }


        this->model->patches[ids[i]]->energy = 0;
    }

    delete [] indices;
    delete [] tex;
    //    delete [] texFront;
    //    delete [] texTop;
    //    delete [] texBottom;
    //    delete [] texLeft;
    //    delete [] texRight;
    delete [] isSetEnergy;
}

/**
 * Computation form factor
 * @param glm::vec3 RecvPos - world-space position of this element
 * @param glm::vec3 ShootPos - world-space position of shooter
 * @param glm::vec3 RecvNormal - world-space normal of this element
 * @param glm::vec3 ShootNormal - world-space normal of shooter
 * @param glm::vec3 ShooterEnergy - energy from shooter residual texture
 * @param float ShootDArea - the delta area of the shooter
 * @param glm::vec3 RecvColor - the reflectivity of this element
 */
double PGR_radiosity::formFactor(glm::vec3 RecvPos, glm::vec3 ShootPos, glm::vec3 RecvNormal, glm::vec3 ShootNormal, float ShootDArea)
{
    // a normalized vector from shooter to receiver

    if (ShootPos == RecvPos)
    {
        return 0.0;
    }

    glm::vec3 r = ShootPos - RecvPos;

    double distance2 = glm::dot(r, r);
    r = glm::normalize(r);

    // the angles of the receiver and the shooter from r
    double cosi = glm::dot(RecvNormal, r);
    double cosj = -glm::dot(ShootNormal, r);

    // retrun computed disc approximation form factor
    return (max(cosi * cosj, (double) 0) / (M_PI * distance2)) * ShootDArea;
}

void PGR_radiosity::computeRadiosityCL()
{
    /* Prepare CL structures */
    if (this->prepareCL() != 0)
    {
        this->releaseCL();
        return;
    }

    /* Run OpenCL kernel that computes radiosity. It includes a loop */
    this->runRadiosityKernelCL();

    clFinish(this->queue);

    int status = clEnqueueReadBuffer(this->queue,
                                     this->diffColorsCL,
                                     CL_TRUE, //blocking read
                                     0,
                                     this->model->getPatchesCount() * sizeof (cl_uchar3),
                                     this->raw_diffColors,
                                     0,
                                     0,
                                     0);
    CheckOpenCLError(status, "Read diffColors.");

    status = clEnqueueReadBuffer(this->queue,
                                 this->intensitiesCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_float),
                                 this->raw_intensities,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(status, "Read intensities.");

    /* Decode opencl memory objects */
    this->model->decodeData(this->raw_diffColors, this->raw_intensities, this->model->getPatchesCount());

    this->model->recomputeColors();

    this->model->updateArrays();

    /* Release CL structures */
    this->releaseCL();
}

const char *PGR_radiosity::CLErrorString(cl_int _err)
{
    switch (_err)
    {
    case CL_SUCCESS: return "Success!";
    case CL_DEVICE_NOT_FOUND: return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE: return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE: return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES: return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY: return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE: return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP: return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH: return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE: return "Program build failure";
    case CL_MAP_FAILURE: return "Map failure";
    case CL_INVALID_VALUE: return "Invalid value";
    case CL_INVALID_DEVICE_TYPE: return "Invalid device type";
    case CL_INVALID_PLATFORM: return "Invalid platform";
    case CL_INVALID_DEVICE: return "Invalid device";
    case CL_INVALID_CONTEXT: return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES: return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE: return "Invalid command queue";
    case CL_INVALID_HOST_PTR: return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT: return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE: return "Invalid image size";
    case CL_INVALID_SAMPLER: return "Invalid sampler";
    case CL_INVALID_BINARY: return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS: return "Invalid build options";
    case CL_INVALID_PROGRAM: return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE: return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME: return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION: return "Invalid kernel definition";
    case CL_INVALID_KERNEL: return "Invalid kernel";
    case CL_INVALID_ARG_INDEX: return "Invalid argument index";
    case CL_INVALID_ARG_VALUE: return "Invalid argument value";
    case CL_INVALID_ARG_SIZE: return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS: return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION: return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE: return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE: return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET: return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST: return "Invalid event wait list";
    case CL_INVALID_EVENT: return "Invalid event";
    case CL_INVALID_OPERATION: return "Invalid operation";
    case CL_INVALID_GL_OBJECT: return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE: return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL: return "Invalid mip-map level";
    default: return "Unknown";
    }
}

void PGR_radiosity::CheckOpenCLError(cl_int _ciErr, const char *_sMsg, ...)
{
    char buffer[1024];

    va_list arg;
    va_start(arg, _sMsg);
    vsprintf(buffer, _sMsg, arg);
    va_end(arg);

    if (_ciErr != CL_SUCCESS)
    {
        printf("ERROR: %s: (%i)%s\n", buffer, _ciErr, CLErrorString(_ciErr));
    }
    else
    {
        if (debug_log)
            printf("%f:    OK: %s\n", GetTime(), buffer);
    }
}

int PGR_radiosity::prepareCL()
{
    cl_int ciErr = CL_SUCCESS;

    // Get Platform
    cl_platform_id *cpPlatforms;
    cl_uint cuiPlatformsCount;
    ciErr = clGetPlatformIDs(0, NULL, &cuiPlatformsCount);
    this->CheckOpenCLError(ciErr, "clGetPlatformIDs: cuiPlatformsNum=%i", cuiPlatformsCount);
    cpPlatforms = (cl_platform_id*) malloc(cuiPlatformsCount * sizeof (cl_platform_id));
    ciErr = clGetPlatformIDs(cuiPlatformsCount, cpPlatforms, NULL);
    this->CheckOpenCLError(ciErr, "clGetPlatformIDs");

    cl_platform_id platform = 0;

    const unsigned int TMP_BUFFER_SIZE = 1024;
    char sTmp[TMP_BUFFER_SIZE];

    for (unsigned int f0 = 0; f0 < cuiPlatformsCount; f0++)
    {
        //bool shouldBrake = false;
        ciErr = clGetPlatformInfo(cpPlatforms[f0], CL_PLATFORM_PROFILE, TMP_BUFFER_SIZE, sTmp, NULL);
        this->CheckOpenCLError(ciErr, "clGetPlatformInfo: Id=%i: CL_PLATFORM_PROFILE=%s", f0, sTmp);
        ciErr = clGetPlatformInfo(cpPlatforms[f0], CL_PLATFORM_VERSION, TMP_BUFFER_SIZE, sTmp, NULL);
        this->CheckOpenCLError(ciErr, "clGetPlatformInfo: Id=%i: CL_PLATFORM_VERSION=%s", f0, sTmp);
        ciErr = clGetPlatformInfo(cpPlatforms[f0], CL_PLATFORM_NAME, TMP_BUFFER_SIZE, sTmp, NULL);
        this->CheckOpenCLError(ciErr, "clGetPlatformInfo: Id=%i: CL_PLATFORM_NAME=%s", f0, sTmp);
        ciErr = clGetPlatformInfo(cpPlatforms[f0], CL_PLATFORM_VENDOR, TMP_BUFFER_SIZE, sTmp, NULL);
        this->CheckOpenCLError(ciErr, "clGetPlatformInfo: Id=%i: CL_PLATFORM_VENDOR=%s", f0, sTmp);

        //prioritize AMD and CUDA platforms

        if ((strcmp(sTmp, "NVIDIA Corporation") == 0))
        {
            platform = cpPlatforms[f0];
        }

        //        if ((strcmp(sTmp, "Advanced Micro Devices, Inc.") == 0))
        //        {
        //            platform = cpPlatforms[f0];
        //        }

        //prioritize Intel
        /*if ((strcmp(sTmp, "Intel(R) Corporation") == 0)) {
            platform = cpPlatforms[f0];
        }*/

        ciErr = clGetPlatformInfo(cpPlatforms[f0], CL_PLATFORM_EXTENSIONS, TMP_BUFFER_SIZE, sTmp, NULL);
        this->CheckOpenCLError(ciErr, "clGetPlatformInfo: Id=%i: CL_PLATFORM_EXTENSIONS=%s", f0, sTmp);
    }

    if (platform == 0)
    { //no prioritized found
        if (cuiPlatformsCount > 0)
        {
            platform = cpPlatforms[0];
        }
        else
        {
            cerr << "No device was found" << endl;
            return -1;
        }
    }
    // Get Devices
    cl_uint cuiDevicesCount;
    ciErr = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &cuiDevicesCount);
    CheckOpenCLError(ciErr, "clGetDeviceIDs: cuiDevicesCount=%i", cuiDevicesCount);
    cl_device_id *cdDevices = (cl_device_id*) malloc(cuiDevicesCount * sizeof (cl_device_id));
    ciErr = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, cuiDevicesCount, cdDevices, NULL);
    CheckOpenCLError(ciErr, "clGetDeviceIDs");

    unsigned int deviceIndex = 0;

    for (unsigned int f0 = 0; f0 < cuiDevicesCount; f0++)
    {
        cl_device_type cdtTmp;
        size_t iDim[3];

        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_TYPE, sizeof (cdtTmp), &cdtTmp, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_TYPE=%s%s%s%s", f0, cdtTmp & CL_DEVICE_TYPE_CPU ? "CPU," : "",
                         cdtTmp & CL_DEVICE_TYPE_GPU ? "GPU," : "",
                         cdtTmp & CL_DEVICE_TYPE_ACCELERATOR ? "ACCELERATOR," : "",
                         cdtTmp & CL_DEVICE_TYPE_DEFAULT ? "DEFAULT," : "");

        if (cdtTmp & CL_DEVICE_TYPE_GPU)
        { //prioritize gpu if both cpu and gpu are available
            deviceIndex = f0;
        }

        cl_bool bTmp;
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_AVAILABLE, sizeof (bTmp), &bTmp, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_AVAILABLE=%s", f0, bTmp ? "YES" : "NO");
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_NAME, TMP_BUFFER_SIZE, sTmp, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_NAME=%s", f0, sTmp);
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_VENDOR, TMP_BUFFER_SIZE, sTmp, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_VENDOR=%s", f0, sTmp);
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DRIVER_VERSION, TMP_BUFFER_SIZE, sTmp, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DRIVER_VERSION=%s", f0, sTmp);
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_PROFILE, TMP_BUFFER_SIZE, sTmp, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_PROFILE=%s", f0, sTmp);
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_VERSION, TMP_BUFFER_SIZE, sTmp, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_VERSION=%s", f0, sTmp);
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof (iDim), iDim, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_MAX_WORK_ITEM_SIZES=%ix%ix%i", f0, iDim[0], iDim[1], iDim[2]);
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof (size_t), iDim, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_MAX_WORK_GROUP_SIZE=%i", f0, iDim[0]);
        ciErr = clGetDeviceInfo(cdDevices[f0], CL_DEVICE_EXTENSIONS, TMP_BUFFER_SIZE, sTmp, NULL);
        CheckOpenCLError(ciErr, "clGetDeviceInfo: Id=%i: CL_DEVICE_EXTENSIONS=%s", f0, sTmp);
    }

    cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0};



    /* Create context */
    this->context = clCreateContext(cps, 1, &cdDevices[deviceIndex], NULL, NULL, &ciErr);
    CheckOpenCLError(ciErr, "clCreateContext");

    /* Create a command queue */
    this->queue = clCreateCommandQueue(this->context, cdDevices[deviceIndex], CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ciErr);
    CheckOpenCLError(ciErr, "clCreateCommandQueue");

    /* Create and compile and openCL program */
    char *cSourceCL = loadProgSource("kernels.cl");

    this->program = clCreateProgramWithSource(this->context, 1, (const char **) &cSourceCL, NULL, &ciErr);
    CheckOpenCLError(ciErr, "clCreateProgramWithSource");
    free(cSourceCL);

    ciErr = clBuildProgram(this->program, 0, NULL, NULL, NULL, NULL);
    CheckOpenCLError(ciErr, "clBuildProgram");

    cl_int logStatus;
    char *buildLog = NULL;
    size_t buildLogSize = 0;
    logStatus = clGetProgramBuildInfo(this->program,
                                      cdDevices[deviceIndex],
                                      CL_PROGRAM_BUILD_LOG,
                                      buildLogSize,
                                      buildLog,
                                      &buildLogSize);

    CheckOpenCLError(logStatus, "clGetProgramBuildInfo.");

    buildLog = (char*) malloc(buildLogSize);
    if (buildLog == NULL)
    {
        printf("Failed to allocate host memory. (buildLog)");
        return -1;
    }
    memset(buildLog, 0, buildLogSize);

    logStatus = clGetProgramBuildInfo(this->program,
                                      cdDevices[deviceIndex],
                                      CL_PROGRAM_BUILD_LOG,
                                      buildLogSize,
                                      buildLog,
                                      NULL);
    CheckOpenCLError(logStatus, "clGetProgramBuildInfo.");
    free(buildLog);

    size_t tempKernelWorkGroupSize;

    /* Create kernels */
    this->radiosityKernel = clCreateKernel(program, "radiosity", &ciErr);
    CheckOpenCLError(ciErr, "clCreateKernel radiosity");
    this->sortKernel = clCreateKernel(program, "sort", &ciErr);
    CheckOpenCLError(ciErr, "clCreateKernel sort");

    this->maxWorkGroupSize = 512;
    this->workGroupSize = 512;

    ciErr = clGetKernelWorkGroupInfo(this->radiosityKernel,
                                     cdDevices[deviceIndex],
                                     CL_KERNEL_WORK_GROUP_SIZE,
                                     sizeof (size_t),
                                     &tempKernelWorkGroupSize,
                                     0);
    CheckOpenCLError(ciErr, "clGetKernelInfo");
    this->maxWorkGroupSize = MIN(tempKernelWorkGroupSize, this->maxWorkGroupSize);

    if (this->workGroupSize > this->maxWorkGroupSize)
    {
        cout << "Out of Resources!" << endl;
        cout << "Group Size specified: " << this->workGroupSize << endl;
        cout << "Max Group Size supported on the kernel: " << this->maxWorkGroupSize << endl;
        cout << "Falling back to " << this->maxWorkGroupSize << endl;

        this->workGroupSize = this->maxWorkGroupSize;
    }


    /* Allocate buffer of colors */
    this->patchesColorsCL = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->model->getPatchesCount() * sizeof (cl_uchar3), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer patchesCL");

    this->raw_patchesColors = new cl_uchar3[this->model->getPatchesCount()];
    this->raw_patchesEnergies = new cl_float[this->model->getPatchesCount()];
    this->raw_diffColors = new cl_uchar3[this->model->getPatchesCount()];
    this->raw_intensities = new cl_float[this->model->getPatchesCount()];
    this->model->getPatchesCL(this->raw_patchesColors, this->raw_patchesEnergies);

    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->patchesColorsCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_uchar3),
                                 this->raw_patchesColors,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Copy patches colors");

    /* Alocate buffer of energies */
    this->patchesEnergiesCL = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->model->getPatchesCount() * sizeof (cl_float), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer patchesCL");

    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->patchesEnergiesCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_float),
                                 this->raw_patchesEnergies,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Copy patches");

    /* Allocate buffer of patches geometry */
    this->patchesGeoCL = clCreateBuffer(this->context, CL_MEM_READ_ONLY, this->model->getPatchesCount() * sizeof (cl_float8), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer patchesGeometryCL");

    this->raw_patchesGeo = new cl_float8[this->model->getPatchesCount()];
    this->model->getPatchesGeometryCL(raw_patchesGeo);
    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->patchesGeoCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_float8),
                                 this->raw_patchesGeo,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Copy patches geometry");


    this->indicesCL = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->maxWorkGroupSize * sizeof (cl_uint), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer indicesCL");

    this->indicesCountCL = clCreateBuffer(this->context, CL_MEM_READ_WRITE, sizeof (cl_uint), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer indicesCountCL");

    this->maximalEnergyCL = clCreateBuffer(this->context, CL_MEM_READ_WRITE, sizeof (cl_float), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer maximalEnergyCL");

    this->diffColorsCL = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->model->getPatchesCount() * sizeof (cl_uchar3), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer diffColorsCL");

    cl_uchar3* zeros = new cl_uchar3[this->model->getPatchesCount()];
    memset(zeros, 0, this->model->getPatchesCount() * sizeof (cl_uchar3));
    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->diffColorsCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_uchar3),
                                 zeros,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Clear diff colors");
    delete [] zeros;

    this->intensitiesCL = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->model->getPatchesCount() * sizeof (cl_float), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer intensitiesCL");

    cl_float* zeroIntensity = new cl_float[this->model->getPatchesCount()];
    memset(zeroIntensity, 0, this->model->getPatchesCount() * sizeof (cl_float));
    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->intensitiesCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_float),
                                 zeroIntensity,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Clear intensities");
    delete [] zeroIntensity;

    this->texturesCL = clCreateBuffer(this->context, CL_MEM_READ_ONLY, this->maxWorkGroupSize * 768 * 256 * sizeof (cl_uchar3), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer texturesCL");

    this->visitedCL = clCreateBuffer(this->context, CL_MEM_READ_WRITE, this->maxWorkGroupSize * this->model->getPatchesCount() * sizeof (cl_bool), 0, &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer visitedCL");

    cl_bool* zeroVisited = new cl_bool[this->maxWorkGroupSize * this->model->getPatchesCount()];
    memset(zeroVisited, 0, this->maxWorkGroupSize * this->model->getPatchesCount() * sizeof (cl_bool));
    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->visitedCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_bool),
                                 zeroVisited,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Clear visited flags");
    delete [] zeroVisited;

    free(cdDevices);

    return 0;
}

char* PGR_radiosity::loadProgSource(const char* cFilename)
{
    // locals
    FILE* pFileStream = NULL;
    size_t szSourceLength;

    pFileStream = fopen(cFilename, "rb");
    if (pFileStream == 0)
    {
        return NULL;
    }


    // get the length of the source code
    fseek(pFileStream, 0, SEEK_END);
    szSourceLength = ftell(pFileStream);
    fseek(pFileStream, 0, SEEK_SET);

    // allocate a buffer for the source code string and read it in
    char* cSourceString = (char *) malloc(szSourceLength + 1);
    if (fread(cSourceString, szSourceLength, 1, pFileStream) != 1)
    {
        fclose(pFileStream);
        free(cSourceString);
        return 0;
    }

    // close the file and return the total length of the combined (preamble + source) string
    fclose(pFileStream);
    cSourceString[szSourceLength] = '\0';

    return cSourceString;
}

void PGR_radiosity::runRadiosityKernelCL()
{
    int status;
    int cycles = 0;

    /* Events */
    //cl_event event_bufferPatchesInfo;
    cl_event event_radiosity, event_sort, event_maximalEnergy, event_indices, event_textures, event_indicesCount;

    float maximalEnergy;

    maximalEnergy = (float) this->model->getMaximalEnergy();

    /* Setup arguments to the radiosity kernel */
    status = clSetKernelArg(this->radiosityKernel, 0, sizeof (cl_mem), &this->patchesGeoCL);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCL)");

    status = clSetKernelArg(this->radiosityKernel, 1, sizeof (cl_mem), &this->patchesColorsCL);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCL)");

    cl_uint patchesCount = (cl_uint)this->model->getPatchesCount();
    status = clSetKernelArg(this->radiosityKernel, 2, sizeof (cl_uint), &patchesCount);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCount)");

    /* Set initial energy */
    this->raw_indices = new cl_uint[this->workGroupSize];
    cl_uint indicesCount = this->model->getIdsOfNMostEnergizedPatchesCL(this->raw_indices, this->workGroupSize, LIMIT);

    status = clEnqueueWriteBuffer(this->queue,
                                  this->indicesCountCL,
                                  CL_TRUE, //blocking write
                                  0,
                                  sizeof (cl_uint),
                                  &indicesCount,
                                  0,
                                  0,
                                  0);
    CheckOpenCLError(status, "Copy indicesCount to GPU");

    status = clEnqueueWriteBuffer(this->queue,
                                  this->indicesCL,
                                  CL_TRUE, //blocking write
                                  0,
                                  indicesCount * sizeof (cl_uint),
                                  this->raw_indices,
                                  0,
                                  0,
                                  0);
    CheckOpenCLError(status, "Copy indices to GPU");

    status = clSetKernelArg(this->radiosityKernel, 3, sizeof (cl_mem), &this->indicesCL);
    CheckOpenCLError(status, "clSetKernelArg. (indicesCL)");

    status = clSetKernelArg(this->radiosityKernel, 4, sizeof (cl_mem), &this->indicesCountCL);
    CheckOpenCLError(status, "clSetKernelArg. (indicesCount)");

    status = clSetKernelArg(this->radiosityKernel, 5, sizeof (cl_mem), &this->patchesEnergiesCL);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCL)");

    status = clSetKernelArg(this->radiosityKernel, 6, sizeof (cl_mem), &this->diffColorsCL);
    CheckOpenCLError(status, "clSetKernelArg. (diffColorsCL)");

    status = clSetKernelArg(this->radiosityKernel, 7, sizeof (cl_mem), &this->intensitiesCL);
    CheckOpenCLError(status, "clSetKernelArg. (intensitiesCL)");

    this->raw_textures = new cl_uchar3[768 * 256 * this->workGroupSize];
    this->model->getTextureCL(this->raw_textures, this->raw_indices, indicesCount);
    status = clEnqueueWriteBuffer(this->queue,
                                  this->texturesCL,
                                  CL_TRUE, //blocking write
                                  0,
                                  768 * 256 * indicesCount * sizeof (cl_uchar3),
                                  this->raw_textures,
                                  0,
                                  0,
                                  0);
    CheckOpenCLError(status, "Copy textures to GPU");

    status = clSetKernelArg(this->radiosityKernel, 8, sizeof (cl_mem), &this->texturesCL);
    CheckOpenCLError(status, "clSetKernelArg. (texturesCL)");

    status = clSetKernelArg(this->radiosityKernel, 9, sizeof (cl_mem), &this->visitedCL);
    CheckOpenCLError(status, "clSetKernelArg. (visitedCL)");

    size_t globalThreadsMain[] = {this->workGroupSize};
    size_t localThreadsMain[] = {this->workGroupSize};

    /* Set arguments for sort kernel */
    status = clSetKernelArg(this->sortKernel, 0, sizeof (cl_mem), &this->patchesEnergiesCL);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCL)");

    status = clSetKernelArg(this->sortKernel, 1, sizeof (cl_uint), &patchesCount);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCount)");

    status = clSetKernelArg(this->sortKernel, 2, sizeof (cl_mem), &this->indicesCL);
    CheckOpenCLError(status, "clSetKernelArg. (indicesCL)");

    status = clSetKernelArg(this->sortKernel, 3, sizeof (cl_mem), &this->indicesCountCL);
    CheckOpenCLError(status, "clSetKernelArg. (indicesCount)");

    status = clSetKernelArg(this->sortKernel, 4, sizeof (cl_uint), &this->workGroupSize);
    CheckOpenCLError(status, "clSetKernelArg. (n)");

    float limit = LIMIT;
    status = clSetKernelArg(this->sortKernel, 5, sizeof (cl_float), &limit);
    CheckOpenCLError(status, "clSetKernelArg. (limit)");

    status = clSetKernelArg(this->sortKernel, 6, sizeof (cl_mem), &this->maximalEnergyCL);
    CheckOpenCLError(status, "clSetKernelArg. (maximalEnergy)");

    size_t globalThreadsSort[] = {1}; //only one kernel computes
    size_t localThreadsSort[] = {1};

    cl_bool* zeroVisited = new cl_bool[this->maxWorkGroupSize * this->model->getPatchesCount()];
    memset(zeroVisited, 0, this->maxWorkGroupSize * this->model->getPatchesCount() * sizeof (cl_bool));

    debug_log = false;
    while (maximalEnergy > LIMIT)
    {

        cout << cycles << " energy: " << maximalEnergy << endl;
        cycles++;

        /* Start kernel - radiosity step*/
        status = clEnqueueNDRangeKernel(this->queue,
                                        this->radiosityKernel,
                                        1, //1D
                                        NULL, //offset
                                        globalThreadsMain,
                                        localThreadsMain,
                                        0,
                                        NULL,
                                        &event_radiosity);

        CheckOpenCLError(status, "clEnqueueNDRangeKernel radiosityKernel.");

        /* Start kernel - recompute indices array */
        status = clEnqueueNDRangeKernel(this->queue,
                                        this->sortKernel,
                                        1, //1D
                                        NULL, //offset
                                        globalThreadsSort,
                                        localThreadsSort,
                                        1,
                                        &event_radiosity,
                                        &event_sort);
        CheckOpenCLError(status, "clEnqueueNDRangeKernel sortKernel.");


        /* Read maximal energy from buffer */
        status = clEnqueueReadBuffer(this->queue,
                                     this->maximalEnergyCL,
                                     CL_TRUE, //blocking write
                                     0,
                                     sizeof (cl_float),
                                     &maximalEnergy,
                                     1,
                                     &event_sort,
                                     &event_maximalEnergy);
        CheckOpenCLError(status, "Read maximal energy");

        status = clEnqueueReadBuffer(this->queue,
                                     this->indicesCountCL,
                                     CL_TRUE, //blocking write
                                     0,
                                     sizeof (cl_uint),
                                     &indicesCount,
                                     1,
                                     &event_sort,
                                     &event_indicesCount);
        CheckOpenCLError(status, "Read indices count");

        status = clWaitForEvents(1, &event_indicesCount);
        CheckOpenCLError(status, "clWaitForEvents read indices count.");

        //cout << indicesCount << "," << maximalEnergy << endl;
        if (indicesCount == 0)
            break;

        //cout << indicesCount << "," << maximalEnergy << endl;
        status = clEnqueueReadBuffer(this->queue,
                                     this->indicesCL,
                                     CL_TRUE, //blocking write
                                     0,
                                     indicesCount * sizeof (cl_uint),
                                     this->raw_indices,
                                     0,
                                     NULL,
                                     &event_indices);
        CheckOpenCLError(status, "Read indices");

        status = clWaitForEvents(1, &event_indices);
        CheckOpenCLError(status, "clWaitForEvents read Indices");

        this->model->getTextureCL(this->raw_textures, this->raw_indices, indicesCount);
        status = clEnqueueWriteBuffer(this->queue,
                                      this->texturesCL,
                                      CL_TRUE, //blocking write
                                      0,
                                      768 * 256 * indicesCount * sizeof (cl_uchar3),
                                      this->raw_textures,
                                      0,
                                      NULL,
                                      &event_textures);
        CheckOpenCLError(status, "Copy textures to GPU");
        status = clWaitForEvents(1, &event_textures);
        CheckOpenCLError(status, "clWaitForEvents write textures.");


                status = clEnqueueWriteBuffer(this->queue,
                                              this->visitedCL,
                                             CL_TRUE, //blocking write
                                             0,
                                             this->model->getPatchesCount() * sizeof (cl_bool),
                                             zeroVisited,
                                             0,
                                             0,
                                             0);
                CheckOpenCLError(status, "Clear visited flags");


        status = clWaitForEvents(1, &event_maximalEnergy);
        CheckOpenCLError(status, "clWaitForEvents read Maximal energy.");
        //break;
    }

    delete [] zeroVisited;
    cout << "cycles: " << cycles << endl;
    debug_log = true;
}

void PGR_radiosity::releaseCL()
{
    /* Releases OpenCL resources (Context, Memory etc.) */
    cl_int status;

    status = clReleaseKernel(this->sortKernel);
    CheckOpenCLError(status, "clReleaseKernel sortKernel.");
    status = clReleaseKernel(this->radiosityKernel);
    CheckOpenCLError(status, "clReleaseKernel radiosityKernel.");

    status = clReleaseMemObject(this->indicesCL);
    CheckOpenCLError(status, "clReleaseMemObject indicesCL");
    status = clReleaseMemObject(this->patchesColorsCL);
    CheckOpenCLError(status, "clReleaseMemObject patchesCL");
    status = clReleaseMemObject(this->patchesGeoCL);
    CheckOpenCLError(status, "clReleaseMemObject patchesGeometryCL");
    status = clReleaseMemObject(this->indicesCountCL);
    CheckOpenCLError(status, "clReleaseMemObject indicesCountCL");
    status = clReleaseMemObject(this->patchesEnergiesCL);
    CheckOpenCLError(status, "clReleaseMemObject patchesEnergiesCL");
    status = clReleaseMemObject(this->maximalEnergyCL);
    CheckOpenCLError(status, "clReleaseMemObject maximalEnergyCL");
    status = clReleaseMemObject(this->diffColorsCL);
    CheckOpenCLError(status, "clReleaseMemObject diffColorsCL");
    status = clReleaseMemObject(this->intensitiesCL);
    CheckOpenCLError(status, "clReleaseMemObject intensitiesCL");
    status = clReleaseMemObject(this->texturesCL);
    CheckOpenCLError(status, "clReleaseMemObject texturesCL");

    status = clReleaseProgram(this->program);
    CheckOpenCLError(status, "clReleaseProgram.");

    status = clReleaseCommandQueue(this->queue);
    CheckOpenCLError(status, "clReleaseCommandQueue.");

    status = clReleaseContext(this->context);
    CheckOpenCLError(status, "clReleaseContext.");

    delete [] this->raw_patchesColors;
    delete [] this->raw_patchesGeo;
    delete [] this->raw_patchesEnergies;
    delete [] this->raw_intensities;
    delete [] this->raw_diffColors;
    delete [] this->raw_textures;
}