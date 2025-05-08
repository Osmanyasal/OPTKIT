<div id="user-content-toc" align=center>
  <ul>
    <summary><h1 style="display: inline-block;">Optimizer Toolkit-Core</h1></summary>
  </ul>
</div>
<div align="center">  
  <img alt="ICON" width="500" height="450" src="https://github-production-user-asset-6210df.s3.amazonaws.com/22853419/283257414-9ebee916-e82e-4ded-941e-caec84ff0e8f.png?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20231115%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20231115T215419Z&X-Amz-Expires=300&X-Amz-Signature=28932b080f7cc05fcfe7febeb633bd9576a1226755b18166d0d3650909013c86&X-Amz-SignedHeaders=host&actor_id=22853419&key_id=0&repo_id=705565955" />  
</div>  
<div align="center">
                <img align="center" title="C++" alt="C++" width="80px" src="https://github.com/Osmanyasal/Recursion-Engine/assets/22853419/f79dfb8b-ec5e-43a9-b3ec-7db4555b54e0" />
                &nbsp;&nbsp; 
                <img align="center" title="Linux" alt="Linux" width="60px" src="https://github.com/Osmanyasal/Recursion-Engine/assets/22853419/e67369bc-582f-4d90-bd92-7765b6f2a357" /> 
                &nbsp;&nbsp;
                <img align="center" title="Mac" alt="Mac" width="60px" src="https://github.com/Osmanyasal/Recursion-Engine/assets/22853419/a635c69d-cc34-46df-a0ed-de5771609a4d" />
                &nbsp;&nbsp;
                <!-- <img align="center" alt="Windows" width="60px" src="https://github.com/Osmanyasal/Recursion-Engine/assets/22853419/c1717d00-63d2-426f-a648-18a2d618f849" /> -->
</div>
<div align="center">
                <img align="center" title="GLFW" alt="GLFW" width="60px" src="https://www.glfw.org/img/favicon/favicon-196x196.png" />
                &nbsp;&nbsp; 
                <img align="center" title="libpfm4" alt="libpfm4" width="100px" src="https://github.com/Osmanyasal/Optimizer-Toolkit/assets/22853419/5b4313af-b87e-4615-b110-75733b67289d" /> 
                &nbsp;&nbsp; 
                <img align="center" title="Dear Imgui" alt="Dear Imgui" width="60px" src="https://raw.githubusercontent.com/wiki/sammyfreg/netImgui/Web/img/DearImGui.png" /> 
                &nbsp;&nbsp; 
                <img align="center" title="Doxygen" alt="Doxygen" width="150px" src="https://www.doxygen.nl/images/doxygen.png" /> 
</div>

## About 
* Cross Platform Performance Ecosystem mainly developed for Linux and MacOS environments.<br>
* <b>Utilizes</b> all advantageous features of <b>PAPI, Linux Perf, Intel Vtune/Advisor, and AMD uProf profilers into a single, unified platform</b> by harnessing the code and binary instrumentation techniques while providing a straightforward but elegant GUI for advanced in- and post-development monitoring.
* <b>100% CPU agnostic profiling</b>
* Provides <b>100% accuracy</b> thanks to code-line instrumentation.
* <b>An Intuitive Programming API for in-development monitoring support</b>
* Supports diverse PMU monitoring with <b>0 learning curve</b>.
* <b>Bottleneck detection in any phase of the development.</b>
* <b>Overhead is very very low</b> and upto the programmer itself.
* Python binding. Yes, you’ll have full Python support in code-line instrumentation.
* Developed with C++ and Make/Bash scripts.
* <b>You can monitor any executable file</b>
* <b>Use in your c++ and python projects</b>

## To run 🚀 
```
git clone --recurse-submodules https://github.com/Osmanyasal/Optimizer-Toolkit-Core.git
cd ./Optimizer-Toolkit-Core
make
```

## IN DEVELOPMENT PERFORMANCE AID 🔎  
<img align="center" alt="ICON" width="50%" src="https://github.com/Osmanyasal/Optimizer-Toolkit/assets/22853419/039b5dd2-6635-435d-9dfc-4977336db0a8" />
 
 * Recursion Optimizer Toolkit gives you an API that you can use in your C++ and Python projects to monitor Performance Monitoring Unit (PMU) events in your CPU.<br>
 * With the help of RAII, you can monitor any pmu events including (cycles, instruction count, LXX cache hits/misses) or use cases including (microarchitecture analysis, energy-consumption analysis, thread interaction monitoring, etc.)
 * API can also draw charts as your program is being run for live monitoring



## POST MORTERM PERFORMANCE MONITORING 🔎  
 <img align="center" alt="ICON" width="50%" src="https://github.com/Osmanyasal/Optimizer-Toolkit/assets/22853419/f17c6d7f-0d50-4785-8282-5259d277a653" />
 
 * Recursion Optimizer Toolkit also gives you a total GUI to perform post-mortem analysis on `any` executable.
 * The binary executable does `NOT` need to be written with Recursion Optimizer Toolkit API.
 * It simply injects API code into binary executable via static and dynamic binary instrumentation with the Intel PIN tool.
 * User can monitor any method in the executable, all monitoring in API is available here as well.

 
