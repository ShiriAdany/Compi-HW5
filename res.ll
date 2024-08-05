define i32 @main(){

declare i32 @printf(i8*, ...)
declare void @exit(i32)
declare i32 @scanf(i8*, ...)
@.divZero = internal constant [23 x i8] c"Error division by zero\00"
%a2 = add i32 %a0 %a1
%a5 = mul i32 %a3 %a4
%a9 = mul i32 %a7 %a8
%a10 = sub i32 %a6 %a9
%a13=icmp eq i32 0 %a12
br i1 %a13,label %label_2 ,label %label_3
label_2:
call i32 (i8*, ...) @printf(i8* getelementptr ([23 x i8], [23 x i8]* @.divZero, i32 0, i32 0))
ret i32 0
label_3:
%a14 = sdiv i32 %a11 %a12
%a17 = add i32 %a15 %a16
%a20 = add i32 %a18 %a19
%a20 = and i32 %a20, 255
%a23 = icmp sge i32 %a21 %a22
%a26 = icmp ugt i32 %a24 %a25
%a29 = icmp slt i32 %a27 %a28
ret i32 0
}
