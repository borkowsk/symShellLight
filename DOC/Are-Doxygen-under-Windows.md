# DOXYGEN for MS Windows
## @date 2026-04-29 (created)

Doxygen is fully available for Windows and remains one of the most popular tools for generating documentation on this platform.

You can install it in several ways:

## 1. Official Installer (Recommended)

Ready-made Windows binaries can be found on the _[doxygen.nl](https://doxygen.nl/)_ website:

- __Installer__ (.exe): A standard installation file that configures system paths.

- __Portable version__ (.zip): An archive containing executable files that only need to be extracted (no installation required).

## 2. Package Managers

If you use a terminal, you can install Doxygen using popular managers:

- __Winget__: `winget install doxygen.doxygen`

- __Chocolatey__: `choco install doxygen`

## 3. Additional tools worth having on Windows:

* __Doxywizard__: A graphical user interface (GUI) included with the Windows installer. It significantly simplifies creating and editing the configuration file (Doxyfile) without the need for manual text editing.

* __Graphviz__: It is recommended to install this separately. Doxygen uses the dot tool from the Graphviz package to generate class, inheritance, and dependency diagrams. Without it, the documentation will only contain text and simple tables.

* __HTML Help Workshop__: Necessary if you plan to generate documentation in the .chm format (an older but still used Windows help format).
    
    
