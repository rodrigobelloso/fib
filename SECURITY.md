# Security Policy

## Supported Versions

We take security seriously and provide security updates for the following versions of fib:

| Version | Supported          |
| ------- | ------------------ |
| 0.1.x   | :white_check_mark: |
| < 0.1.0 | :x:                |

## Security Considerations

### Input Validation

The fib calculator accepts numerical inputs for Fibonacci sequence calculations. While the program uses the GMP library for arbitrary-precision arithmetic, users should be aware of the following:

- **Input Range**: Extremely large input values may result in very long computation times and high memory usage
- **Resource Limits**: Consider system memory constraints when calculating very large Fibonacci numbers
- **Input Sanitization**: The program validates input to ensure it's a valid positive integer

### Memory Safety

This project is written in C and uses the GMP library. We take the following precautions:

- **Memory Management**: All GMP variables are properly initialized and cleared to prevent memory leaks
- **Buffer Safety**: The code avoids unsafe string operations and buffer overflows
- **Sanitizer Testing**: Multiple sanitizers are available for testing (AddressSanitizer, MemorySanitizer, UndefinedBehaviorSanitizer, ThreadSanitizer)

To build and test with sanitizers:

```sh
make asan    # Address Sanitizer
make ubsan   # Undefined Behavior Sanitizer
make msan    # Memory Sanitizer
make tsan    # Thread Sanitizer
```

### Dependencies

This project depends on:

- **GMP (GNU Multiple Precision Arithmetic Library)**: A well-established, widely-audited library for arbitrary-precision arithmetic
- **Standard C Library**: C99 or higher compatible compiler

We recommend keeping your GMP library updated to the latest stable version to ensure you have all security patches.

### Static Analysis

The project uses:

- **CodeQL**: Automated security vulnerability scanning via GitHub Actions
- **Compiler Warnings**: Strict compilation flags to catch potential issues early
- **Lint Scripts**: Available in `tests/lint.sh` for code quality checks

## Reporting a Vulnerability

We appreciate the security community's efforts to responsibly disclose vulnerabilities. If you discover a security issue, please follow these steps:

### How to Report

1. **Do NOT** create a public GitHub issue for security vulnerabilities
2. Send an email to the repository maintainer (available on the GitHub profile of @rodrigobelloso)
3. Include the following information:
   - Description of the vulnerability
   - Steps to reproduce the issue
   - Potential impact
   - Suggested fix (if available)
   - Your contact information for follow-up questions

### What to Expect

- **Acknowledgment**: We will acknowledge receipt of your report within 48 hours
- **Initial Assessment**: We will provide an initial assessment within 5 business days
- **Updates**: We will keep you informed of our progress at least every 7 days
- **Resolution**: We aim to release a fix within 30 days for critical vulnerabilities
- **Credit**: With your permission, we will credit you in the security advisory and release notes

### Disclosure Policy

- We request that you give us reasonable time to address the issue before public disclosure
- We will coordinate with you on the disclosure timeline
- Once a fix is released, we will publish a security advisory on GitHub
- We may request a CVE identifier for significant vulnerabilities

## Security Best Practices for Users

When using fib, consider the following best practices:

1. **System Resources**: Monitor system memory when calculating very large Fibonacci numbers
2. **Input Validation**: Validate inputs in any scripts or applications that call fib
3. **Build from Source**: When possible, build from source and verify the integrity of the code
4. **Keep Updated**: Use the latest version to benefit from security patches and improvements
5. **Test Thoroughly**: Use the provided test suite before deploying in production environments:
   ```sh
   make test
   make test-full
   ```

## Security Testing

The project includes several testing mechanisms:

- **Unit Tests**: Basic functionality tests (`make test`)
- **Comprehensive Tests**: Full test suite (`make test-full`)
- **Sanitizer Builds**: Multiple sanitizer configurations for detecting memory issues
- **CI/CD Pipeline**: Automated testing via GitHub Actions on every commit

## Code Auditing

We welcome security audits of our codebase. The project structure is straightforward:

- `fib.c` - Main program entry point
- `algorithms.c` - Fibonacci calculation implementations
- `matrix.c` - Matrix multiplication for matrix-based algorithm
- `ui.c` - User interface handling
- `utils.c` - Utility functions

All source code is available under the GPL-3.0 license for review.

## Additional Resources

- [GMP Security Information](https://gmplib.org/)
- [CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)
- [CWE - Common Weakness Enumeration](https://cwe.mitre.org/)

## Contact

For security-related inquiries, please contact the repository maintainer through GitHub.

---

Last Updated: November 2, 2025
