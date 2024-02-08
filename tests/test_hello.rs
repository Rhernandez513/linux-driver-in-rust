// tests/test_hello.rs
#[path = "../app/hello.rs"]
mod app;

// Unit tests
#[test]
fn test_hello_world() {
    // Redirect standard output to capture println!
    // let output: Vec<u8> = Vec::new();
    // let mut output: Vec<u8> = Vec::new();
    std::io::stdout().write_all(b"").unwrap();
    std::io::stdout().flush().unwrap();
    let result = std::panic::catch_unwind(|| {
        // Run the hello function under test
        hello();
    });

    // Restore standard output
    std::io::stdout().flush().unwrap();

    // Check if the main function executed without panicking
    assert!(result.is_ok());

    // Uncomment the following line if you want to check the printed output
    assert_eq!(String::from_utf8_lossy(&output), "Hello, World!\n");
}

// suppress warning as there is no implementation yet,
// This code just demonstrates how to create a module and a function
// with a unit test
#[allow(dead_code)]
fn main() {
}