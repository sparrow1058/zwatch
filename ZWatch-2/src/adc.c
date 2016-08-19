#include "ioCC1110.h"
#include "mytypedef.h"
#define HAL_ADC_REF_125V        0x00
#define HAL_ADC_DEC_128         0x01
#define HAL_ADC_CHN_0           0x00
uchar readVoltage(void)
{
  uint value;
  ADCIF=0;
  ADCCON3=(HAL_ADC_REF_125V|HAL_ADC_DEC_128|HAL_ADC_CHN_0);
    while ( !ADCIF );
 
  //获得转换结果
  value = ADCL;
  value |= ((uint) ADCH) << 8;
}