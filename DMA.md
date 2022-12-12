# Xilinx DMA - PCIe

<img width="656" alt="Screen Shot 2022-12-11 at 4 17 13 PM" src="https://user-images.githubusercontent.com/42010432/206937753-437d4018-d588-4d75-80b7-d5d8e07f822b.png">

The DMA/Bridge Subsystem for PCI Express® ( PCIe® ) can be configured to be either a high-performance direct memory access (DMA) data mover or a bridge between the PCI Express and AXI memory spaces.

## DMA Data Mover
As a DMA, the core can be configured with either an AXI (memory mapped) interface or with an AXI streaming interface to allow for direct connection to RTL logic.

## Bridge Between PCIe and AXI Memory
As a PCIe Bridge, received PCIe packets are converted to AXI traffic and received AXI traffic is converted to PCIe traffic. 
