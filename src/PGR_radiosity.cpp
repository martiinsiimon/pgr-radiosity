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
            cycles++;
        }
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

    for (int i = 0; i < count; i++)
    {
        double x, y, z;

        bool isSetEnergy[this->model->patches.size()];

        for(int n = 0; n < this->model->patches.size(); n++)
        {
            isSetEnergy[n] = false;
        }

        // center of patches
        x = (this->model->patches[ids[i]]->vertices[0].position[0] + this->model->patches[ids[i]]->vertices[1].position[0] + this->model->patches[ids[i]]->vertices[2].position[0] + this->model->patches[ids[i]]->vertices[3].position[0]) / 4.0;
        y = (this->model->patches[ids[i]]->vertices[0].position[1] + this->model->patches[ids[i]]->vertices[1].position[1] + this->model->patches[ids[i]]->vertices[2].position[1] + this->model->patches[ids[i]]->vertices[3].position[1]) / 4.0;
        z = (this->model->patches[ids[i]]->vertices[0].position[2] + this->model->patches[ids[i]]->vertices[1].position[2] + this->model->patches[ids[i]]->vertices[2].position[2] + this->model->patches[ids[i]]->vertices[3].position[2]) / 4.0;
        glm::vec3 ShootPos (x, y, z);

        x = this->model->patches[ids[i]]->vertices[0].normal[0];
        y = this->model->patches[ids[i]]->vertices[0].normal[1];
        z = this->model->patches[ids[i]]->vertices[0].normal[2];
        glm::vec3 ShootNormal (x, y, z);

        float ShootDArea = this->model->patches[ids[i]]->area;

        cl_float3 *texFront = new cl_float3 [256 * 256];
        cl_float3 *texTop = new cl_float3 [256 * 128];
        cl_float3 *texBottom = new cl_float3 [256 * 128];
        cl_float3 *texLeft = new cl_float3 [128 * 256];
        cl_float3 *texRight = new cl_float3 [128 * 256];

        this->model->getViewFromPatch(ids[i], this->framebuffer, this->texture, &texFront, &texTop, &texBottom, &texLeft, &texRight);

        // Front
        for(int h = 0; h < 256; h++)
        {
            for(int w = 0; w < 256; w++)
            {
                int j = this->model->uniqueColorToId(texFront[w + h * 256]);

                if(isSetEnergy[j] == false) {
                    x = (this->model->patches[j]->vertices[0].position[0] + this->model->patches[j]->vertices[1].position[0] + this->model->patches[j]->vertices[2].position[0] + this->model->patches[j]->vertices[3].position[0]) / 4.0;
                    y = (this->model->patches[j]->vertices[0].position[1] + this->model->patches[j]->vertices[1].position[1] + this->model->patches[j]->vertices[2].position[1] + this->model->patches[j]->vertices[3].position[1]) / 4.0;
                    z = (this->model->patches[j]->vertices[0].position[2] + this->model->patches[j]->vertices[1].position[2] + this->model->patches[j]->vertices[2].position[2] + this->model->patches[j]->vertices[3].position[2]) / 4.0;
                    glm::vec3 RecvPos (x, y, z);

                    x = this->model->patches[j]->vertices[0].normal[0];
                    y = this->model->patches[j]->vertices[0].normal[1];
                    z = this->model->patches[j]->vertices[0].normal[2];
                    glm::vec3 RecvNormal (x, y, z);

                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);

                    this->model->patches[j]->vertices[0].color[0] =
                    this->model->patches[j]->vertices[1].color[0] =
                    this->model->patches[j]->vertices[2].color[0] =
                        this->model->patches[j]->vertices[3].color[0] += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[1] =
                    this->model->patches[j]->vertices[1].color[1] =
                    this->model->patches[j]->vertices[2].color[1] =
                        this->model->patches[j]->vertices[3].color[1] += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[2] =
                    this->model->patches[j]->vertices[1].color[2] =
                    this->model->patches[j]->vertices[2].color[2] =
                        this->model->patches[j]->vertices[3].color[2] += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
                    isSetEnergy[j] = true;
                }
            }
        }

        // Top
        for (int h = 0; h < 128; h++)
        {
            for(int w = 0; w < 256; w++)
            {
                int j = this->model->uniqueColorToId(texTop[w + h * 256]);

                if(isSetEnergy[j] == false) {
                    x = (this->model->patches[j]->vertices[0].position[0] + this->model->patches[j]->vertices[1].position[0] + this->model->patches[j]->vertices[2].position[0] + this->model->patches[j]->vertices[3].position[0]) / 4.0;
                    y = (this->model->patches[j]->vertices[0].position[1] + this->model->patches[j]->vertices[1].position[1] + this->model->patches[j]->vertices[2].position[1] + this->model->patches[j]->vertices[3].position[1]) / 4.0;
                    z = (this->model->patches[j]->vertices[0].position[2] + this->model->patches[j]->vertices[1].position[2] + this->model->patches[j]->vertices[2].position[2] + this->model->patches[j]->vertices[3].position[2]) / 4.0;
                    glm::vec3 RecvPos (x, y, z);

                    x = this->model->patches[j]->vertices[0].normal[0];
                    y = this->model->patches[j]->vertices[0].normal[1];
                    z = this->model->patches[j]->vertices[0].normal[2];
                    glm::vec3 RecvNormal (x, y, z);

                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);

                    this->model->patches[j]->vertices[0].color[0] =
                    this->model->patches[j]->vertices[1].color[0] =
                    this->model->patches[j]->vertices[2].color[0] =
                        this->model->patches[j]->vertices[3].color[0] += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[1] =
                    this->model->patches[j]->vertices[1].color[1] =
                    this->model->patches[j]->vertices[2].color[1] =
                        this->model->patches[j]->vertices[3].color[1] += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[2] =
                    this->model->patches[j]->vertices[1].color[2] =
                    this->model->patches[j]->vertices[2].color[2] =
                        this->model->patches[j]->vertices[3].color[2] += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
                    isSetEnergy[j] = true;
                }
            }
        }

        // Bottom
        for(int h = 0; h < 128; h++)
        {
            for(int w = 0; w < 256; w++)
            {
                int j = this->model->uniqueColorToId(texBottom[w + h * 256]);

                if(isSetEnergy[j] == false) {
                    x = (this->model->patches[j]->vertices[0].position[0] + this->model->patches[j]->vertices[1].position[0] + this->model->patches[j]->vertices[2].position[0] + this->model->patches[j]->vertices[3].position[0]) / 4.0;
                    y = (this->model->patches[j]->vertices[0].position[1] + this->model->patches[j]->vertices[1].position[1] + this->model->patches[j]->vertices[2].position[1] + this->model->patches[j]->vertices[3].position[1]) / 4.0;
                    z = (this->model->patches[j]->vertices[0].position[2] + this->model->patches[j]->vertices[1].position[2] + this->model->patches[j]->vertices[2].position[2] + this->model->patches[j]->vertices[3].position[2]) / 4.0;
                    glm::vec3 RecvPos (x, y, z);

                    x = this->model->patches[j]->vertices[0].normal[0];
                    y = this->model->patches[j]->vertices[0].normal[1];
                    z = this->model->patches[j]->vertices[0].normal[2];
                    glm::vec3 RecvNormal (x, y, z);

                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);

                    this->model->patches[j]->vertices[0].color[0] =
                    this->model->patches[j]->vertices[1].color[0] =
                    this->model->patches[j]->vertices[2].color[0] =
                        this->model->patches[j]->vertices[3].color[0] += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[1] =
                    this->model->patches[j]->vertices[1].color[1] =
                    this->model->patches[j]->vertices[2].color[1] =
                        this->model->patches[j]->vertices[3].color[1] += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[2] =
                    this->model->patches[j]->vertices[1].color[2] =
                    this->model->patches[j]->vertices[2].color[2] =
                        this->model->patches[j]->vertices[3].color[2] += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
                    isSetEnergy[j] = true;
                }
            }
        }

        // Left
        for(int h = 0; h < 256; h++)
        {
            for (int w = 0; w < 128; w++)
            {
                int j = this->model->uniqueColorToId(texLeft[w + h * 256]);

                if(isSetEnergy[j] == false) {
                    x = (this->model->patches[j]->vertices[0].position[0] + this->model->patches[j]->vertices[1].position[0] + this->model->patches[j]->vertices[2].position[0] + this->model->patches[j]->vertices[3].position[0]) / 4.0;
                    y = (this->model->patches[j]->vertices[0].position[1] + this->model->patches[j]->vertices[1].position[1] + this->model->patches[j]->vertices[2].position[1] + this->model->patches[j]->vertices[3].position[1]) / 4.0;
                    z = (this->model->patches[j]->vertices[0].position[2] + this->model->patches[j]->vertices[1].position[2] + this->model->patches[j]->vertices[2].position[2] + this->model->patches[j]->vertices[3].position[2]) / 4.0;
                    glm::vec3 RecvPos (x, y, z);

                    x = this->model->patches[j]->vertices[0].normal[0];
                    y = this->model->patches[j]->vertices[0].normal[1];
                    z = this->model->patches[j]->vertices[0].normal[2];
                    glm::vec3 RecvNormal (x, y, z);

                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);

                    this->model->patches[j]->vertices[0].color[0] =
                    this->model->patches[j]->vertices[1].color[0] =
                    this->model->patches[j]->vertices[2].color[0] =
                        this->model->patches[j]->vertices[3].color[0] += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[1] =
                    this->model->patches[j]->vertices[1].color[1] =
                    this->model->patches[j]->vertices[2].color[1] =
                        this->model->patches[j]->vertices[3].color[1] += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[2] =
                    this->model->patches[j]->vertices[1].color[2] =
                    this->model->patches[j]->vertices[2].color[2] =
                        this->model->patches[j]->vertices[3].color[2] += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
                    isSetEnergy[j] = true;
                }
            }
        }

        // Right
        for(int h = 0; h < 256; h++)
        {
            for(int w = 0; w < 128; w++)
            {
                int j = this->model->uniqueColorToId(texRight[w + h * 256]);

                if(isSetEnergy[j] == false) {
                    x = (this->model->patches[j]->vertices[0].position[0] + this->model->patches[j]->vertices[1].position[0] + this->model->patches[j]->vertices[2].position[0] + this->model->patches[j]->vertices[3].position[0]) / 4.0;
                    y = (this->model->patches[j]->vertices[0].position[1] + this->model->patches[j]->vertices[1].position[1] + this->model->patches[j]->vertices[2].position[1] + this->model->patches[j]->vertices[3].position[1]) / 4.0;
                    z = (this->model->patches[j]->vertices[0].position[2] + this->model->patches[j]->vertices[1].position[2] + this->model->patches[j]->vertices[2].position[2] + this->model->patches[j]->vertices[3].position[2]) / 4.0;
                    glm::vec3 RecvPos (x, y, z);

                    x = this->model->patches[j]->vertices[0].normal[0];
                    y = this->model->patches[j]->vertices[0].normal[1];
                    z = this->model->patches[j]->vertices[0].normal[2];
                    glm::vec3 RecvNormal (x, y, z);

                    double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShootDArea);

                    this->model->patches[j]->vertices[0].color[0] =
                    this->model->patches[j]->vertices[1].color[0] =
                    this->model->patches[j]->vertices[2].color[0] =
                        this->model->patches[j]->vertices[3].color[0] += this->model->patches[ids[i]]->vertices[0].color[0] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[1] =
                    this->model->patches[j]->vertices[1].color[1] =
                    this->model->patches[j]->vertices[2].color[1] =
                        this->model->patches[j]->vertices[3].color[1] += this->model->patches[ids[i]]->vertices[0].color[1] * 0.5 * delta;
                    this->model->patches[j]->vertices[0].color[2] =
                    this->model->patches[j]->vertices[1].color[2] =
                    this->model->patches[j]->vertices[2].color[2] =
                        this->model->patches[j]->vertices[3].color[2] += this->model->patches[ids[i]]->vertices[0].color[2] * 0.5 * delta;
                    this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * 0.5 * delta;
                    isSetEnergy[j] = true;
                }
            }
        }


        this->model->patches[ids[i]]->energy = 0;

        delete[] texFront;
        delete[] texTop;
        delete[] texBottom;
        delete[] texLeft;
        delete[] texRight;
    }

    this->model->updateArrays();
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

    if (ShootPos == RecvPos || visible() == false)
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

bool PGR_radiosity::visible()
{
    return true;
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

    /* Events */
    cl_event event_bufferPatchesInfo; //, event_bufferPatchesGeo;

    clFinish(this->queue);
    /* Read buffers from gpu memory */
    int status = clEnqueueReadBuffer(this->queue,
                                     this->patchesInfoCL,
                                     CL_TRUE, //blocking write
                                     0,
                                     this->model->getPatchesCount() * sizeof (cl_float4),
                                     this->raw_patchesInfo,
                                     0,
                                     0,
                                     &event_bufferPatchesInfo);
    CheckOpenCLError(status, "read patches.");

    /* Decode opencl memory objects */
    this->model->decodePatchesCL(this->raw_patchesInfo, this->raw_patchesEnergies, this->model->getPatchesCount());
    //this->model->decodePatchesGeometryCL(this->raw_patchesGeo, this->model->getPatchesCount());

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

        if ((strcmp(sTmp, "Advanced Micro Devices, Inc.") == 0) || (strcmp(sTmp, "NVIDIA Corporation") == 0))
        {
            platform = cpPlatforms[f0];
        }

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

        if (cdtTmp & CL_DEVICE_TYPE_CPU)
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


    /* Allocate buffer of patches */
    this->patchesInfoCL = clCreateBuffer(this->context,
                                     CL_MEM_READ_WRITE,
                                     this->model->getPatchesCount() * sizeof (cl_float4),
                                     0,
                                     &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer patchesCL");

    this->raw_patchesInfo = new cl_float4[this->model->getPatchesCount()];
    this->raw_patchesEnergies = new cl_double[this->model->getPatchesCount()];
    this->model->getPatchesCL(this->raw_patchesInfo, this->raw_patchesEnergies);

    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->patchesInfoCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_float4),
                                 this->raw_patchesInfo,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Copy patches colors");

    /* Alocate buffer of energies */
    this->patchesEnergiesCL = clCreateBuffer(this->context,
                                         CL_MEM_READ_WRITE,
                                         this->model->getPatchesCount() * sizeof (cl_double),
                                         0,
                                         &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer patchesCL");
    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->patchesEnergiesCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_double),
                                 this->raw_patchesEnergies,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Copy patches");

    /* Allocate buffer of patches geometry */
    this->patchesGeoCL = clCreateBuffer(this->context,
                                             CL_MEM_READ_WRITE,
                                             this->model->getPatchesCount() * sizeof (cl_float16),
                                             0,
                                             &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer patchesGeometryCL");

    this->raw_patchesGeo = new cl_float16[this->model->getPatchesCount()];
    this->model->getPatchesGeometryCL(raw_patchesGeo);
    ciErr = clEnqueueWriteBuffer(this->queue,
                                 this->patchesGeoCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getPatchesCount() * sizeof (cl_float16),
                                 this->raw_patchesGeo,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Copy patches geometry");


    /* Allocate buffer of indices */
    this->indicesCL = clCreateBuffer(this->context,
                                     CL_MEM_READ_WRITE,
                                     this->model->getPatchesCount() * sizeof (cl_uint),
                                     0,
                                     &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer indicesCL");

    this->indicesCountCL = clCreateBuffer(this->context,
                                          CL_MEM_READ_WRITE,
                                          sizeof (cl_uint),
                                          0,
                                          &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer indicesCountCL");

    this->maximalEnergyCL = clCreateBuffer(this->context,
                                          CL_MEM_READ_WRITE,
                                           sizeof (cl_double),
                                          0,
                                          &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer indicesCountCL");

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

    this->maxWorkGroupSize = 1024;
    this->workGroupSize = 1024;

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
    cl_event event_radiosity, event_sort, event_maximalEnergy;

    double maximalEnergy;

    maximalEnergy = this->model->getMaximalEnergy();

    /* Setup arguments to the radiosity kernel */
    status = clSetKernelArg(this->radiosityKernel, 0, sizeof (cl_mem), &this->patchesGeoCL);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCL)");

    status = clSetKernelArg(this->radiosityKernel, 1, sizeof (cl_mem), &this->patchesInfoCL);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCL)");

    cl_uint patchesCount = (uint)this->model->getPatchesCount();
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

    //TODO add argument with big memory blob of texture from point
    //TODO write initial memory data ammount

    //TODO add size of one memory blob as a constant - to skip to another texture

    //TODO add parameter as local memory of patchCount*sizeof(bit) size

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

    double limit = LIMIT;
    status = clSetKernelArg(this->sortKernel, 5, sizeof (cl_double), &limit);
    CheckOpenCLError(status, "clSetKernelArg. (limit)");

    status = clSetKernelArg(this->sortKernel, 6, sizeof (cl_mem), &this->maximalEnergyCL);
    CheckOpenCLError(status, "clSetKernelArg. (maximalEnergy)");

    size_t globalThreadsSort[] = {1}; //only one kernel computes
    size_t localThreadsSort[] = {1};

    debug_log = false;
    while (maximalEnergy > LIMIT)
    {

        cout << cycles << " energy: " << maximalEnergy << endl;
        cycles++;

        /* Star kernel - radiosity step*/
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

        /* Start kerne - recompute indices array */
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

        /* Wait until indices computation ends */
        status = clWaitForEvents(1, &event_sort);
        CheckOpenCLError(status, "clWaitForEvents sortKernel.");


        /* Read maximal energy from buffer */
        status = clEnqueueReadBuffer(this->queue,
                                     this->maximalEnergyCL,
                                     CL_TRUE, //blocking write
                                     0,
                                     sizeof (cl_double),
                                     &maximalEnergy,
                                     0,
                                     NULL,
                                     &event_maximalEnergy);
        CheckOpenCLError(status, "Read maximal energy");
        status = clWaitForEvents(1, &event_maximalEnergy);
        CheckOpenCLError(status, "clWaitForEvents read Maximal energy.");

        //TODO read indices to determine which patches need to have texture view
        //TODO get texture view from these patches
        //TODO create one big memory blob
        //TODO write the memory blob to gpu
    }
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
    status = clReleaseMemObject(this->patchesInfoCL);
    CheckOpenCLError(status, "clReleaseMemObject patchesCL");
    status = clReleaseMemObject(this->patchesGeoCL);
    CheckOpenCLError(status, "clReleaseMemObject patchesGeometryCL");
    status = clReleaseMemObject(this->indicesCountCL);
    CheckOpenCLError(status, "clReleaseMemObject indicesCountCL");
    status = clReleaseMemObject(this->patchesEnergiesCL);
    CheckOpenCLError(status, "clReleaseMemObject patchesEnergiesCL");
    status = clReleaseMemObject(this->maximalEnergyCL);
    CheckOpenCLError(status, "clReleaseMemObject maximalEnergyCL");

    status = clReleaseProgram(this->program);
    CheckOpenCLError(status, "clReleaseProgram.");

    status = clReleaseCommandQueue(this->queue);
    CheckOpenCLError(status, "clReleaseCommandQueue.");

    status = clReleaseContext(this->context);
    CheckOpenCLError(status, "clReleaseContext.");

    delete [] this->raw_patchesInfo;
    delete [] this->raw_patchesGeo;
}