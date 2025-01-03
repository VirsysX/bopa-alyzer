# Website Technology Identifier

A command-line tool written in C++ to identify website technologies. It analyzes the HTML content, HTTP headers, and checks for the presence of common files to detect various web technologies used on a website.

## Key Features

* **HTML Content Analysis:** Uses regular expressions to detect technology patterns within the HTML source code.
* **HTTP Header Analysis:** Inspects HTTP response headers for clues about server software, backend frameworks, and other technologies.
* **File Existence Checks:**  Verifies the presence of common files or directories associated with specific technologies (e.g., `wp-login.php` for WordPress).
* **Extensible:** Easily add new technologies and detection patterns by modifying the source code.

## Prerequisites

* **C++ Compiler:** (e.g., g++)
* **libcurl:**  You need to have the `libcurl` development library installed on your system.

    * **For Debian/Ubuntu:** `sudo apt-get update && sudo apt-get install libcurl4-openssl-dev`
    * **For Fedora/CentOS:** `sudo dnf install libcurl-devel` or `sudo yum install libcurl-devel`
    * **For macOS (using Homebrew):** `brew install curl`

## Installation (Compilation)

1. Save the C++ code (e.g., as `bopa-alyzer.cpp`).
2. Open your terminal or command prompt.
3. Navigate to the directory where you saved the file.
4. Compile the code using a C++ compiler like g++:

   ```bash
   g++ bopa-alyzer.cpp -o bopa-alyzer -lcurl -std=c++17
   ```

   * Make sure you have `libcurl` installed correctly, otherwise the compilation will fail. You might need to adjust `-std=c++17` based on your compiler's support.

## Usage

To run the tool, use the following command in your terminal:

```bash
./bopa-alyzer <URL>
```

Replace `<URL>` with the website address you want to analyze (e.g., `https://www.example.com`).

**Example:**

```bash
./bopa-alyzer https://www.wordpress.org
```

**Output:**

The tool will output information about the fetched website content and any detected technologies. For example:

```
Website content fetched successfully (HTTP 200).
Detected technologies for https://www.wordpress.org:
- WordPress (found in HTML)
- PHP (found in HTML)
- Nginx (found in headers - Server)
```

If no specific technologies are detected based on the defined patterns, it will output:

```
Website content fetched successfully (HTTP 200).
No specific technologies detected based on current patterns.
```

## Configuration

Currently, the technology detection patterns are defined within the `identifyTechnologies` function in the source code. To add or modify technology detections, you need to:

1. Open the `bopa-alyzer.cpp` file.
2. Locate the `identifyTechnologies` function.
3. Add or modify entries in the `technologies` vector. Each entry defines a technology name and the regular expressions/methods used to detect it.
4. Recompile the code after making changes.

## Contributing

Contributions are welcome! If you want to add support for detecting more technologies or improve the existing detection logic, feel free to:

* **Submit Pull Requests:**  Propose changes to the code with new technology patterns or improvements.
* **Open Issues:** Report bugs or suggest new features.

When contributing, please follow these guidelines:

* Ensure your code adheres to good C++ practices.
* Provide clear and concise commit messages.
* Explain the purpose of your changes in your pull request.

## Disclaimer

This tool relies on pattern matching and heuristics for technology detection. It may not always be accurate and could produce false positives or false negatives. The accuracy depends on the comprehensiveness of the defined detection patterns.

## License

[Your License Here (e.g., MIT License)]
