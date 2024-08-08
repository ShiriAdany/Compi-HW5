define i32 @main(){

declare i32 @printf(i8*, ...)
declare void @exit(i32)
declare i32 @scanf(i8*, ...)
@.divZero = internal constant [23 x i8] c"Error division by zero\00"
%a0 = alloca i32, i32 50
%a1 = add i32 0, 90
%a1 = zext i8 %a1 to i32
%a2 = getelementptr i32, i32* %a0, i32 0
store i32 %a1, i32* %a2
ret i32 0
}
