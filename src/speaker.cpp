// Speaker library by Julien Vanier <jvanier@gmail.com>

#include "speaker.h"

#include "stm32f2xx.h"

// Constructor
Speaker::Speaker(uint16_t bufferSize)
    : bufferSize(bufferSize)
{
    buffer0 = new uint16_t[bufferSize];
    buffer1 = new uint16_t[bufferSize];
}

Speaker::~Speaker()
{
    delete[] buffer0;
    delete[] buffer1;
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

    /* DAC channel1 Configuration */
    DAC_DeInit();

    DAC_InitTypeDef DAC_InitStructure;
    DAC_StructInit(&DAC_InitStructure);
    
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;

    /* TIM6 Configuration */
    TIM_DeInit(TIM6);

    TIM_SetAutoreload(TIM6, timerAutoReloadValue(audioFrequency));

    /* TIM6 TRGO selection */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    /* Enable TIM6 update interrupt */
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    /* DMA clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    
}

uint16_t Speaker::timerAutoReloadValue(uint16_t audioFrequency)
{
    return SystemCoreClock / audioFrequency;
}

void Speaker::end()
{

}

bool Speaker::ready()
{
    return true;
}

uint16_t *Speaker::getBuffer()
{
    return buffer0;
}
