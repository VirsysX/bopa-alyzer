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

1. Save the C++ code (e.g., as `tech_identifier.cpp`).
2. Open your terminal or command prompt.
3. Navigate to the directory where you saved the file.
4. Compile the code using a C++ compiler like g++:

   ```bash
   g++ tech_identifier.cpp -o tech_identifier -lcurl -std=c++17
