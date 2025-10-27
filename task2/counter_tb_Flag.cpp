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
    for (i = 0; i < 300; i++) {
     
        // dump variables into VCD file and toggle clock
        for (clk = 0; clk < 2; clk++) {
            tfp->dump(2*i + clk);
            top->clk = !top->clk;
            top->eval();
        }

        vbdHex(4, (int(top->count) >> 16) & 0xF);
        vbdHex(3, (int(top->count) >> 8) & 0xF);
        vbdHex(2, (int(top->count) >> 4) & 0xF);
        vbdHex(1, int(top->count) & 0xF);
        vbdCycle(i+1);


        top->rst = (i < 2) | (i == 15);
        top->en = vbdFlag(); //Vbuddy’s rotary encode has a push-button switch. Vbuddy keeps an internal flag which, by default, will toggle between ‘0’ and ‘1’ each time the button is pressed. You can interrogate this toggle switch state with vbdFlag();, which will return its current state and then toggle. A little postbox showing the flag state is drawn in the footer of the TFT display.
        if (Verilated::gotFinish()) exit(0);
    }

    vbdClose();
    tfp->close();
    exit(0);
}