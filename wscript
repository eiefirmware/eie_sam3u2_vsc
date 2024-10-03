# This is the main build script used by waf. Many details of how waf works are in the waf-book
# https://waf.io/book/
# It provides a good introduction to most of what's going on here.

# This script uses a few of the extra tools not built into waf by default. If you are updating the
# checked-in version of waf you will need to tell waf-light to include the following extra tools:
#
# $ ./waf-light configure build --tools=clang_compilation_database,color_gcc,gccdeps
#
# Also consider adding the --nostrip option if working on the wscript, to generate a waf pack
# that includes comments (or set your WAFDIR variable, see the book). The version you check in
# should be stripped though to keep size down.

# Onto the actual build script! It's a bit more involved than most waf examples you can find due
# to the nature of cross-compiling for bare-metal targets.

# First import a few things we will need from python's stdlib and Waf's core library.
import os
import pathlib
import re
import subprocess

import waflib.Configure as ConfMod
from waflib.Configure import conf
from waflib.Node import Node
from waflib.Task import Task
from waflib.TaskGen import after, feature
import waflib.Utils as Utils

# This will make waf automatically re-run configuration if the wscript is modified.
# Useful for those not familiar with waf or the config-build-install cycle.
# The biggest boon is that if configure hasn't run at all it will auto-run during a build command.
ConfMod.autoconfig = True

# TODO: Remaining tasks
# - Move non-command functinos to tool, or make private so that they stop polluting the help text.

# The following functions implement waf commands. You can define arbitrary commands here (see the
# waf book) but we only use standard ones. If you don't specify one waf uses "build" by default,
# and a few are always run automatically.


# The "options" command is also always implicitly run. It's generally used to add command line
# arguments for use in other steps, and many tools will require loading here to add their own
# arguments. See the waf-book for how to define args. Arge defined here will show up when you
# run waf with the "--help" option.
def options(ctx):
    # This tool provides some nice coloring to highlight errors and warnings in the terminal.
    ctx.load("color_gcc")

    cfg_gr = ctx.get_option_group("Configuration options")

    # Add an option to set the board type being built for.
    cfg_gr.add_option(
        "--board",
        type="string",
        help="Set the board type to build for. Valid options are 'ASCII' or 'DOT_MATRIX'.",
        default="",
        action="store",
        dest="board",
    )

    gr = ctx.get_option_group("Build and installation options")

    # Add an option to install the final program to an attached devboard.
    gr.add_option(
        "-F",
        "--flash",
        action="store_true",
        help="Write compiled firmware to devboard",
    )


# The "configure" command is the first command we are overriding which isn't run by default. It's
# usual role is to inspect the host system and find all the programs/libraries that will be needed
# to do builds. In general though it can be used to do any slow checks you don't want to run
# every time. Anything it stores in ctx.env will be available in all future build commands.
def configure(ctx):
    # Waf uses DEST_OS for some magic behaviour. If you don't set it manually it assumes you are
    # trying to target the host computer that waf is running on, which will do things we don't want.
    ctx.env.DEST_OS = "bare-metal"

    gcc_srch = get_gcc_srch_path()

    # The gcc tool assumes that the compiler name is just "gcc". This is not the case when cross
    # compiling so we need to manually find the right program. The gcc tool will re-use whatever
    # is stored in "CC", which find_program will do for us.
    ctx.find_program(
        "arm-none-eabi-gcc",
        var="CC",
        path_list=gcc_srch,
        msg="Checking for cross-gcc",
    )

    # GCC can be the assembler as well.
    ctx.env.AS = ctx.env.CC

    # Do the same for "AR" (linker for static libs, we don't actually use it but the c tool pulls it
    # in automatically).
    ctx.find_program(
        "arm-none-eabi-ar", var="AR", path_list=gcc_srch, msg="Checking for cross-ar"
    )

    # Need objcopy for converting to a hex file.
    ctx.find_program(
        "arm-none-eabi-objcopy",
        var="OBJCOPY",
        path_list=gcc_srch,
        msg="Checking for cross-objcopy",
    )

    # Now we can load the gcc compiler tool, this is how waf knows what to do with c files and how
    # to link the program.
    ctx.load("gcc gas")

    # There's a few additional tools that are nice to have when compiling c code.
    # gccdeps uses gcc itself to tell us what a file depends on, so waf can be a lot smarter about
    # only recompiling the files that actually need to be recompiled.
    ctx.load("gccdeps")
    # Ensure the pre-processor runs on assembly files. Without this gccdeps doesn't work right
    # (and also some assembly becomes a pain to write).
    ctx.env.append_value("ASFLAGS", "-xassembler-with-cpp")

    # This tool doesn't actually use clang, but it generates a .json file with the exact command
    # line used for each .c file. This is nice because vscode understands this format and can use
    # it to provide accurate intellisense/go-to-definition operations.
    ctx.load("clang_compilation_database")

    # Segger doesn't like to be consistent, need to use a different name based on platform.
    jlink_name = "JLinkExe"
    jlink_ext = ""
    if Utils.is_win32:
        jlink_name = "Jlink"
        jlink_ext = ".exe"

    # Find the Jlink tools. Not needed for much, just if you want to flash the firmware through waf
    # directly.
    ctx.find_program(
        jlink_name,
        var="JLINK",
        path_list=get_jlink_srch_path(f"{jlink_name}{jlink_ext}"),
    )

    # Set the board type based on the command line option.
    ctx.set_board()


@conf
def set_board(ctx):
    """
    Set the board type to build for.
    """

    board = ctx.options.board.lower()

    # Save the board type in the environment variables so the --board
    # option doesn't need to be specified every time.
    if board == "ascii":
        ctx.env.BOARD = "ASCII"
    elif board == "dot_matrix":
        ctx.env.BOARD = "DOT_MATRIX"
    else:
        ctx.fatal(f"Invalid board type '{board}' specified.")


# The "build" command is the default one run by waf if you don't specify anthing.
#
# An important thing to keep in mind about waf's build process is that this function itself doesn't
# actually build-anything. It just creates task-generators, which are fancy collections of variables
# used by tools to generate the actual tasks run later (eg. the c tool uses the "source" variable
# to create a separate compilation task for each .c file you specify). Generally the tools will
# document what variables they expect.
def build(ctx):
    # First setup all the special flags we need to pass to the tools.
    # TODO: A lot of this isn't specific to our app, and could be factored out into a general tool
    # for arm cross-compilation.

    # Compiler
    cflags = [
        # Tell GCC to use newlib-nano
        "--specs=nano.specs",
        # Tell GCC what processor to build for.
        "-mcpu=cortex-m3",
        # Our processor only supports thumb2, so target it by default.
        "-mthumb",
        "-mno-thumb-interwork",
        "-masm-syntax-unified",
        # This processor does not do anything special on arithmetic overflow.
        # Make sure GCC knows to avoid optimizations that assume that.
        "-fno-strict-overflow",
        # We are using C99 as the target C standard.
        "-std=c99",
        # Tell GCC to generate all debug info
        "-ggdb",
        # Set optimizations for best debugging.
        "-Og",
        # Allow "asm" keyword, old CMSIS headers use it.
        "-fasm",
        # Warning config. Turn on most, but disable a few that are commonly triggered
        # by the used code style.
        "-Wall",
        "-Wno-unused-function",
        "-Wno-pointer-sign",
    ]

    # Assembler:
    asflags = [
        # Most of these are just duplicating cflags
        "--specs=nano.specs",
        "-mcpu=cortex-m3",
        "-mthumb",
        "-masm-syntax-unified",
    ]

    # Linker
    linkflags = [
        # Tell GCC to use newlib-nano
        "--specs=nano.specs",
        # We are using our own custom startup code.
        "-nostartfiles",
        # Target specifics, see cflags.
        "-mcpu=cortex-m3",
        "-mno-thumb-interwork",
    ]

    # Now collect all the things we want to build.

    # To avoid manual lists of all the files, use some globbing to pick up everything of interest
    # As long as you organize things that should turn on/off as a unit into folders this will
    # work.
    work_folders = [
        "firmware_common/cmsis",
        "firmware_common/bsp",
        "firmware_common/drivers",
        "firmware_common/application",
    ]

    # Files specific to the ascii and dot matrix boards.
    ascii_folders = [
        "firmware_ascii/bsp",
        "firmware_ascii/drivers",
    ]
    dot_matrix_folders = [
        "firmware_dotmatrix/bsp/",
        "firmware_dotmatrix/drivers",
        "firmware_dotmatrix/libraries/captouch",
        "firmware_dotmatrix/libraries/captouch/include",
    ]

    # Add blade files directly to the source to avoid compiling the template files
    source = [
        "firmware_common/application/blade/blade_api.c",
        "firmware_common/application/blade/blade_imu_lsm6dsl.c",
    ]
    includes = ["firmware_common/application/blade"]
    defines = []
    target = ""

    if ctx.env.BOARD == "ASCII":
        work_folders += ascii_folders
        defines.append("EIE_ASCII")
        target = "firmware-ascii"
    elif ctx.env.BOARD == "DOT_MATRIX":
        work_folders += dot_matrix_folders
        defines += ["EIE_DOTMATRIX", "EIE_NO_CAPTOUCH"]
        target = "firmware-dot-matrix"
    else:
        ctx.fatal("No board type specified.")

    for folder in work_folders:
        source += ctx.srcnode.ant_glob(f"{folder}/*.s")  # assembly files
        source += ctx.srcnode.ant_glob(f"{folder}/*.c")  # C source
        includes.append(folder)  # Make sure the matching headers can be found.

    # The program() function creates a task gen with all the features needed to compile+link based
    # on what source files you specify (stepping through with a python debugger can be nice to
    # understand exactly how it does that :) )
    #
    # If you have a need to you can actually call program() multiple times to build multiple
    # executable files.
    ctx.program(
        target=target,
        features="mapfile ihex",
        source=source,
        includes=includes,
        cflags=cflags,
        asflags=asflags,
        linkflags=linkflags,
        defines=defines,
        linker_script="firmware_common/bsp/sam3u2.ld",
    )

    if ctx.options.flash:

        def do_flash(ctx):
            # Input we would type interactively.
            input = "\n".join(
                [
                    "erase",
                    f"loadfile build/{target}.hex",
                    "reset",
                    "go",
                    "quit",
                ]
            ).encode()

            # Actually do the command.
            ctx.exec_command(
                [
                    ctx.env.JLINK[0],
                    "-exitonerror",
                    "1",
                    "-autoconnect",
                    "1",
                    "-device",
                    "ATSAM3U2C",
                    "-if",
                    "SWD",
                    "-speed",
                    "4000",
                ],
                input=input,
                timeout=10,
            )

        # Use a post-build function here for simplicity. Could also be done as a task, but getting
        # the command line right would be a pain.
        ctx.add_post_fun(do_flash)


# The rest of these functions are supporting items used during the configure/build commands.


def check_jlink_ver(pth: pathlib.Path) -> None | tuple[str, str, str]:
    """
    Query the version number from a specific jlink commander executable.

    Returns as tuple of (major, minor, patch). For example 7.98i becomes (7, 98, i).

    If the version could not be determined then None is returned.
    """
    if not pth.exists():
        return None

    try:
        res = subprocess.run(
            [pth, "-AutoConnect", "0", "-NoGui", "1"],
            text=True,
            check=True,
            capture_output=True,
            input="exit\n",
        )
    except:
        return None

    ver_match = re.search(r"version V(\d+)\.(\d+)([a-z])", res.stdout)

    if ver_match:
        return (ver_match[1], ver_match[2], ver_match[3])

    return None


def get_jlink_srch_path(exe_name: str):
    """
    Get search path to use for JLink programs/DLLs
    """

    paths = os.environ.get("PATH").split(os.pathsep)

    # JLink installer gives the option to update the "current instance", which
    # is installed under the "Jlink" subfolder. Otherwise it installs a version
    # specific folder.

    install_roots = []
    jlink_installs = []

    if Utils.is_win32:

        install_roots = [
            "C:\\Program Files\\SEGGER\\",
            "C:\\Program Files (x86)\\SEGGER\\",
            "D:\\SEGGER\\",
        ]

    elif Utils.unversioned_sys_platform() == "darwin":
        install_roots = ["/Application/SEGGER/JLink"]

    elif Utils.unversioned_sys_platform() == "linux":
        install_roots = ["/opt/SEGGER/JLink"]

    for root_s in install_roots:
        root = pathlib.Path(root_s)
        if not root.exists():
            continue

        for pth in root.iterdir():
            ver = check_jlink_ver(pth / exe_name)
            if ver:
                jlink_installs.append((ver, str(pth)))

    # Sorting tuples goes from left to right. So this will source by version and then by path
    # name.
    jlink_paths = [pth for (_, pth) in sorted(jlink_installs, reverse=True)]
    return jlink_paths + paths


def check_gcc_ver(pth: pathlib.Path, ext=""):
    """
    Attempt to extract the version from a specific GCC.
    If successful returned as a tuple (maj, min, rel).
    Otherwise returns None
    """

    name = "arm-none-eabi-gcc"
    if ext:
        name += f".{ext}"

    exe_pth = pth / name
    if not exe_pth.exists():
        return None

    try:
        ver = subprocess.run(
            [exe_pth, "-dumpversion"], stdout=subprocess.PIPE
        ).stdout.decode()
    except:
        return None

    return tuple(int(v) for v in ver.split("."))


def get_gcc_srch_path_win32():
    """
    Look for typical GCC installations. Checked on 11.3 rel1 and 12.3 rel1.
    If your gcc is older this may need to be modified (or just make sure it's on the path when you
    run configure).
    """

    # Extend with anything we find in the registry or at the default install location.

    # Import a few things JIT. Some of these will only work on windows.
    from collections import defaultdict
    import winreg

    REGISTRY_PATHS = [(winreg.HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\ARM")]
    INSTALL_PATHS = [
        "C:\\Program Files (x86)\\Arm GNU Toolchain arm-none-eabi",
        "D:\\Arm_GNU_Toolchain\\13_3_rel1\\arm-none-eabi",
    ]

    gcc_vers = defaultdict(set)  # Map from version numbers to discovered paths.

    def check(pth):
        pth = pathlib.Path(pth) / "bin"
        ver = check_gcc_ver(pth, ext="exe")
        if ver:
            # Some older/alternate toolchains use the year as the current version number
            # (eg. "2021"). For EiE we will just ignore them.
            if ver[0] < 1000:
                gcc_vers[ver].add(str(pth))

    for root, subk in REGISTRY_PATHS:
        try:
            key = winreg.OpenKey(root, subk)
            (num_subks, _, _) = winreg.QueryInfoKey(key)
            for i in range(num_subks):
                try:
                    subk = winreg.EnumKey(key, i)
                    subk = winreg.OpenKey(key, subk)
                    (pth, _) = winreg.QueryValueEx(subk, "InstallFolder")
                    check(pth)
                    subk.Close()
                except:
                    continue
            key.Close()
        except:
            continue

    for install_pth in INSTALL_PATHS:
        install_pth = pathlib.Path(install_pth)
        if not install_pth.exists():
            continue

        for pth in install_pth.iterdir():
            check(pth)

    if not gcc_vers:
        # No installs found.
        return None

    vers = sorted(gcc_vers.keys())
    vers.reverse()

    # If different paths to the same version were found, using sorting to at least be
    # consistent between runs.
    pths = os.environ.get("PATH").split(os.pathsep)
    for v in vers:
        pths += sorted(gcc_vers[v])
    return pths


def get_gcc_srch_path_darwin():
    from collections import defaultdict

    gcc_vers = defaultdict(set)  # Map from version numbers to discovered paths.

    # Known default install paths on Mac
    INSTALL_PATHS = ["/Applications/ArmGNUToolchain/"]

    for install_pth in INSTALL_PATHS:
        install_pth = pathlib.Path(install_pth)
        for pth in install_pth.iterdir():
            # Not sure why but the mac paths have an extra path segment here
            pth = pth / "arm-none-eabi" / "bin"
            ver = check_gcc_ver(pth)
            if ver:
                gcc_vers[ver].add(str(pth))

    if not gcc_vers:
        # No installs found.
        return None

    vers = sorted(gcc_vers.keys())
    vers.reverse()

    # If different paths to the same version where found, using sorting to at least be
    # consistent between runs.
    pths = os.environ.get("PATH").split(os.pathsep)
    for v in vers:
        pths += sorted(gcc_vers[v])
    return pths


def get_gcc_srch_path():
    if Utils.is_win32:
        return get_gcc_srch_path_win32()
    elif Utils.unversioned_sys_platform() == "darwin":
        return get_gcc_srch_path_darwin()
    else:
        return None  # Tools should just be in the right spot.


# This is an example of how to hook into waf's task-generation process.
# by using @feature() here the function will be called on any task-generators
# that specified matching values in their "feature" variable. The function can
# do anything it wants, but typically it will either define a task to run during
# the build process, or it will modify the values of variables for other hooks
# to use (eg. you could add things to source to compile additional files.)
@feature("mapfile")  # Run on anything with the "mapfile" feature.
@after(
    "apply_link"
)  # Run after "apply_link". Needed because that's what creates the link_task
# that we modify
def apply_mapfile(tg):
    """
    Instruct the GCC linker to produce a .map file detailing the result of the link process.
    """
    # Get the file name to use for the mapfile.
    outfile = tg.link_task.outputs[0].change_ext(".map")
    # Extend the link task's flags with one telling gcc to generate the map file.
    tg.env.append_value("LINKFLAGS", [f"-Wl,-Map={outfile}"])
    tg.link_task.set_outputs([outfile])


@feature("c", "cxx", "d", "fc", "asm")
@after("apply_link")
def apply_custom_linker_script(tg):
    """
    process the "linker_script" attribute to add a custom linker script.
    Handles the command line, but also sets up waf to recognize it as a dependency.
    """
    script = getattr(tg, "linker_script", None)
    if not script:
        return
    if type(script) != Node:
        script = tg.bld.srcnode.find_resource(script)

    tg.link_task.dep_nodes.append(script)
    tg.env.append_value("LINKFLAGS", [f"-T{script}"])


class ihex(Task):
    """
    Waf task for converting executables to .hex files.
    Used by the ihex feature.
    """

    color = "BLUE"
    ext_out = [".hex"]
    run_str = "${OBJCOPY} -O ihex ${SRC} ${TGT}"

    def keyword(self):
        return "Creating"

    def __str__(self):
        def nice_path(node):
            return node.path_from(node.ctx.launch_node())

        return f"{nice_path(self.outputs[0])}"


@feature("ihex")
@after("apply_link")
def apply_ihex(tg):
    inf = tg.link_task.outputs[0]
    outf = inf.change_ext(".hex")
    tg.create_task("ihex", inf, outf)
