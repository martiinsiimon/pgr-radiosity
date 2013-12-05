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

PGR_radiosity::PGR_radiosity()
{
}

PGR_radiosity::PGR_radiosity(PGR_model * m)
{
    this->model = m;
    this->computedFactors = false;
    this->computedRadiosity = false;
    this->core = PGR_CPU;
}

PGR_radiosity::PGR_radiosity(const PGR_radiosity& orig)
{
}

PGR_radiosity::~PGR_radiosity()
{
}

void PGR_radiosity::compute()
{
    if (this->core == PGR_CPU)
    {
        this->computeRadiosity();
    }
    else
    {
        this->computeRadiosityCL();
    }
}

void PGR_radiosity::computeRadiosity()
{
    /**
     * Compute form factors, set this->computedFactors = true
     * Compute radiosity, set this->computedRadiosity = true
     *
     * Note: Na form faktory nejspis nezbude misto - pokud spravne pocitan a na jeden faktor je treba 4 byty, tak dohromady pro vsechny se jedna pri milionu ploskach o 36 terabytu... doufam, ze jsem nekde udelal mega chybu a mluvime jen o MB...
     */

    //int *ids = new int [N];
    vector<int> ids;
    int count = this->model->getIdsOfNMostEnergizedPatches(&ids, N);

    for (int i = 0; i < count; i++)
    {
        double x, y, z;

        // center of patches
        x = (this->model->patches[ids[i]]->vertices[0].position[0] + this->model->patches[ids[i]]->vertices[1].position[0] + this->model->patches[ids[i]]->vertices[2].position[0] + this->model->patches[ids[i]]->vertices[3].position[0]) / 4.0;
        y = (this->model->patches[ids[i]]->vertices[0].position[1] + this->model->patches[ids[i]]->vertices[1].position[1] + this->model->patches[ids[i]]->vertices[2].position[1] + this->model->patches[ids[i]]->vertices[3].position[1]) / 4.0;
        z = (this->model->patches[ids[i]]->vertices[0].position[2] + this->model->patches[ids[i]]->vertices[1].position[2] + this->model->patches[ids[i]]->vertices[2].position[2] + this->model->patches[ids[i]]->vertices[3].position[2]) / 4.0;
        glm::vec3 ShootPos (x, y, z);

        x = this->model->patches[ids[i]]->vertices[0].normal[0];
        y = this->model->patches[ids[i]]->vertices[0].normal[1];
        z = this->model->patches[ids[i]]->vertices[0].normal[2];
        glm::vec3 ShootNormal (x, y, z);

        x = y = z = this->model->patches[ids[i]]->energy;
        glm::vec3 ShooterEnergy (x, y, z);

        float ShootDArea = this->model->patches[ids[i]]->area;

        for(int j = 0; j < this->model->patches.size(); j++) {
            x = (this->model->patches[j]->vertices[0].position[0] + this->model->patches[j]->vertices[1].position[0] + this->model->patches[j]->vertices[2].position[0] + this->model->patches[j]->vertices[3].position[0]) / 4.0;
            y = (this->model->patches[j]->vertices[0].position[1] + this->model->patches[j]->vertices[1].position[1] + this->model->patches[j]->vertices[2].position[1] + this->model->patches[j]->vertices[3].position[1]) / 4.0;
            z = (this->model->patches[j]->vertices[0].position[2] + this->model->patches[j]->vertices[1].position[2] + this->model->patches[j]->vertices[2].position[2] + this->model->patches[j]->vertices[3].position[2]) / 4.0;
            glm::vec3 RecvPos (x, y, z);

            x = this->model->patches[j]->vertices[0].normal[0];
            y = this->model->patches[j]->vertices[0].normal[1];
            z = this->model->patches[j]->vertices[0].normal[2];
            glm::vec3 RecvNormal (x, y, z);

            x = this->model->patches[j]->vertices[0].color[0];
            y = this->model->patches[j]->vertices[0].color[1];
            z = this->model->patches[j]->vertices[0].color[2];
            glm::vec3 RecvColor (x, y, z);

            double delta = this->formFactor(RecvPos, ShootPos, RecvNormal, ShootNormal, ShooterEnergy, ShootDArea, RecvColor);

            this->model->patches[j]->vertices[0].color[0] =
            this->model->patches[j]->vertices[1].color[0] =
            this->model->patches[j]->vertices[2].color[0] =
            this->model->patches[j]->vertices[3].color[0] += this->model->patches[ids[i]]->vertices[0].color[0] * delta;
            this->model->patches[j]->vertices[0].color[1] =
            this->model->patches[j]->vertices[1].color[1] =
            this->model->patches[j]->vertices[2].color[1] =
            this->model->patches[j]->vertices[3].color[1] += this->model->patches[ids[i]]->vertices[0].color[1] * delta;
            this->model->patches[j]->vertices[0].color[2] =
            this->model->patches[j]->vertices[1].color[2] =
            this->model->patches[j]->vertices[2].color[2] =
            this->model->patches[j]->vertices[3].color[2] += this->model->patches[ids[i]]->vertices[0].color[2] * delta;
            this->model->patches[j]->energy += this->model->patches[ids[i]]->energy * delta;
        }
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
double PGR_radiosity::formFactor(glm::vec3 RecvPos, glm::vec3 ShootPos, glm::vec3 RecvNormal, glm::vec3 ShootNormal, glm::vec3 ShooterEnergy, float ShootDArea, glm::vec3 RecvColor)
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
        cout << "DBG: jsem tady2" << endl;
        this->releaseCL();
        return;
    }
    //
    //    if (!this->computedFactors)
    //    {
    //        //TODO begin scope to compute factors
    //    }
    //    else
    //    {
    //        //TODO set factors to this->factorsCL
    //    }

    /* Run OpenCL kernel that computes radiosity. It includes a loop */
    this->runRadiosityKernelCL();

    //TODO read buffers from GPU and copy that back to this->model

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

    cl_context_properties cps[3] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties) platform,
        0
    };

    /* Create context */
    this->context = clCreateContext(cps, 1, &cdDevices[deviceIndex], NULL, NULL, &ciErr);
    CheckOpenCLError(ciErr, "clCreateContext");

    /* Create a command queue */
    this->commandQueue = clCreateCommandQueue(this->context, cdDevices[deviceIndex],
                                              CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &ciErr);
    CheckOpenCLError(ciErr, "clCreateCommandQueue");


    /* Allocate buffer o patches */
    this->patchesCL = clCreateBuffer(this->context,
                                     CL_MEM_READ_WRITE,
                                     this->model->getIndicesCount() * sizeof (cl_float4),
                                     0,
                                     &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer patchesCL");

    raw_patches = new cl_float4[this->model->getIndicesCount()];
    this->model->getPatchesCL(raw_patches);
    ciErr = clEnqueueWriteBuffer(this->commandQueue,
                                 this->patchesCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getIndicesCount() * sizeof (cl_float4),
                                 raw_patches,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Copy patches");


    /* Allocate buffer of patches geometry */
    this->patchesGeometryCL = clCreateBuffer(this->context,
                                             CL_MEM_READ_WRITE,
                                             this->model->getIndicesCount() * sizeof (cl_float16),
                                             0,
                                             &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer patchesGeometryCL");

    raw_patchesGeometry = new cl_float16[this->model->getIndicesCount()];
    this->model->getPatchesGeometryCL(raw_patchesGeometry);
    ciErr = clEnqueueWriteBuffer(this->commandQueue,
                                 this->patchesGeometryCL,
                                 CL_TRUE, //blocking write
                                 0,
                                 this->model->getIndicesCount() * sizeof (cl_float16),
                                 raw_patchesGeometry,
                                 0,
                                 0,
                                 0);
    CheckOpenCLError(ciErr, "Copy patches geometry");


    /* Allocate buffer of indices */
    this->indicesCL = clCreateBuffer(this->context,
                                     CL_MEM_READ_WRITE,
                                     this->model->getIndicesCount() * sizeof (cl_uchar),
                                     0,
                                     &ciErr);
    CheckOpenCLError(ciErr, "CreateBuffer indicesCL");

    //    factorsCL = clCreateBuffer(this->context,
    //                               CL_MEM_READ_WRITE,
    //                               this->model->getIndicesCount() * this->model->getIndicesCount() * sizeof (cl_float), //probably out of memory
    //                               0,
    //                               &ciErr);
    //    CheckOpenCLError(ciErr, "CreateBuffer factorsCL");


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

    size_t tempKernelWorkGroupSize; //TODO choose sizes

    /* Create kernels */
    this->sortKernel = clCreateKernel(program, "sort_parallel", &ciErr);
    CheckOpenCLError(ciErr, "clCreateKernel sort_parallel");
    this->radiosityKernel = clCreateKernel(program, "radiosity", &ciErr);
    CheckOpenCLError(ciErr, "clCreateKernel radiosity");



    // Check group size against group size returned by kernel
    ciErr = clGetKernelWorkGroupInfo(this->sortKernel,
                                     cdDevices[deviceIndex],
                                     CL_KERNEL_WORK_GROUP_SIZE,
                                     sizeof (size_t),
                                     &tempKernelWorkGroupSize,
                                     0);
    CheckOpenCLError(ciErr, "clGetKernelInfo");
    //kernelWorkGroupSize = MIN(tempKernelWorkGroupSize, kernelWorkGroupSize);//TODO choose sizes
    cout << "DBG: maxWorkGroupSize = " << tempKernelWorkGroupSize << endl;


    ciErr = clGetKernelWorkGroupInfo(this->radiosityKernel,
                                     cdDevices[deviceIndex],
                                     CL_KERNEL_WORK_GROUP_SIZE,
                                     sizeof (size_t),
                                     &tempKernelWorkGroupSize,
                                     0);
    CheckOpenCLError(ciErr, "clGetKernelInfo");
    //kernelWorkGroupSize = MIN(tempKernelWorkGroupSize, kernelWorkGroupSize);//TODO choose sizes
    cout << "DBG: maxWorkGroupSize = " << tempKernelWorkGroupSize << endl;

    //TODO rewrite!!!
    //    if ((blockSizeX * blockSizeY) > kernelWorkGroupSize)
    //    {
    //        printf("Out of Resources!\n");
    //        printf("Group Size specified: %i\n", blockSizeX * blockSizeY);
    //        printf("Max Group Size supported on the kernel: %i\n", kernelWorkGroupSize);
    //        printf("Falling back to %i.\n", kernelWorkGroupSize);
    //
    //        if (blockSizeX > kernelWorkGroupSize)
    //        {
    //            blockSizeX = kernelWorkGroupSize;
    //            blockSizeY = 1;
    //        }
    //    }

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

int PGR_radiosity::printTiming(cl_event event, const char* title)
{
    cl_ulong startTime;
    cl_ulong endTime;
    /* Display proiling info */
    cl_int status = clGetEventProfilingInfo(event,
                                            CL_PROFILING_COMMAND_START,
                                            sizeof (cl_ulong),
                                            &startTime,
                                            0);
    CheckOpenCLError(status, "clGetEventProfilingInfo.(startTime)");


    status = clGetEventProfilingInfo(event,
                                     CL_PROFILING_COMMAND_END,
                                     sizeof (cl_ulong),
                                     &endTime,
                                     0);

    CheckOpenCLError(status, "clGetEventProfilingInfo.(stopTime)");

    cl_double elapsedTime = (endTime - startTime) * 1e-6;

    printf("%s elapsedTime %.3lf ms\n", title, elapsedTime);

    return 0;
}

void PGR_radiosity::runRadiosityKernelCL()
{
    int status;
    cl_event event_result, event_sort, event_peaks;

    cl_uint patchesCount = (uint)this->model->getPatchesCount();
    /* Setup arguments to the kernel */

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // mean-shift

    /* patches buffer */
    status = clSetKernelArg(sortKernel,
                            0,
                            sizeof (cl_mem),
                            &this->patchesCL);
    CheckOpenCLError(status, "clSetKernelArg. (patchesCL)");

    /* indices buffer */
    status = clSetKernelArg(sortKernel,
                            1,
                            sizeof (cl_mem),
                            &this->indicesCL);

    CheckOpenCLError(status, "clSetKernelArg. (indicesCL)");

    /* count of patches */
    status = clSetKernelArg(sortKernel,
                            2,
                            sizeof (cl_uint),
                            &patchesCount);

    CheckOpenCLError(status, "clSetKernelArg. (count)");


    //the global number of threads in each dimension has to be divisible
    // by the local dimension numbers
    size_t globalThreadsSort[] = {
        patchesCount //TODO fix it!!
    };
    //size_t localThreadsMeanshift[] = {width, 1};

    status = clEnqueueNDRangeKernel(commandQueue, //TODO is this correct???
                                    sortKernel,
                                    1, //1D
                                    NULL, //offset
                                    globalThreadsSort,
                                    //localThreadsMeanshift,
                                    NULL,
                                    0,
                                    NULL,
                                    &event_sort);

    CheckOpenCLError(status, "clEnqueueNDRangeKernel sortKernel.");


    status = clWaitForEvents(1, &event_sort);
    CheckOpenCLError(status, "clWaitForEvents meanshift.");
    cout << "test" << endl;

    //    //////////////////////////////////////////////////////////////////////////////////////////////////
    //    // PEAKS KERNEL
    //    /* input buffer peaks*/
    //    status = clSetKernelArg(meanshiftPeaksKernel,
    //                            0,
    //                            sizeof (cl_mem),
    //                            &d_peaksBuffer);
    //    CheckOpenCLError(status, "clSetKernelArg. (peaksBuffer)");
    //
    //    /* image width */
    //    status = clSetKernelArg(meanshiftPeaksKernel,
    //                            1,
    //                            sizeof (cl_uint),
    //                            &width);
    //
    //    CheckOpenCLError(status, "clSetKernelArg. (width)");
    //
    //    /* image height */
    //    status = clSetKernelArg(meanshiftPeaksKernel,
    //                            2,
    //                            sizeof (cl_uint),
    //                            &height);
    //
    //    CheckOpenCLError(status, "clSetKernelArg. (height)");
    //
    //    cl_event wait_events[] = {event_meanshift};
    //
    //
    //
    //    status = clEnqueueNDRangeKernel(commandQueue,
    //                                    meanshiftPeaksKernel,
    //                                    2,
    //                                    NULL, //offset
    //                                    globalThreadsMeanshift,
    //                                    //localThreadsMeanshift,
    //                                    NULL,
    //                                    1,
    //                                    wait_events,
    //                                    &event_peaks);
    //
    //    status = clWaitForEvents(1, &event_peaks);
    //    CheckOpenCLError(status, "clWaitForEvents peaks boost.");
    //
    //    //////////////////////////////////////////////////////////////////////////////////////////////////
    //    // RESULT KERNEL
    //    /* input buffer X*/
    //    status = clSetKernelArg(meanshiftResultKernel,
    //                            0,
    //                            sizeof (cl_mem),
    //                            &d_countsBuffer);
    //    CheckOpenCLError(status, "clSetKernelArg. (countsBuffer)");
    //
    //
    //    /* input buffer Y*/
    //    status = clSetKernelArg(meanshiftResultKernel,
    //                            1,
    //                            sizeof (cl_mem),
    //                            &d_peaksBuffer);
    //    CheckOpenCLError(status, "clSetKernelArg. (peaksBuffer)");
    //
    //    /* output buffer */
    //    status = clSetKernelArg(meanshiftResultKernel,
    //                            2,
    //                            sizeof (cl_mem),
    //                            &d_outputImageBuffer);
    //
    //    CheckOpenCLError(status, "clSetKernelArg. (outputImage)");
    //
    //    /* image width */
    //    status = clSetKernelArg(meanshiftResultKernel,
    //                            3,
    //                            sizeof (cl_uint),
    //                            &width);
    //
    //    CheckOpenCLError(status, "clSetKernelArg. (width)");
    //
    //    /* image height */
    //    status = clSetKernelArg(meanshiftResultKernel,
    //                            4,
    //                            sizeof (cl_uint),
    //                            &height);
    //
    //    CheckOpenCLError(status, "clSetKernelArg. (height)");
    //
    //
    //    //the global number of threads in each dimension has to be divisible
    //    // by the local dimension numbers
    //    //size_t globalThreadsResult[] = {
    //    //    ((width + blockSizeX - 1) / blockSizeX) * blockSizeX,
    //    //    ((height + blockSizeY - 1) / blockSizeY) * blockSizeY
    //    //};
    //
    //    //size_t localThreadsResult[] = {blockSizeX, blockSizeY};
    //
    //    cl_event wait_events_res[] = {event_peaks};
    //
    //    size_t globalThreadsResult[] = {
    //        width,
    //        height
    //    };
    //    //size_t localThreadsResult[] = {width, 1};
    //
    //    status = clEnqueueNDRangeKernel(commandQueue,
    //                                    meanshiftResultKernel,
    //                                    2,
    //                                    NULL, //offset
    //                                    globalThreadsResult,
    //                                    //localThreadsResult,
    //                                    NULL,
    //                                    1,
    //                                    wait_events_res,
    //                                    &event_result);
    //
    //    CheckOpenCLError(status, "clEnqueueNDRangeKernel.");
    //
    //    status = clWaitForEvents(1, &event_result);
    //    CheckOpenCLError(status, "clWaitForEvents.");

    printTiming(event_sort, "Mean-shift: ");
    printTiming(event_peaks, "Mean-shift Peaks: ");
    printTiming(event_result, "Mean-shift Result: ");

    //Read back the image - if textures were used for showing this wouldn't be necessary
    //blocking read
    //    status = clEnqueueReadBuffer(commandQueue,
    //                                 d_outputImageBuffer,
    //                                 CL_TRUE,
    //                                 0,
    //                                 width * height * sizeof (cl_uchar4),
    //                                 h_outputImageData,
    //                                 0,
    //                                 0,
    //                                 0);
    //
    //    CheckOpenCLError(status, "read output.");

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
    status = clReleaseMemObject(this->patchesCL);
    CheckOpenCLError(status, "clReleaseMemObject patchesCL");
    status = clReleaseMemObject(this->patchesGeometryCL);
    CheckOpenCLError(status, "clReleaseMemObject patchesGeometryCL");
    //status = clReleaseMemObject(this->factorsCL);
    //CheckOpenCLError(status, "clReleaseMemObject factorsCL");

    status = clReleaseProgram(this->program);
    CheckOpenCLError(status, "clReleaseProgram.");

    status = clReleaseCommandQueue(this->commandQueue);
    CheckOpenCLError(status, "clReleaseCommandQueue.");

    status = clReleaseContext(this->context);
    CheckOpenCLError(status, "clReleaseContext.");

    delete [] this->raw_patches;
    delete [] this->raw_patchesGeometry;
}