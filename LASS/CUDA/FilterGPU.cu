
#include "../../LASS/src/LASS.h"
#include "FilterGPU.h"

#define CUDA_CHECK(call)                                                     \
do {                                                                         \
    cudaError_t err = call;                                                  \
    if (err != cudaSuccess) {                                                \
        fprintf(stderr, "CUDA error at %s:%d: %s\n", __FILE__, __LINE__,     \
                cudaGetErrorString(err));                                    \
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
} while (0)

#define CEIL_MULT(x, y)  ( (( (x) + (y) - 1 ) / (y) ) * (y) )

__global__ void LPCombFilterGPU(float *inputSample, float *outputSample, float inputGain, long inputDelay, float inputLpf_gain, float *delaybuf0, float *delaybuf1, long sampleSize){
    float gain=inputGain, lpf_gain=inputLpf_gain;
    double gaine;
    int tx = threadIdx.x, idx;
    long delay = inputDelay, ps=(double)(sampleSize + delay - 1)/delay, pb=(double)(delay + blockDim.x - 1)/blockDim.x;
    //__shared__ float Z0[4096], Z1[4096];
    float *Zsrc = delaybuf0, *Zdest = delaybuf1, *Ztemp;

    for (int i = 0; i < pb; i++){
        idx = i * blockDim.x + tx;
        if (idx < delay)
            outputSample[idx] = 0;
    }

    for (int i = 0; i < pb; i++){
        idx = i * blockDim.x + tx;
        if (idx < delay){
            Zsrc[idx] = inputSample[idx];
            outputSample[idx + delay] = Zsrc[idx];
        }
    }

    for(int j = 2; j < ps; ++j){
        gaine = lpf_gain;
        for (int off = 1; off < pb * blockDim.x; off *= 2) {
            __syncthreads();
            for (int i = 0; i < pb; i++) {
                idx = tx + i * blockDim.x;
                if (idx >= off) {
                    Zdest[idx] = Zsrc[idx] + gaine * Zsrc[idx - off];
                }
                else 
                    Zdest[idx] = Zsrc[idx];
            }
            gaine *= gaine;
            Ztemp = Zsrc;
            Zsrc = Zdest;
            Zdest = Ztemp;
        }

        __syncthreads();
        
        for (int i = 0; i < pb; ++i){
            idx = i * blockDim.x + tx;
            if (idx < delay){
                Zsrc[idx] = gain * Zsrc[idx] + inputSample[(j-1) * delay + idx];
                outputSample[j * delay + idx] = Zsrc[idx];
            }
        }
        if (tx == 0)
            Zsrc[0] += outputSample[j * delay - 1];
    }
}

__global__ void HexAllPassFilterGPU(float *inputSample, float *inputSample0, float *inputSample1, float *inputSample2, float *inputSample3, float *inputSample4, float *inputSample5, float* outputSample, float* envData, float inputGain, long inputDelay, float *delaybuf0, float *delaybuf1, long sampleSize){
    float gain=inputGain, gsqrd=gain*gain, x;
    int tx = blockIdx.x*blockDim.x+threadIdx.x, idx;
    long delay = inputDelay, ps=(double)(sampleSize+delay-1)/delay, pb=(double)(delay+gridDim.x*blockDim.x-1)/(gridDim.x*blockDim.x);
    int stridesz=(delay+blockDim.x-1)/blockDim.x*blockDim.x;
    //__shared__ float X[4096], Y[4096];

    for (int i = 0; i < pb; ++i){
        idx = i * gridDim.x * blockDim.x + tx;
        if (idx < delay){
            delaybuf0[stridesz*blockIdx.x+blockDim.x*i+threadIdx.x] = (inputSample0[idx]+inputSample1[idx]+inputSample2[idx]+inputSample3[idx]+inputSample4[idx]+inputSample5[idx])/6;
            delaybuf1[stridesz*blockIdx.x+blockDim.x*i+threadIdx.x] = -gain*delaybuf0[stridesz*blockIdx.x+blockDim.x*i+threadIdx.x];
            outputSample[idx] = delaybuf1[stridesz*blockIdx.x+blockDim.x*i+threadIdx.x]*envData[idx] + (1-envData[idx])*inputSample[idx];
        }
    }

    for(int i=1; i<ps; ++i){
        for (int j = 0; j < pb; ++j){
            idx = j* gridDim.x * blockDim.x + tx;
            if (idx < delay&& i*delay+idx < sampleSize){
                x=delaybuf0[stridesz*blockIdx.x+blockDim.x*j+threadIdx.x];
                delaybuf0[stridesz*blockIdx.x+blockDim.x*j+threadIdx.x] = (inputSample0[i*delay+idx]+inputSample1[i*delay+idx]+inputSample2[i*delay+idx]+inputSample3[i*delay+idx]+inputSample4[i*delay+idx]+inputSample5[i*delay+idx])/6;
                delaybuf1[stridesz*blockIdx.x+blockDim.x*j+threadIdx.x] = -gain*delaybuf0[stridesz*blockIdx.x+blockDim.x*j+threadIdx.x]+(1-gsqrd)*(gain*delaybuf1[stridesz*blockIdx.x+blockDim.x*j+threadIdx.x]+x);
                outputSample[i*delay+idx] = delaybuf1[stridesz*blockIdx.x+blockDim.x*j+threadIdx.x]*envData[i*delay+idx]+(1-envData[i*delay+idx])*inputSample[i*delay+idx];
            }
        }
    }
}

__global__ void getEnvData(float *xyPoints, int *segmentTypes, float *envData, int segmentSize, long sampleSize){
    int tx = blockDim.x*blockIdx.x+threadIdx.x, samples, idx, start, i, j;
    float x0, y0, x1, y1, m0=0, m1=0, t, a, b;
    for(i=0; i<segmentSize; ++i){
        x0=xyPoints[2*i];
        y0=xyPoints[2*i+1];
        x1=xyPoints[2*i+2];
        y1=xyPoints[2*i+3];
        samples=(x1-x0)*sampleSize;
        start=sampleSize * x0;
        if(segmentTypes[i]==2){

            m0=(y1-y0)/(x1-x0)/sampleSize;
            for(j=0; j<samples/(blockDim.x*gridDim.x); ++j){
                idx = j * (blockDim.x*gridDim.x)+ tx;
                envData[start + idx] = y0 + idx*m0;
            }

            idx = j * (blockDim.x*gridDim.x) + tx;
            if(start+ idx<sampleSize)
                envData[start + idx] = y0 + idx*m0;

        }
        else if(segmentTypes[i]==1){
            //Cubic Hermite spline
            if(i!=0){
                if(segmentTypes[i-1]==0){
                    a=y0>y1?3:-3;
                    b=pow(2.71828, (double)a);
                    m0=a*(y0-xyPoints[2*i-1])*b/(x0-xyPoints[2*i-2])/(b-1);
                }
                else if(segmentTypes[i-1]==1)
                    m0=(y1-xyPoints[2*i-1])/(x1-xyPoints[2*i-2]);
                else 
                    m0=(y0-xyPoints[2*i-1])/(x0-xyPoints[2*i-2]);
            }
            else
                m0=0;

            if(i+1<segmentSize){
                if(segmentTypes[i+1]==0){
                    a=xyPoints[2*i+5]>y1?3:-3;
                    m1=a*(xyPoints[2*i+5]-y1)/(xyPoints[2*i+4]-x1)/(pow(2.71828, (double)a)-1);
                }
                else if(segmentTypes[i+1]==1)
                    m1=(xyPoints[2*i+5]-y0)/(xyPoints[2*i+4]-x0);
                else 
                    m1=(xyPoints[2*i+5]-y1)/(xyPoints[2*i+4]-x1);
            }
            else
                m1=0;

            a= -3*y0+3*y1-2*m0-m1;
            b= 2*y0-2*y1+m0+m1;

            for(j=0; j<samples/(blockDim.x*gridDim.x); ++j){
                idx = j * (blockDim.x*gridDim.x) + tx;
                t=(float)idx/samples;
                envData[start + idx] = y0 + m0*t + a*t*t + b*t*t*t;
            }
            idx = j * (blockDim.x*gridDim.x) + tx;
            if(start + idx<sampleSize){
                t=(float)idx/samples;
                envData[start + idx] = y0 + m0*t + a*t*t + b*t*t*t;
            }
        }
        else{
            a=y1>y0?3:-3;
            b=pow(2.71828, (double)a);

            for(j=0; j<samples/(blockDim.x*gridDim.x); ++j){
                idx = j * (blockDim.x*gridDim.x) + tx;
                envData[start + idx] = y0+(y1-y0)*(1-pow(2.71828, (double)a*idx/samples))/(1-b);
            }

            idx = j * (blockDim.x*gridDim.x) + tx;
            if(start+ idx<sampleSize)
                envData[start + idx] = y0+(y1-y0)*(1-pow(2.71828, (double)a*idx/samples))/(1-b);
        }
        __syncthreads();
    }
}

void plotWithGnuplot(const std::vector<float>& data) {
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe) {
        // Set up the plot
        fprintf(gnuplotPipe, "set title 'Plot of Floats from 0 to 2'\n");
        fprintf(gnuplotPipe, "set xlabel 'Index'\n");
        fprintf(gnuplotPipe, "set ylabel 'Value'\n");
        fprintf(gnuplotPipe, "plot '-' with lines\n");
        
        // Send data to GNUplot
        for(size_t i = 0; i < data.size(); ++i){
            fprintf(gnuplotPipe, "%zu %f\n", i, data[i]);
        }
        fprintf(gnuplotPipe, "e\n");
        pclose(gnuplotPipe);
    } else {
        std::cerr << "Could not open pipe to GNUplot.\n";
    }
}

SoundSample* do_reverb_SoundSample_GPU(SoundSample *inWave, Envelope *percentReverbinput, LPCombFilter **lpCombFilter, AllPassFilter *allPassFilter){ 
    SoundSample *outWave=new SoundSample(inWave->getSampleCount(),inWave->getSamplingRate());
    float *inWaveData=inWave->getData(), *outWaveDataD0, *outWaveDataD1, *outWaveDataD2, *outWaveDataD3, *outWaveDataD4, *outWaveDataD5, *outWaveDataD, *inWaveDataD, *outWaveData=new float[inWave->getSampleCount()];
    float *delay0bufD0, *delay0bufD1, *delay0bufD2, *delay0bufD3, *delay0bufD4, *delay0bufD5;
    float *delay1bufD0, *delay1bufD1, *delay1bufD2, *delay1bufD3, *delay1bufD4, *delay1bufD5;
    float *delay0bufAllP, *delay1bufAllP;
    long sampleSize=inWave->getSampleCount();
    float durationofEnv=percentReverbinput->getDuration();
    float *envData=new float[sampleSize], *envDataD, *envXY, *envXYD;
    int *envSegType, *envSegTypeD, segSize;

    Collection<envelope_segment> *segs=percentReverbinput->getSegments();
    envelope_segment seg;
    segSize=segs->size();

    envXY=new float[segSize*2];
    envSegType=new int[segSize-1];

    for (int i = 1; i < segSize; ++i){
        seg=segs->get(i);
        envXY[i*2]=seg.x;
        envXY[i*2+1]=seg.y;
        envSegType[i-1]=seg.interType;
    }
    
    seg=segs->get(0);
    envXY[0]=seg.x;
    envXY[1]=seg.y;

    cudaMalloc(&inWaveDataD, sampleSize*sizeof(float));
    cudaMalloc(&outWaveDataD0, sampleSize*sizeof(float));
    cudaMalloc(&outWaveDataD1, sampleSize*sizeof(float));
    cudaMalloc(&outWaveDataD2, sampleSize*sizeof(float));
    cudaMalloc(&outWaveDataD3, sampleSize*sizeof(float));
    cudaMalloc(&outWaveDataD4, sampleSize*sizeof(float));
    cudaMalloc(&outWaveDataD5, sampleSize*sizeof(float));
    cudaMalloc(&delay0bufD0, CEIL_MULT(lpCombFilter[0]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay0bufD1, CEIL_MULT(lpCombFilter[1]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay0bufD2, CEIL_MULT(lpCombFilter[2]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay0bufD3, CEIL_MULT(lpCombFilter[3]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay0bufD4, CEIL_MULT(lpCombFilter[4]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay0bufD5, CEIL_MULT(lpCombFilter[5]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay1bufD0, CEIL_MULT(lpCombFilter[0]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay1bufD1, CEIL_MULT(lpCombFilter[1]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay1bufD2, CEIL_MULT(lpCombFilter[2]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay1bufD3, CEIL_MULT(lpCombFilter[3]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay1bufD4, CEIL_MULT(lpCombFilter[4]->get_D(), 256)*sizeof(float));
    cudaMalloc(&delay1bufD5, CEIL_MULT(lpCombFilter[5]->get_D(), 256)*sizeof(float));
    cudaMalloc(&outWaveDataD, sampleSize*sizeof(float));
    cudaMalloc(&envDataD, sampleSize*sizeof(float));
    cudaMemcpy(inWaveDataD, inWaveData, sampleSize*sizeof(float), cudaMemcpyHostToDevice);
    LPCombFilterGPU<<<1, 256>>>(inWaveDataD, outWaveDataD0, lpCombFilter[0]->get_g(), lpCombFilter[0]->get_D(), lpCombFilter[0]->get_lpf_g(), delay0bufD0, delay1bufD0, sampleSize);
    LPCombFilterGPU<<<1, 256>>>(inWaveDataD, outWaveDataD1, lpCombFilter[1]->get_g(), lpCombFilter[1]->get_D(), lpCombFilter[1]->get_lpf_g(), delay0bufD1, delay1bufD1, sampleSize);
    LPCombFilterGPU<<<1, 256>>>(inWaveDataD, outWaveDataD2, lpCombFilter[2]->get_g(), lpCombFilter[2]->get_D(), lpCombFilter[2]->get_lpf_g(), delay0bufD2, delay1bufD2, sampleSize);
    LPCombFilterGPU<<<1, 256>>>(inWaveDataD, outWaveDataD3, lpCombFilter[3]->get_g(), lpCombFilter[3]->get_D(), lpCombFilter[3]->get_lpf_g(), delay0bufD3, delay1bufD3, sampleSize);
    LPCombFilterGPU<<<1, 256>>>(inWaveDataD, outWaveDataD4, lpCombFilter[4]->get_g(), lpCombFilter[4]->get_D(), lpCombFilter[4]->get_lpf_g(), delay0bufD4, delay1bufD4, sampleSize);
    LPCombFilterGPU<<<1, 256>>>(inWaveDataD, outWaveDataD5, lpCombFilter[5]->get_g(), lpCombFilter[5]->get_D(), lpCombFilter[5]->get_lpf_g(), delay0bufD5, delay1bufD5, sampleSize);
    cudaDeviceSynchronize();

    cudaMalloc(&envXYD, segSize*2*sizeof(float));
    cudaMalloc(&envSegTypeD, (segSize-1)*sizeof(int));
    cudaMalloc(&delay0bufAllP, CEIL_MULT(allPassFilter->get_D(), 256)*6*sizeof(float));
    cudaMalloc(&delay1bufAllP, CEIL_MULT(allPassFilter->get_D(), 256)*6*sizeof(float));

    cudaMemcpy(envXYD, envXY, segSize*2*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(envSegTypeD, envSegType, (segSize-1)*sizeof(int), cudaMemcpyHostToDevice);

    getEnvData<<<6, 256>>>(envXYD, envSegTypeD, envDataD, segSize-1, sampleSize);
    cudaDeviceSynchronize();

    //cout<<"envXY "<<envXY[0]<<" "<<envXY[1]<<" "<<envXY[2]<<" "<<envXY[3]<<" "<<envXY[4]<<" "<<envXY[5]<<endl;
    //cout<<"envSegType "<<envSegType[0]<<" "<<envSegType[1]<<endl;
    //cout<<"segSize "<<segSize<<endl;
    
    //cudaMemcpy(envData, envDataD, sampleSize*sizeof(float), cudaMemcpyDeviceToHost);
    //std::vector<float> plot;
    //
    //for (int i = 0; i < sampleSize; i+=1000) {
    //    plot.push_back(envData[i]);
    //}
    //plotWithGnuplot(plot);

    HexAllPassFilterGPU<<<6, 256>>>(inWaveDataD, outWaveDataD0, outWaveDataD1, outWaveDataD2, outWaveDataD3, outWaveDataD4, outWaveDataD5, outWaveDataD, envDataD, allPassFilter->get_g(), allPassFilter->get_D(), delay0bufAllP, delay1bufAllP, sampleSize);

    cudaDeviceSynchronize();


    cudaMemcpy(outWave->getData(), outWaveDataD, sampleSize*sizeof(float), cudaMemcpyDeviceToHost);
    // cout<<"outwave 0 "<<(*outWave)[0]<<endl;
    // cout<<"outwave 1000 "<<(*outWave)[1000]<<endl;
    // cout<<"outwave 10000 "<<(*outWave)[10000]<<endl;
    // cout<<"outwave 100000 "<<(*outWave)[100000]<<endl;

    cudaFree(inWaveDataD);
    cudaFree(outWaveDataD0);
    cudaFree(outWaveDataD1);
    cudaFree(outWaveDataD2);
    cudaFree(outWaveDataD3);
    cudaFree(outWaveDataD4);
    cudaFree(outWaveDataD5);
    cudaFree(delay0bufD0);
    cudaFree(delay0bufD1);
    cudaFree(delay0bufD2);
    cudaFree(delay0bufD3);
    cudaFree(delay0bufD4);
    cudaFree(delay0bufD5);
    cudaFree(delay1bufD0);
    cudaFree(delay1bufD1);
    cudaFree(delay1bufD2);
    cudaFree(delay1bufD3);
    cudaFree(delay1bufD4);
    cudaFree(delay1bufD5);
    cudaFree(delay0bufAllP);
    cudaFree(delay1bufAllP);
    cudaFree(outWaveDataD);
    cudaFree(envDataD);
    cudaFree(envXYD);
    cudaFree(envSegTypeD);
    delete[] envData;
    delete[] outWaveData;
    delete[] envXY;
    delete[] envSegType;

    return outWave;
}