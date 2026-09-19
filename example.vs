fn fibonacci(n){
    if (n <= 2){
        return 1;
    };
    return fibonacci(n - 2) + fibonacci(n - 1);
};

let n = fibonacci(10);
print(n);