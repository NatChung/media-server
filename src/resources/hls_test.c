
const char* OUTPUT_PATH = "/home/ubuntu/s3/doorbell/test/out.aac";
const char* INPUT_PATH = "/home/ubuntu/libraries/doorbell-gateway/src/media-server/native/src/music.mulaw";

void test2(){
    initFaacHandler(&gMpegTsHandler, 8000, 1, 16);
    FILE* fpIn = fopen(INPUT_PATH, "rb");
    FILE* fpOut = fopen(OUTPUT_PATH, "wb");
    char ulawData[800];

    while(1){
        int readByte = fread(ulawData, 1, 800, fpIn);
        if(readByte < 800) break;
        mpegTsInputUlaw(ulawData, 800, fpOut);
    }

    fclose(fpIn);
    fclose(fpOut);
    printf("\n Convert done\n");
}


void test(){
    ULONG nSampleRate = 8000;  // 采样率
    UINT nChannels = 1;         // 声道数
    UINT nPCMBitSize = 16;      // 单样本位数
    ULONG nInputSamples = 0;
    ULONG nMaxOutputBytes = 0;

    int nRet;
    faacEncHandle hEncoder;
    faacEncConfigurationPtr pConfiguration; 

    int nBytesRead;
    int nPCMBufferSize;
    int nULAWBufferSize;
    BYTE* pbULAWBuffer;
    BYTE* pbPCMBuffer;
    BYTE* pbAACBuffer;

    FILE* fpIn; // PCM file for input
    FILE* fpOut; // AAC file for output

    fpIn = fopen(INPUT_PATH, "rb");
    if(fpIn == NULL){
        fprintf(stderr," Open %s failed\n", INPUT_PATH);
        exit(-1);
    }

    fpOut = fopen(OUTPUT_PATH, "wb");
    if(fpOut == NULL){
        fprintf(stderr," Open %s error\n", OUTPUT_PATH);
        exit(-1);
    }

    // (1) Open FAAC engine
    hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);
    if(hEncoder == NULL){
        printf("[ERROR] Failed to call faacEncOpen()\n");
        exit(-1);
    }

    printf(" nInputSamples = %d\n", nInputSamples);

    nPCMBufferSize = nInputSamples * nPCMBitSize / 8;
    nULAWBufferSize = nPCMBufferSize /2;
    pbPCMBuffer = (BYTE *)malloc(nPCMBufferSize);//new BYTE [nPCMBufferSize];
    pbULAWBuffer = (BYTE *)malloc(nULAWBufferSize);
    pbAACBuffer = (BYTE *)malloc(nMaxOutputBytes);//new BYTE [nMaxOutputBytes];

    // (2.1) Get current encoding configuration
    pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
    pConfiguration->inputFormat = FAAC_INPUT_16BIT;

    // (2.2) Set encoding configuration
    nRet = faacEncSetConfiguration(hEncoder, pConfiguration);
    for(int i = 0; 1; i++){
        int readByte = fread(pbULAWBuffer, 1, nULAWBufferSize, fpIn);
        nBytesRead = 0;
        for(int n=0;n<readByte;n++){
            int16_t data = MuLaw_Decode(pbULAWBuffer[n]);
            memcpy(pbPCMBuffer+nBytesRead, &data, 2);
            nBytesRead += 2;
        }

        // // 输入样本数，用实际读入字节数计算，一般只有读到文件尾时才不是nPCMBufferSize/(nPCMBitSize/8);
        nInputSamples = nBytesRead / (nPCMBitSize / 8);
        printf("nInputSamples = %d\n", nInputSamples);

        // // (3) Encode
        nRet = faacEncEncode(hEncoder, (int*) pbPCMBuffer, nInputSamples, pbAACBuffer, nMaxOutputBytes);
        fwrite(pbAACBuffer, 1, nRet, fpOut);
        printf("%d: faacEncEncode returns %d\n", i, nRet);
        if(nBytesRead <= 0) break;
    }

    // (4) Close FAAC engine
    nRet = faacEncClose(hEncoder);

    free(pbPCMBuffer);
    free(pbAACBuffer);
    fclose(fpIn);
    fclose(fpOut);

    printf("\n Convert done\n");

    return 0;

}