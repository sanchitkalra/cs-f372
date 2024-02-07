console.log("Hello");

let promise = new Promise((resolve, reject) => {
  setTimeout(() => {
    console.log("Inside promise");
  }, 5000);
});

console.log("After promise");
