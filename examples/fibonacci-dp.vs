fn fibonacci(n){
    let previous = 0;
    let current = 1;

    for (let i=0;i<n;i++){
        let next = previous + current;
        previous = current;
        current = next;
    };

    return previous;
};

let result = fibonacci(10);
print(result);