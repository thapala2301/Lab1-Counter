#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"  

int main(int argc, char **argv, char **env){
    int i;
    int clk;

    Verilated::commandArgs(argc, argv);

    Vcounter* top = new Vcounter;

    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("counter.vcd");

    if(vbdOpen()!=1) return(-1);
    vbdHeader("L1T2 Challenge");

    top->clk = 1;
    top->rst = 1;
    top->en = 0;
    top->dir = 0;  // Initialize direction

    for(i=0; i<1000; i++){
        for(clk=0; clk<2; clk++){
            tfp->dump(2*i+clk);
            top->clk = !top->clk;
            top->eval();
        }

        // Flag controls direction, counter is always enabled
        top->rst = (i < 2);                    
        top->en = 1;                           
        top->dir = vbdFlag();                  
        
        // 7-segment display
        vbdHex(4, (int(top->count) >> 16) & 0xF);
        vbdHex(3, (int(top->count) >> 8) & 0xF);
        vbdHex(2, (int(top->count) >> 4) & 0xF);
        vbdHex(1, int(top->count) & 0xF);
        
        // Real-time plotting
        vbdPlot(int(top->count), 0, 255);
        vbdCycle(i + 1);

        // Show direction status on screen
        if (top->dir) {
            vbdText("Direction: UP  ", 0, 12);
        } else {
            vbdText("Direction: DOWN", 0, 12);
        }

        if (Verilated::gotFinish()) exit(0);
    }

    vbdClose();
    tfp->close();
    exit(0);
}
