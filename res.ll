declare i32 @scanf(i8*, ...)
declare i32 @printf(i8*, ...)
declare void @exit(i32)
@.int_specifier_scan = constant [3 x i8] c"%d\00"
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"

define i32 @readi(i32) {
    %ret_val = alloca i32
    %spec_ptr = getelementptr [3 x i8], [3 x i8]* @.int_specifier_scan, i32 0, i32 0
    call i32 (i8*, ...) @scanf(i8* %spec_ptr, i32* %ret_val)
    %val = load i32, i32* %ret_val
    ret i32 %val
}

define void @printi(i32) {
    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0
    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)
    ret void
}

define void @print(i8*) {
    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0
    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)
    ret void
}

@.divZero = internal constant [23 x i8] c"Error division by zero\00"

define i32 @main(){

%a0 = alloca i32, i32 50
%a1 = add i32 0, 5
%a2 = getelementptr i32, i32* %a0, i32 0
store i32 %a1, i32* %a2
br label %label_2
label_2:
br label %label_3
label_3:
br label %label_5
label_5:
%a4 = getelementptr i32, i32* %a0, i32 0
%a3 = load i32, i32* %a4
%a5 = add i32 0, 0
%a6 = icmp sgt i32 %a3, %a5
br i1 %a6, label %label_6, label %label_7
label_6:
br label %label_7
br label %label_5
label_7:
br label %label_4
br label %label_2
label_4:
ret i32 0
}
