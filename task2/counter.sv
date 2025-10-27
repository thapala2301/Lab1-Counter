module counter #(
    parameter WIDTH = 8
)(
    input logic clk,
    input logic rst,
    input logic en,
    input logic dir,    // NEW: 1 = up, 0 = down (only when en=1)
    output logic [WIDTH-1:0] count
);

always_ff @ (posedge clk) 
if (rst) count <= {WIDTH{1'b0}};
else if (en) count <= count + {{WIDTH-1{1'b0}}, dir ? 1'b1 : 1'b0};

endmodule
