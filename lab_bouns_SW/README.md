# Lab bonus - Smith-Waterman (SW) algorithm Guideline

The roadmap of this lab:

1. Create project (same as lab2)
2. Implement the SW core
3. Use 01_run_core.sh to debug the design
4. Implement a Avalon master to control RS-232 (SW_Wrapper.sv) and wrap your core
5. Use 02_run_wrapper.sh to debug the wrapper design
6. Build Qsys system (same as lab2)
7. Compile and program (same as lab2)

## Implement the SW Core
> When you design an architecture, you design the dataflow first.
> Write Verilog only after you have make sure of all dataflow.

We intdoruce two very common but simple protocol dataflow.
The one wire protocol is quite simple: if "sender" set the val (valid signal)
to high, then the "sender" has prepared a valid data at that cycle.

    output val         ----> input val
    output dat1        ----> input dat1
    output [10:0] dat2 ----> input [10:0] dat2

Often the module cannot handle data at the moment, so another signal rdy (ready)
is used to stop the data transfer, which can be called as the two wire protocol.

    output val         ----> input  val
    input  rdy         <---- output rdy
    output dat1        ----> input  dat1
    output [10:0] dat2 ----> input [10:0] dat2

val (valid signal) means that "sender" want to send the data, and the sender must hold the data you want to send.
rdy (ready signal) means that "receiver" can accept the data.
If val is 0, then there is no effect whether rdy is 0 or 1 (aka don't care).
If val is 1, when "receiver" set rdy to 1 in this cycle, the "sender" may start the next transfer or
change the data in the next cycle.
On the other hand, you may assume the data hold if "receiver" set rdy to 0.

So now it's easy to understand the design for core module.

{reference seq, read seq} -> SW core module -> {alignment score, row, column}

Your mission is to implement a module that accept {reference seq, read seq} and
output highest alignment score, row and column by performing SW algorithm.

## Implement a Avalon Master to Control RS-232 and Wrap Your Core

The RS-232 module mainly has 2 functionalities.

1. Read a byte from computer
2. Write a byte from computer

Basically, your module (Avalon master) will work like this.

1. Receive 32 bytes from computer (ref seq)
2. Receive 32 bytes from computer (read seq)
3. Compute
5. Send 31 bytes result to computer (null (7 bytes), column (8 bytes), row (8 bytes), score(8 bytes))
6. Go to 1.

And you have to use Avalon protocol to actually read/write a byte from the RS-232 Qsys module.
The "read a byte" is done by the following sequence:

1. Read RX\_READY bit of the STATUS word
2. If it's 1 and the avm_waitrequest is 0, go to 3 and change addr, else go to 1.
3. Read the lower byte of the RX word

Simliarly, the "write a byte" is done by the following sequence:

1. Read TX\_READY bit of the STATUS word
2. If it's 1 and the avm_waitrequest is 0, go to 3 and change addr, else go to 1.
3. Write the lower byte of the TX word

If you understand the two wire protocol in the previous part,
then the Avalon protocol is just a variant and combination of the two wire protocol.
It's your task to read the Avalon protocol and RS-232 document for more details (available on NAS).
We also prepared a testbench for you.

## Build Qsys system
Please follow the powerpoint.

# Requirements
The requirements are:

* Connect your PC and FPGA with RS-232 cable.
* You have to install Python and serial library. Try to install that by yourself.
* Execute pc_python/test_rs232.py to perform SW algorithm and the testing data is random_pattern.bin (There will be hidden data).

## Bonus
* Design a better protocol so you don't have to reset every time.
* The design can also output the backtracking alignment results.

# Appendix
## File Structure

* src/DE2\_115
	* All files related to the FPGA
* src/cpp/
  * Software Code (c++) for SW algorithm and generating testing data
* src/pc_python/
	* Python executable test program for PC
* src/tb_verilog/
	* Verilog testbench for SW core and wrapper
* src/SW_core.sv
    * Implement Smith-Waterman algorithm here
* src/SW_Wrapper.sv
    * Implement controller for RS232 protocol
    * Including reading check bits and read/write data. 

## Run pc_python program on pc

* Recommended python version: Python2
* Usage
    * Windows: install python compiler
    * Mac/Linux: run with command line
* Command
```
python test_rs232.py [COM? | /dev/ttyS0 | /dev/ttyUSB0]
```
## Generating Testing Data
* Move to cpp/
  ```
  cd ./lab_bonus_SW/src/cpp
  ```
* Compile ./cpp/src/gen_data.cpp
  ```
  source compile_gen.sh
  ```
* Generate testing data
  ```
  source generate_testdata.sh
  ```
* There are two files being generated
  * ./test_data/random_pattern.txt
  * ./test_data/random_pattern.bin
  
## Perform SW Algorithm (c code)
* Move to cpp/
  ```
  cd ./lab_bonus_SW/src/cpp
  ```
* Compile ./cpp/src/main.cpp
  ```
  source compile_SW.sh
  ```
* Perform SW algorithm
  ```
  source run_SW.sh
  ```
* The results will be recorded in
  * ./exe_SW.log

## Testbench Usage

* Test SW_Core.sv
```
source 01_run_core.sh
```
* Test SW_Wrapper.sv 
```
source 02_run_wrapper.sh
```
