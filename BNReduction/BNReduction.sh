#!/bin/bash

inputfile="$1" 

./BuildAndNot/BuildAndNot < "$inputfile" | ./NetReductionBoost/NetReduction > "$inputfile".red;
./ToPolynomial/ToPolynomial < "$inputfile".red > "$inputfile".poly;

echo    ' filename="'"$inputfile".poly'"; file=openOutAppend("'"$inputfile".red'"); 
file<< "\n\nFIXED_POINTS_REDUCED\n";
F=lines(get(filename)); if #F==0 then (file<< "1 \n\n"; close(file); exit);
needsPackage "FPGB"; QR=makeRing(#F,2); 
F=apply(F,f->(value f)_QR); 
solutions = modifyOutput(T3(F,QR,toList(#F:1)), QR); 
file<<#solutions<<"\n";
scan(solutions,   ss->(s:="";scan(ss,si->(s=s|toString(si)|" "));file<<s; file<<"\n";));  
close(file);  '    |    M2 > M2.log 

./MapFixedPoints/MapFixedPoints  < "$inputfile".red > "$inputfile".fp 

rm "$inputfile".poly;
