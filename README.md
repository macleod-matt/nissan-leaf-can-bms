# Nissan Leaf  BMS Decoder using RP2040 Processor  

Author: Matt MacLeod

Last Updated: 2023/02/22, working api for configurable sensor drivers

********

## Overview
********
The following project is inteded to decode the Nissan Leaf BMS CAN bus messages inorder to get an idea of the condition of the battery packs. In addtion to being a plug in to monitor the condition of the packs remotley using Aretas Software as they degrade. 

********
## What about active CAN-polling?
********
Actively asking the different control units for info is another thing. The database files here won't help you (those are for passive listening), but here is a list of the different control units query and response IDs. Please note that the availability of the control modules varies depending on model year (Only ZE0 has 'Shift' module for instance)

| Control Unit  |    ID Query   |  ID Response  |
| ------------- | ------------- | ------------- |
|   Consult3+   |     0x7D2     |               |
|      VCM      |     0x797     |     0x79A     |
|      BCM      |     0x745     |     0x765     |
|      ABS      |     0x740     |     0x760     |
|   LBC(BMS)    |     0x79B     |     0x7BB     |
|  INVERTER/MC  |     0x784     |     0x78C     |
|  M&A (Meter)  |     0x743     |     0x763     |
|     HVAC      |     0x744     |     0x764     |
|     BRAKE     |     0x70E     |               |
|      VSP      |     0x73F     |     0x761     |
|      EPS      |     0x742     |               |
|      TCU      |     0x746     |               |
|   Multi AV    |     0x747     |               |
|   IPDM E/R    |     0x74D     |               |
|    AIRBAG     |     0x752     |               |
|    CHARGER    |     0x792     |     0x793     |
|     SHIFT     |     0x79D     |     0x7BD     |
|      AVM      |     0x7B7     |               |

List on ZE1 (2018+) CAN polling: https://drive.google.com/file/d/1jH9cgm5v23qnqVnmZN3p4TvdaokWKPjM/view

