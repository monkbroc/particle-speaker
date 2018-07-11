// Speaker library by Julien Vanier <jvanier@gmail.com>
//   Adaptions by ScruffR
//     - make double buffering optinal
//     - option to provide "external" buffer(s) to the library
//       which allows to play PCM data stored in flash without copying 

#include "speaker.h"

#include "stm32f2xx.h"
#include <algorithm>

// The DAC symbol from the STM32 standard library is shadowed by the DAC Particle pin name
#define _DAC                 ((DAC_TypeDef *) DAC_BASE)

Speaker::Speaker(uint16_t bufferSize, bool dblBuffer)
    : bufferSize(bufferSize),
    lastBuffer(0xFF),
    audioFrequency(8000),
    privateBuffer(true)
{
    numBuffers = dblBuffer ? 2 : 1;
    for (int i = 0; i < numBuffers; i++) {
        buffer[i] = new uint16_t[bufferSize];
        std::fill(buffer[i], &buffer[i][bufferSize], 0);
    }
    if (!dblBuffer)
      buffer[1] = buffer[0];
}

Speaker::Speaker(uint16_t *buffer, uint16_t bufferSize) 
    : Speaker(buffer, buffer, bufferSize)
{ }

Speaker::Speaker(uint16_t *buffer0, uint16_t *buffer1, uint16_t bufferSize)
    : bufferSize(bufferSize) ,
    lastBuffer(0xFF),
    audioFrequency(8000),
    privateBuffer(false)
{
    numBuffers = (buffer0 == buffer1) ? 1 : 2;
    buffer[0] = buffer0;
    buffer[1] = buffer1;
}

Speaker::~Speaker()
{
    if (privateBuffer) {
        for (int i = 0; i < numBuffers; i++) {
            delete buffer[i];
        }
    }
}

void Speaker::begin(uint16_t audioFrequency)
{
    this->audioFrequency = audioFrequency;
    setupHW(audioFrequency);
}

void Speaker::setupHW(uint16_t audioFrequency)
{
    /* TIM6 and DAC clocks enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 | RCC_APB1Periph_DAC, ENABLE);
    /* DMA clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    /* DMA1 Stream5 channel7 configuration */
    DMA_DeInit(DMA1_Stream5);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_BufferSize = bufferSize;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &_DAC->DHR12L1;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buffer[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_Channel = DMA_Channel_7; // Stream5 Channel7 = DAC1
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);

    /* Configure DMA1 Stream5 double buffering */
    DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t) buffer[1], DMA_Memory_1);
    DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE);

    /* Enable DMA1 Stream5 */
    DMA_Cmd(DMA1_Stream5, ENABLE);

    /* DAC channel1 Configuration */
    DAC_DeInit();

    DAC_InitTypeDef DAC_InitStructure;
    DAC_StructInit(&DAC_InitStructure);
    
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    /* Enable DAC Channel1 */
    DAC_Cmd(DAC_Channel_1, ENABLE);

    /* Enable DMA for DAC Channel1 */
    DAC_DMACmd(DAC_Channel_1, ENABLE);

    /* TIM6 Configuration */
    TIM_DeInit(TIM6);

    TIM_SetAutoreload(TIM6, timerAutoReloadValue(audioFrequency));

    /* TIM6 TRGO selection */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
    /* Enable TIM6 update interrupt */
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    /* Start TIM6 */
    TIM_Cmd(TIM6, ENABLE);
}

uint16_t Speaker::timerAutoReloadValue(uint16_t audioFrequency)
{
    return SystemCoreClock / 2 / audioFrequency;
}

void Speaker::end()
{
    /* Disable TIM6 update interrupt */
    TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
    /* Disable TIM6 */
    TIM_Cmd(TIM6, DISABLE);
}

uint8_t Speaker::currentBuffer()
{
   return (uint8_t) DMA_GetCurrentMemoryTarget(DMA1_Stream5);
}

bool Speaker::ready()
{
    uint8_t cur = currentBuffer();
    if (cur != lastBuffer)
    {
        lastBuffer = cur;
        return true;
    }
    else
    {
        return false;
    }
}

uint16_t *Speaker::getBuffer()
{
    /* Return alternate buffer for DMA1 Stream5 double buffering */
    return currentBuffer() ? buffer[0] : buffer[1];
}
