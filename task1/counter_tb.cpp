#include "Vcounter.h" //Mandatory header files e.g. Vcounter.h for module counter
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char **argv, char **env) {
    int i; //i counts the number of clock cycles to simulate
    int clk; //clk is the module clock signal

    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    
    // init top verilog instance
    Vcounter* top = new Vcounter; //instantiate the counter module
    
    // init trace dump
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("counter.vcd"); //the last 4 lines turns on signal tracing and tells verilator to dump waveform

    // initializing simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->en = 0; //only top-level entities are visible

    // run simulation for many clock cycles
    int pause_counter = 0;
    bool control = true;

    for (i = 0; i < 300; i++) {
     
        // dump variables into VCD file and toggle clock
        if(top->count == 0x9 && control == true){
         control = false;
         pause_counter = 3;
        }

        if (control == false){
               pause_counter --;
               if (pause_counter <= 0){
                  control = true;
               }
            }
        
        for (clk = 0; clk < 2; clk++) {
            tfp->dump(2*i + clk);
            top->clk = !top->clk;
            top->eval();

            
        }

        top->rst = (i < 2) | (i == 15);
        top->en = (i > 4) && control;
        if (Verilated::gotFinish()) exit(0);
    }

    tfp->close();
    exit(0);
}

/*

TESTBENCH ANALYSIS & EXPLANATION (16/10/2025)
================================

PURPOSE:
- This C++ testbench simulates and verifies the behavior of a SystemVerilog counter module
- It drives input signals to the counter and records the resulting waveform

KEY COMPONENTS:

1. HEADER FILES:
   - Vcounter.h: Auto-generated from counter.sv by Verilator - contains our counter hardware model
   - verilated.h: Main Verilator library for simulation control
   - verilated_vcd_c.h: Library for VCD waveform dumping

2. SIMULATION SETUP:
   - Verilated::commandArgs(): Initializes Verilator simulation environment
   - Vcounter* top = new Vcounter: Instantiates the counter module as a C++ object
   - traceEverOn() & VcdC setup: Enables signal tracing for waveform generation

3. SIGNAL INITIALIZATION:
   - clk=1, rst=1, en=0: Starts with clock high, reset active, counter disabled
   - This ensures counter begins in known state (reset to zero)

4. SIMULATION LOOP STRUCTURE:
   - Outer loop (i=0 to 299): Represents 300 main simulation cycles
   - Inner loop (clk=0 to 1): Generates complete clock cycles (low→high transitions)
   - Each inner loop iteration:
     * tfp->dump(): Captures signal states for waveform
     * top->clk toggle: Creates clock edges
     * top->eval(): Updates counter logic based on current inputs

5. CONTROL LOGIC:
   - Reset (rst): Active (1) during cycles 0-1 AND at cycle 15
     Purpose: Tests reset functionality at start and mid-simulation
   - Enable (en): Active (1) after cycle 4
     Purpose: Tests that counter only increments when enabled

6. WAVEFORM GENERATION:
   - VCD (Value Change Dump) file records all signal transitions
   - File "counter.vcd" can be viewed in GTKWave for visual analysis
   - Timestamps use arbitrary units (2*i + clk) since no timescale defined

7. EXPECTED BEHAVIOR:
   - Cycles 0-1: Reset active → count = 0
   - Cycles 2-4: Reset inactive, enable inactive → count frozen at 0
   - Cycles 5-14: Enable active → count increments each cycle: 1,2,3...
   - Cycle 15: Reset pulses → count resets to 0
   - Cycles 16-299: Enable active → count increments from 0 again

USAGE:
- Run with: ./obj_dir/Vcounter
- View results: gtkwave counter.vcd
- Recompile after changes: verilator -Wall --trace --cc counter.sv --exe counter_tb.cpp

*/