#include "Vcounter.h" //Mandatory header files e.g. Vcounter.h for module counter
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"

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

    //init Vbuddy
    if (vbdOpen()!=1) return (-1);
    vbdHeader("Quick Counter");
    // initializing simulation inputs
    top->clk = 1;
    top->rst = 1;
    top->en = 0; //only top-level entities are visible

    // run simulation for many clock cycles
    for (i = 0; i < 1000; i++) {
     
        // dump variables into VCD file and toggle clock
        for (clk = 0; clk < 2; clk++) {
            tfp->dump(2*i + clk);
            top->clk = !top->clk;
            top->eval();
        }

        vbdPlot(int(top->count), 0, 255);
        vbdCycle(i+1);


        top->rst = (i < 2) | (i == 15);
        top->en = vbdFlag();
        if (Verilated::gotFinish()) exit(0);
    }

    vbdClose();
    tfp->close();
    exit(0);
}

/*
 * SWITCHED FROM 7-SEGMENT TO TFT PLOTTING:
 * - Replaced vbdHex() displays with vbdPlot() for real-time graphing
 * - Increased simulation cycles from 300 to 1000 for longer, smoother plots  
 * - Added vbdFlag() button control to start/stop counting interactively
 * - Now shows counter values as a continuous waveform on TFT screen instead of individual digits
 * - Much faster update rate allows for visualizing the complete counter cycle pattern
 */