# Setup

[TOC]

## Installation


### Required tools
The following tools are necessary to build the project

- [Python 3](#python-3)
- [The ARM GNU Toolchain](#arm-gnu-toolchain)
- [JLink](#jlink)
- [Git](#git)

#### Python 3

Python 3 is used to run the waf script that controls the configuration and build process of the project

Install the latest version of python 3 for your operating system from [Python.org](https://www.python.org/downloads/). If you have an older version of python 3 that will probably work fine.
It is recommended to select the option to add python to the path if available during the install.

#### ARM GNU Toolchain

The GNU arm toolchain contains the compiler and other tools necessary to build the project. To install

1. Go to the page for the [ARM GNU Toolchain Download](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
2. Download the version 13.3.Rel1 of the **ARM-none-eabi** toolchain for your operating system.
    1. For windows this is the .exe file.
    2. For macOS this is the .pkg file (make sure to get the correct one for if you are running an x86 or Apple Silicon Mac).
    3. For debian based linux you can run `sudo apt install gcc-arm-non-eabi` and skip downloading from the website.
3. Run the installer. On windows when prompted at the end deselect all boxes except for `Add to path`

#### JLink

Segger's JLink tools are necessary to program and debug the board.

1. Go to the page for the [J-Link Downloads](https://www.segger.com/downloads/jlink/)
2. Download the latest version of J-Link for your operating system.
3. Run the installer
4. When prompted to choose a destination select `Update existing install` even if don't have J-Link already installed.

#### Git

Git is the version control platform used to keep track of the project files. Follow the installation instructions for git from the [git-scm](https://git-scm.com/downloads) for your operating system.

##### Windows Specific Instructions

1. Download the latest version of git for windows from the [git-scm's download page](https://git-scm.com/download/win).
2. Run the installer
3. When prompted to select an editor for git to use, unless you are already familiar with Vim, it is **highly recommended** to switch to an editor you are familiar and comfortable with.
4. It is recommended to install the [Windows Terminal App](https://apps.microsoft.com/detail/9n0dx20hk701) from the Microsoft Store
